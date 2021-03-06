#include "GeneticAlgorithm.h"
#include "../Framework/stats.hpp"

/*
 * ================================================================================ *
 * GENETIC ALGORITHM.
 * ================================================================================ *
 */
#include "../ExternalClusteringACO/CACO.h"

/*
 * Genetic Algorithm Constructor.
 */
GeneticAlgorithm::GeneticAlgorithm(int SizeOfPopulation, int Generations, int NumMutations) {
    LS = new localSearch(3, 3);

    sizeOfPopulation = SizeOfPopulation;
    generations = Generations;
    probabiltyOfMutation = NumMutations;
    childPopulationCounter = 0;

    parentPopulation = new int *[sizeOfPopulation];
    childPopulation = new int *[sizeOfPopulation + sizeOfPopulation];
    for (int populationIndex = 0; populationIndex < sizeOfPopulation; ++populationIndex) {
        parentPopulation[populationIndex] = nullptr;
    }

    for (int populationIndex = 0; populationIndex < sizeOfPopulation + sizeOfPopulation; ++populationIndex) {
        childPopulation[populationIndex] = nullptr;
    }
}

/*
 * Genetic Algorithm Destructor.
 */
GeneticAlgorithm::~GeneticAlgorithm() {
    for (int i = 0; i < sizeOfPopulation; ++i) {
        delete[] parentPopulation[i];
    }
    delete[] parentPopulation;
    for (int i = 0; i < sizeOfPopulation+sizeOfPopulation; ++i) {
        delete[] childPopulation[i];
    }
    delete[] childPopulation;
    delete LS;
}

/*
 * Updates the best route by calling generate tour.
 */
void GeneticAlgorithm::checkSolution() {
    GenerateTour::getRouteLength(parentPopulation[0]);
}

/*
 * Generates a random route of all the customers.
 */
void GeneticAlgorithm::randomRoute(int *route) {
    auto tempRoute = std::vector<int>();
    //Creates list of customers.
    for (int i = 0; i <= NUM_OF_CUSTOMERS; ++i)
        tempRoute.push_back(i);

    int randIndex;
    for (int i = 0; i <= NUM_OF_CUSTOMERS; ++i) {
        randIndex = rand() % tempRoute.size();
        route[i] = tempRoute[randIndex];
        tempRoute.erase(tempRoute.begin() + randIndex);
    }

    //Local Search to improve starting population.
    LS->randomPheromoneLocalSearchWithTwoOpt(route);
}

/*
 * Clustered ACO for optimal starting population generation.
 */
int* GeneticAlgorithm::getCACO(){
    srand (rand());
    KMeansClustering::createClusters(4);
    //Minimal parameters chosen as optimality of route was not important, but runtime was
    int numAnts= 3, iterations = 5, probabilityArraySize = 2, twoOptIteration = 3,randomSearchIteration = 3;
    double pheromoneDecrease = 0.9, Q = 1,alpha = 0.6, beta=2.1;
    auto* a = new CACO(numAnts,pheromoneDecrease,Q,probabilityArraySize,alpha,beta,twoOptIteration,randomSearchIteration);

    a->optimize(iterations);

    int* temp = a->returnResults();

    //Local search used to locate optimal route.
    LS->twoOptLocalPheromoneAddonSearch(temp); //BEST
//    LS->randomPheromoneLocalSearchWithTwoOpt(temp);
//    LS->LKSearch(temp);
    delete a;
    KMeansClustering::freeClusters();
    return temp;
}

/*
 * Creates a random population of children which are then selected to be parents.
 */
void GeneticAlgorithm::generateStartingPopulation() {
    //Uncomment to use the different populations.
    /*
     * Random Starting Population.
     */
//    for (int populationIndex = 0; populationIndex < sizeOfPopulation + sizeOfPopulation; ++populationIndex) {
//        childPopulation[populationIndex] = new int[NUM_OF_CUSTOMERS+1];
//        randomRoute(childPopulation[populationIndex]);
//        childPopulationCounter++;
//    }
//    selectChildrenForParents();


    /*
     * Clustered ACO starting population.
     */
    for (int i = 0; i < sizeOfPopulation; ++i) {
        parentPopulation[i] = getCACO();
    }
}

/*
 * Displays the current populations.
 */
void GeneticAlgorithm::displayPopulation() {
    printf("-- Parent Population --\n");
    for (int populationIndex = 0; populationIndex < sizeOfPopulation; ++populationIndex) {
        printf("Population %d:\n", populationIndex + 1);
        for (int customer = 0; customer <= NUM_OF_CUSTOMERS; ++customer)
            printf("%d, ", parentPopulation[populationIndex][customer]);
        printf("\n");
    }

}

/*
 * Frees routes from a population.
 */
void GeneticAlgorithm::deleteSegmentOfArray(int **population, int begin, int end) {
    for (int popCounter = end-1; popCounter >= begin; --popCounter) {
        delete[] population[popCounter];
    }
}

/*
 * Determines the best route within the parent population.
 */
