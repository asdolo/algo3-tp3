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

std::string variableVecindad="1InterchangeRandom";
unsigned int demandaRuta (std::vector<unsigned int> ruta, TSPLibInstance tspInstance){
    uint total = 0;
    for(unsigned int i = 0; i<ruta.size();i++){
        total += tspInstance.demand[ruta[i]];
    }
    return total;
}


std::vector<std::vector <route> > getVecindad1Interchange(std::vector<route> solucion, TSPLibInstance tspInstance) {
    std::vector <std::vector<route>> res10,res01,res11;
    for (uint i = 0; i < solucion.size(); i++) {
        for (uint j = i + 1; j < solucion.size(); j++) {

            for (uint k = 1; k < solucion[i].ruta.size()-1; k++) {
                for (uint l = 1; l < solucion[j].ruta.size()-1; l++) {//O(n^2 * ...)


//                	Shifteos (1,0)
                    std::vector<route> sol1(solucion);//O(n)
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
    res01.insert(res01.end(),res10.begin(),res10.end());
    res01.insert(res01.end(),res11.begin(),res11.end());
    return res01;
}

bool compararTuplas0(std::tuple<double,int,int> a,std::tuple<double,int,int> b){
    return std::get<0>(a) < std::get<0>(b);
}

bool compararTuplas2(std::tuple<double,int,int> a,std::tuple<double,int,int> b){
    return std::get<2>(a) < std::get<2>(b);
}


std::vector<std::vector <route> > getVecindad1InterchangeRandom(std::vector<route> solucion ,TSPLibInstance tspInstance) {
    std::vector <std::vector<route>> res;
    res = getVecindad1Interchange(solucion,tspInstance);
    std::random_shuffle ( res.begin(), res.end());
    return res;
}

bool esMejores(std::vector<std::tuple<double,int,int> > v,int ruta,int camino){
    for(uint i = 0; i< v.size();i++){
        if(std::get<1>(v[i]) == ruta and (std::get<2>(v[i]) == camino or std::get<2>(v[i])+1 == camino)){
            return true;
        }
    }
    return false;
}

int mejorPosicion(std::vector<unsigned int> v,int c,std::vector<std::vector<double> > matriz){
    double costominimo = INT_MAX;
    int mejorPosicion = 1;
    for(uint i=1;i<v.size()-1;i++) {
        std::vector<unsigned int> ruta (v);
        ruta.insert(ruta.begin()+i,c);
        double costo = 0;
        for (uint j = 0; j < ruta.size() - 1; j++) {
            costo += matriz[ruta[j]][ruta[j + 1]];
        }

        if(costo < costominimo){
            costominimo = costo;
            mejorPosicion = i;
        }
    }
    return mejorPosicion;
}

std::vector<std::vector<route> > getVecindadMoveHighestAverage(std::vector<route > solucion, TSPLibInstance tspInstance){
    std::vector <std::vector<route>> res;
    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generador(seed);
    std::uniform_int_distribution<int> random(0,INT_MAX);

    std::vector<std::tuple<double,int,int> > caminosMayorPromedio;

    for(uint i=0;i<solucion.size();i++){ //O(n)
        for(uint j=1; j<solucion[i].ruta.size()-1 ; j++) {
            std::tuple <double,int,int> elem (((matrizDeAdyacencia[solucion[i].ruta[j]][solucion[i].ruta[j-1]] + matrizDeAdyacencia[solucion[i].ruta[j]][solucion[i].ruta[j+1]] )/2)
                    ,i,j);
            caminosMayorPromedio.push_back(elem);
        }
    }

    std::sort(caminosMayorPromedio.begin(),caminosMayorPromedio.end(),compararTuplas0); //n*log n

    std::reverse(caminosMayorPromedio.begin(),caminosMayorPromedio.end());//n

    caminosMayorPromedio.erase(caminosMayorPromedio.begin()+5,caminosMayorPromedio.end());

    std::sort(caminosMayorPromedio.begin(),caminosMayorPromedio.end(),compararTuplas2);

    std::reverse(caminosMayorPromedio.begin(),caminosMayorPromedio.end());


    for(uint i =0;i<caminosMayorPromedio.size();i++){//O(1)????
        unsigned int aux = solucion[std::get<1>(caminosMayorPromedio[i])].ruta[std::get<2>(caminosMayorPromedio[i])];
        solucion[std::get<1>(caminosMayorPromedio[i])].ruta.erase(solucion[std::get<1>(caminosMayorPromedio[i])].ruta.begin()+std::get<2>(caminosMayorPromedio[i]));
        bool termine = false;
        while(!termine) {
            int ruta = random(generador) % (int) solucion.size();
            if (demandaRuta(solucion[ruta].ruta, tspInstance) + tspInstance.demand[aux] < tspInstance.capacity) {
                int j = mejorPosicion(solucion[ruta].ruta,aux,tspInstance.getTSPGraph());
                solucion[ruta].ruta.insert(solucion[ruta].ruta.begin()+j,aux);
                termine = true;
            }
        }
    }


    for (uint x = 0; x < solucion.size(); x++)//O(n)
    {
        if (solucion[x].ruta.size() < 3)
        {
            solucion.erase(solucion.begin() + x);
            x--;
        }
    }



    std::vector<std::tuple<double,int,int> > mejoresCaminos;

    for(uint i =0 ; i<solucion.size();i++){ //O(n)
        for(uint j=0; j<solucion[i].ruta.size()-1 ; j++){
            std::tuple <double,int,int> elem (matrizDeAdyacencia [solucion[i].ruta[j]][solucion[i].ruta[j+1]],i,j);
            mejoresCaminos.push_back(elem);
        }
    }

    std::sort(mejoresCaminos.begin(),mejoresCaminos.end(),compararTuplas0);

    mejoresCaminos.erase(mejoresCaminos.begin()+5,mejoresCaminos.end());

    for(uint i = 0; i < 5 ; i++){

        int ruta = random(generador) % (int) solucion.size();
        int camino = random(generador) % ((int) solucion[ruta].ruta.size()-2) + 1 ;

        if(esMejores(mejoresCaminos,ruta,camino)){
            i--;
            continue;
        }

        unsigned int aux = solucion[ruta].ruta[camino];
        solucion[ruta].ruta.erase(solucion[ruta].ruta.begin()+camino);

        bool termine = false;
        while(!termine){
            int ruta2 = random(generador) % (int) solucion.size();
            if(demandaRuta(solucion[ruta2].ruta,tspInstance) + tspInstance.demand[aux] < tspInstance.capacity){
                int camino2 = random(generador)%((int) solucion[ruta2].ruta.size()-1) + 1;
                if(esMejores(mejoresCaminos,ruta2,camino2)){
                    continue;
                }
                solucion[ruta2].ruta.insert(solucion[ruta2].ruta.begin()+camino2,aux);
                termine = true;
            }
        }
    }
    res.push_back(solucion);
    return res;
}



std::vector<std::vector <route> > getVecindad(std::vector<route> solucion, TSPLibInstance tspInstance,std::string variableVecindad) {
    std::vector<std::vector <route> > res;
    if(variableVecindad == "1Interchange"){ 
        return getVecindad1Interchange(solucion,tspInstance);
    }else if(variableVecindad == "1InterchangeRandom") {
        return getVecindad1Interchange(solucion,tspInstance);
    }else{
        return getVecindadMoveHighestAverage(solucion,tspInstance);
    }

}

void ejecutarSimulatedAnnealing(TSPLibInstance& tspInstance,std::vector<route>& solucionMejor,int R){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);//Para generar el random
    std::vector<std::vector<double>> matrizDeAdyacencia = tspInstance.getTSPGraph();
    std::vector<route> solucionInicial;
    std::vector<uint> demanda = tspInstance.demand;
    ejecutarSavings(matrizDeAdyacencia,tspInstance.depot[0],tspInstance.capacity,solucionInicial,demanda);

    for (uint i = 0; i < solucionInicial.size(); i++)
    {
        if (solucionInicial[i].ruta.size() == 0)
        {
            solucionInicial.erase(solucionInicial.begin() + i);
            i--;
        }
    }

//    imprimirSolucion(matrizDeAdyacencia,solucionInicial);
    //usa las que estan arriba copiadas
    // double cInicial = calcularCosto(matrizDeAdyacencia, solucionInicial);
    double delta,deltamax,deltamin;
    deltamin = 0;
    deltamax = 0;


    std::vector<std::vector <route> > vecindad = getVecindad(solucionInicial,tspInstance,variableVecindad);

    //Recorro la vecindad para conseguir los parametros iniciales
    // for(uint i =0;i<vecindad.size();i++) {
    //     delta = calcularCosto(matrizDeAdyacencia,vecindad[i]) - cInicial;

    //     if(fabs(delta)> deltamax) deltamax = fabs(delta);

    //     if(delta != 0 and fabs(delta)< deltamin) deltamin = fabs(delta);

    // }
    double tempInicial = deltamax;              //Metodo 1 de temp
    double tempFinal = deltamin;                //

    double nFeas = vecindad.size();             //
    double gamma = matrizDeAdyacencia.size();   //
    double alpha = gamma * nFeas;               //

    int k = 1;

   tempInicial = calcularCosto(matrizDeAdyacencia,solucionInicial)*0.2;    //Metodo 2 de temp
   tempFinal = 0.001;
   alpha = 0.95;

    solucionMejor = solucionInicial;
    std::vector<route> solucionActual = solucionInicial;

    while(R != 0){
        double tempActual = tempInicial;
        k = 1;
        while(tempActual >= tempFinal) {

            vecindad = getVecindad(solucionActual, tspInstance, variableVecindad);
            for (unsigned long i = 0; i < vecindad.size(); i++) {
                delta = calcularCosto(matrizDeAdyacencia, vecindad[i]) -
                        calcularCosto(matrizDeAdyacencia, solucionActual);

                if ((delta <= 0) or
                    (delta > 0 and ((exp(-delta / tempActual)) > dist(mt)))) { // SA, pero no funciona bien
                    // S = S'
                    solucionActual = vecindad[i];

                    if (calcularCosto(matrizDeAdyacencia, solucionActual) <
                        calcularCosto(matrizDeAdyacencia, solucionMejor)) {
                        //Sb = S'
                        solucionMejor = solucionActual;
                    }
                    i = vecindad.size();
                }


//            Actualizo parametros de temperatura
            // double beta = (tempInicial - tempFinal) / ((alpha + gamma * sqrt(k)) * (tempInicial * tempFinal)); //1
            // tempActual = tempActual / (1 + beta * tempActual);

           tempActual = tempActual * alpha;//2

            k++;

            if(tempActual < tempFinal) break;

            }
        }
//        std::cout << k << std::endl;
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
    int R = argc >= 6 ? std::stoi(argv[5]) : 3;
    int tipoVecindad = argc >= 7 ? std::stoi(argv[6]) : 1;
    if(tipoVecindad==1){
        variableVecindad="1Interchange";
    }else if(tipoVecindad==3){
        variableVecindad="MoveHighestAverage";
    }
    
//    // Creo una nueva instancia de TSPLIB a partir de lo que venga por stdin
    TSPLibInstance tspInstance(std::cin);
//
//
//
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