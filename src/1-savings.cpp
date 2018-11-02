#include <iostream>
#include <vector>
#include <map> 
#include "tsplib-helper/instance.hpp"
#include "aux.hpp"
using namespace std;
map <uint, uint> clienteARuta;

struct saving
{
    uint i;
    uint j;
    double sav;
    saving(uint a,uint b,double s) : i(a),j(b),sav(s)
        {
        }
};

vector< saving > computeSavings(vector< vector<double> >& matriz){
    vector<saving> savings;
    for(uint i = 0; i < matriz.size(); i++)
    {
       for(uint j=i+1;j < matriz.size();j++){
           if(matriz[i][0] + matriz[0][j] - matriz[i][j] > 0){
                savings.push_back(saving(i,j,matriz[i][0] + matriz[0][j] - matriz[i][j]));
            }
       }
    }
    return savings;
}

vector< route > createRoutes(vector<vector<double>> matriz ,vector<uint> demand,uint indiceDeposito){
    vector < route > res;
    for(uint i = 0; i < matriz.size(); i++)
    {
        if(i!=indiceDeposito){
            clienteARuta.insert(pair<uint,uint>(i,res.size()));
            vector< uint > ruta;
            ruta.push_back(indiceDeposito);
            ruta.push_back(i);
            ruta.push_back(indiceDeposito);
            double distancia = 2*matriz[indiceDeposito][i];
            res.push_back(route(i,ruta,demand[i],distancia));
            
        }
    }
    return res;
}
void mergearRutas(vector<route>& routes,uint a,uint b,uint indiceDeposito){
    
    uint sizeRutaA = routes[clienteARuta[a]].ruta.size();
    uint sizeRutaB = routes[clienteARuta[b]].ruta.size();
    if(sizeRutaA == 0 || sizeRutaB == 0) return;
    if(sizeRutaA>sizeRutaB){
        if(routes[clienteARuta[a]].ruta[sizeRutaA-2] == a && routes[clienteARuta[b]].ruta[sizeRutaB-3] == indiceDeposito ){
            routes[clienteARuta[a]].ruta.erase(routes[clienteARuta[a]].ruta.begin()+sizeRutaA-1);
            bool agregar=false;
            for(uint i=0; i<sizeRutaB;i++){
                if(!agregar){
                    if(routes[clienteARuta[b]].ruta[i]==b){
                        agregar=true;
                        routes[clienteARuta[a]].ruta.push_back(b);
                    }
                }else{
                    routes[clienteARuta[a]].ruta.push_back(routes[clienteARuta[b]].ruta[i]);
                }
            }
            routes[clienteARuta[b]].ruta.clear();
            routes[clienteARuta[a]].capacityRoute+=routes[clienteARuta[b]].capacityRoute;
            routes[clienteARuta[b]].indiceRuta=routes[clienteARuta[a]].indiceRuta;
            routes[clienteARuta[b]].capacityRoute = 0;
            clienteARuta[b]=clienteARuta[a];
        }
    }else{ 
        if(routes[clienteARuta[b]].ruta[sizeRutaB-2] == b && routes[clienteARuta[a]].ruta[sizeRutaA-3] == indiceDeposito){
            routes[clienteARuta[b]].ruta.erase(routes[clienteARuta[b]].ruta.begin()+sizeRutaB-1);
            bool agregar=false;
            for(uint i=0; i<sizeRutaA;i++){
                if(!agregar){
                    if(routes[clienteARuta[a]].ruta[i]==a){
                        agregar=true;
                        routes[clienteARuta[b]].ruta.push_back(a);
                    }
                }else{
                    routes[clienteARuta[b]].ruta.push_back(routes[clienteARuta[a]].ruta[i]);
                }
            }
            routes[clienteARuta[a]].ruta.clear();
            routes[clienteARuta[b]].capacityRoute += routes[clienteARuta[a]].capacityRoute;
            routes[clienteARuta[a]].indiceRuta = routes[clienteARuta[b]].indiceRuta;
            routes[clienteARuta[a]].capacityRoute = 0;
            clienteARuta[a]=clienteARuta[b];
        }
    }
    return;
}
void mergeRoutes(saving s,vector< route >& routes,uint capacityMax,uint indiceDeposito){
    uint i = s.i;
    uint j = s.j;
    //Me fijo si ya estan en la misma ruta
    if(clienteARuta[i] == clienteARuta[j]){
        //Ya estan en la misma ruta, no los puedo mergear
        return;
    }
    
    //Ahora me fijo si al unirlos , el volumen requerido se pasa de la capacidad
    if(routes[clienteARuta[i]].capacityRoute+routes[clienteARuta[j]].capacityRoute > capacityMax){
        //No los puedo unir ya que el camion se desbordaria.
        return;
    }
    mergearRutas(routes,i,j,indiceDeposito);

}
bool savingCompare(saving a, saving b) { 
    return a.sav >= b.sav; 
}


void printSavings(vector< saving >& savings){
    for(uint i=0;i<savings.size();i++){
        cout << "Saving " << i << ":" << endl;
        cout << "   i=" << savings[i].i << endl;
        cout << "   j=" << savings[i].j << endl;
        cout << "   saving=" << savings[i].sav << endl;
        cout << endl;
    }
}



int main(int argc, char *argv[])
{
    // Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(cin);

    // Obtengo el grafo que modela la instancia
    vector<vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();
    
    // Obtengo el id del deposito
    uint indiceDeposito = tspInstance.depot[0];

    // Obtengo la capacidad del camión
    uint capacityTruck = tspInstance.capacity;
    // Calculo cuanto me puedo ahorrar uniendo rutas. Solo guardo las que me generan un ahorro real.
    vector< saving > savings = computeSavings(matrizDeAdyacencia);

    // Ordendo decrecientemente los savings.
    std::sort(savings.begin(), savings.end(), savingCompare);

    // Imprimo los savings
    //printSavings(savings);
    
    // Creo la solucion inicial.
    // La solucion consistira en rutas basicas que consisten en un camion por cliente.
    // Cada ruta es deposito-cliente-deposito
    vector< route > routes = createRoutes(matrizDeAdyacencia,tspInstance.demand,indiceDeposito);
    
    // Imprimo las rutas
    //printRoutes(routes);

    // Imprimo el costo total si dejaria las rutas como estan.
    //cout << "Costo Inicial:" << calcularCosto(matrizDeAdyacencia,routes) << endl;

    //Mergeo las rutas comenzando por las que mas ahorro me dan.
    for(uint i=0;i<savings.size();i++){
        mergeRoutes(savings[i],routes,capacityTruck,indiceDeposito);
    }

    // Borro las rutas que quedaron vacias
    for(uint i=0;i<routes.size();i++){
        vector<uint> ruta = routes[i].ruta;
        if(ruta.size() == 0) {
            routes.erase(routes.begin()+i);
            i--;
        }
    }
    // Enumero las rutas desde 1 hasta cantidad de rutas necesarias
    /* for(uint i=0;i<routes.size();i++){
        routes[i].indiceRuta = i+1;
    }*/

    // Imprimo las rutas mergeadas
    //printRoutes(routes);

    // Imprimo el costo total con las nuevas rutas.
    //cout << "Costo Final:" << calcularCosto(matrizDeAdyacencia,routes) << endl;

    imprimirSolucionTP(matrizDeAdyacencia,routes);
    return 0;
}