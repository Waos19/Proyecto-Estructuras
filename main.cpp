//#include <SFML/Graphics.hpp>
#include "Grafo.h"

int main() {
    // Crear ventana SFML
    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Visualización de Grafo");
    ventana.setFramerateLimit(60);

    // Crear e inicializar grafo
    Grafo grafo;
    if (!grafo.cargarGrafo("campus_graph.json")) {  // Asegúrate de tener este archivo
        return 1;
    }

    // Bucle principal
    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) {
                ventana.close();
            }
        }

        // Limpiar ventana
        ventana.clear(sf::Color::Black);

        // Dibujar el grafo
        grafo.Dibujar(ventana);

        // Mostrar lo dibujado
        ventana.display();
    }

    return 0;
}