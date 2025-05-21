#ifndef GRAFO_H
#include <iostream>
#include <vector>
// #include <list>
#include <fstream>
#include <queue>
#include "Nodo.h"
#include "json.hpp"
#include <SFML/Graphics.hpp>

using namespace std;
using json = nlohmann::json;


class Grafo
{
    private:
        vector<Nodo> nodos;
        vector<vector<pair<int, double>>> adyacencias;
    public:
        vector<Nodo> getNodos() const;
        bool cargarGrafo(const string& nombreArchivo);
        void mostrarConexiones(int idNodo);
        vector<double> caminoCorto(int origen, vector<int>predecesores);
        vector<int> reconstruirCamino(int destino, const vector<int>& predecesores);
        void Dibujar(sf::RenderWindow &ventana);

};


#endif