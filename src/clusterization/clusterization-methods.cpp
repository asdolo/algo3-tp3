#include <vector>
#include <queue>
#include "clusterization-methods.hpp"
#include "../tsplib-helper/instance.hpp"

// Devuelve una tupla donde el primer item es el vector de clusters y el segundo item es la cantidad de clusters
uint SweepClusterization(TSPLibInstance &tspInstance, std::priority_queue<NodeInPolarCoords> &nodesInPolarCoords, std::vector<uint> &clusters)
{
    // Vector donde se indica, para cada id de nodo, a qué cluster pertenece
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