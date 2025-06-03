import sys
import os
import subprocess
import folium
import osmnx as ox
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.QtWebEngineWidgets import QWebEngineView
from PyQt5.QtCore import QUrl, pyqtSlot, QObject, pyqtSignal
from PyQt5.QtWebChannel import QWebChannel


class Communicator(QObject):
    logMessage = pyqtSignal(str)

    @pyqtSlot(str)
    def log(self, message):
        print(message)


class MapaInteractivo(QMainWindow):
    def __init__(self, G):
        super().__init__()
        self.setWindowTitle("Mapa Interactivo - Universidad de los Llanos")
        self.setGeometry(100, 100, 800, 600)

        self.G = G
        self.origen = None
        self.destino = None
        self.ruta_coords = None

        self.view = QWebEngineView()
        self.setCentralWidget(self.view)
        self.view.titleChanged.connect(self.titulo_cambiado)

        self.comms = Communicator()
        self.comms.logMessage.connect(self.log_to_console)
        self.view.loadFinished.connect(self.inicializar_webchannel)

        self.actualizar_mapa()

    def log_to_console(self, message):
        print(f"JavaScript: {message}")

    def actualizar_mapa(self):
        centro = ox.graph_to_gdfs(self.G, nodes=False).geometry.union_all().centroid
        self.mapa = folium.Map(location=[centro.y, centro.x], zoom_start=30, control_scale=True)

        for node, data in self.G.nodes(data=True):
            lat, lon = data['y'], data['x']
            folium.CircleMarker(
                location=(lat, lon),
                radius=5,
                color='blue',
                fill=True,
                fill_color='blue',
                fill_opacity=0.7,
                tooltip=f"ID: {node}",
            ).add_to(self.mapa)
        if self.origen is not None:
            lat_o = self.G.nodes[self.origen]['y']
            lon_o = self.G.nodes[self.origen]['x']
            folium.CircleMarker(
                location=(lat_o, lon_o),
                radius=6,
                color='green',
                fill=True,
                fill_color='green',
                fill_opacity=1.0,
                popup="Origen"
            ).add_to(self.mapa)

        if self.destino is not None:
            lat_d = self.G.nodes[self.destino]['y']
            lon_d = self.G.nodes[self.destino]['x']
            folium.CircleMarker(
                location=(lat_d, lon_d),
                radius=6,
                color='red',
                fill=True,
                fill_color='red',
                fill_opacity=1.0,
                popup="Destino"
            ).add_to(self.mapa)


        # ✅ Dibujar ruta si existe
        if self.ruta_coords:
            folium.PolyLine(
                locations=self.ruta_coords,
                color='red',
                weight=5,
                opacity=0.8
            ).add_to(self.mapa)

        # Script de QWebChannel
        script_tag = '<script type="text/javascript" src="./qwebchannel.js"></script>'
        self.mapa.get_root().header.add_child(folium.Element(script_tag))

        mapa_js = """
            var lastCoords = null;
            function getFoliumMap() {
                for (var key in window) {
                    if (key.startsWith("map_") && window[key] instanceof L.Map) {
                        return window[key];
                    }
                }
                return null;
            }
            function setupMapInteractions(communicator) {
                function onMapClick(e) {
                    lastCoords = e.latlng.lat + "," + e.latlng.lng;
                    communicator.log("Click registrado en: " + lastCoords);
                    alert("Click registrado en: " + lastCoords + "\\nPresiona 'o' para origen o 'd' para destino");
                }
                document.addEventListener('keydown', function(event) {
                    if (lastCoords) {
                        if (event.key === 'o' || event.key === 'O') {
                            document.title = lastCoords + ";origen";
                            lastCoords = null;
                        } else if (event.key === 'd' || event.key === 'D') {
                            document.title = lastCoords + ";destino";
                            lastCoords = null;
                        }
                    }
                });
                var folium_map = getFoliumMap();
                if (folium_map) {
                    folium_map.on('click', onMapClick);
                }
            }
            function waitForQtAndInit() {
                if (typeof qt !== 'undefined') {
                    new QWebChannel(qt.webChannelTransport, function(channel) {
                        window.communicator = channel.objects.communicator;
                        setupMapInteractions(window.communicator);
                    });
                } else {
                    console.log("Esperando a que qt esté disponible...");
                    setTimeout(waitForQtAndInit, 100);
                }
            }
            if (typeof QWebChannel !== "undefined") {
                waitForQtAndInit();
            } else {
                alert("❌ QWebChannel no está definido.");
            }
        """

        self.mapa.get_root().html.add_child(folium.Element(f"<script>{mapa_js}</script>"))

        if not os.path.exists("temp"):
            os.makedirs("temp")
        ruta_html = os.path.abspath("temp/mapa.html")
        self.mapa.save(ruta_html)

        self.view.setUrl(QUrl("about:blank"))
        self.view.setUrl(QUrl.fromLocalFile(ruta_html))

    def titulo_cambiado(self, title):
        if ";origen" in title or ";destino" in title:
            coords, tipo = title.split(";")
            lat, lon = map(float, coords.split(","))
            nodo_mas_cercano = ox.distance.nearest_nodes(self.G, lon, lat)

            if tipo == "origen":
                self.origen = nodo_mas_cercano
            elif tipo == "destino":
                self.destino = nodo_mas_cercano

            if self.origen is not None and self.destino is not None:
                self.calcular_ruta()

            self.actualizar_mapa()

    def calcular_ruta(self):
        print("🔁 Iniciando cálculo de ruta...")

        if self.origen is None or self.destino is None:
            print("❌ Origen o destino no están definidos.")
            return

        print(f"🧭 Nodo origen: {self.origen}")
        print(f"🎯 Nodo destino: {self.destino}")

        entrada_path = "temp/entrada.txt"
        self.exportar_grafo(self.G, entrada_path)
        print(f"📄 Grafo exportado en: {entrada_path}")

        parametros_path = "temp/parametros.txt"
        try:
            with open(parametros_path, "w") as f:
                f.write(f"{self.origen} {self.destino}\n")
            print(f"📄 parámetros guardados en: {parametros_path}")
        except Exception as e:
            print(f"❌ Error al escribir parametros.txt: {e}")
            return

        try:
            with open(parametros_path, "r") as f:
                print("📄 Contenido de parametros.txt:", f.read().strip())
        except Exception as e:
            print(f"❌ No se pudo leer parametros.txt: {e}")
            return

        print("🚀 Ejecutando main.exe para calcular la ruta...")
        try:
            resultado = subprocess.run(["./main.exe"], capture_output=True, text=True, encoding='utf-8', timeout=10)
        except subprocess.TimeoutExpired:
            print("⏰ main.exe tardó demasiado y fue forzado a terminar.")
            return
        except Exception as e:
            print(f"❌ Error al ejecutar main.exe: {e}")
            return

        print("📤 STDOUT de main.exe:\n", resultado.stdout.strip())
        print("📥 STDERR de main.exe:\n", resultado.stderr.strip())

        if resultado.returncode != 0:
            print(f"❌ main.exe terminó con código {resultado.returncode}")
            return

        salida_path = "temp/salida.txt"
        if not os.path.exists(salida_path):
            print("❌ No se generó temp/salida.txt. El ejecutable puede haber fallado.")
            return

        try:
            ruta = []
            with open(salida_path, "r") as f:
                for linea in f:
                    linea = linea.strip()
                    if not linea:
                        continue
                    try:
                        nodo_id = int(linea)
                    except ValueError:
                        print(f"⚠️ Línea no válida en salida.txt: '{linea}'")
                        continue
                    if nodo_id not in self.G.nodes:
                        print(f"⚠️ Nodo {nodo_id} no encontrado en el grafo.")
                        continue
                    ruta.append(nodo_id)

            if not ruta:
                print("⚠️ La ruta está vacía (no se encontró camino posible).")
                self.ruta_coords = None
                self.actualizar_mapa()
                return

            print("✅ Ruta obtenida con", len(ruta), "nodos.")
            self.ruta_coords = [(self.G.nodes[n]['y'], self.G.nodes[n]['x']) for n in ruta]
            self.actualizar_mapa()
        except Exception as e:
            print(f"❌ Error al procesar salida.txt: {e}")

    @staticmethod
    def exportar_grafo(G, archivo_salida="entrada.txt"):
        if not os.path.exists("temp"):
            os.makedirs("temp")
        with open(archivo_salida, "w") as f:
            f.write(f"{len(G.nodes)}\n")
            for node, data in G.nodes(data=True):
                f.write(f"{node} {data['y']} {data['x']}\n")

            aristas = list(G.edges(data=True))
            f.write(f"{len(aristas) * 2}\n")
            for u, v, data in aristas:
                peso = data.get("length", 1.0)
                f.write(f"{u} {v} {peso}\n")
                f.write(f"{v} {u} {peso}\n")

    def inicializar_webchannel(self):
        channel = QWebChannel(self.view.page())
        channel.registerObject('communicator', self.comms)
        self.view.page().setWebChannel(channel)
        print("✅ Canal QWebChannel inicializado.")
        self.view.page().runJavaScript("""
            if (typeof QWebChannel !== "undefined") {
                new QWebChannel(qt.webChannelTransport, function(channel) {
                    window.communicator = channel.objects.communicator;
                    console.log("✅ Canal JS conectado.");
                });
            } else {
                alert("❌ QWebChannel no está definido.");
            }
        """)


if __name__ == "__main__":
    G = ox.graph_from_xml("data/Campus_Graph.osm").to_undirected()
    app = QApplication(sys.argv)
    ventana = MapaInteractivo(G)
    ventana.show()
    sys.exit(app.exec_())
