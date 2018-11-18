#include <vector>
#include <iostream>
#include <math.h>
#include <tuple>
#include <fstream>
#include <string>
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
void printRutaSolucion(route r, std::ofstream & archivoRutas)
{
    std::vector<uint> ruta = r.ruta;

    for (uint j = 0; j < ruta.size(); j++)
    {
        archivoRutas << ruta[j] + 1;
        std::cout << ruta[j] + 1;
        if (j != ruta.size() - 1)
        {
            archivoRutas << " ";
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void imprimirSolucionTP(std::vector<std::vector<double>> matriz, std::vector<route> routes,std::string rutas)
{   
    std::ofstream archivoRutas;
    archivoRutas.open(rutas, std::ios::out | std::ios::trunc);
    archivoRutas << "camion,ruta,distancia" << std::endl;

    std::cout << routes.size() << std::endl;

    for (uint i = 0; i < routes.size(); i++)
    {
        archivoRutas << i+1 << ",";
        printRutaSolucion(routes[i],archivoRutas);
        archivoRutas << "," << routes[i].distancia << std::endl;
    }
    std::cout << calcularCosto(matriz, routes) << std::endl;
}

route obtenerMinimaRuta(std::vector<route> rutas){
    double minimaDistancia = rutas[0].distancia;
    uint indiceRutaMinima = 0;
    
    for(uint i = 1; i < rutas.size(); i++)
    {
        if(rutas[i].distancia < minimaDistancia){
            minimaDistancia= rutas[i].distancia;
            indiceRutaMinima = i;
        }
    }
    return rutas[indiceRutaMinima];
}
route obtenerRutasCombinadas(std::vector<std::vector<double>> matriz,route a,route b){
    //Calculo la nueva capacidad de la ruta
    double capacidad = a.capacityRoute+b.capacityRoute;
    //Copio la ruta A
    std::vector<uint> rutaA = a.ruta;
    // Obtengo la distancia de la ruta A , quitandole la ultima arista al deposito.
    double distanciaA = a.distancia - matriz[rutaA[rutaA.size()-2]][rutaA[rutaA.size()-1]];
    rutaA.erase(rutaA.begin()+rutaA.size()-1);
    // Hago la combinacion generando la ruta : deposito-rutaA(sin deposito)-rutaB(sin deposito) - deposito
    for(uint i =1 ; i < b.ruta.size();i++){
        rutaA.push_back(b.ruta[i]);
        distanciaA+= matriz[rutaA[rutaA.size()-2]][rutaA[rutaA.size()-1]];
    }
    route res(a.indiceRuta,rutaA,capacidad,distanciaA);
    return res;
}

// Devuelve la mejor combinacion de las rutas a y b , mejor con respecto a menor distancia.
// Complejidad: O(N)
route obtenerLaMejorCombinacionDeRutas(std::vector<std::vector<double>>& matriz,route a, route b){
    std::vector<uint> rutaA = a.ruta;
    std::vector<uint> rutaB = b.ruta;
    std::reverse(rutaA.begin(),rutaA.end());
    std::reverse(rutaB.begin(),rutaB.end());
    route reversaA(a.indiceRuta,rutaA,a.capacityRoute,a.distancia);
    route reversaB(b.indiceRuta,rutaB,b.capacityRoute,b.distancia);
    std::vector<route> rutasPosibles;

    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,a,b));
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,b,a));

    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,reversaA,b));
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,b,reversaA));
    
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,a,reversaB));
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,reversaB,a));

    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,reversaA,reversaB));
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,reversaB,reversaA));
    // Devuelvo la ruta combinada que menos distancia recorre.
    return obtenerMinimaRuta(rutasPosibles);
}
//-----------------------------------------------------------------------------------
//Ej 3 y Ej 4

// A utility function to find the vertex with
// minimum key value, from the set of vertices
// not yet included in MST
int minKey(std::vector<double> key, std::vector<bool> mstSet)
{
    uint n = key.size();

    // Initialize min value
    int min = INT_MAX; // +inf
    int min_index;

    for (uint v = 0; v < n; v++)
    {
        if (mstSet[v] == false && key[v] < min)
            min = key[v], min_index = v;
    }

    return min_index;
}

