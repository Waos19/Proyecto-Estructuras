#ifndef INTERFAZ_H
#define INTERFAZ_H

#include "Grafo.h"
#include <SFML/Graphics.hpp>
#include <utility> // Para std::pair

using namespace sf;

class Interfaz
{
private:
    Grafo& grafo;
    sf::Font fuente;
    unordered_map<string, Color> coloresNodos;
    int nodoSeleccionado = -1; // Índice del nodo seleccionado
    bool arrastrandoNodo = false; // Indica si se está arrastrando un nodo
    sf::Vector2f offsetMapa = {0.f, 0.f}; // Desplazamiento actual del mapa
    bool arrastrandoMapa = false;
    sf::Vector2i mouseAnterior;
    float zoom = 1.0f; // 1.0 = 100%, >1 = acercar, <1 = alejar

public:
    Interfaz(Grafo& grafo);
    void manejarEventoMouse(RenderWindow& ventana, const sf::Event& event);
    void manejarEventoMapa(RenderWindow& ventana, const sf::Event& event);
    void manejarEventoZoom(const sf::Event& event);
    void inicializarColores();
    void dibujarGrafo(RenderWindow& ventana, const vector<string>& camino = {});
    void dibujarMapa(RenderWindow& ventana, const string& rutaMapa);
    pair<double, double> invertirMapeo(float x, float y, const RenderWindow& ventana) const;
    void cargarFuente();
    pair<float, float> mapearCoordenadas(double x, double y, const RenderWindow& ventana) const;
    bool aristaEnCamino(const string& u, const string& v, const vector<string>& camino) const;

    
};

#endif