std::pair<int *, int> GeneticAlgorithm::getBestRoute() {
    int *CB, CBCount;
    double CBLength = INT_MAX, CLength;
    for (int popCounter = 0; popCounter < sizeOfPopulation; ++popCounter) {
        CLength = GenerateTour::getBasicLength(parentPopulation[popCounter]);
        if (CBLength > CLength) {
            CBLength = CLength;
            CB = parentPopulation[popCounter];
            CBCount = popCounter;
        }
    }
    return {CB, CBCount};
}

/*
 * Main body of the genetic algorithm, calls the Crossover, mutation, selection and repair operators.
 */
void GeneticAlgorithm::runGenerations() {
    for (int x = 1; x <= generations; ++x) {
        childPopulationCounter = 0;
        crossoverOperator();
        //randomMutateChildren() is commented out when using PX or GPX.
        randomMutateChildren();
        selectChildrenForParents();
        addRunDataToFile(x,best_sol->tour_length);
        //repairParents();
    }
}

/*
 * Calls the different crossover operators to generate children from the parents.
 */
void GeneticAlgorithm::crossoverOperator() {
    childPopulationCounter = 0;
    for (int recombineCounter = 1; recombineCounter < sizeOfPopulation; ++recombineCounter) {

        //SELECT CROSSOVER OPERATOR BY UNCOMMENTING.
        //GPX:
//        int** tempChildren = CrossoverOperators::GPCRecombine(parentPopulation[0], parentPopulation[recombineCounter]);
        //PX:
//     int** tempChildren = CrossoverOperators::PCRecombine(parentPopulation[0], parentPopulation[recombineCounter]);
        //Test Recombination -- Debugging:
//      int** tempChildren = CrossoverOperators::testRecombination(parentPopulation[0],parentPopulation[recombineCounter]);
        //PMX:
        int** tempChildren = CrossoverOperators::partiallyMappedCrossover(parentPopulation[0],parentPopulation[recombineCounter]);

        //Add the generated children to the children population.
        childPopulation[childPopulationCounter] = new int[NUM_OF_CUSTOMERS+1];
        for (int i = 0; i <= NUM_OF_CUSTOMERS; ++i) {
            childPopulation[childPopulationCounter][i] = tempChildren[0][i];
        }
        childPopulationCounter++;

        if(tempChildren[1][0] != INT_MAX) {
            childPopulation[childPopulationCounter] = new int[NUM_OF_CUSTOMERS+1];
            for (int i = 0; i <= NUM_OF_CUSTOMERS; ++i) {
                childPopulation[childPopulationCounter][i] = tempChildren[1][i];
            }
            childPopulationCounter++;
        }


        delete[] tempChildren[0]; delete[] tempChildren[1]; delete[] tempChildren;
    }
    if(childPopulationCounter < sizeOfPopulation) {
        childPopulation[childPopulationCounter++] = getCACO();
    }
}

/*
 * Uses a selection operator to select viable children to be parents.
 */
void GeneticAlgorithm::selectChildrenForParents() {
    for (int i = 0; i < sizeOfPopulation; ++i) {
        delete[] parentPopulation[i];
        parentPopulation[i] = nullptr;
    }

    //Select different selection operators by uncommenting.
    //Truncation Selection:
    int** tempParentPopulation = Selection::greedySelection(childPopulation,childPopulationCounter,sizeOfPopulation);
    //Correlated family-based Selection:
//        int** tempParentPopulation = Selection::correlativeFamilyBasedSelection(childPopulation,childPopulationCounter,sizeOfPopulation);

    for (int i = 0; i < sizeOfPopulation; ++i) {
        parentPopulation[i] = new int[NUM_OF_CUSTOMERS+1];
        for (int j = 0; j <= NUM_OF_CUSTOMERS; ++j) {
            parentPopulation[i][j] = tempParentPopulation[i][j];
        }
        delete[] tempParentPopulation[i];
    }
    delete[] tempParentPopulation;

    childPopulationCounter = 0;
}

/*
 * Uses a repair operator to repair issues (e.g. duplicate customers) within the parent routes.
 */
void GeneticAlgorithm::repairParents(){
    for (int i = 0; i < sizeOfPopulation; ++i)
        RepairOperators::basicRepair(parentPopulation[i]);
}

/*
 * Uses a mutation operator to mutate the children population based upon a predetermined probability.
 * This helps to reduce the Genetic Algorithm from getting stuck in local optimums.
 */
void GeneticAlgorithm::randomMutateChildren(){
    for (int i = 0; i < childPopulationCounter; ++i) {
        int toMutate = rand()%probabiltyOfMutation;
        if(toMutate == 0) {
            //Mutation Operators, uncomment to select different operators.
            //Local Search Mutation:
            LS->randomPheromoneLocalSearchWithTwoOpt(childPopulation[i]);
            //Random Swap Mutation:
//            Mutation::randomSwapMutation(childPopulation[i]);
            //Lin-Kernighan Mutation:
//            Mutation::LKMutation(childPopulation[i],LS);
        }
    }
}
