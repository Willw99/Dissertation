//
// Created by wmw13 on 05/02/2021.
//
#include<cmath>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<cstring>
#include<math.h>
#include<fstream>
#include<limits.h>

#include "randomHeuristic.h"
#include "LocalSearches.h"

void randomHeuristic(){
    localSearch* LS = new localSearch(3,3);
    /*generate a random solution for the random heuristic*/
    int i,help, object, tot_assigned =0;
    int *r;
    double energy_temp = 0.0;
    double capacity_temp = 0.0;
    int from, to, temp;
    int charging_station;

    r = new int[NUM_OF_CUSTOMERS+1];
    //set indexes of objects
    for(i = 1; i <= NUM_OF_CUSTOMERS; i++){
        r[i-1]=i;

    }
    //randomly change indexes of objects
    for(i = 0; i <= NUM_OF_CUSTOMERS; i++){
        object = (int) ((rand()/(RAND_MAX+1.0)) * (double)(NUM_OF_CUSTOMERS-tot_assigned));
        help = r[i];
        r[i]=r[i+object];
        r[i+object]=help;
        tot_assigned++;
    }

    LS->randomPheromoneLocalSearchWithTwoOpt(r);
    double val = LS->getRouteLength(r);
//    best_sol->steps = 0;
//    best_sol->tour_length = INT_MAX;
//
//    best_sol->tour[0] = DEPOT;
//    best_sol->steps++;
//
//    i = 0;
//    while(i < NUM_OF_CUSTOMERS) {
//        from = best_sol->tour[best_sol->steps-1];
//        to = r[i];
//        if((capacity_temp + get_customer_demand(to)) <= MAX_CAPACITY && energy_temp+get_energy_consumption(from,to) <= BATTERY_CAPACITY){
//            capacity_temp  += get_customer_demand(to);
//            energy_temp += get_energy_consumption(from,to);
//            best_sol->tour[best_sol->steps] = to;
//            best_sol->steps++;
//            i++;
//        } else if ((capacity_temp + get_customer_demand(to)) > MAX_CAPACITY){
//            capacity_temp = 0.0;
//            energy_temp = 0.0;
//            best_sol->tour[best_sol->steps] = DEPOT;
//            best_sol->steps++;
//        } else if (energy_temp+get_energy_consumption(from,to) > BATTERY_CAPACITY){
//            charging_station = rand() % (ACTUAL_PROBLEM_SIZE-NUM_OF_CUSTOMERS-1)+NUM_OF_CUSTOMERS+1;
//            if(is_charging_station(charging_station)==true){
//                energy_temp = 0.0;
//                best_sol->tour[best_sol->steps] =  charging_station;
//                best_sol->steps++;
//            }
//        } else {
//            capacity_temp = 0.0;
//            energy_temp = 0.0;
//            best_sol->tour[best_sol->steps] =  DEPOT;
//            best_sol->steps++;
//        }
//    }
//
//    //close EVRP tour to return back to the depot
//    if(best_sol->tour[best_sol->steps-1]!=DEPOT){
//        best_sol->tour[best_sol->steps] = DEPOT;
//        best_sol->steps++;
//    }
//
//    best_sol->tour_length = fitness_evaluation(best_sol->tour, best_sol->steps);


    //free memory
    delete[] r;
    delete LS;
}