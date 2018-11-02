#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "tsplib-helper/instance.hpp"
using namespace std;
map <uint, uint> clienteARuta;
struct route
{
    uint indiceRuta;
    vector<uint> ruta;
    uint capacityRoute;
    double distancia;
    route(uint i,vector<uint> r,uint c,double d) : indiceRuta(i),ruta(r),capacityRoute(c),distancia(d)
        {
        }
};

double calcularCosto(vector< vector<double> > matriz, vector< route > routes){
    double res=0;
    for(uint i =0 ;i <routes.size() ;i++){
        vector<uint> ruta = routes[i].ruta;
        for(uint j = 0 ; j<ruta.size()-1;j++){
            res += matriz[ruta[j]-1][ruta[j+1]-1];
        }
    }
    return res;
}
uint obtenerClienteMasCercano(vector<vector<double>> matriz,map<uint, uint> demand,vector<bool> loAgregue,uint capacidadActual,uint capacityTruck,uint clienteActual){
    double max = -1;
    uint clienteMasCercano = 0;
    for(uint j=0;j<matriz.size();j++){
        if(j!=clienteActual-1 && !loAgregue[j] && matriz[clienteActual-1][j]>max && demand[j+1]+capacidadActual<=capacityTruck){
            clienteMasCercano = j+1;
            max = matriz[clienteActual-1][j];
        }
    }
    return clienteMasCercano;
}
bool todosTrue(vector<bool> v){
    for(uint i =0; i<v.size();i++){
        if(v[i]==false)return false;
    }
    return true;
}
vector< route > createRoutes(vector<vector<double>> matriz ,map<uint, uint> demand,uint depositoId,uint capacityTruck){
    vector<bool> loAgregue(matriz.size(),false);
    loAgregue[depositoId-1]=true;
    vector < route > res;
    int nroRuta=1;
    while(!todosTrue(loAgregue)){
        vector< uint > ruta;
        uint capacidadActual = 0;
        double distancia = 0;
        ruta.push_back(depositoId);
        uint clienteActual = depositoId;
        for(uint i = 0 ; i < matriz.size(); i++){
            uint clienteMasCercano=obtenerClienteMasCercano(matriz,demand,loAgregue,capacidadActual,capacityTruck,clienteActual);
            if(clienteMasCercano!=0){                
                ruta.push_back(clienteMasCercano);
                loAgregue[clienteMasCercano-1]=true;
                capacidadActual+=demand[clienteMasCercano];
                clienteARuta.insert(pair <uint, uint> (clienteMasCercano,nroRuta)); 
                distancia += matriz[clienteActual-1][clienteMasCercano-1];
                clienteActual=clienteMasCercano;
            }
        }
        ruta.push_back(depositoId);
        res.push_back(route(nroRuta++,ruta,capacidadActual,distancia));
    }
    return res;
}

void printRoute(route r){
    cout << "Ruta " << r.indiceRuta << ":" << endl;
    cout << "   ruta= ";
    vector<uint> ruta = r.ruta;
    
    for(uint j = 0; j < ruta.size(); j++)
    {
        cout << ruta[j];
        if(j!=ruta.size()-1){
            cout << ",";
        }
    }
    cout << endl;
    cout << "   volumen necesario de la ruta= " << r.capacityRoute << endl;
    cout << "   distancia recorrida de la ruta= " << r.distancia << endl;
    cout << endl;
}
void printRoutes(vector< route >& routes){
    for(uint i=0;i<routes.size();i++){
        printRoute(routes[i]);
    }
}
bool compareByDistance(const route &a, const route &b)
{
    return a.distancia >= b.distancia;
}
void exchangeClients(vector<route>& routes , uint nroRutaA,uint nroRutaB ){
    vector<uint> rutaA = routes[nroRutaA].ruta;
    vector<uint> rutaB = routes[nroRutaB].ruta;
    vector<uint> nuevaRutaA;
    nuevaRutaA.push_back(rutaA[0]);
    nuevaRutaA.push_back(rutaA[1]);
    nuevaRutaA.push_back(rutaB[rutaB.size()-2]);
    nuevaRutaA.push_back(rutaA[rutaA.size()-1]);
    vector<uint> nuevaRutaB;
    
    for(uint i = 0; i < rutaB.size()-2; i++)
    {
        nuevaRutaB.push_back(rutaB[i]);
    }
    nuevaRutaB.push_back(rutaA[1]);
    nuevaRutaB.push_back(rutaB[rutaB.size()-1]);
    routes[nroRutaA].ruta=nuevaRutaA;
    routes[nroRutaB].ruta=nuevaRutaB;
}

void printRutaSolucion(route r){
    vector<uint> ruta = r.ruta;
    
    for(uint j = 0; j < ruta.size(); j++)
    {
        cout << ruta[j];
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
int main(int argc, char *argv[])
{
    uint repeticionesGrasp=argc >= 2 ? stoi(argv[1]) : 3;
     // Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(cin);

    // Obtengo el grafo que modela la instancia
    vector<vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();
    
    // Obtengo el id del deposito
    uint depositoId = tspInstance.depot[0];

    // Obtengo la capacidad del camión
    uint capacityTruck = tspInstance.capacity;

    // Construyo la solución inicial.
    // La solucion inicial consistira en ir yendo al cliente mas cercano mientras tenga capacidad.
    vector< route > routes = createRoutes(matrizDeAdyacencia,tspInstance.demand,depositoId, capacityTruck);
    
    // Imprimo como quedaron las rutas iniciales.
    //printRoutes(routes);
    
    // Ordeno las rutas por distancia decrecientemente
    sort(routes.begin(), routes.end(), compareByDistance);
    
    // Calculo el costo total de la solucion inicial
    double costoTotal = calcularCosto(matrizDeAdyacencia,routes);

    // Recorro el grafo de soluciones utilizando GRASP
    while(repeticionesGrasp > 0){
        vector< route > routesAux = routes;
        // Elijo dos rutas al azar
        srand (time(NULL));
        uint randomNumber1 = rand() % (routesAux.size()-1);
        uint randomNumber2 = randomNumber1;
        while(randomNumber2 == randomNumber1)randomNumber2 = rand() % (routesAux.size()-1);
        
        // Busco en la vecindad utilizando exchange 
        exchangeClients(routesAux,randomNumber1,randomNumber2);
        double costoTotalAux = calcularCosto(matrizDeAdyacencia,routesAux);
        if(costoTotal>costoTotalAux){
            routes=routesAux;
            costoTotal=costoTotalAux;
        }
        repeticionesGrasp--;
    }
    
    imprimirSolucionTP(matrizDeAdyacencia,routes);

    return 0;
}