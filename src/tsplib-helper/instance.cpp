#include <vector>
#include <iostream>
#include <string>
#include <math.h>
#include "instance.hpp"

using namespace std;

// Lee una instancia desde un istream
TSPLibInstance::TSPLibInstance(istream &stream)
{
    string input;
    do
    {
        stream >> input;

        if (input.compare("NAME") == 0)
        {
            stream.ignore(3, EOF);
            getline(stream, name);
        }
        else if (input.compare("COMMENT") == 0)
        {
            stream.ignore(3, EOF);
            getline(stream, comment);
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
            for (uint i = 0; i < dimension; i++)
            {
                uint id;
                uint x, y;

                stream >> id >> x >> y;
                nodeCoords[id] = tuple<uint, uint>(x, y);
            }
        }
        else if (input.compare("DEMAND_SECTION") == 0)
        {
            for (uint i = 0; i < dimension; i++)
            {
                uint id;
                uint node_demand;

                stream >> id >> node_demand;
                demand[id] = node_demand;
            }
        }
        else if (input.compare("DEPOT_SECTION") == 0)
        {
            int node;

            stream >> node;
            while (node != -1)
            {
                depot.push_back((uint)node);
                stream >> node;
            }
        }
    } while (input.compare("EOF") != 0);
}

// Imprime una instancia a un ostream.
void TSPLibInstance::printToStream(ostream &stream)
{
    stream << "NAME : " << name << endl;
    stream << "COMMENT : " << comment << endl;
    stream << "TYPE : " << type << endl;
    stream << "DIMENSION : " << dimension << endl;
    stream << "EDGE_WEIGHT_TYPE : " << edgeWeightType << endl;
    stream << "CAPACITY : " << capacity << endl;
    stream << "NODE_COORD_SECTION" << endl;

    for (pair<uint, tuple<uint, uint>> element : nodeCoords)
    {
        uint id = element.first;
        uint x = get<0>(element.second);
        uint y = get<1>(element.second);

        stream << " " << id << " " << x << " " << y << endl;
    }

    stream << "DEMAND_SECTION" << endl;

    for (pair<uint, uint> element : demand)
    {
        uint id = element.first;
        uint node_demand = element.second;

        stream << id << " " << node_demand << endl;
    }

    stream << "DEPOT_SECTION" << endl;

    for (uint i = 0; i < depot.size(); i++)
    {
        stream << " " << depot[i] << endl;
    }

    stream << " " << -1 << endl;

    stream << "EOF" << endl;
}

// Devuelve el grafo completo correspondiente a la instancia, donde
// dicho grafo modela la instancia según lo argumentado en el informe
// adjunto a este trabajo. 
vector<vector<double>> TSPLibInstance::getTSPGraph()
{
    vector<vector<double>> matrix(dimension);

    // Inicializamos la matriz resultante en 0
    for (uint i = 0; i < dimension; i++)
    {
        matrix[i] = vector<double>(dimension);

        for (uint j = 0; j < dimension; j++)
        {
            int xd = get<0>(nodeCoords[i+1]) - get<0>(nodeCoords[j+1]);
            int yd = get<1>(nodeCoords[i+1]) - get<1>(nodeCoords[j+1]);

            // Calculo la norma/distancia entre los dos puntos
            double distance = sqrt(xd*xd + yd*yd);

            matrix[i][j] = distance;
        }
    }

    return matrix;
}