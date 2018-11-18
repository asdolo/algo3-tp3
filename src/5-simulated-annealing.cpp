#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <string>
#include <chrono>
#include <numeric>
#include "tsplib-helper/instance.hpp"
#include "aux.hpp"
#include "savings/savings.hpp"

unsigned int demandaRuta (std::vector<unsigned int> ruta, TSPLibInstance tspInstance){
    uint total = 0;
    for(unsigned int i = 0; i<ruta.size();i++){
        total += tspInstance.demand[ruta[i]];
    }
    return total;
}



std::vector<std::vector <route> > getvecindad1Interchange(std::vector<route> solucion, TSPLibInstance tspInstance) {
    std::vector <std::vector<route>> res10,res01,res11;
    for (uint i = 0; i < solucion.size(); i++) {
        for (uint j = i + 1; j < solucion.size(); j++) {

            for (uint k = 1; k < solucion[i].ruta.size()-1; k++) {
                for (uint l = 1; l < solucion[j].ruta.size()-1; l++) {


//                	Shifteos (1,0)
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

					bool esValida = true;

                    for (uint x = 0; x < sol1.size(); x++){
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
                    for (uint x = 0; x < sol2.size(); x++){
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
                    for (uint x = 0; x < sol3.size(); x++){
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




std::vector<std::vector <route> > getvecindad1Interchange(std::vector<route> solucion) {
    std::vector <std::vector<route>> res;
    for (uint i = 0; i < solucion.size(); i++) {
        for (uint j = i + 1; j < solucion.size(); j++) {

            for (uint k = 1; k < solucion[i].ruta.size()-1; k++) {
                for (uint l = 1; l < solucion[j].ruta.size()-1; l++) {


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

void ejecutarSimulatedAnnealing(TSPLibInstance& tspInstance,std::vector<route>& solucionMejor,int R){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);//Para generar el random
    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();
    std::vector<route> solucionInicial;
    std::vector<uint> demanda = tspInstance.demand;
    ejecutarSavings(matrizDeAdyacencia,tspInstance.depot[0],tspInstance.capacity,solucionInicial,demanda);
    														  //usa las que estan arriba copiadas
    double cInicial = calcularCosto(matrizDeAdyacencia, solucionInicial);
    double delta,deltamax,deltamin;
    deltamin = 0;
    deltamax = 0;

    std::vector<std::vector <route> > vecindad = getvecindad1Interchange(solucionInicial);

    //Recorro la vecindad para conseguir los parametros iniciales
    for(uint i =0;i<vecindad.size();i++) {
        delta = calcularCosto(matrizDeAdyacencia,vecindad[i]) - cInicial;

        if(delta > deltamax) deltamax = delta;

        if(delta < deltamin) deltamin = delta;

    }
    double tempInicial = deltamax;
    double tempFinal = deltamin;

    double gamma = matrizDeAdyacencia.size();
    double alpha = gamma * vecindad.size();

    int k = 1;

    solucionMejor = solucionInicial;
    std::vector<route> solucionActual = solucionInicial;
    //double beta = 1.05; LO COMENTO PORQUE NO SE USA

    tempInicial = 5000;
    tempFinal = 0.001;

    alpha = 0.99;
    while(R != 0){
        double tempActual = tempInicial;
        while(tempActual >= tempFinal) {

            vecindad = getvecindad1Interchange(solucionActual,tspInstance);
            for (unsigned long i = 0; i < vecindad.size(); i++) {
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
}
int main(int argc, char *argv[])
{
    std::string archivoRutas;
    std::ofstream archivoTablaClusters;
    std::ofstream archivoTiempo;
    if(argc<2)system("mkdir -p output/5-simulated-anealing/");
    archivoRutas=argc >= 2 ? argv[1] : "output/5-simulated-anealing/rutas.csv";
    archivoTablaClusters.open(argc >= 3 ? argv[2] : "output/5-otra-clusterizacion/clusters.csv", std::ios::out | std::ios::trunc);
    archivoTiempo.open(argc >= 4 ? argv[3] : "output/5-simulated-anealing/tiempo.csv", std::ios_base::app);
    int cantidadRepeticiones = argc >= 5 ? std::stoi(argv[4]) : 1;
    int R = argc >= 6 ? std::stoi(argv[5]) : 10;
	// Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(std::cin);


    
    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();
    std::vector<route> solucionMejor;
    auto startTime = std::chrono::steady_clock::now();
    ejecutarSimulatedAnnealing(tspInstance,solucionMejor,R);
    auto endTime = std::chrono::steady_clock::now();
    //Calculo cuanto tiempo costo.
    auto diff = endTime - startTime;
    uint repeticiones = cantidadRepeticiones-1;
    while(repeticiones>0){
        // Ejecuto el algoritmo
        startTime = std::chrono::steady_clock::now();
        ejecutarSimulatedAnnealing(tspInstance,solucionMejor,R);
        endTime = std::chrono::steady_clock::now();
        
        //Calculo cuanto tiempo costo.
        diff += (endTime - startTime);
        repeticiones--;
    }
    diff/=cantidadRepeticiones;
    double costoTotal=calcularCosto(matrizDeAdyacencia, solucionMejor);
    //Imprimo los tiempos del algoritmo
    archivoTiempo << tspInstance.dimension;
    archivoTiempo << ",";
    archivoTiempo << std::chrono::duration <double, std::milli>(diff).count();
    archivoTiempo << ",";
    archivoTiempo << costoTotal;
    archivoTiempo << std::endl;
    archivoTiempo.close();

    // Guardamos los clusters en el archivo .csv correspondiente
    archivoTablaClusters << "id,x,y" << std::endl;
    for (uint i = 0; i < tspInstance.dimension; i++)
    {
        uint id = i + 1;
        double x = std::get<0>(tspInstance.nodeCoords[i]);
        double y = std::get<1>(tspInstance.nodeCoords[i]);

        archivoTablaClusters << id << "," << x << "," << y << std::endl;
    }
    archivoTablaClusters.close();


    imprimirSolucionTP(matrizDeAdyacencia,solucionMejor,archivoRutas);

    return 0;
}