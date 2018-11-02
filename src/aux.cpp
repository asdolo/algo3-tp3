#include <vector>
#include <iostream>
#include "aux.hpp"
using namespace std;
//General
void printMatriz(vector<vector<double>> matriz, ostream &stream)
{
    for (uint i = 0; i < matriz.size(); i++)
    {
        for (uint j = 0; j < matriz.size(); j++)
        {
            stream << matriz[i][j] << "\t\t";
        }
        stream << endl;
    }
}
//-----------------------------------------------------------------------------------
//Ej 1 y Ej 2 
double calcularCosto(vector< vector<double> > matriz, vector< route > routes){
    double res=0;
    for(uint i =0 ;i <routes.size() ;i++){
        vector<uint> ruta = routes[i].ruta;
        for(uint j = 0 ; j<ruta.size()-1;j++){
            res += matriz[ruta[j]][ruta[j+1]];
        }
    }
    return res;
}
void printRoutes(vector< route >& routes){
    for(uint i=0;i<routes.size();i++){
        printRoute(routes[i]);
    }
}
void printRoute(route r){
    cout << "Ruta " << r.indiceRuta << ":" << endl;
    cout << "   ruta= ";
    vector<uint> ruta = r.ruta;
    
    for(uint j = 0; j < ruta.size(); j++)
    {
        cout << ruta[j]+1;
        if(j!=ruta.size()-1){
            cout << ",";
        }
    }
    cout << endl;
    cout << "   volumen necesario de la ruta= " << r.capacityRoute << endl;
    cout << endl;
}
void printRutaSolucion(route r){
    vector<uint> ruta = r.ruta;
    
    for(uint j = 0; j < ruta.size(); j++)
    {
        cout << ruta[j]+1;
        if(j != ruta.size()-1){
            cout << " ";
        }
    }
    cout << endl;
}

void imprimirSolucionTP(vector< vector<double> > matriz, vector< route > routes){
    cout << routes.size() << endl;
    
    for(uint i = 0; i < routes.size(); i++)
    {
        printRutaSolucion(routes[i]);
    }
    cout << calcularCosto(matriz,routes) << endl;
}

//-----------------------------------------------------------------------------------
//Ej 3 y Ej 4