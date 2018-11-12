#include <vector>
#include <iostream>
#include "../tsplib-helper/instance.hpp"

#ifndef CLUSTERIZATION_METHODS_HPP
#define CLUSTERIZATION_METHODS_HPP

struct NodeInPolarCoords
{
    uint m_id;
    double m_r;
    double m_theta;
    double m_demand;

    NodeInPolarCoords(uint id, double r, double theta, uint demand) : m_id(id), m_r(r), m_theta(theta), m_demand(demand)
    {
    }

    bool operator<(const struct NodeInPolarCoords &other) const
    {
        return m_theta < other.m_theta;
    }
};

uint SweepClusterization(TSPLibInstance &tspInstance, std::priority_queue<NodeInPolarCoords> &nodesInPolarCoords, std::vector<uint> &clusters);
uint TP2Clusterization(TSPLibInstance &tspInstance, std::vector<uint> &clusters);

std::vector<std::tuple<int, int, int>> obtenerListaDeAristas(std::vector<std::vector<double>> E);
void definirClusterTP2(std::vector<std::vector<double>> grafo, std::vector<uint> &result, std::vector<bool> &visitados, uint i, int nroCluster);

#endif