#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <map>
#include "aux.hpp"
#include <limits>
#include <algorithm>
#include <string>
#include <fstream>
#include <tuple>
#include <chrono>
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
void ejecutarGolosa(std::vector<std::vector<double>>& matrizDeAdyacencia,uint indiceDeposito,uint capacityTruck,std::vector<route>& routes,std::vector<uint>& demanda){
    // Construyo la solución inicial.
    // La solucion inicial consistira en ir yendo al cliente mas cercano mientras tenga capacidad.
    routes = createRoutes(matrizDeAdyacencia, demanda, indiceDeposito, capacityTruck);
}
int main(int argc, char *argv[])
{
    // Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(std::cin);

    std::string stringRutas = argc >= 2 ? argv[1] : "output/2-golosa/rutas.csv";
    std::string stringTabla = argc >= 3 ? argv[2] : "output/2-golosa/tabla.csv";
    std::string stringTiempo = argc >= 4 ? argv[3] : "output/2-golosa/tiempo.csv";
    int cantidadRepeticiones = argc >= 5 ? std::stoi(argv[4]) : 1;
    if(argc<2)system("mkdir -p output/2-golosa/");
    std::ofstream archivoTabla;
    std::ofstream archivoTiempo;
    archivoTabla.open(stringTabla, std::ios::out | std::ios::trunc);
    archivoTiempo.open(stringTiempo, std::ios_base::app);

    //Imprimo entrada en CSV para experimentos
    archivoTabla << "id,x,y" << std::endl;
    for (uint i = 0; i < tspInstance.dimension; i++)
    {
        uint id = i + 1;
        double x = std::get<0>(tspInstance.nodeCoords[i]);
        double y = std::get<1>(tspInstance.nodeCoords[i]);

        archivoTabla << id << "," << x << "," << y << std::endl;
    }
    archivoTabla.close();

    // Obtengo el grafo que modela la instancia
    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();

    // Obtengo el id del deposito
    uint indiceDeposito = tspInstance.depot[0];

    // Obtengo la capacidad del camión
    uint capacityTruck = tspInstance.capacity;

    // Obtengo la demanda de los clientes 
    std::vector<uint> demanda = tspInstance.demand;

    std::vector<route> routes;
    // Ejecuto el algoritmo
    auto startTime = std::chrono::steady_clock::now();
    ejecutarGolosa(matrizDeAdyacencia,indiceDeposito,capacityTruck,routes,demanda);
    auto endTime = std::chrono::steady_clock::now();
    //Calculo cuanto tiempo costo.
    auto diff = endTime - startTime;
    uint repeticiones = cantidadRepeticiones-1;
    while(repeticiones>0){
        // Ejecuto el algoritmo
        startTime = std::chrono::steady_clock::now();
        ejecutarGolosa(matrizDeAdyacencia,indiceDeposito,capacityTruck,routes,demanda);
        endTime = std::chrono::steady_clock::now();
        
        //Calculo cuanto tiempo costo.
        diff += (endTime - startTime);
        repeticiones--;
    }
    diff/=cantidadRepeticiones;

    //Imprimo los tiempos del algoritmo
    archivoTiempo << tspInstance.dimension;
    archivoTiempo << ",";
    archivoTiempo << std::chrono::duration <double, std::milli>(diff).count();
    archivoTiempo << ",";
    archivoTiempo << calcularCosto(matrizDeAdyacencia, routes);
    archivoTiempo << std::endl;
    archivoTiempo.close();
    
    imprimirSolucionTP(matrizDeAdyacencia, routes,stringRutas);

    return 0;
}