std::vector<std::vector<double>> prim(std::vector<std::vector<double>> E)
{
    uint n = E.size();

    // Inicializo la matriz de adyacencia del AGM en +infinito (-1)
    std::vector<std::vector<double>> res(n);

    for (uint i = 0; i < n; i++)
    {
        res[i] = std::vector<double>(n);
        for (uint j = 0; j < n; j++)
        {
            res[i][j] = -1;
        }
    }

    // Array to store constructed MST
    std::vector<int> parent(n);
    // Key values used to pick minimum weight edge in cut
    std::vector<double> key(n);
    // To represent set of vertices not yet included in MST
    std::vector<bool> mstSet(n);

    // Initialize all keys as INFINITE
    for (uint i = 0; i < n; i++)
    {
        key[i] = INT_MAX;
        mstSet[i] = false;
    }

    // Always include first 1st vertex in MST.
    // Make key 0 so that this vertex is picked as first vertex.
    key[1] = 0;
    parent[1] = -1; // First node is always root of MST

    // The MST will have V vertices
    for (uint count = 0; count < n - 1; count++)
    {
        // Pick the minimum key vertex from the
        // set of vertices not yet included in MST
        int u = minKey(key, mstSet);

        // Add the picked vertex to the MST Set
        mstSet[u] = true;

        // Update key value and parent index of
        // the adjacent vertices of the picked vertex.
        // Consider only those vertices which are not
        // yet included in MST
        for (uint v = 0; v < n; v++)
        {
            // graph[u][v] is non zero only for adjacent vertices of m
            // mstSet[v] is false for vertices not yet included in MST
            // Update the key only if graph[u][v] is smaller than key[v]
            if (E[u][v] && mstSet[v] == false && E[u][v] < key[v])
            {
                parent[v] = u, key[v] = E[u][v];
            }
        }
    }

    for (uint i = 0; i < parent.size(); i++)
    {
        int nodoPadre = parent[i];

        if (nodoPadre != -1)
        {
            res[i][nodoPadre] = E[i][nodoPadre];
            res[nodoPadre][i] = E[nodoPadre][i];
        }
    }

    return res;
}

std::vector<std::tuple<int, int, int>> obtenerListaDeAristas(std::vector<std::vector<double>> E)
{
    std::vector<std::tuple<int, int, int>> res;
    for (uint i = 0; i < E[0].size(); i++) //O(N^2)
    {
        for (uint j = i + 1; j < E[0].size(); j++)
        {
            if (E[i][j] != -1 && i != j)
            {
                std::tuple<int, int, int> t{i, j, E[i][j]};
                res.push_back(t);
            }
        }
    }
    return res;
}

void definirClusterTP2(std::vector<std::vector<double>> grafo, std::vector<uint> &result, std::vector<bool> &visitados, uint i, int nroCluster)
{
    for (uint j = 0; j < grafo.size(); j++)
    {
        if (i != j && grafo[i][j] != -1 && visitados[j] == false)
        {
            visitados[j] = true;
            result[j] = nroCluster;
            definirClusterTP2(grafo, result, visitados, j, nroCluster);
        }
    }
}

bool isConsistentEdge(std::vector<std::vector<double>> &agm, int nodoA, int nodoB, int vecindad, int version, double excesoNecesarioDesvioEstandar, double ratioExceso)
{
    std::vector<double> distanciasVecinosNodoA(0);
    double sumatoriaNodoA = 0;
    obtenerVecinosN(agm, nodoA, nodoB, vecindad, distanciasVecinosNodoA, sumatoriaNodoA); //O(N^2)
    std::vector<double> distanciasVecinosNodoB(0);
    double sumatoriaNodoB = 0;
    obtenerVecinosN(agm, nodoB, nodoA, vecindad, distanciasVecinosNodoB, sumatoriaNodoB); //O(N^2)
    double promedioA = (distanciasVecinosNodoA.size() > 0 ? sumatoriaNodoA / distanciasVecinosNodoA.size() : 0);
    double promedioB = (distanciasVecinosNodoB.size() > 0 ? sumatoriaNodoB / distanciasVecinosNodoB.size() : 0);
    double desvioEstandarA = standardDeviation(distanciasVecinosNodoA, distanciasVecinosNodoA.size()); //O(N)
    double desvioEstandarB = standardDeviation(distanciasVecinosNodoB, distanciasVecinosNodoB.size()); //O(N)
    double distanciaEntreAyB = agm[nodoA][nodoB];
    double excesoDeInconsistenciaA = (desvioEstandarA != 0 ? distanciaEntreAyB / desvioEstandarA : 0);
    double excesoDeInconsistenciaB = (desvioEstandarB != 0 ? distanciaEntreAyB / desvioEstandarB : 0);
    double ratioA = distanciaEntreAyB / promedioA;
    double ratioB = distanciaEntreAyB / promedioB;
    if (version == 1)
        return (excesoDeInconsistenciaA > excesoNecesarioDesvioEstandar && excesoDeInconsistenciaB > excesoNecesarioDesvioEstandar);
    else if (version == 2)
        return (ratioA > ratioExceso && ratioB > ratioExceso);

    return false;
}

void obtenerVecinosN(std::vector<std::vector<double>> &agm, uint nodoA, uint nodoB, uint n, std::vector<double> &res, double &sumatoria)
{
    if (n == 0)
    {
        return;
    }
    for (uint i = 0; i < agm[0].size(); i++)
    {
        if (agm[nodoA][i] != -1 && i != nodoB && nodoA != i)
        {
            res.push_back(agm[nodoA][i]);
            sumatoria += agm[nodoA][i];
            obtenerVecinosN(agm, i, nodoA, n - 1, res, sumatoria);
        }
    }
}

double standardDeviation(std::vector<double> arr, int n)
{
    return sqrt(variance(arr, n));
}

double variance(std::vector<double> &a, int n)
{
    // Compute mean (average of elements)
    double sum = 0;
    for (int i = 0; i < n; i++)
        sum += a[i];
    double mean = (double)sum /
                  (double)n;

    // Compute sum squared
    // differences with mean.
    double sqDiff = 0;
    for (int i = 0; i < n; i++)
        sqDiff += (a[i] - mean) *
                  (a[i] - mean);
    return sqDiff / n;
}