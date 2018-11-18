#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "savings.hpp"
#include "../aux.hpp"


// Genera un vector de savings que consisten en los ahorros posibles de combinar rutas tales que no excedan la capacidad del camion
// Complejidad: O(N^2)
std::vector<saving> computeSavingsSolucionInicial(std::vector<std::vector<double>> &matriz,std::vector<route>& routes,uint capacityTruck)
{
    std::vector<saving> savings;
    for (uint i = 0; i < routes.size(); i++)
    {
        for (uint j = i+1; j < routes.size(); j++)
        {
            // Me fijo si uniendo las rutas no me paso de la capacidad del camion.
            if(routes[i].capacityRoute+routes[j].capacityRoute < capacityTruck){
                // Creo la ruta nueva combinando las dos rutas de la mejor manera
                route rutaNueva = obtenerLaMejorCombinacionDeRutas(matriz,routes[i],routes[j]);
                // Obtengo el costo de la nueva ruta
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
    std::vector<saving> nuevosSavings;
    //Copio solo los savings que tienen sentido
    //Savings puede tener tamaño n^2 y push_back cuesta O(1)
    for(uint i=0;i<savings.size();i++){
        if(savings[i].i != rutaViejaA && savings[i].i !=rutaViejaB && savings[i].j != rutaViejaA && savings[i].j !=rutaViejaB){
            nuevosSavings.push_back(saving(savings[i].i, savings[i].j,savings[i].sav));
        }
    }

    //Veo si existen savings nuevos con la ruta nueva.
    for (uint j = 0; j < routes.size(); j++)
    {
        // Me fijo de que no sea la misma ruta y que la ruta exista
        if(j != rutaMergeada && routes[j].ruta.size()>0){
            //Me fijo que al unirlas no se exceda de la capacidad del camion.
            if(routes[rutaMergeada].capacityRoute+routes[j].capacityRoute < capacityTruck){

                route rutaNueva = obtenerLaMejorCombinacionDeRutas(matriz,routes[rutaMergeada],routes[j]);
                double costoNuevaRuta=rutaNueva.distancia;
                double costoSumaDeRutas=routes[rutaMergeada].distancia + routes[j].distancia;
                if(costoNuevaRuta < costoSumaDeRutas)
                {
                    //Inserto ordenado en O(N)
                    nuevosSavings.insert(std::upper_bound( nuevosSavings.begin(), nuevosSavings.end(), saving(rutaMergeada, j,costoNuevaRuta), savingCompare ),saving(rutaMergeada, j,costoNuevaRuta));
                    
                }
            }
        }
        
    }
    savings = nuevosSavings;
}

// Crea la solucion inicial que consiste en rutas : deposito-cliente-deposito
// Complejidad: O(N)
std::vector<route> crearSolucionInicialSavings(std::vector<std::vector<double>>& matriz, std::vector<uint>& demand, uint indiceDeposito)
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





void printSavings(std::vector<saving> &savings)
{
    for (uint i = 0; i < savings.size(); i++)
    {
        std::cout << "Saving " << i << ":" << std::endl;
        std::cout << "   i=" << savings[i].i << std::endl;
        std::cout << "   j=" << savings[i].j << std::endl;
        std::cout << "   saving=" << savings[i].sav << std::endl;
        std::cout << std::endl;
    }
}

void ejecutarSavings(std::vector<std::vector<double>>& matrizDeAdyacencia,uint indiceDeposito,uint capacityTruck,std::vector<route>& routes,std::vector<uint>& demanda){
    // Creo la solucion inicial.
    // La solucion consistira en rutas basicas que consisten en un camion por cliente.
    // Cada ruta es deposito-cliente-deposito
    routes = crearSolucionInicialSavings(matrizDeAdyacencia, demanda, indiceDeposito); //O(N)

    // Calculo cuanto me puedo ahorrar uniendo rutas. Solo guardo las que me generan un ahorro real.
    std::vector<saving> savings = computeSavingsSolucionInicial(matrizDeAdyacencia,routes,capacityTruck); //O(N^2)

    // Ordendo decrecientemente los savings.
    std::stable_sort(savings.begin(), savings.end(), savingCompare); //O(N (LOG_2(N))^2)
    
    //Mergeo las rutas comenzando por las que mas ahorro me dan.
    while(savings.size()>0){ //O(N^3) total. Ya que a por cada mergeo tengo una ruta menos, por lo tanto como mucho iterare cantidad de rutas veces que comienzan siendo N
        route rutaA=routes[savings[0].i];
        route rutaB=routes[savings[0].j];
        route r = obtenerLaMejorCombinacionDeRutas(matrizDeAdyacencia,rutaA,rutaB); //O(N)
        routes.push_back(r);
        routes[savings[0].i].ruta.clear();
        routes[savings[0].j].ruta.clear();
        recomputeSavings(matrizDeAdyacencia,savings,routes,capacityTruck,savings[0].i,savings[0].j,routes.size()-1);//O(N^2)
    }
}