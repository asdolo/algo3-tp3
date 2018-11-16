#include <vector>
#include <iostream>
#include <math.h>
#include <fstream>
#include <queue>
#include <chrono>

#include "tsplib-helper/instance.hpp"
#include "tsp-solvers/tsp-solvers.hpp"
#include "clusterization/clusterization-methods.hpp"
#include "aux.hpp"

#define _USE_MATH_DEFINES

void ejecutarOtraClusterizacion(TSPLibInstance& tspInstance,std::vector<uint>& clusters,uint& cantClusters,std::vector<std::vector<uint>>& rutasVehiculos){

     // Obtengo el grafo que modela la instancia
    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();
    
    // Obtengo el id del nodo que es depósito
    uint depot = tspInstance.depot[0];

    // Sección cluster-first:

    // Ejecuto el Algoritmo de clusterización del TP 2 para obtener los clústers
    cantClusters = TP2Clusterization(tspInstance, clusters);
    clusters[depot] = 0;

    // Sección route-second:

    // Para cada cluster, corro un algoritmo heurístico de TSP
    // En este caso, usaremos Nearest Neighbor
    for (uint nroCluster = 1; nroCluster <= cantClusters; nroCluster++)
    {
        // Obtengo todos los nodos del cluster actual
        std::vector<uint> nodosDelCluster(0);
        for (uint j = 0; j < tspInstance.dimension; j++)
        {
            if (clusters[j] == nroCluster)
                nodosDelCluster.push_back(j);
        }

        std::vector<uint> ruta = PrimerSolucionFactibleEnCluster(depot, nodosDelCluster);
        rutasVehiculos.push_back(ruta);
    }
}


double costo(uint cantClusters,std::vector<std::vector<uint>> rutasVehiculos,std::vector<std::vector<double>> matrizDeAdyacencia){
    double costoTotal = 0;
    for (uint i = 0; i < cantClusters; i++)
    {
        std::vector<uint> ruta = rutasVehiculos[i];
        for (uint j = 0; j < ruta.size() - 1; j++)
        {
            costoTotal += matrizDeAdyacencia[ruta[j]][ruta[j + 1]];
        }
    }
    return costoTotal;
}

/*
 Argumentos:
    1. General del TP:
        Archivo .csv donde se va a guardar, para cada vehículo k, la ruta que recorre (para dibujarla) y su costo.
    2. Especifico de este ejercicio:
        Archivo .csv donde se va a guardar, para cada punto, sus coordenadas cartesianas y a qué clúster pertenece.
    
*/
int main(int argc, char *argv[])
{
    std::ofstream archivoRutas;
    std::ofstream archivoTablaClusters;
    std::ofstream archivoTiempo;
    if(argc<2)system("mkdir -p output/4-otra-clusterizacion/");
    archivoRutas.open(argc >= 2 ? argv[1] : "output/4-otra-clusterizacion/rutas.csv", std::ios::out | std::ios::trunc);
    archivoTablaClusters.open(argc >= 3 ? argv[2] : "output/4-otra-clusterizacion/clusters.csv", std::ios::out | std::ios::trunc);
    archivoTiempo.open(argc >= 4 ? argv[3] : "output/3-sweep-algorithm/tiempo.csv", std::ios_base::app);
    int cantidadRepeticiones = argc >= 5 ? std::stoi(argv[4]) : 1;

    // Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(std::cin);

    // Obtengo el grafo que modela la instancia
    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();

    std::vector<uint> clusters;
    uint cantClusters;
    std::vector<std::vector<uint>> rutasVehiculos(0);
    auto startTime = std::chrono::steady_clock::now();
    ejecutarOtraClusterizacion(tspInstance,clusters,cantClusters,rutasVehiculos);
    auto endTime = std::chrono::steady_clock::now();

    //Calculo cuanto tiempo costo.
    auto diff = endTime - startTime;
    uint repeticiones = cantidadRepeticiones-1;
    while(repeticiones>0){
        rutasVehiculos = std::vector<std::vector<uint>>(0);
        // Ejecuto el algoritmo
        startTime = std::chrono::steady_clock::now();
        ejecutarOtraClusterizacion(tspInstance,clusters,cantClusters,rutasVehiculos);
        endTime = std::chrono::steady_clock::now();
        
        //Calculo cuanto tiempo costo.
        diff += (endTime - startTime);
        repeticiones--;
    }
    diff/=cantidadRepeticiones;

    //Calculamos el costo total
    double costoTotal = costo(cantClusters,rutasVehiculos,matrizDeAdyacencia);

    //Imprimo los tiempos del algoritmo
    archivoTiempo << tspInstance.dimension;
    archivoTiempo << ",";
    archivoTiempo << std::chrono::duration <double, std::milli>(diff).count();
    archivoTiempo << ",";
    archivoTiempo << costoTotal;
    archivoTiempo << std::endl;
    archivoTiempo.close();

    // Guardamos los clusters en el archivo .csv correspondiente
    archivoTablaClusters << "id,x,y,cluster" << std::endl;
    for (uint i = 0; i < tspInstance.dimension; i++)
    {
        uint id = i + 1;
        double x = std::get<0>(tspInstance.nodeCoords[i]);
        double y = std::get<1>(tspInstance.nodeCoords[i]);

        uint cluster = clusters[i];

        archivoTablaClusters << id << "," << x << "," << y << "," << cluster << std::endl;
    }
    archivoTablaClusters.close();

    

    // Imprimimos la cantidad de vehículos utilizados, que coincide con la cantidad de clústers
    // (tanto en la salida estándar como en el archivo csv correspondiente)
    std::cout << cantClusters << std::endl;
    archivoRutas << "camion,ruta,distancia" << std::endl;

    // Imprimimos cada ruta de cada vehículo
    for (uint i = 0; i < cantClusters; i++)
    {
        std::vector<uint> ruta = rutasVehiculos[i];
        double costoRuta = 0;

        archivoRutas << i + 1 << ",";

        for (uint j = 0; j < ruta.size() - 1; j++)
        {
            costoRuta += matrizDeAdyacencia[ruta[j]][ruta[j + 1]];

            std::cout << ruta[j] + 1 << " ";
            archivoRutas << ruta[j] + 1 << " ";
        }

        std::cout << ruta[ruta.size() - 1] + 1 << std::endl;
        archivoRutas << ruta[ruta.size() - 1] + 1 << "," << costoRuta << std::endl;
    }

    // Imprimimos el costo total de las rutas
    std::cout << costoTotal << std::endl;

    return 0;
}