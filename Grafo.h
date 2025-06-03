#ifndef GRAFO_H
#define GRAFO_H

#include "Nodo.h"
#include <unordered_map>
#include <fstream>
#include <queue>
#include <limits>
#include <algorithm>

class Grafo
{
    public:
        unordered_map<long long, Nodo> nodos;
        void cargarGrafo(const string& nombreArchivo);
        void agregarNodo(long long id, double latitud, double longitud);
        void agregarArista(long long idOrigen, long long idDestino, double peso);
        void imprimirGrafo();
        vector<long long> dijkstra(long long origen, long long destino);
};

#endif