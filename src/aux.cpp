#include <vector>
#include <iostream>
#include "aux.hpp"

//General
void printMatriz(std::vector<std::vector<double>> matriz, std::ostream &stream)
{
    for (uint i = 0; i < matriz.size(); i++)
    {
        for (uint j = 0; j < matriz.size(); j++)
        {
            stream << matriz[i][j] << "\t\t";
        }
        stream << std::endl;
    }
}
//-----------------------------------------------------------------------------------
//Ej 1 y Ej 2
double calcularCosto(std::vector<std::vector<double>> matriz, std::vector<route> routes)
{
    double res = 0;
    for (uint i = 0; i < routes.size(); i++)
    {
        std::vector<uint> ruta = routes[i].ruta;
        for (uint j = 0; j < ruta.size() - 1; j++)
        {
            res += matriz[ruta[j]][ruta[j + 1]];
        }
    }
    return res;
}
void printRoutes(std::vector<route> &routes)
{
    for (uint i = 0; i < routes.size(); i++)
    {
        printRoute(routes[i]);
    }
}
void printRoute(route r)
{
    std::cout << "Ruta " << r.indiceRuta << ":" << std::endl;
    std::cout << "   ruta= ";
    std::vector<uint> ruta = r.ruta;

    for (uint j = 0; j < ruta.size(); j++)
    {
        std::cout << ruta[j] + 1;
        if (j != ruta.size() - 1)
        {
            std::cout << ",";
        }
    }
    std::cout << std::endl;
    std::cout << "   volumen necesario de la ruta= " << r.capacityRoute << std::endl;
    std::cout << std::endl;
}
void printRutaSolucion(route r)
{
    std::vector<uint> ruta = r.ruta;

    for (uint j = 0; j < ruta.size(); j++)
    {
        std::cout << ruta[j] + 1;
        if (j != ruta.size() - 1)
        {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void imprimirSolucionTP(std::vector<std::vector<double>> matriz, std::vector<route> routes)
{
    std::cout << routes.size() << std::endl;

    for (uint i = 0; i < routes.size(); i++)
    {
        printRutaSolucion(routes[i]);
    }
    std::cout << calcularCosto(matriz, routes) << std::endl;
}

//-----------------------------------------------------------------------------------
//Ej 3 y Ej 4