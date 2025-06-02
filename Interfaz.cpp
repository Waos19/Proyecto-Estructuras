#include "Interfaz.h"
#include <iostream>
#include <algorithm>

Interfaz::Interfaz(Grafo& grafo) : grafo(grafo)
{
    cargarFuente();
    inicializarColores();
}

void Interfaz::manejarEventoMouse(sf::RenderWindow& ventana, const sf::Event& event) {
    const auto& nodos = grafo.getNodos(); // Usar referencia const para objetos const
    // Si getNodos() retorna una referencia const, necesitas modificar Grafo para que retorne una referencia no-const cuando sea necesario.
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        float mx = event.mouseButton.x;
        float my = event.mouseButton.y;
        for (size_t i = 0; i < nodos.size(); ++i) {
            auto [x, y] = mapearCoordenadas(nodos[i].getLatitud(), nodos[i].getLongitud(), ventana);
            float radio = 8.0f;
            if (std::hypot(mx - x, my - y) < radio) {
                nodoSeleccionado = static_cast<int>(i);
                arrastrandoNodo = true;
                break;
            }
        }
    }
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        arrastrandoNodo = false;
        nodoSeleccionado = -1;
    }
    if (event.type == sf::Event::MouseMoved && arrastrandoNodo && nodoSeleccionado != -1) {
        float mx = event.mouseMove.x;
        float my = event.mouseMove.y;
        double nuevaLat, nuevaLon;
        std::tie(nuevaLat, nuevaLon) = invertirMapeo(mx, my, ventana);
        // Obtener referencia no-const a los nodos para modificar
        auto& nodosModificables = (grafo).getNodos();
        nodosModificables[nodoSeleccionado].setLatitud(nuevaLat);
        nodosModificables[nodoSeleccionado].setLongitud(nuevaLon);
    }
}

void Interfaz::manejarEventoMapa(sf::RenderWindow& ventana, const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
        arrastrandoMapa = true;
        mouseAnterior = {event.mouseButton.x, event.mouseButton.y};
    }
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
        arrastrandoMapa = false;
    }
    if (event.type == sf::Event::MouseMoved && arrastrandoMapa) {
        sf::Vector2i mouseActual = {event.mouseMove.x, event.mouseMove.y};
        sf::Vector2f delta = sf::Vector2f(mouseActual - mouseAnterior);
        offsetMapa += delta;
        mouseAnterior = mouseActual;
    }
}

pair<double, double> Interfaz::invertirMapeo(float x, float y, const sf::RenderWindow& ventana) const {
    const auto& nodos = grafo.getNodos();
    if (nodos.empty()) return {0.0, 0.0};

    auto [minLat, maxLat] = std::minmax_element(nodos.begin(), nodos.end(),
        [](const Nodo& a, const Nodo& b) { return a.getLatitud() < b.getLatitud(); });
    auto [minLon, maxLon] = std::minmax_element(nodos.begin(), nodos.end(),
        [](const Nodo& a, const Nodo& b) { return a.getLongitud() < b.getLongitud(); });

    const float margin = 0.0005f;
    float ancho = ventana.getSize().x;
    float alto  = ventana.getSize().y;

    double deltaLon = maxLon->getLongitud() - minLon->getLongitud();
    double deltaLat = maxLat->getLatitud() - minLat->getLatitud();

    if (std::abs(deltaLon) < 1e-9) deltaLon = 1.0;
    if (std::abs(deltaLat) < 1e-9) deltaLat = 1.0;

    // Aplica zoom inverso centrado en el centro de la ventana
    sf::Vector2f centro(ancho / 2.f, alto / 2.f);
    x = (x - centro.x) / zoom + centro.x;
    y = (y - centro.y) / zoom + centro.y;

    double lon = (x / ancho) * (deltaLon + 2*margin) + minLon->getLongitud() + margin;
    double lat = ((alto - y) / alto) * (deltaLat + 2*margin) + minLat->getLatitud() + margin;

    return {lat, lon};
}

void Interfaz::cargarFuente()
{
    if (!fuente.loadFromFile("ARIAL.TTF")) {
        std::cerr << "Error al cargar la fuente" << std::endl;
    }
}

void Interfaz::inicializarColores() {
    // Asignar colores según el tipo de nodo
    coloresNodos = {
        {"building", sf::Color(70, 130, 180)},    // Azul acero
        {"cafe", sf::Color(210, 105, 30)},        // Chocolate
        {"bench", sf::Color(100, 149, 237)},      // Azul claro
        {"waste_basket", sf::Color(47, 79, 79)},  // Gris oscuro
        {"node", sf::Color(169, 169, 169)}        // Gris medio
    };
}

