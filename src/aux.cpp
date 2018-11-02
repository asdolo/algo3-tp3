#include <vector>
#include <iostream>
#include "aux.hpp"

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