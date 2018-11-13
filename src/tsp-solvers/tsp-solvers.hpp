#include <vector>
#include <tuple>

#ifndef TSP_SOLVERS_HPP
#define TSP_SOLVERS_HPP

int getNodoMasCercanoNoVisitadoEnCluster(uint nodo, std::vector<std::vector<double>> &distancias, std::vector<uint> nodosDelCluster, std::vector<bool> &visitado);
std::vector<uint> NearestNeighborEnCluster(uint root, std::vector<std::vector<double>> &distancias, std::vector<uint> nodosDelCluster);

std::vector<uint> PrimerSolucionFactibleEnCluster(uint root, std::vector<uint> nodosDelCluster);
#endif