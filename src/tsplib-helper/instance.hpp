#include <vector>
#include <tuple>
#include <map>
using namespace std;

#ifndef TSPLIB_INSTANCE_H
#define TSPLIB_INSTANCE_H

class TSPLibInstance 
{ 
    public:
    string name; 
    string comment; 
    string type; 
    uint dimension; 
    string edgeWeightType; 
    uint capacity;
    map<uint, tuple<uint, uint>> nodeCoords;
    map<uint, uint> demand;
    vector<uint> depot;
  
    TSPLibInstance(istream& stream);
    void printToStream(ostream& stream);
    vector<vector<double>> getTSPGraph();
};

#endif