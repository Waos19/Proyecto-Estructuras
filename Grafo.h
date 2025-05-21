#ifndef GRAFO_H
#include <iostream>
#include <vector>
#include <list>
#include "Nodo.h"
#include "json.hpp"
#include <fstream>

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
};


#endif