#include "Nodo.h"

Nodo::Nodo(string id, double x, double y, string nombre, string amenity, string type) : id(id), latitud(x), longitud(y), nombre(nombre), amenity(amenity), type(type){}

string Nodo::getId() const
{
    return id;
}

double Nodo::getLatitud() const
{
    return latitud;
}

double Nodo::getLongitud() const
{
    return longitud;
}

string Nodo::getNombre() const
{
    return nombre;
}

string Nodo::getAmenity() const
{
    return amenity;
}

string Nodo::getType() const
{
    return type;
}
