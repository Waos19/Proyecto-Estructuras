//#include <SFML/Graphics.hpp>
#include "Grafo.h"
#include <iostream>

using namespace std;

int main()
{
    Grafo campus;

    cout << "Cargando grafo..." << endl;

    if (!campus.cargarGrafo("campus_graph.json"))
    {
        cerr << "No se pudo cargar el grafo." << endl;
        return 1;
    }

    int origen = 0;
    int destino = 6;

    vector<int> predecesores;
    vector<double> distancias = campus.caminoCorto(origen, predecesores);

    cout << "Distancia desde nodo " << origen << " hasta nodo " << destino << ": " << distancias[destino] << endl;

    vector<int> camino = campus.reconstruirCamino(destino, predecesores);
    cout << "Camino más corto: ";
    for (int id : camino)
    {
        cout << id << " ";
    }
    cout << endl;
    return 0;
}