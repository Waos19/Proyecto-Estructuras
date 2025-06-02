#ifndef NODO_H
#define NODO_H
#include <iostream>
using namespace std;

class Nodo
{
    public:
        string id;
        double latitud, longitud;
        string nombre, amenity, type;
        Nodo(string id, double x, double y, string nombre, string amenity, string type);
        string getId() const;
        double getLatitud() const;
        double getLongitud() const;
        string getNombre() const;
        string getAmenity() const;
        string getType() const;
        void setLatitud(double lat);
        void setLongitud(double lon);
        void setNombre(const string& nombre);
        void setAmenity(const string& amenity);
        void setType(const string& type);
        

};


#endif