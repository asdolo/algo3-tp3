#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include "tsplib-helper/instance.hpp"
#include "aux.hpp"

struct saving
{
    uint i;
    uint j;
    double sav;
    saving(uint a, uint b, double s) : i(a), j(b), sav(s)
    {
    }
};


void printRutaSolucionn(route r)
{
    std::vector<uint> ruta = r.ruta;

    for (uint j = 0; j < ruta.size(); j++)
    {
        std::cout << ruta[j] + 1;
        if (j != ruta.size() - 1)
        {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}


 void imprimirSolucion(std::vector<std::vector<double>> matriz,std::vector<route> routes)
 {
     std::cout << routes.size() << std::endl;
 
     for (uint i = 0; i < routes.size(); i++)
     {
         printRutaSolucionn(routes[i]);
     }
     std::cout << calcularCosto(matriz, routes) << std::endl;
}


route obtenerMinimaRuta(std::vector<route> rutas){
    double minimaDistancia = rutas[0].distancia;
    uint indiceRutaMinima = 0;
    
    for(uint i = 1; i < rutas.size(); i++)
    {
        if(rutas[i].distancia < minimaDistancia){
            minimaDistancia= rutas[i].distancia;
            indiceRutaMinima = i;
        }
    }
    return rutas[indiceRutaMinima];
}


route obtenerRutasCombinadas(std::vector<std::vector<double>> matriz,route a,route b){
    //Calculo la nueva capacidad de la ruta
    double capacidad = a.capacityRoute+b.capacityRoute;
    //Copio la ruta A
    std::vector<uint> rutaA = a.ruta;
    // Obtengo la distancia de la ruta A , quitandole la ultima arista al deposito.
    double distanciaA = a.distancia - matriz[rutaA[rutaA.size()-2]][rutaA[rutaA.size()-1]];
    rutaA.erase(rutaA.begin()+rutaA.size()-1);
    // Hago la combinacion generando la ruta : deposito-rutaA(sin deposito)-rutaB(sin deposito) - deposito
    for(uint i =1 ; i < b.ruta.size();i++){
        rutaA.push_back(b.ruta[i]);
        distanciaA+= matriz[rutaA[rutaA.size()-2]][rutaA[rutaA.size()-1]];
    }
    route res(a.indiceRuta,rutaA,capacidad,distanciaA);
    return res;
}

// Devuelve la mejor combinacion de las rutas a y b , mejor con respecto a menor distancia.
// Complejidad: O(N)
route combinarRuta(std::vector<std::vector<double>>& matriz,route a, route b){
    std::vector<uint> rutaA = a.ruta;
    std::vector<uint> rutaB = b.ruta;
    std::reverse(rutaA.begin(),rutaA.end());
    std::reverse(rutaB.begin(),rutaB.end());
    route reversaA(a.indiceRuta,rutaA,a.capacityRoute,a.distancia);
    route reversaB(b.indiceRuta,rutaB,b.capacityRoute,b.distancia);
    std::vector<route> rutasPosibles;

    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,a,b));
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,b,a));

    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,reversaA,b));
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,b,reversaA));
    
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,a,reversaB));
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,reversaB,a));

    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,reversaA,reversaB));
    rutasPosibles.push_back(obtenerRutasCombinadas(matriz,reversaB,reversaA));
    // Devuelvo la ruta combinada que menos distancia recorre.
    return obtenerMinimaRuta(rutasPosibles);
}




// Genera un vector de savings que consisten en los ahorros posibles de combinar rutas tales que no excedan la capacidad del camion
// Complejidad: O(N^3)
std::vector<saving> computeSavings(std::vector<std::vector<double>> &matriz,std::vector<route>& routes,uint capacityTruck)
{
    std::vector<saving> savings;
    for (uint i = 0; i < routes.size(); i++)
    {
        for (uint j = i+1; j < routes.size(); j++)
        {
            // Me fijo si uniendo las rutas no me paso de la capacidad del camion.
            if(routes[i].capacityRoute+routes[j].capacityRoute < capacityTruck){
                // Creo la ruta nueva combinando las dos rutas de la mejor manera
                route rutaNueva = combinarRuta(matriz,routes[i],routes[j]);
                //Calculo el costo de la nueva ruta
                double costoNuevaRuta=rutaNueva.distancia;
                //Calculo el costo de la suma de las dos rutas sin unirlas
                double costoSumaDeRutas=routes[i].distancia+routes[j].distancia;
                //Me fijo si al unir las rutas ahorro distancia.
                if(costoNuevaRuta < costoSumaDeRutas)
                {
                    savings.push_back(saving(i, j,costoNuevaRuta));
                }
            }
        }
    }
    return savings;
}
// Funcion para ordenar decreciemente el vector savings.
// Complejidad: O(1)
bool savingCompare(saving a, saving b)
{
    return a.sav > b.sav;
}

