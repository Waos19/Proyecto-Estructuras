#include "Grafo.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

Grafo::Grafo(const string& jsonFilePath) {
    try {
        ifstream inputFile(jsonFilePath);
        if (!inputFile.is_open()) {
            throw runtime_error("No se pudo abrir el archivo JSON: " + jsonFilePath);
        }

        json jsonData;
        inputFile >> jsonData;
        cargarDesdeJSON(jsonData);

        cout << "Grafo cargado exitosamente. Nodos: " << nodos.size() 
                << ", Aristas: " << Adyacencias.size() << endl;
    } catch (const exception& e) {
        cerr << "Error al cargar el grafo: " << e.what() << endl;
        throw;
    }
}

void Grafo::cargarDesdeJSON(const json& jsonData) {
    // Cargar nodos
    if (!jsonData.contains("nodes")) {
        throw runtime_error("El JSON no contiene la sección 'nodes'");
    }
    for (const auto& nodoJson : jsonData["nodes"]) {
        procesarNodo(nodoJson);
    }

    // Cargar aristas
    if (!jsonData.contains("edges")) {
        throw runtime_error("El JSON no contiene la sección 'edges'");
    }
    for (const auto& aristaJson : jsonData["edges"]) {
        procesarArista(aristaJson);
    }
}

void Grafo::procesarNodo(const json& nodoJson) {
    try {
        Nodo nuevoNodo(
            nodoJson["id"].get<string>(),
            nodoJson["lat"].get<double>(),
            nodoJson["lon"].get<double>(),
            nodoJson.value("name", ""),
            nodoJson.value("amenity", ""),
            nodoJson["type"].get<string>()
        );
        nodos.push_back(nuevoNodo);
    } catch (const exception& e) {
        cerr << "Error al procesar nodo: " << e.what() << endl;
    }
}

void Grafo::procesarArista(const json& aristaJson) {
    try {
        string origen = aristaJson["source"];
        string destino = aristaJson["target"];
        double peso = aristaJson["weight"];
        
        // Agregar en ambas direcciones para grafo no dirigido
        Adyacencias[origen].emplace_back(destino, peso);
        Adyacencias[destino].emplace_back(origen, peso);
    } catch (const exception& e) {
        cerr << "Error al procesar arista: " << e.what() << endl;
    }
}

const vector<Nodo>& Grafo::getNodos() const {
    return nodos;
}

const unordered_map<string, vector<pair<string, double>>>& Grafo::getAdyacencias() const {
    return Adyacencias;
}

int Grafo::encontrarIndiceNodo(const string& id) const {
    for (size_t i = 0; i < nodos.size(); ++i) {
        if (nodos[i].getId() == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}