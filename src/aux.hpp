#include <vector>

#ifndef AUX_HPP
#define AUX_HPP

//General
void printMatriz(std::vector<std::vector<double>> matriz, std::ostream &stream);

//-----------------------------------------------------------------------------------
//Ej 1 y Ej 2
struct route
{
    uint indiceRuta;
    std::vector<uint> ruta;
    uint capacityRoute;
    double distancia;
    route(uint i, std::vector<uint> r, uint c, double d) : indiceRuta(i), ruta(r), capacityRoute(c), distancia(d)
    {
    }
};

double calcularCosto(std::vector<std::vector<double>> matriz, std::vector<route> routes);
void printRoute(route r);
void printRoutes(std::vector<route> &routes);
void printRutaSolucion(route r);
void imprimirSolucionTP(std::vector<std::vector<double>> matriz, std::vector<route> routes);

//-----------------------------------------------------------------------------------
//Ej 3 y Ej 4

#endif