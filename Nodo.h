#ifndef NODO_H
#define NODO_H

#include <iostream>
#include <unordered_map>
using namespace std;



class Nodo
{
    public:
        long long id;
        double latitud;
        double longitud;
        unordered_map<long long, double> vecinos;

        Nodo() : id(-1), latitud(0.0), longitud(0.0) {}
        Nodo(long long id, double latitud, double longitud);
        void agregarVecino(long long idVecino, double peso);
};


#endif