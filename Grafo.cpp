#include "Grafo.h"

void Grafo::cargarGrafo(const string &nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo." << endl;
        return;
    }

    int nNodos;
    archivo >> nNodos;

    for (int i = 0; i < nNodos; ++i) {
        long long id;
        double lat, lon;
        archivo >> id >> lat >> lon;
        agregarNodo(id, lat, lon);
    }

    cout << "Nodos cargados: " << nodos.size() << endl;


    int nAristas;
    archivo >> nAristas;

    for (int i = 0; i < nAristas; ++i) {
        long long id1, id2;
        double peso;
        archivo >> id1 >> id2 >> peso;
        agregarArista(id1, id2, peso);
    }

    archivo.close();
}

void Grafo::agregarNodo(long long id, double latitud, double longitud) {
    nodos.emplace(id, Nodo(id, latitud, longitud));
}

void Grafo::agregarArista(long long idOrigen, long long idDestino, double peso) {
    nodos[idOrigen].agregarVecino(idDestino, peso);
    nodos[idDestino].agregarVecino(idOrigen, peso);
}

void Grafo::imprimirGrafo() {
    for (const auto& [id, nodo] : nodos) {
        cout << "Nodo " << id << " (" << nodo.latitud << ", " << nodo.longitud << ") -> ";
        for (const auto& [vecino_id, peso] : nodo.vecinos)
            cout << vecino_id << "(" << peso << ") ";
        cout << endl;
    }
}

vector<long long> Grafo::dijkstra(long long origen, long long destino) {
    unordered_map<long long, double> distancias;
    unordered_map<long long, long long> anteriores;
    priority_queue<pair<double, long long>, vector<pair<double, long long>>, greater<>> cola;

    for (const auto& [id, _] : nodos)
        distancias[id] = numeric_limits<double>::infinity();

    distancias[origen] = 0;
    cola.emplace(0, origen);

    while (!cola.empty()) {
        auto [dist, actual] = cola.top(); cola.pop();

        if (actual == destino) break;

        for (const auto& [vecino, peso] : nodos[actual].vecinos) {
            double nueva_dist = dist + peso;
            if (nueva_dist < distancias[vecino]) {
                distancias[vecino] = nueva_dist;
                anteriores[vecino] = actual;
                cola.emplace(nueva_dist, vecino);
            }
        }
    }

    vector<long long> ruta;
    if (!anteriores.count(destino)) return ruta;

    for (long long at = destino; at != origen; at = anteriores[at])
        ruta.push_back(at);
    ruta.push_back(origen);
    reverse(ruta.begin(), ruta.end());
    return ruta;
}
