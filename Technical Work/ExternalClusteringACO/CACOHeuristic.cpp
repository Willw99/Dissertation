#include "CACOHeuristic.h"
#include "CACO.h"
/*
 * Creates and configures CACO object using the created variables.
 * Runs optimise function to find a local optimum.
 * Adds local optimum as best solution.
 *
 * CACO updates pheromones based on length of  total route of customers, charging stations, and depots
 * rather than just the route of the customers.
 */
void CACOHeuristic(){
    int numAnts= 8, iterations = 500, probabilityArraySize = 2, twoOptIteration = 3,randomSearchIteration = 3;
    double pheromoneDecrease = 0.98, Q = 1,alpha = 0.6, beta=0.6;
    auto* a = new CACO(numAnts,pheromoneDecrease,Q,probabilityArraySize,alpha,beta,twoOptIteration,randomSearchIteration);

    a->optimize(iterations);
    int * route = a->returnResults();

    a->getRL(route);

    delete a;
}

