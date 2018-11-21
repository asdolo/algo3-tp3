#include <vector>
#include <queue>

#include "clusterization-methods.hpp"
#include "../tsplib-helper/instance.hpp"
#include "../aux.hpp"

/*
    Función que clusteriza un grafo que representa un CVRP usando el algoritmo de Sweep, que consiste en agrupar a los nodos según
sus coordenadas polares respecto del nodo depósito.
    Devuelve la cantidad de clusters resultantes y el vector clusters que indica, para cada id de nodo, a qué clúster pertenece.
*/
uint SweepClusterization(TSPLibInstance &tspInstance, std::priority_queue<NodeInPolarCoords> &nodesInPolarCoords, std::vector<uint> &clusters)
{
    // Vector donde se indica, para cada id de nodo, a qué clúster pertenece
    clusters = std::vector<uint>(tspInstance.dimension, 0);
    uint cantClusters = 1;

    uint capacidadRestanteClusterActual = tspInstance.capacity;
    do
    {
        // Miramos el primer nodo del heap (aquel con ángulo theta menor)
        NodeInPolarCoords nodo = nodesInPolarCoords.top();
        if (nodo.m_demand > capacidadRestanteClusterActual)
        {
            // Si la demanda de este nodo es mayor que la capacidad restante para este clúster,
            // aumentamos la cantidad de clusters
            cantClusters++;
            capacidadRestanteClusterActual = tspInstance.capacity;
        }

        clusters[nodo.m_id] = cantClusters;
        capacidadRestanteClusterActual -= nodo.m_demand;

        // Sacamos este nodo del heap
        nodesInPolarCoords.pop();
    } while (nodesInPolarCoords.size() > 0);

    return cantClusters;
}

/*
    Función que clusteriza un grafo que representa un CVRP usando el algortimo de clusterización del TP 2.
    Esta vez, los parámetros del algoritmo serán fijos, ya que en este caso no nos interesa estudiar la clusterización en sí,
sino que simplemente diremos que corresponde a una heurística.
    Devuelve la cantidad de clusters resultantes y el vector clusters que indica, para cada id de nodo, a qué clúster pertenece.
*/
uint TP2Clusterization(TSPLibInstance &tspInstance, std::vector<uint> &clusters)
{
    uint vecindad = 2;
    uint version = 1;
    double excesoNecesarioDesvioEstandar = 3;
    double ratioExceso = 2;

    // Vector donde se indica, para cada id de nodo, a qué clúster pertenece
    clusters = std::vector<uint>(tspInstance.dimension, 0);

    // Ejecutamos prim, que nos devuelve una matriz de adyacencia que representa un AGM
    // del grafo que le pasamos.
    std::vector<std::vector<double>> agm;
    agm = prim(tspInstance.getTSPGraph(false));

    std::vector<std::tuple<int, int>> ejesInconsistentes;
    std::vector<std::tuple<int, int, int>> listaDeAristas = obtenerListaDeAristas(agm); //O(N^2)
    for (uint i = 0; i < listaDeAristas.size(); i++)                                    //O(N^3)
    {
        if (isConsistentEdge(agm, std::get<0>(listaDeAristas[i]), std::get<1>(listaDeAristas[i]), vecindad, version, excesoNecesarioDesvioEstandar, ratioExceso))
        {
            std::tuple<int, int> inconsistente;
            std::get<0>(inconsistente) = std::get<0>(listaDeAristas[i]);
            std::get<1>(inconsistente) = std::get<1>(listaDeAristas[i]);
            ejesInconsistentes.push_back(inconsistente);
        }
    }

    //Saco los ejes inconsistentes del agm. Quedan x componentes conexas
    for (uint i = 0; i < ejesInconsistentes.size(); i++) //O(N) caso que todos los ejes sean inconsistentes, es un arbol.
    {
        agm[std::get<0>(ejesInconsistentes[i])][std::get<1>(ejesInconsistentes[i])] = -1;
        agm[std::get<1>(ejesInconsistentes[i])][std::get<0>(ejesInconsistentes[i])] = -1;
    }

    //agm solamente tiene n componentes conexas, donde cada componente conexa es un cluster
    //Para obtener que punto pertenece a cada cluster veo los vecinos
    std::vector<bool> visitados(agm.size(), false);
    uint cantClusters = 1;
    uint capacidadRestanteClusterActual = tspInstance.capacity;

    for (uint i = 1; i < visitados.size(); i++) //O(N^3) (empieza en 1 así no pone al depósito como cluster)
    {
        if (visitados[i] == false)
        {
            visitados[i] = true;
            clusters[i] = cantClusters;
            capacidadRestanteClusterActual -= tspInstance.demand[i];

            definirClusterTP2(tspInstance, agm, clusters, visitados, i, cantClusters, capacidadRestanteClusterActual); //O(N^2)
            cantClusters++;
        }
    }

    return cantClusters - 1;
}