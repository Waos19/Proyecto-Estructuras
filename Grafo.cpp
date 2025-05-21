#include "Grafo.h"

vector<Nodo> Grafo::getNodos() const
{
    return this->nodos;
}

bool Grafo::cargarGrafo(const string &nombreArchivo)
{
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open())
    {
        cerr << "Error al abrir el archivo: " << nombreArchivo << endl;
        return false;
    }

    json j;
    archivo >> j;

    cout << "Leyendo nodos..." << endl;
    for (const auto& nodo : j["nodes"])
    {
        int index = nodo["index"];
        double x = nodo["x"];
        double y = nodo["y"];
        nodos.emplace_back(index, x, y);
    }
    cout << "Total nodos: " << nodos.size() << endl;

    adyacencias.resize(nodos.size());


    cout << "Leyendo aristas..." << endl;
    for (const auto& arista : j["edges"])
    {
        int origen = arista["from"];
        int destino = arista["to"];
        double peso = arista["weight"];

        if (origen < adyacencias.size() && destino < adyacencias.size())
        {
            adyacencias[origen].emplace_back(destino, peso);
            adyacencias[destino].emplace_back(origen, peso);
        }


    }

    return true;
}

void Grafo::mostrarConexiones(int idNodo)
{
    if (idNodo < 0 || idNodo >= nodos.size() || idNodo >= adyacencias.size())
    {
        cerr << "ID de nodo inválido: " << idNodo << endl;
        return;
    }

    cout << "Conexiones del nodo " << nodos[idNodo].id << " (" << nodos[idNodo].x << ", " << nodos[idNodo].y << "):" << endl;

    for (const auto& conexion : adyacencias[idNodo])
    {
        int idVecino = conexion.first;
        double peso = conexion.second;

        cout << " → Nodo " << idVecino;
        if (idVecino >= 0 && idVecino < nodos.size())
        {
            cout << " (" << nodos[idVecino].x << ", " << nodos[idVecino].y << ")";
        }
        cout << " con peso " << peso << endl;
    }
}

vector<double> Grafo::caminoCorto(int origen, vector<int> predecesores)
{
    const double INFINITO = 1e9;
    int n = nodos.size();
    vector<double> distancias(n, INFINITO);
    predecesores.resize(n, -1);
    distancias[origen] = 0.0;

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
    pq.emplace(0.0, origen);

    while (!pq.empty())
    {
        auto [distActual, u] = pq.top();
        pq.pop();

        if (distActual > distancias[u]) continue;

        for (const auto& [v, peso] : adyacencias[u])
        {
            double nuevaDistancia = distancias[u] + peso;
            if (nuevaDistancia < distancias[v])
            {
                distancias[v] = nuevaDistancia;
                predecesores[v] = u;
                pq.emplace(nuevaDistancia, v);
            }
        }
    }

    return distancias;
}

vector<int> Grafo::reconstruirCamino(int destino, const vector<int> &predecesores)
{
    vector<int> camino;
    for (int at = destino; at != -1; at = predecesores[at])
    {
        camino.push_back(at);
    }
    reverse(camino.begin(), camino.end());
    return camino;
}
