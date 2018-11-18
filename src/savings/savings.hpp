#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "../aux.hpp"

#ifndef SAVINGS_HPP
#define SAVINGS_HPP

struct saving
{
    uint i;
    uint j;
    double sav;
    saving(uint a, uint b, double s) : i(a), j(b), sav(s)
    {
    }
};


std::vector<saving> computeSavingsSolucionInicial(std::vector<std::vector<double>> &matriz,std::vector<route>& routes,uint capacityTruck);
bool savingCompare(saving a, saving b);
void recomputeSavings(std::vector<std::vector<double>>& matriz,std::vector<saving>& savings,std::vector<route>& routes,uint capacityTruck,uint rutaViejaA,uint rutaViejaB,uint rutaMergeada);
std::vector<route> crearSolucionInicialSavings(std::vector<std::vector<double>>& matriz, std::vector<uint>& demand, uint indiceDeposito);
void printSavings(std::vector<saving> &savings);
void ejecutarSavings(std::vector<std::vector<double>>& matrizDeAdyacencia,uint indiceDeposito,uint capacityTruck,std::vector<route>& routes,std::vector<uint>& demanda);

#endif