// Funcion que elimina los savings en los que la rutaViejaA y la rutaViejaB esten involucrados y luego ..
// Genera nuevos savings con la nueva ruta ( producto de la combinacion de rutaViejaA y la rutaViejaB) rutaMergeada.
// Modifica savings
// Complejidad: O(N^2)
void recomputeSavings(std::vector<std::vector<double>>& matriz,std::vector<saving>& savings,std::vector<route>& routes,uint capacityTruck,uint rutaViejaA,uint rutaViejaB,uint rutaMergeada)
{
    //Elimino los savings que ya no tienen sentido
    for(uint i=0;i<savings.size();i++){
        if(savings[i].i == rutaViejaA || savings[i].i ==rutaViejaB || savings[i].j == rutaViejaA || savings[i].j ==rutaViejaB){
            savings.erase(savings.begin()+i);
            i--;
        }
    }

    //Veo si existen savings nuevos con la ruta nueva.
    for (uint j = 0; j < routes.size(); j++)
    {
        // Me fijo de que no sea la misma ruta y que la ruta exista
        if(j != rutaMergeada && routes[j].ruta.size()>0){
            //Me fijo que al unirlas no se exceda de la capacidad del camion.
            if(routes[rutaMergeada].capacityRoute+routes[j].capacityRoute < capacityTruck){

                route rutaNueva = combinarRuta(matriz,routes[rutaMergeada],routes[j]);
                double costoNuevaRuta=rutaNueva.distancia;
                double costoSumaDeRutas=routes[rutaMergeada].distancia + routes[j].distancia;
                if(costoNuevaRuta < costoSumaDeRutas)
                {
                    //Inserto ordenado
                    savings.insert(std::upper_bound( savings.begin(), savings.end(), saving(rutaMergeada, j,costoNuevaRuta), savingCompare ),saving(rutaMergeada, j,costoNuevaRuta));
                    
                }
            }
        }
        
    }
}

// Crea la solucion inicial que consiste en rutas : deposito-cliente-deposito
// Complejidad: O(N)
std::vector<route> createRoutes(std::vector<std::vector<double>>& matriz, std::vector<uint>& demand, uint indiceDeposito)
{
    std::vector<route> res;
    for (uint i = 0; i < matriz.size(); i++)
    {
        if (i != indiceDeposito)
        {
            std::vector<uint> ruta;
            ruta.push_back(indiceDeposito);
            ruta.push_back(i);
            ruta.push_back(indiceDeposito);
            double distancia =matriz[indiceDeposito][i] + matriz[i][indiceDeposito];
            res.push_back(route(i, ruta, demand[i], distancia));
        }
    }
    return res;
}



std::vector<route> savings(TSPLibInstance tspInstance){
	// Obtengo el grafo que modela la instancia
    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();

    // Obtengo el id del deposito
    uint indiceDeposito = tspInstance.depot[0];

    // Obtengo la capacidad del camión
    uint capacityTruck = tspInstance.capacity;

    // Creo la solucion inicial.
    // La solucion consistira en rutas basicas que consisten en un camion por cliente.
    // Cada ruta es deposito-cliente-deposito
    std::vector<route> routes = createRoutes(matrizDeAdyacencia, tspInstance.demand, indiceDeposito);

    // Calculo cuanto me puedo ahorrar uniendo rutas. Solo guardo las que me generan un ahorro real.
    std::vector<saving> savings = computeSavings(matrizDeAdyacencia,routes,capacityTruck);

    // Ordendo decrecientemente los savings.
    std::sort(savings.begin(), savings.end(), savingCompare);
    //Mergeo las rutas comenzando por las que mas ahorro me dan.
    while(savings.size()>0){
        route rutaA=routes[savings[0].i];
        route rutaB=routes[savings[0].j];
        route r = combinarRuta(matrizDeAdyacencia,rutaA,rutaB);
        routes.push_back(r);
        routes[savings[0].i].ruta.clear();
        routes[savings[0].j].ruta.clear();
        recomputeSavings(matrizDeAdyacencia,savings,routes,capacityTruck,savings[0].i,savings[0].j,routes.size()-1);
    }

    // Borro las rutas que quedaron vacias
    for (uint i = 0; i < routes.size(); i++)
    {
        if (routes[i].ruta.size() == 0)
        {
            routes.erase(routes.begin() + i);
            i--;
        }
    }
    return routes;
}



