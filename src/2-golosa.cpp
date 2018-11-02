#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <map>
#include "aux.hpp"
#include <limits>
#include <algorithm>
#include "tsplib-helper/instance.hpp"

std::map<uint, uint> clienteARuta;

uint obtenerClienteMasCercano(std::vector<std::vector<double>> matriz, std::vector<uint> demand, std::vector<bool> loAgregue, uint capacidadActual, uint capacityTruck, uint clienteActual)
{
    double min = std::numeric_limits<double>::infinity();
    uint clienteMasCercano = 0;
    for (uint j = 0; j < matriz.size(); j++)
    {
        if (j != clienteActual && !loAgregue[j] && matriz[clienteActual][j] < min && demand[j] + capacidadActual <= capacityTruck)
        {
            clienteMasCercano = j;
            min = matriz[clienteActual][j];
        }
    }
    return clienteMasCercano;
}
bool todosTrue(std::vector<bool> v)
{
    for (uint i = 0; i < v.size(); i++)
    {
        if (v[i] == false)
            return false;
    }
    return true;
}
std::vector<route> createRoutes(std::vector<std::vector<double>> matriz, std::vector<uint> demand, uint indiceDeposito, uint capacityTruck)
{
    std::vector<bool> loAgregue(matriz.size(), false);
    loAgregue[indiceDeposito] = true;
    std::vector<route> res;
    uint nroRuta = 0;
    while (!todosTrue(loAgregue))
    {
        std::vector<uint> ruta;
        uint capacidadActual = 0;
        double distancia = 0;
        ruta.push_back(indiceDeposito);
        uint clienteActual = indiceDeposito;
        for (uint i = 0; i < matriz.size(); i++)
        {
            uint clienteMasCercano = obtenerClienteMasCercano(matriz, demand, loAgregue, capacidadActual, capacityTruck, clienteActual);
            if (clienteMasCercano != 0)
            {
                ruta.push_back(clienteMasCercano);
                loAgregue[clienteMasCercano] = true;
                capacidadActual += demand[clienteMasCercano];
                clienteARuta.insert(std::pair<uint, uint>(clienteMasCercano, nroRuta));
                distancia += matriz[clienteActual][clienteMasCercano];
                clienteActual = clienteMasCercano;
            }
        }
        ruta.push_back(indiceDeposito);
        res.push_back(route(nroRuta++, ruta, capacidadActual, distancia));
    }
    return res;
}

bool compareByDistance(const route &a, const route &b)
{
    return a.distancia >= b.distancia;
}
void exchangeClients(std::vector<route> &routes, uint nroRutaA, uint nroRutaB)
{
    std::vector<uint> rutaA = routes[nroRutaA].ruta;
    std::vector<uint> rutaB = routes[nroRutaB].ruta;
    std::vector<uint> nuevaRutaA;
    nuevaRutaA.push_back(rutaA[0]);
    nuevaRutaA.push_back(rutaA[1]);
    nuevaRutaA.push_back(rutaB[rutaB.size() - 2]);
    nuevaRutaA.push_back(rutaA[rutaA.size() - 1]);
    std::vector<uint> nuevaRutaB;

    for (uint i = 0; i < rutaB.size() - 2; i++)
    {
        nuevaRutaB.push_back(rutaB[i]);
    }
    nuevaRutaB.push_back(rutaA[1]);
    nuevaRutaB.push_back(rutaB[rutaB.size() - 1]);
    routes[nroRutaA].ruta = nuevaRutaA;
    routes[nroRutaB].ruta = nuevaRutaB;
}

int main(int argc, char *argv[])
{
    uint repeticionesGrasp = argc >= 2 ? std::stoi(argv[1]) : 3;
    // Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(std::cin);

    // Obtengo el grafo que modela la instancia
    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();

    // Obtengo el id del deposito
    uint indiceDeposito = tspInstance.depot[0];

    // Obtengo la capacidad del camión
    uint capacityTruck = tspInstance.capacity;

    // Construyo la solución inicial.
    // La solucion inicial consistira en ir yendo al cliente mas cercano mientras tenga capacidad.
    std::vector<route> routes = createRoutes(matrizDeAdyacencia, tspInstance.demand, indiceDeposito, capacityTruck);

    // Imprimo como quedaron las rutas iniciales.
    //printRoutes(routes);

    // Ordeno las rutas por distancia decrecientemente
    std::sort(routes.begin(), routes.end(), compareByDistance);

    // Calculo el costo total de la solucion inicial
    double costoTotal = calcularCosto(matrizDeAdyacencia, routes);

    // Recorro el grafo de soluciones utilizando GRASP
    while (repeticionesGrasp > 0)
    {
        std::vector<route> routesAux = routes;
        // Elijo dos rutas al azar
        srand(time(NULL));
        uint randomNumber1 = rand() % (routesAux.size() - 1);
        uint randomNumber2 = randomNumber1;
        while (randomNumber2 == randomNumber1)
            randomNumber2 = rand() % (routesAux.size() - 1);

        // Busco en la vecindad utilizando exchange
        exchangeClients(routesAux, randomNumber1, randomNumber2);
        double costoTotalAux = calcularCosto(matrizDeAdyacencia, routesAux);
        if (costoTotal > costoTotalAux)
        {
            routes = routesAux;
            costoTotal = costoTotalAux;
        }
        repeticionesGrasp--;
    }

    imprimirSolucionTP(matrizDeAdyacencia, routes);

    return 0;
}