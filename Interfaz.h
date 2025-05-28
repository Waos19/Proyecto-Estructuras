#ifndef INTERFAZ_H
#define INTERFAZ_H

#include "Grafo.h"
#include <SFML/Graphics.hpp>
#include <utility> // Para std::pair

using namespace sf;

class Interfaz
{
private:
    const Grafo& grafo;
    sf::Font fuente;
    unordered_map<string, Color> coloresNodos;

public:
    Interfaz(const Grafo& grafo);
    void inicializarColores();
    void dibujarGrafo(RenderWindow& ventana, const vector<string>& camino = {});
    void dibujarMapa(RenderWindow& ventana, const string& rutaMapa);
    void cargarFuente();
    pair<float, float> mapearCoordenadas(double x, double y, const RenderWindow& ventana) const;
    bool aristaEnCamino(const string& u, const string& v, const vector<string>& camino) const;

    
};

#endif