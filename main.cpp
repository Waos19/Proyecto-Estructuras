#include "Interfaz.h"
#include "Grafo.h"
#include <SFML/Graphics.hpp>

int main() {
    // Crear ventana
    sf::RenderWindow ventana(sf::VideoMode(1200, 800), "Visualizador de Grafo del Campus");
    
    // Cargar grafo desde JSON
    Grafo grafo("campus_graph.json");
    
    // Crear interfaz visual
    Interfaz interfaz(grafo);

    // Definir un camino de ejemplo (usar IDs reales de tu JSON)
    std::vector<std::string> caminoEjemplo = {
        "12832500514",  // Bench
        "12832500515",  // Waste basket
        "12839112483"   // Bench
    };

    // Bucle principal
    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            interfaz.manejarEventoZoom(evento);
            interfaz.manejarEventoMapa(ventana, evento);
            interfaz.manejarEventoMouse(ventana, evento);
            if (evento.type == sf::Event::Closed) {

                ventana.close();
            }
        }

        ventana.clear(sf::Color(240, 240, 240)); // Fondo gris claro
        
        // Dibujar el grafo
        interfaz.dibujarGrafo(ventana, caminoEjemplo);
        
        ventana.display();
    }

    return 0;
}