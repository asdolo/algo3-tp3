#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include "tsplib-helper/instance.hpp"
#include <string>
#include <fstream>
#include "aux.hpp"
#include "savings/savings.hpp"


int main(int argc, char *argv[])
{
    // Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(std::cin);

    std::string stringRutas = argc >= 2 ? argv[1] : "output/1-savings/rutas.csv";
    std::string stringTabla = argc >= 3 ? argv[2] : "output/1-savings/tabla.csv";
    std::string stringTiempo = argc >= 4 ? argv[3] : "output/1-savings/tiempo.csv";
    int cantidadRepeticiones = argc >= 5 ? std::stoi(argv[4]) : 1;
    if(argc<2)system("mkdir -p output/1-savings/");
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
    ejecutarSavings(matrizDeAdyacencia,indiceDeposito,capacityTruck,routes,demanda);
    auto endTime = std::chrono::steady_clock::now();

    //Calculo cuanto tiempo costo.
    auto diff = endTime - startTime;

    uint repeticiones = cantidadRepeticiones-1;
    while(repeticiones>0){
        // Ejecuto el algoritmo
        startTime = std::chrono::steady_clock::now();
        ejecutarSavings(matrizDeAdyacencia,indiceDeposito,capacityTruck,routes,demanda);
        endTime = std::chrono::steady_clock::now();
        
        //Calculo cuanto tiempo costo.
        diff += (endTime - startTime);
        repeticiones--;
    }
    diff/=cantidadRepeticiones;
    // Borro las rutas que quedaron vacias
    for (uint i = 0; i < routes.size(); i++)
    {
        if (routes[i].ruta.size() == 0)
        {
            routes.erase(routes.begin() + i);
            i--;
        }
    }
    //Imprimo los tiempos del algoritmo
    archivoTiempo << tspInstance.dimension;
    archivoTiempo << ",";
    archivoTiempo << std::chrono::duration <double, std::milli>(diff).count();
    archivoTiempo << ",";
    archivoTiempo << calcularCosto(matrizDeAdyacencia, routes);
    archivoTiempo << std::endl;
    archivoTiempo.close();

    //Imprimo la solucion requerida por el enunciado
    imprimirSolucionTP(matrizDeAdyacencia, routes,stringRutas);

    return 0;
}