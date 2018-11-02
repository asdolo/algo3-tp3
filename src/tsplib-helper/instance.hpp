#include <vector>
#include <tuple>

#ifndef TSPLIB_INSTANCE_HPP
#define TSPLIB_INSTANCE_HPP

class TSPLibInstance
{
public:
  std::string name;
  std::string comment;
  std::string type;
  uint dimension;
  std::string edgeWeightType;
  uint capacity;
  std::vector<std::tuple<double, double>> nodeCoords;
  std::vector<uint> demand;
  std::vector<uint> depot;

  TSPLibInstance(std::istream &stream);
  void printToStream(std::ostream &stream);
  std::vector<std::vector<double>> getTSPGraph();
};

#endif