#include <iostream>
#include <vector>
#include "tsplib-helper/instance.hpp"
using namespace std;

struct saving
{
    uint i;
    uint j;
    double sav;
    saving(uint a,uint b,double s) : i(a),j(b),sav(s)
        {
        }
};

struct route
{
    int indiceRuta;
    vector<uint> ruta;
    uint capacityRoute;
    route(int i,vector<uint> r,double c) : indiceRuta(i),ruta(r),capacityRoute(c)
        {
        }
};
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
    cout << endl;
}
vector< saving > computeSavings(vector< vector<double> >& matriz){
    vector<saving> savings;
    for(uint i = 1; i < matriz.size(); i++)
    {
       for(uint j=i+1;j < matriz.size();j++){
           if(matriz[i][0] + matriz[0][j] - matriz[i][j] > 0){
                savings.push_back(saving(i+1,j+1,matriz[i][0] + matriz[0][j] - matriz[i][j]));
            }
       }
    }
    return savings;
}

vector< route > createRoutes(uint size,map<uint, uint> demand,uint depositoId){
    vector < route > res;
    for(uint i = 1; i <= size; i++)
    {
        if(i!=depositoId){
            vector< uint > ruta;
            ruta.push_back(depositoId);
            ruta.push_back(i);
            ruta.push_back(depositoId);
            res.push_back(route(i,ruta,demand[i]));
        }
    }
    return res;
}
void mergearRutas(vector<route>& routes,uint a,uint b,uint depositoId){
    
    int indiceRutaA = routes[a-2].indiceRuta - 2;
    int indiceRutaB = routes[b-2].indiceRuta - 2;
    uint sizeRutaA = routes[indiceRutaA].ruta.size();
    uint sizeRutaB = routes[indiceRutaB].ruta.size();
    if(sizeRutaA == 0 || sizeRutaB == 0) return;
    if(sizeRutaA>sizeRutaB){
        if(routes[indiceRutaA].ruta[sizeRutaA-2] == a && routes[indiceRutaB].ruta[sizeRutaB-3] == depositoId ){
            routes[indiceRutaA].ruta.erase(routes[indiceRutaA].ruta.begin()+sizeRutaA-1);
            bool agregar=false;
            for(uint i=0; i<sizeRutaB;i++){
                if(!agregar){
                    if(routes[indiceRutaB].ruta[i]==b){
                        agregar=true;
                        routes[indiceRutaA].ruta.push_back(b);
                    }
                }else{
                    routes[indiceRutaA].ruta.push_back(routes[indiceRutaB].ruta[i]);
                }
            }
            routes[indiceRutaB].ruta.clear();
            routes[indiceRutaA].capacityRoute+=routes[indiceRutaB].capacityRoute;
            routes[indiceRutaB].indiceRuta=routes[indiceRutaA].indiceRuta;
            routes[indiceRutaB].capacityRoute = 0;
        }
    }else{ 
        if(routes[indiceRutaB].ruta[sizeRutaB-2] == b && routes[indiceRutaA].ruta[sizeRutaA-3] == depositoId){
            routes[indiceRutaB].ruta.erase(routes[indiceRutaB].ruta.begin()+sizeRutaB-1);
            bool agregar=false;
            for(uint i=0; i<sizeRutaA;i++){
                if(!agregar){
                    if(routes[indiceRutaA].ruta[i]==a){
                        agregar=true;
                        routes[indiceRutaB].ruta.push_back(a);
                    }
                }else{
                    routes[indiceRutaB].ruta.push_back(routes[indiceRutaA].ruta[i]);
                }
            }
            routes[indiceRutaA].ruta.clear();
            routes[indiceRutaB].capacityRoute += routes[indiceRutaA].capacityRoute;
            routes[indiceRutaA].indiceRuta = routes[indiceRutaB].indiceRuta;
            routes[indiceRutaA].capacityRoute = 0;
        }
    }
    return;
}
void mergeRoutes(saving s,vector< route >& routes,uint capacityMax,uint depositoId){
    uint i = s.i;
    uint j = s.j;
    int indiceRutaI = routes[i-2].indiceRuta - 2;
    int indiceRutaJ = routes[j-2].indiceRuta - 2;
    //Me fijo si ya estan en la misma ruta
    if(indiceRutaI == indiceRutaJ){
        //Ya estan en la misma ruta, no los puedo mergear
        return;
    }
    
    //Ahora me fijo si al unirlos , el volumen requerido se pasa de la capacidad
    if(routes[indiceRutaI].capacityRoute+routes[indiceRutaJ].capacityRoute > capacityMax){
        //No los puedo unir ya que el camion se desbordaria.
        return;
    }
    mergearRutas(routes,i,j,depositoId);

}
bool savingCompare(saving a, saving b) { 
    return a.sav >= b.sav; 
}
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

void printSavings(vector< saving >& savings){
    for(uint i=0;i<savings.size();i++){
        cout << "Saving " << i << ":" << endl;
        cout << "   i=" << savings[i].i << endl;
        cout << "   j=" << savings[i].j << endl;
        cout << "   saving=" << savings[i].sav << endl;
        cout << endl;
    }
}



void printRoutes(vector< route >& routes){
    for(uint i=0;i<routes.size();i++){
        printRoute(routes[i]);
    }
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
    // Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(cin);

    // Obtengo el grafo que modela la instancia
    vector<vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();
    
    // Obtengo el id del deposito
    uint depositoId = tspInstance.depot[0];

    // Obtengo la capacidad del camión
    uint capacityTruck = tspInstance.capacity;
    // Calculo cuanto me puedo ahorrar uniendo rutas. Solo guardo las que me generan un ahorro real.
    vector< saving > savings = computeSavings(matrizDeAdyacencia);

    // Ordendo decrecientemente los savings.
    std::sort(savings.begin(), savings.end(), savingCompare);

    // Imprimo los savings
    //printSavings(savings);
    
    // Creo las rutas basicas que consisten en un camion por cliente.
    // Cada ruta es deposito-cliente-deposito
    vector< route > routes = createRoutes(matrizDeAdyacencia.size(),tspInstance.demand,depositoId);
    
    // Imprimo las rutas
    //printRoutes(routes);

    // Imprimo el costo total si dejaria las rutas como estan.
    //cout << "Costo Inicial:" << calcularCosto(matrizDeAdyacencia,routes) << endl;

    //Mergeo las rutas comenzando por las que mas ahorro me dan.
    for(uint i=0;i<savings.size();i++){
        mergeRoutes(savings[i],routes,capacityTruck,depositoId);
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
    for(uint i=0;i<routes.size();i++){
        routes[i].indiceRuta = i+1;
    }

    // Imprimo las rutas mergeadas
    //printRoutes(routes);

    // Imprimo el costo total con las nuevas rutas.
    //cout << "Costo Final:" << calcularCosto(matrizDeAdyacencia,routes) << endl;

    imprimirSolucionTP(matrizDeAdyacencia,routes);
    return 0;
}