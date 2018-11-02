#include <vector>
#include <tuple>

#ifndef TSP_SOLVERS_HPP
#define TSP_SOLVERS_HPP

int getNodoMasCercanoEnClusterNoVisitado(uint nodo, std::vector<std::vector<double>> &distancias, std::vector<uint> nodosDelCluster, std::vector<bool> &visitado);
std::vector<uint> NearestNeighborConClusters(uint root, std::vector<std::vector<double>> &distancias, std::vector<uint> nodosDelCluster);

#endif