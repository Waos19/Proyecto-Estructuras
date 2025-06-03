#include "Grafo.h"

int main()
{
    Grafo grafo;
    cout << "Cargando grafo desde temp/entrada.txt...\n";
    grafo.cargarGrafo("temp/entrada.txt");
    cout << "Grafo cargado correctamente.\n";

    // Leer origen y destino desde parametros.txt
    cout << "Leyendo parametros desde temp/parametros.txt...\n";
    std::ifstream archivoParametros("temp/parametros.txt");
    if (!archivoParametros.is_open()) {
        std::cerr << "❌ No se pudo abrir parametros.txt\n";
        return 1;
    }

    long long origen, destino;
    archivoParametros >> origen >> destino;
    archivoParametros.close();

    if (!grafo.nodos.count(origen) || !grafo.nodos.count(destino)) {
        std::cerr << "❌ Nodo origen o destino no existe en el grafo.\n";
        return 1;
    }

    // Mostrar vecinos
    cout << "Vecinos de " << origen << ": ";
    for (const auto& [v, _] : grafo.nodos[origen].vecinos)
        cout << v << " ";
    cout << "\n";

    cout << "Vecinos de " << destino << ": ";
    for (const auto& [v, _] : grafo.nodos[destino].vecinos)
        cout << v << " ";
    cout << "\n";

    // Calcular ruta
    cout << "Calculando ruta desde " << origen << " hasta " << destino << "...\n";
    std::vector<long long> ruta = grafo.dijkstra(origen, destino);

    if (ruta.empty()) {
        cout << "❌ No se encontró una ruta entre " << origen << " y " << destino << ".\n";
    } else {
        cout << "✅ Ruta encontrada:\n";
        double peso_total = 0.0;
        for (size_t i = 0; i < ruta.size(); ++i) {
            cout << ruta[i];
            if (i < ruta.size() - 1) {
                cout << " -> ";
                long long u = ruta[i];
                long long v = ruta[i + 1];
                peso_total += grafo.nodos[u].vecinos[v];
            }
        }
        cout << "\n🔢 Peso total: " << peso_total << "\n";
    }

    // ✅ Guardar la ruta correctamente como long long
    std::ofstream archivoSalida("temp/salida.txt");
    if (!archivoSalida.is_open()) {
        std::cerr << "❌ No se pudo abrir salida.txt\n";
        return 1;
    }

    for (long long nodo : ruta)
        archivoSalida << nodo << '\n';

    archivoSalida.close();

    return 0;
}
