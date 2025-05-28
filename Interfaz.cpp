#include "Interfaz.h"
#include <iostream>
#include <algorithm>

Interfaz::Interfaz(const Grafo& grafo) : grafo(grafo) {
    cargarFuente();
    inicializarColores();
}

void Interfaz::cargarFuente() {
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
        return {0.f, 0.f}; // Evita segmentation fault si no hay nodos
    }

    // Encontrar límites geográficos
    auto [minLat, maxLat] = std::minmax_element(nodos.begin(), nodos.end(),
        [](const Nodo& a, const Nodo& b) { return a.getLatitud() < b.getLatitud(); });

    auto [minLon, maxLon] = std::minmax_element(nodos.begin(), nodos.end(),
        [](const Nodo& a, const Nodo& b) { return a.getLongitud() < b.getLongitud(); });

    // Mapear a coordenadas de pantalla con márgenes
    const float margin = 0.0005f;
    float x = ((lon - minLon->getLongitud() - margin) /
              (maxLon->getLongitud() - minLon->getLongitud() + 2*margin)) * ventana.getSize().x;

    float y = ventana.getSize().y - ((lat - minLat->getLatitud() - margin) /
              (maxLat->getLatitud() - minLat->getLatitud() + 2*margin)) * ventana.getSize().y;

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

    // Dibujar nodos
    for (const auto& nodo : nodos) {
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