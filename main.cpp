#include <SFML/Graphics.hpp>
#include <iostream>
#include "Grafo.h"

int main()
{
    Grafo grafo;

    if (!grafo.cargarGrafo("campus_graph.json"))
    {
        std::cerr << "Error al cargar el grafo." << std::endl;
        return 1;
    }

    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Visualizacion del Grafo");

    while (ventana.isOpen())
    {
        sf::Event evento;
        while (ventana.pollEvent(evento))
        {
            if (evento.type == sf::Event::Closed)
                ventana.close();
        }

        ventana.clear(sf::Color::Black);

        // Dibujar el grafo con un camino vacío
        grafo.Dibujar(ventana, {});

        ventana.display();
    }

    return 0;
}
