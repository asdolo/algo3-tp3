#include <vector>
#include <iostream>
#include <string>
#include <math.h>
#include "instance.hpp"

// Lee una instancia desde un istream
TSPLibInstance::TSPLibInstance(std::istream &stream)
{
    std::string input;
    do
    {
        stream >> input;

        if (input.compare("NAME") == 0)
        {
            stream.ignore(3, EOF);
            std::getline(stream, name);
        }
        else if (input.compare("COMMENT") == 0)
        {
            stream.ignore(3, EOF);
            std::getline(stream, comment);
        }
        else if (input.compare("TYPE") == 0)
        {
            stream.ignore(3, EOF);
            stream >> type;
        }
        else if (input.compare("DIMENSION") == 0)
        {
            stream.ignore(3, EOF);
            stream >> dimension;
        }
        else if (input.compare("EDGE_WEIGHT_TYPE") == 0)
        {
            stream.ignore(3, EOF);
            stream >> edgeWeightType;
        }
        else if (input.compare("CAPACITY") == 0)
        {
            stream.ignore(3, EOF);
            stream >> capacity;
        }
        else if (input.compare("NODE_COORD_SECTION") == 0)
        {
            nodeCoords = std::vector<std::tuple<double, double>>(0);

            for (uint i = 0; i < dimension; i++)
            {
                uint id;
                double x, y;

                stream >> id >> x >> y;
                nodeCoords.push_back(std::tuple<double, double>(x, y));
            }
        }
        else if (input.compare("DEMAND_SECTION") == 0)
        {
            demand = std::vector<uint>(0);

            for (uint i = 0; i < dimension; i++)
            {
                uint id;
                uint node_demand;

                stream >> id >> node_demand;
                demand.push_back(node_demand);
            }
        }
        else if (input.compare("DEPOT_SECTION") == 0)
        {
            int node;

            stream >> node;
            while (node != -1)
            {
                depot.push_back((uint)node - 1);
                stream >> node;
            }
        }
    } while (input.compare("EOF") != 0);
}

// Imprime una instancia a un ostream.
void TSPLibInstance::printToStream(std::ostream &stream)
{
    stream << "NAME : " << name << std::endl;
    stream << "COMMENT : " << comment << std::endl;
    stream << "TYPE : " << type << std::endl;
    stream << "DIMENSION : " << dimension << std::endl;
    stream << "EDGE_WEIGHT_TYPE : " << edgeWeightType << std::endl;
    stream << "CAPACITY : " << capacity << std::endl;
    stream << "NODE_COORD_SECTION" << std::endl;

    for (uint i = 0; i < dimension; i++)
    {
        uint id = i + 1;
        double x = std::get<0>(nodeCoords[i]);
        double y = std::get<1>(nodeCoords[i]);

        stream << " " << id << " " << x << " " << y << std::endl;
    }

    stream << "DEMAND_SECTION" << std::endl;

    for (uint i = 0; i < dimension; i++)
    {
        uint id = i + 1;
        uint node_demand = demand[i];

        stream << id << " " << node_demand << std::endl;
    }

    stream << "DEPOT_SECTION" << std::endl;

    for (uint i = 0; i < depot.size(); i++)
    {
        stream << " " << depot[i] << std::endl;
    }

    stream << " " << -1 << std::endl;

    stream << "EOF" << std::endl;
}

// Devuelve el grafo completo correspondiente a la instancia, donde
// dicho grafo modela la instancia según lo argumentado en el informe
// adjunto a este trabajo.
std::vector<std::vector<double>> TSPLibInstance::getTSPGraph()
{
    std::vector<std::vector<double>> matrix(dimension);

    // Inicializamos la matriz resultante en 0
    for (uint i = 0; i < dimension; i++)
    {
        matrix[i] = std::vector<double>(dimension);

        for (uint j = 0; j < dimension; j++)
        {
            double xd = std::get<0>(nodeCoords[i]) - std::get<0>(nodeCoords[j]);
            double yd = std::get<1>(nodeCoords[i]) - std::get<1>(nodeCoords[j]);

            // Calculo la norma/distancia entre los dos puntos
            double distance = sqrt(xd * xd + yd * yd);

            matrix[i][j] = distance;
        }
    }

    return matrix;
}