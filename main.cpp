//#include <SFML/Graphics.hpp>
#include "Grafo.h"
#include <iostream>

using namespace std;

int main()
{
    Grafo campus;
    cout << "Cargando grafo..." << endl;
    if(campus.cargarGrafo("campus_graph.json"))
    {
        cout << "Grafo cargado correctamente." << endl;
        campus.mostrarConexiones(1); // Muestra las conexiones del nodo con ID 0
    }
    return 0;
}