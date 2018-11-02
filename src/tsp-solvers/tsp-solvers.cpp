#include <vector>
#include <tuple>
#include "tsp-solvers.hpp"

int getNodoMasCercanoEnClusterNoVisitado(uint nodo, std::vector<std::vector<double>> &distancias, std::vector<uint> nodosDelCluster, std::vector<bool> &visitado)
{
    int minimoId = -1;

    for (uint i = 0; i < nodosDelCluster.size(); i++)
    {
        if (!visitado[i])
        {
            if (minimoId == -1)
            {
                minimoId = i;
            }
            else
            {
                if (distancias[nodo][nodosDelCluster[i]] < distancias[nodo][nodosDelCluster[minimoId]])
                    minimoId = i;
            }
        }
    }

    return minimoId;
}

std::vector<uint> NearestNeighborConClusters(uint root, std::vector<std::vector<double>> &distancias, std::vector<uint> nodosDelCluster)
{
    std::vector<uint> solucion(0);
    uint cantNodosRestantes = nodosDelCluster.size();
    std::vector<bool> visitado(nodosDelCluster.size(), false);

    // Agregamos el root a la solución (es el primer nodo del ciclo)
    solucion.push_back(root);

    do
    {
        uint ultimoNodo = solucion[solucion.size() - 1];

        // Tomamos el índice del nodo con menor distancia al último nodo, dentro del arreglo nodosDelCluster
        int indiceNodoMasCercano = getNodoMasCercanoEnClusterNoVisitado(ultimoNodo, distancias, nodosDelCluster, visitado);

        // Agregamos el nodo correspondiente a dicho índice a la solución
        solucion.push_back(nodosDelCluster[indiceNodoMasCercano]);

        // Lo marcamos como visitado
        visitado[indiceNodoMasCercano] = true;
        cantNodosRestantes--;

    } while (cantNodosRestantes > 0);

    // Agregamos el root a la solución (es el último nodo del ciclo)
    solucion.push_back(root);

    return solucion;
}