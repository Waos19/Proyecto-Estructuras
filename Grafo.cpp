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

vector<double> Grafo::caminoCorto(int origen, vector<int> &predecesores)
{
    const double INFINITO = 1e9;
    int n = nodos.size();
    vector<double> distancias(n, INFINITO);
    predecesores.assign(n, -1);
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

void Grafo::Dibujar(sf::RenderWindow &ventana, const std::vector<int>& camino)
{
    // Carga una fuente para mostrar texto
    sf::Font fuente;
    if (!fuente.loadFromFile("ARIAL.ttf"))
    {
        std::cerr << "No se pudo cargar la fuente arial.ttf" << std::endl;
    }

    // Encontrar los valores mínimos y máximos para normalizar
    double minX = nodos[0].x, maxX = nodos[0].x;
    double minY = nodos[0].y, maxY = nodos[0].y;
    
    for (const auto& nodo : nodos)
    {
        if (nodo.x < minX) minX = nodo.x;
        if (nodo.x > maxX) maxX = nodo.x;
        if (nodo.y < minY) minY = nodo.y;
        if (nodo.y > maxY) maxY = nodo.y;
    }

    // Añadir un pequeño margen
    double margin = 0.001;
    minX -= margin;
    maxX += margin;
    minY -= margin;
    maxY += margin;

    // Dimensiones de la ventana
    float ventanaWidth = ventana.getSize().x;
    float ventanaHeight = ventana.getSize().y;

    // Función para mapear coordenadas geográficas a coordenadas de pantalla
    auto mapToScreen = [&](double x, double y) {
        float screenX = static_cast<float>((x - minX) / (maxX - minX) * (ventanaWidth - 50) + 25);
        float screenY = static_cast<float>(ventanaHeight - ((y - minY) / (maxY - minY) * (ventanaHeight - 50) + 25));
        return std::make_pair(screenX, screenY);
    };

    // Dibuja aristas
    for (size_t i = 0; i < nodos.size(); ++i)
    {
        auto [x1, y1] = mapToScreen(nodos[i].x, nodos[i].y);

        for (const auto &vecino : adyacencias[i])
        {
            int j = vecino.first;
            if (j > i) // Para dibujar cada arista solo una vez
            {
                auto [x2, y2] = mapToScreen(nodos[j].x, nodos[j].y);

                sf::Color color = sf::Color::White;

                // Comprobar si esta arista está en el camino más corto
                if (camino.size() >= 2)
                {
                    for (size_t k = 0; k < camino.size() - 1; ++k)
                    {
                        if ((camino[k] == i && camino[k + 1] == j) || 
                            (camino[k] == j && camino[k + 1] == i))
                        {
                            color = sf::Color::Red;
                            break;
                        }
                    }
                }

                sf::Vertex linea[] = {
                    sf::Vertex(sf::Vector2f(x1, y1), color),
                    sf::Vertex(sf::Vector2f(x2, y2), color)
                };
                ventana.draw(linea, 2, sf::Lines);
            }
        }
    }

    // Dibuja nodos
    for (const auto &nodo : nodos)
    {
        const float RADIO = 5.0f;
        auto [x, y] = mapToScreen(nodo.x, nodo.y);
        
        sf::CircleShape nodoForma(RADIO);
        nodoForma.setFillColor(sf::Color::Blue);
        nodoForma.setPosition(x - RADIO, y - RADIO);
        ventana.draw(nodoForma);

        // Dibuja el número del nodo
        if (fuente.getInfo().family != "")
        {
            sf::Text texto;
            texto.setFont(fuente);
            texto.setString(std::to_string(nodo.id));
            texto.setCharacterSize(12);
            texto.setFillColor(sf::Color::White);
            texto.setPosition(x + RADIO * 2, y - RADIO);
            ventana.draw(texto);
        }
    }
}