std::pair<float, float> Interfaz::mapearCoordenadas(double lat, double lon, const sf::RenderWindow& ventana) const {
    const auto& nodos = grafo.getNodos();
    if (nodos.empty()) {
        return {0.f, 0.f};
    }

    auto [minLat, maxLat] = std::minmax_element(nodos.begin(), nodos.end(),
        [](const Nodo& a, const Nodo& b) { return a.getLatitud() < b.getLatitud(); });

    auto [minLon, maxLon] = std::minmax_element(nodos.begin(), nodos.end(),
        [](const Nodo& a, const Nodo& b) { return a.getLongitud() < b.getLongitud(); });

    const float margin = 0.0005f;
    double deltaLon = maxLon->getLongitud() - minLon->getLongitud();
    double deltaLat = maxLat->getLatitud() - minLat->getLatitud();

    // Evita división por cero
    if (std::abs(deltaLon) < 1e-9) deltaLon = 1.0;
    if (std::abs(deltaLat) < 1e-9) deltaLat = 1.0;

    float x = ((lon - minLon->getLongitud() - margin) /
              (deltaLon + 2*margin)) * ventana.getSize().x;

    float y = ventana.getSize().y - ((lat - minLat->getLatitud() - margin) /
              (deltaLat + 2*margin)) * ventana.getSize().y;

    // Aplica zoom centrado en el centro de la ventana
    sf::Vector2f centro(ventana.getSize().x / 2.f, ventana.getSize().y / 2.f);
    x = (x - centro.x) * zoom + centro.x + offsetMapa.x;
    y = (y - centro.y) * zoom + centro.y + offsetMapa.y;

    return {x, y};
}

void Interfaz::dibujarGrafo(sf::RenderWindow& ventana, const std::vector<std::string>& camino) {
    const auto& nodos = grafo.getNodos();
    const auto& adyacencias = grafo.getAdyacencias();

    // Dibujar aristas primero (para que queden detrás de los nodos)
    for (const auto& nodo : nodos) {
        auto pos1 = mapearCoordenadas(nodo.getLatitud(), nodo.getLongitud(), ventana);
        auto it = adyacencias.find(nodo.getId());

        if (it != adyacencias.end()) {
            for (const auto& conexion : it->second) {
                // Encontrar el nodo destino
                auto nodoDest = std::find_if(nodos.begin(), nodos.end(),
                    [&](const Nodo& n) { return n.getId() == conexion.first; });

                if (nodoDest != nodos.end()) {
                    auto pos2 = mapearCoordenadas(nodoDest->getLatitud(), nodoDest->getLongitud(), ventana);

                    // Determinar si esta arista está en el camino
                    bool enCamino = std::find(camino.begin(), camino.end(), nodo.getId()) != camino.end() &&
                                    std::find(camino.begin(), camino.end(), conexion.first) != camino.end();

                    sf::Color colorArista = enCamino ? sf::Color::Red : sf::Color(150, 150, 150, 150);
                    float grosor = enCamino ? 3.0f : 1.5f;

                    // Dibujar línea
                    sf::Vertex linea[] = {
                        sf::Vertex(sf::Vector2f(pos1.first, pos1.second), colorArista),
                        sf::Vertex(sf::Vector2f(pos2.first, pos2.second), colorArista)
                    };
                    ventana.draw(linea, 2, sf::Lines);
                }
            }
        }
    }

    // Antes del bucle de nodos
    std::cout << "Dibujando nodos: " << nodos.size() << std::endl;
    // Dibujar nodos
    for (const auto& nodo : nodos) {
        if (std::isnan(nodo.getLatitud()) || std::isnan(nodo.getLongitud())) {
            std::cerr << "Nodo con lat/lon inválida: " << nodo.getId() << std::endl;
            continue;
        }
        auto [x, y] = mapearCoordenadas(nodo.getLatitud(), nodo.getLongitud(), ventana);
        float radio = 8.0f;
        bool enCamino = std::find(camino.begin(), camino.end(), nodo.getId()) != camino.end();

        // Determinar color del nodo
        sf::Color colorNodo;
        if (enCamino) {
            colorNodo = sf::Color::Red;
            radio = 10.0f;
        } else {
            auto tipo = nodo.getType() == "node" ? nodo.getAmenity() : nodo.getType();
            colorNodo = coloresNodos.count(tipo) ? coloresNodos[tipo] : sf::Color::Magenta;
        }

        // Dibujar círculo del nodo
        sf::CircleShape forma(radio);
        forma.setFillColor(colorNodo);
        forma.setOutlineColor(sf::Color::Black);
        forma.setOutlineThickness(1);
        forma.setPosition(x - radio, y - radio);
        ventana.draw(forma);

        // Dibujar etiqueta
        if (fuente.getInfo().family != "") {
            sf::Text texto;
            texto.setFont(fuente);
            texto.setString(nodo.getNombre().empty() ? nodo.getId() : nodo.getNombre());
            texto.setCharacterSize(12);
            texto.setFillColor(sf::Color::Black);
            texto.setPosition(x + radio + 2, y - radio);
            ventana.draw(texto);
        }
    }
}

void Interfaz::manejarEventoZoom(const sf::Event& event)
{
    if (event.type == sf::Event::MouseWheelScrolled)
    {
        if (event.mouseWheelScroll.delta > 0)
            zoom *= 1.1f; // Acercar
        else if (event.mouseWheelScroll.delta < 0)
            zoom /= 1.1f; // Alejar
    }
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Up)
        {
            zoom *= 1.1f; // Acercar
        }
        if (event.key.code == sf::Keyboard::Down) {
            zoom /= 1.1f; // Alejar
        }
    }
    // Limita el zoom
    if (zoom < 0.2f) zoom = 0.2f;
    if (zoom > 5.0f) zoom = 5.0f;
}