std::vector<std::vector <route> > getvecindad1Interchange(std::vector<route> solucion) {
    std::vector <std::vector<route>> res;
    for (int i = 0; i < solucion.size(); i++) {
        for (int j = i + 1; j < solucion.size(); j++) {

            for (int k = 1; k < solucion[i].ruta.size()-1; k++) {
                for (int l = 1; l < solucion[j].ruta.size()-1; l++) {


                	//Shifteos (1,0)
                	std::vector<route> sol1(solucion);  
                	sol1[j].ruta.insert(sol1[j].ruta.begin()+l,sol1[i].ruta[k]);
            		sol1[i].ruta.erase(sol1[i].ruta.begin()+k);

					for (uint x = 0; x < sol1.size(); x++)
					    {
					        if (sol1[x].ruta.size() < 3)
					        {
					            sol1.erase(sol1.begin() + x);
					            x--;
					        }
					}

					res.push_back(sol1);


                	//Shifteos (0,1)
                	std::vector<route> sol2(solucion);
                	sol2[i].ruta.insert(sol2[i].ruta.begin()+k,sol2[j].ruta[l]);
               		sol2[j].ruta.erase(sol2[j].ruta.begin()+l);

					for (uint x = 0; x < sol2.size(); x++)
					    {
					        if (sol2[x].ruta.size() < 3)
					        {
					            sol2.erase(sol2.begin() + x);
					            x--;
					        }
					}

					res.push_back(sol2);


                	//Shifteos (1,1)
                    std::vector <route> sol3(solucion);
                    int aux = sol3[i].ruta[k];
                    sol3[i].ruta[k] = sol3[j].ruta[l];
                    sol3[j].ruta[l] = aux;
                    res.push_back(sol3);
                }

            }
        }
    }
    return res;
}

int main(int argc, char *argv[])
{
	// Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(std::cin);

    int R = 10;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);//Para generar el random

    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();

    std::vector<route> solucionInicial = savings(tspInstance);//funcion que hace todo lo que esta en el main de savings basicamente,
    														  //usa las que estan arriba copiadas
    double cInicial = calcularCosto(matrizDeAdyacencia, solucionInicial);
    double delta,deltamax,deltamin;
    deltamin = 0;
    deltamax = 0;

    std::vector<std::vector <route> > vecindad = getvecindad1Interchange(solucionInicial);


 //    imprimirSolucion(matrizDeAdyacencia,solucionInicial);
 //    for(int i =0;i<21;i++) {
	// 	imprimirSolucion(matrizDeAdyacencia,vecindad[i]);

	// }


    //Recorro la vecindad para conseguir los parametros iniciales
    for(int i =0;i<vecindad.size();i++) {
        delta = calcularCosto(matrizDeAdyacencia,vecindad[i]) - cInicial;

        if(delta > deltamax) deltamax = delta;

        if(delta < deltamin) deltamin = delta;

    }

    double gamma = matrizDeAdyacencia.size();
    double alpha = gamma * vecindad.size();

    int k = 1;

    std::vector<route> solucionMejor = solucionInicial;
    std::vector<route> solucionActual = solucionInicial;

    imprimirSolucion(matrizDeAdyacencia, solucionInicial);

    while(R != 0){//Los resets, puede pasarse despues a una variable de entrada
        double tempActual = deltamax;
        while(tempActual >= deltamin) {

            vecindad = getvecindad1Interchange(solucionActual);
            for (int i = 0; i < vecindad.size(); i++) {
                delta = calcularCosto(matrizDeAdyacencia, vecindad[i]) - calcularCosto(matrizDeAdyacencia, solucionActual);

                // if ((delta <= 0)) { //Esto seria Hill Climbing directamente, y funciona
                if ((delta <= 0) or (delta > 0 and ((exp(-delta / tempActual)) >= dist(mt)))) { // SA, pero no funciona bien
                    // S = S'
                    solucionActual = vecindad[i];

                    if (calcularCosto(matrizDeAdyacencia, solucionActual) < calcularCosto(matrizDeAdyacencia, solucionMejor)) {
                        //Sb = S'
                        solucionMejor = solucionActual;
                    }
	                i = vecindad.size();
                }
            }

            //Actualizo parametros de temperatura
            double beta = (deltamax - deltamin) / ((alpha + gamma * sqrt(k)) * (deltamin * deltamax));
            tempActual = tempActual / (1 + beta * tempActual);
            k++;

        }
        R--;
    }


    imprimirSolucion(matrizDeAdyacencia,solucionMejor);

    return 0;
}