#include "Nodo.h"

Nodo::Nodo(long long id, double latitud, double longitud): id(id), latitud(latitud), longitud(longitud){}

void Nodo::agregarVecino(long long idVecino, double peso)
{
    vecinos[idVecino] = peso;
}
