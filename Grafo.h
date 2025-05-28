#ifndef GRAFO_H
#define GRAFO_H

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include "Nodo.h"
#include "json.hpp" 

using namespace std;
using json = nlohmann::json;

class Grafo {
private:
    vector<Nodo> nodos;
    unordered_map<string, vector<pair<string, double>>> Adyacencias;

public:
    // Constructor que carga desde JSON
    Grafo(const string& jsonFilePath);
    
    // Métodos de acceso
    const vector<Nodo>& getNodos() const;
    const unordered_map<string, vector<pair<string, double>>>& getAdyacencias() const;
    int encontrarIndiceNodo(const string& id) const;

private:
    // Métodos de carga interna
    void cargarDesdeJSON(const json& jsonData);
    void procesarNodo(const json& nodoJson);
    void procesarArista(const json& aristaJson);
};
#endif