#define COMPILANDO_SIMULATED_ANNEALING
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include "tsplib-helper/instance.hpp"
#include "aux.hpp"
#include <fstream>
#include "1-savings.cpp"


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

unsigned int demandaRuta (std::vector<unsigned int> ruta, TSPLibInstance tspInstance){
    uint total = 0;
    for(unsigned int i = 0; i<ruta.size();i++){
        total += tspInstance.demand[ruta[i]];
    }
    return total;
}



std::vector<std::vector <route> > getvecindad1Interchange(std::vector<route> solucion, TSPLibInstance tspInstance) {
    std::vector <std::vector<route>> res10,res01,res11;
    for (int i = 0; i < solucion.size(); i++) {
        for (int j = i + 1; j < solucion.size(); j++) {

            for (int k = 1; k < solucion[i].ruta.size()-1; k++) {
                for (int l = 1; l < solucion[j].ruta.size()-1; l++) {


//                	Shifteos (1,0)
                	std::vector<route> sol1(solucion);
                	sol1[j].ruta.insert(sol1[j].ruta.begin()+l,sol1[i].ruta[k]);
            		sol1[i].ruta.erase(sol1[i].ruta.begin()+k);

					for (int x = 0; x < sol1.size(); x++)
					    {
					        if (sol1[x].ruta.size() < 3)
					        {
					            sol1.erase(sol1.begin() + x);
					            x--;
					        }
					}

					bool esValida = true;

                    for (int x = 0; x < sol1.size(); x++){
                        if(demandaRuta(sol1[x].ruta,tspInstance) > tspInstance.capacity){
                            esValida = false;
                        }
                    }

                    if(esValida) res10.push_back(sol1);


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

					esValida = true;
                    for (int x = 0; x < sol2.size(); x++){
                        if(demandaRuta(sol2[x].ruta,tspInstance) > tspInstance.capacity){
                            esValida = false;
                        }
                    }

					if(esValida) res01.push_back(sol2);


                	//Shifteos (1,1)
                    std::vector <route> sol3(solucion);
                    unsigned int aux = sol3[i].ruta[k];
                    sol3[i].ruta[k] = sol3[j].ruta[l];
                    sol3[j].ruta[l] = aux;


                    esValida = true;
                    for (int x = 0; x < sol3.size(); x++){
                        if(demandaRuta(sol3[x].ruta,tspInstance) > tspInstance.capacity){
                            esValida = false;
                        }
                    }


                    if(esValida) res11.push_back(sol3);
                }

            }
        }
    }
//    res11.insert(res11.end(),res01.begin(),res01.end());
//    res11.insert(res11.end(),res10.begin(),res10.end());
//    return res11;


    res01.insert(res01.end(),res10.begin(),res10.end());
    res01.insert(res01.end(),res11.begin(),res11.end());
    return res01;
}

int main(int argc, char *argv[])
{
    std::ifstream in("/home/luca/Desktop/algo3-tp3/input/A/A-n33-k5.vrp");
    std::cin.rdbuf(in.rdbuf());

	// Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(std::cin);

    int R = 3;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);//Para generar el random

    unsigned int capacidad = tspInstance.capacity;

    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();

    std::vector<route> solucionInicial = savings(tspInstance);//funcion que hace todo lo que esta en el main de savings basicamente,
    														  //usa las que estan arriba copiadas
    double cInicial = calcularCosto(matrizDeAdyacencia, solucionInicial);
    double delta,deltamax,deltamin;
    deltamin = 0;
    deltamax = 0;

    std::vector<std::vector <route> > vecindad = getvecindad1Interchange(solucionInicial,tspInstance);


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

    double tempInicial = deltamax;
    double tempFinal = deltamin;

    double gamma = matrizDeAdyacencia.size();
    double alpha = gamma * vecindad.size();

    int k = 1;

    std::vector<route> solucionMejor = solucionInicial;
    std::vector<route> solucionActual = solucionInicial;

    imprimirSolucion(matrizDeAdyacencia, solucionInicial);



    double beta = 1.05;

    tempInicial = 5000;
    tempFinal = 0.001;

    alpha = 0.99;

    while(R != 0){//Los resets, puede pasarse despues a una variable de entrada
        double tempActual = tempInicial;
        while(tempActual >= tempFinal) {

            vecindad = getvecindad1Interchange(solucionActual,tspInstance);
            for (ulong i = 0; i < vecindad.size(); i++) {
                delta = calcularCosto(matrizDeAdyacencia, vecindad[i]) - calcularCosto(matrizDeAdyacencia, solucionActual);

//                 if ((delta <= 0)) { //Esto seria Hill Climbing directamente, y funciona
                if ((delta <= 0) or (delta > 0 and ((exp(-delta / tempActual)) > dist(mt)))) { // SA, pero no funciona bien
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
//            double beta = (tempInicial - tempFinal) / ((alpha + gamma * sqrt(k)) * (tempInicial * tempFinal));
//            tempActual = tempActual / (1 + beta * tempActual);
            k++;
            tempActual = tempActual * alpha;

        }
        R--;
    }


    imprimirSolucion(matrizDeAdyacencia,solucionMejor);

    return 0;
}