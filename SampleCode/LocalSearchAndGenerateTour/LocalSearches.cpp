#include "LocalSearches.h"

/*
 * ================================================================================ *
 * LOCAL SEARCHES FUNCTIONS
 * ================================================================================ *
 */

//Constructor, sets the number of iterations for each of the local searches.
localSearch::localSearch(int RandomSearchIteration, int TwoOptIterations) {
    randomSearchIteration = RandomSearchIteration;
    twoOptIterations = TwoOptIterations;
    for (int i = 0; i <= NUM_OF_CUSTOMERS; i++) {
        for (int j = i + 1; j <= NUM_OF_CUSTOMERS; j++) {
            localSearchPheromone[GenerateTour::getArcCode(i, j)] = 1;
        }
    }
    for (int i = 0; i < KMeansClustering::numOfClusters; i++) {
        for (int j = i + 1; j < KMeansClustering::numOfClusters; j++) {
            localSearchPheromoneCluster[GenerateTour::getArcCode(i, j)] = 1;
        }
    }
}

//Destructor for the Local Search class.
localSearch::~localSearch() {
}

std::pair<int,int>* localSearch::getIterations(){
    return new std::pair<int,int>(randomSearchIteration,twoOptIterations);
}

/*
 * Finds the closest neighbour to a customer.
 */
int localSearch::findNearestCustomer(int customer) {
    double shortestDist = INT_MAX;
    int nearestNeighbour = -1;
    for (int nearestCustomer = 0; nearestCustomer <= NUM_OF_CUSTOMERS; ++nearestCustomer) {
        if (nearestCustomer != customer) {
            double dist = get_distance(customer, nearestCustomer);
            if (dist < shortestDist) {
                shortestDist = dist;
                nearestNeighbour = nearestCustomer;
            }
        }
    }
    return nearestNeighbour;
}

/*
 * Gets the position of a customer within a route.
 */
int locatePosInRoute(int *bestRoute, int customer) {
    for (int findCustomer = 0; findCustomer <= NUM_OF_CUSTOMERS; ++findCustomer) {
        if (bestRoute[findCustomer] == customer)
            return findCustomer;
    }
    return -1;
}

/*
 * Moves a position marker to the right.
 */
int addToPos(int p) {
    int P;
    if (p + 1 <= NUM_OF_CUSTOMERS)
        P = p + 1;
    else
        P = 0;
    return P;
}

/*
 * Moves a position marker to the left.
 */
int subtractFromPos(int p) {
    int P;
    if (p - 1 >= 0)
        P = p - 1;
    else
        P = NUM_OF_CUSTOMERS;
    return P;
}

/*
 * Checks whether the swaps invalidates the route.
 */
int checkClosure(int *bestRoute, int p1, int p2, int p3, int p4) {
    if (p1 == p2 || p1 == p3 || p1 == p4 || p2 == p3 || p2 == p4 || p3 == p4)
        return 0;

    if ((p1 < p2 && p3 < p4) || (p1 > p2 && p3 > p4))
        return 0;
    else
        return 1;
}

/*
 * Gets the overall gain of the new swaps.
 */
double getGain(int *bestRoute, int p1, int p2, int p3, int p4) {
    double originalDist = get_distance(bestRoute[p1], bestRoute[p2]) + get_distance(bestRoute[p3], bestRoute[p4]);
    double newDist = get_distance(bestRoute[p1], bestRoute[p4]) + get_distance(bestRoute[p2], bestRoute[p3]);
    return originalDist - newDist;
}

/*
 * Constructs a route from swap points.
 */
std::vector<int> constructRoute(const int *bestRoute, int p1, int p2, int p3, int p4) {
    auto subRouteOne = std::vector<int>();
    auto subRouteTwo = std::vector<int>();

    if (p1 < p2) {
        //backwards to find p3 from p1
        //forwards to find p2 to p4
        int marker = p1, index = 0;
        int stop = addToPos(p3);
        while (marker != stop) {

            subRouteOne.push_back(bestRoute[marker]);

            marker--;
            if (marker == -1)
                marker = NUM_OF_CUSTOMERS;
        }
        subRouteOne.push_back(bestRoute[p3]);


        marker = p2, index = 0;
        stop = subtractFromPos(p4);
        while (marker != stop) {

            subRouteTwo.push_back(bestRoute[marker]);

            marker++;
            if (marker == NUM_OF_CUSTOMERS + 1)
                marker = 0;
        }
        subRouteTwo.push_back(bestRoute[p4]);


    } else {
        //forwards to find p1 to p3
        //backwards to find p2 to p4

        int marker = p1, index = 0;
        int stop = subtractFromPos(p3);
        while (marker != stop) {

            subRouteOne.push_back(bestRoute[marker]);

            marker++;
            if (marker == NUM_OF_CUSTOMERS + 1)
                marker = 0;
        }
        subRouteOne.push_back(bestRoute[p3]);

        marker = p2, index = 0;
        stop = addToPos(p4);
        while (marker != stop) {

            subRouteTwo.push_back(bestRoute[marker]);

            marker--;
            if (marker == -1)
                marker = NUM_OF_CUSTOMERS;
        }
        subRouteTwo.push_back(bestRoute[p4]);
    }
    auto newRoute = std::vector<int>();
    int index = 0;
    for (auto s:subRouteOne) {
        newRoute.push_back(s);
    }
    for (auto s:subRouteTwo) {
        newRoute.push_back(s);
    }

    return newRoute;
}

/*
 * Lin-Kernighan Heuristic
 * Version 3 (limited depth).
 */
void localSearch::LKSearch(int *bestRoute) {
    for (int p1 = 0; p1 <= NUM_OF_CUSTOMERS; p1++) {
        int p2 = addToPos(p1);
        int v3 = findNearestCustomer(bestRoute[p2]);
        if (v3 == bestRoute[p1] || get_distance(bestRoute[p1], bestRoute[p2]) <
                                   get_distance(bestRoute[p1], v3)) { //the shortest dist from v2 is to v1.
            p2 = subtractFromPos(p1);
            v3 = findNearestCustomer(bestRoute[p2]);
        }
        if (v3 == -1 || v3 == bestRoute[p1])//Skip Iteration if no nearest neighbour can be found
            continue;
        int p3 = locatePosInRoute(bestRoute, v3);
        if (p3 == -1)
            continue;
        int p4 = addToPos(p3);
        if (checkClosure(bestRoute, p1, p2, p3, p4) == 0) {
            p4 = subtractFromPos(p3);
        }
        if (p1 == p2 || p1 == p3 || p1 == p4 || p2 == p3 || p2 == p4 || p3 == p4)
            continue;
        double gain = getGain(bestRoute, p1, p2, p3, p4);
        int p5,p6;
        if (gain < 0) {
            continue;
        } else {
            bool G = true;
            int v5 = findNearestCustomer(bestRoute[p4]);
            if (v5 == bestRoute[p3])
                G = false;
            else {
                 p5 = locatePosInRoute(bestRoute, v5);
                 p6 = addToPos(p5);
                if (checkClosure(bestRoute, p3, p4, p5, p6) == 0)
                    p6 = subtractFromPos(p5);
                if (checkClosure(bestRoute, p3, p4, p5, p6) == 0)
                    G = false;
                double originalDist =
                        get_distance(bestRoute[p1], bestRoute[p2]) + get_distance(bestRoute[p3], bestRoute[p4]) +
                        get_distance(bestRoute[p5], bestRoute[p6]);
                double newDist =
                        get_distance(bestRoute[p1], bestRoute[p6]) + get_distance(bestRoute[p2], bestRoute[p3]) +
                        get_distance(bestRoute[p4], bestRoute[p5]);
                double nGain = originalDist - newDist;
                if (nGain < gain)
                    G = false;
            }
            if (!G) {
                auto newR = constructRoute(bestRoute, p1, p2, p3, p4);
                //Checks the route is the correct size.
                if (newR.size() == NUM_OF_CUSTOMERS - 1) {
                    int index = 0;
                    for (auto node: newR) {
                        bestRoute[index++] = node;
                    }
                }
            } else {
                auto subRouteOne = std::vector<int>();
                auto subRouteTwo = std::vector<int>();
                auto subRouteThree = std::vector<int>();

                if (p1 < p2) {
                    //backwards to find p3 from p1
                    //forwards to find p2 to p4
                    int marker = p1, index = 0;
                    int stop = addToPos(p3);
                    while (marker != stop) {

                        subRouteOne.push_back(bestRoute[marker]);

                        marker--;
                        if (marker == -1)
                            marker = NUM_OF_CUSTOMERS;
                    }

                    marker = p2, index = 0;
                    stop = subtractFromPos(p5);
                    while (marker != stop) {

                        subRouteTwo.push_back(bestRoute[marker]);

                        marker++;
                        if (marker == NUM_OF_CUSTOMERS + 1)
                            marker = 0;
                    }

                    marker = p4, index =0;
                    stop = addToPos(p6);
                    while (marker != stop){
                        subRouteThree.push_back(bestRoute[marker]);

                        marker--;
                        if (marker == -1)
                            marker = NUM_OF_CUSTOMERS;
                    }

                } else {
                    //forwards to find p1 to p3
                    //backwards to find p2 to p4

                    int marker = p1, index = 0;
                    int stop = subtractFromPos(p3);
                    while (marker != stop) {

                        subRouteOne.push_back(bestRoute[marker]);

                        marker++;
                        if (marker == NUM_OF_CUSTOMERS + 1)
                            marker = 0;
                    }

                    marker = p2, index = 0;
                    stop = addToPos(p5);
                    while (marker != stop) {

                        subRouteTwo.push_back(bestRoute[marker]);

                        marker--;
                        if (marker == -1)
                            marker = NUM_OF_CUSTOMERS;
                    }

                    marker = p4, index =0;
                    stop = subtractFromPos(p6);
                    while (marker != stop){
                        subRouteThree.push_back(bestRoute[marker]);

                        marker++;
                        if (marker == NUM_OF_CUSTOMERS+1)
                            marker = 0;
                    }
                }
                auto newRoute = std::vector<int>();
                int index = 0;
                for (auto s:subRouteOne)
                    newRoute.push_back(s);
                for (auto s:subRouteTwo)
                    newRoute.push_back(s);
                for (auto s:subRouteThree)
                    newRoute.push_back(s);

                if (newRoute.size() == NUM_OF_CUSTOMERS - 1) {
                    index = 0;
                    for (auto node: newRoute) {
                        bestRoute[index++] = node;
                    }
                }
            }
        }

    }
}

/*
 * Lin-Kernighan Heuristic.
 * Version 2. (Doesn't Work).
 */
//void localSearch::LKSearch(int *bestRoute) {
//    double bestLength = GenerateTour::getRouteLength(bestRoute);
//    for (int customer = 0; customer < NUM_OF_CUSTOMERS; customer++) {
//        if (get_distance(bestRoute[customer], bestRoute[customer + 1]) >
//            get_distance(bestRoute[customer], bestRoute[NUM_OF_CUSTOMERS])) {
//            int tempRoute[NUM_OF_CUSTOMERS + 1], tempRouteIndex = 0;
//
//            for (int forwardCustomer = 0; forwardCustomer <= customer; ++forwardCustomer)
//                tempRoute[tempRouteIndex++] = bestRoute[forwardCustomer];
//
//
//            for (int reverseCustomer = NUM_OF_CUSTOMERS; reverseCustomer > customer; --reverseCustomer) {
//                tempRoute[tempRouteIndex++] = bestRoute[reverseCustomer];
//            }
//            randomPheromoneLocalSearchWithTwoOpt(tempRoute);
//            double currentLength = GenerateTour::getRouteLength(tempRoute);
//            if (currentLength < bestLength) {
//                for (int copyCustomer = 0; copyCustomer <= NUM_OF_CUSTOMERS; ++copyCustomer)
//                    bestRoute[copyCustomer] = tempRoute[copyCustomer];
//                bestLength = currentLength;
//                customer = 0;
//            }
//        }
//    }
//}

/*
 * Lin-Kernighan Heuristic.
 * Version 1. (Doesn't Work).
 */
//void localSearch::LKSearch(int *bestRoute) {
//    double bestLength = getRouteLength(bestRoute);
//    for (int customer = 0; customer <= NUM_OF_CUSTOMERS; customer++){
//        int tempRoute[NUM_OF_CUSTOMERS+1], tempRouteIndex = 0;
//
//        for (int forwardCustomer = 0; forwardCustomer <= customer; ++forwardCustomer)
//            tempRoute[tempRouteIndex++] = bestRoute[forwardCustomer];
//
//
//        for (int reverseCustomer = NUM_OF_CUSTOMERS; reverseCustomer > customer; --reverseCustomer) {
//            tempRoute[tempRouteIndex++] = bestRoute[reverseCustomer];
//        }
//        double currentLength = getRouteLength(tempRoute);
//        if (currentLength < bestLength){
//            for (int copyCustomer = 0; copyCustomer <= NUM_OF_CUSTOMERS; ++copyCustomer)
//                bestRoute[copyCustomer] = tempRoute[copyCustomer];
//            bestLength = currentLength;
//        }
//    }
//}

/*
 * Complete 2-opt local search.
 * Loops through all possible instances of the 2-opt approach.
 * Swaps the order of the route between i and j.
 */
void localSearch::twoOptLocalSearch(int *bestRoute) {
    int improve = 0;
    int *tempRoute = new int[NUM_OF_CUSTOMERS + 1];
    //Checks whether there has been an improvement within x number of iterations.
    while (improve < twoOptIterations) {
        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
            tempRoute[index] = bestRoute[index];
        double route_length = GenerateTour::getRouteLength(bestRoute);
        for (int i = 0; i < NUM_OF_CUSTOMERS; ++i) {
            for (int j = i + 1; j <= NUM_OF_CUSTOMERS; ++j) {
                //Swaps the route between index i and j.
                twoOptSwap(i, j, tempRoute, bestRoute);
                double new_route_length = GenerateTour::getRouteLength(tempRoute);
                if (new_route_length < route_length) {
                    improve = 0;
                    for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
                        bestRoute[index] = tempRoute[index];
                    route_length = new_route_length;
                }
            }
        }
        improve++;
    }

    //De-allocates the memory used above.
    delete[] tempRoute;
}

/*
 * Swaps the route between the inputted points used in 2-opt local search.
 */
void localSearch::twoOptSwap(int i, int j, int *route, const int *currRoute) {
    for (int k = 0; k < i; ++k)
        route[k] = currRoute[k];
    int index = i;
    for (int k = j; k >= i; --k) {
        route[index] = currRoute[k];
        index++;
    }
    for (int k = j + 1; k <= NUM_OF_CUSTOMERS; ++k) {
        route[k] = currRoute[k];
    }
}

/*
 * 2-opt local search modified to work alongside Pheromone Random Local Search.
 * Version 2. (worse results, better efficiency).
 */
//void localSearch::twoOptLocalPheromoneAddonSearch(int *currentRoute) {
//    int improve = 0;
//    int *tempRoute = new int[NUM_OF_CUSTOMERS + 1];
//    while (improve < twoOptIterations) {
//        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
//            tempRoute[index] = currentRoute[index];
//        double route_length = getRouteLength(currentRoute);
//        for (int i = 0; i < NUM_OF_CUSTOMERS; ++i) {
//            for (int j = i + 1; j <= NUM_OF_CUSTOMERS; ++j) {
//                if(i == 0){
//                    if(get_distance(currentRoute[i],currentRoute[j+1]) < get_distance(currentRoute[j],currentRoute[j+1]) &&get_energy_consumption(currentRoute[i],currentRoute[j+1]) < get_energy_consumption(currentRoute[j],currentRoute[j+1])){
//                        twoOptSwap(i, j, tempRoute, currentRoute);
//                        double new_route_length = getRouteLength(tempRoute);
//                        if (new_route_length < route_length) {
//                            improve = 0;
//                            for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
//                                currentRoute[index] = tempRoute[index];
//                            route_length = new_route_length;
//                        }
//                    }
//                    improve++;
//                }
//                else if(j == NUM_OF_CUSTOMERS){
//                    if(get_distance(currentRoute[i-1],currentRoute[j]) < get_distance(currentRoute[i-1],currentRoute[i]) && get_energy_consumption(currentRoute[i-1],currentRoute[j]) < get_energy_consumption(currentRoute[i-1],currentRoute[i])){
//                        twoOptSwap(i, j, tempRoute, currentRoute);
//                        double new_route_length = getRouteLength(tempRoute);
//                        if (new_route_length < route_length) {
//                            improve = 0;
//                            for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
//                                currentRoute[index] = tempRoute[index];
//                            route_length = new_route_length;
//                        }
//                    }
//                    improve++;
//                }
//                else{
//                    if(get_distance(currentRoute[i-1],currentRoute[j]) < get_distance(currentRoute[i-1],currentRoute[i]) || get_distance(currentRoute[i],currentRoute[j+1]) < get_distance(currentRoute[j],currentRoute[j+1]) &&
//                    get_energy_consumption(currentRoute[i-1],currentRoute[j]) < get_energy_consumption(currentRoute[i-1],currentRoute[i]) || get_energy_consumption(currentRoute[i],currentRoute[j+1]) < get_energy_consumption(currentRoute[j],currentRoute[j+1])){
//                        twoOptSwap(i, j, tempRoute, currentRoute);
//                        double new_route_length = getRouteLength(tempRoute);
//                        if (new_route_length < route_length) {
//                            improve = 0;
//                            for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
//                                currentRoute[index] = tempRoute[index];
//                            route_length = new_route_length;
//                        }
//                    }        improve++;
//                }
//
//            }
//        }
//
//    }
//    delete[] tempRoute;
//}

/*
 * 2-opt local search modified to work alongside Pheromone Random Local Search.
 * Version 1.
 */
void localSearch::twoOptLocalPheromoneAddonSearch(int *currentRoute) {
    int improve = 0;
    int *tempRoute = new int[NUM_OF_CUSTOMERS + 1];
    while (improve < twoOptIterations) {
        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
            tempRoute[index] = currentRoute[index];
        double route_length = GenerateTour::getRouteLength(currentRoute);
//        double route_length = GenerateTour::getRouteLengthQuick(currentRoute);

        for (int i = 0; i < NUM_OF_CUSTOMERS; ++i) {
            for (int j = i + 1; j <= NUM_OF_CUSTOMERS; ++j) {
                twoOptSwap(i, j, tempRoute, currentRoute);
                double new_route_length = GenerateTour::getRouteLength(tempRoute);
//                double new_route_length = GenerateTour::getRouteLengthQuick(tempRoute);

                if (new_route_length < route_length) {
                    improve = 0;
                    for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
                        currentRoute[index] = tempRoute[index];
                    route_length = new_route_length;
                }

            }
        }
        improve++;
    }
    delete[] tempRoute;
}

/*
 * Decreases all the pheromones being used for the weighted random number generator used in the local search.
 */
void localSearch::decreaseLocalSearchPheromone() {
    for (int i = 0; i <= NUM_OF_CUSTOMERS; i++) {
        for (int j = i + 1; j <= NUM_OF_CUSTOMERS; j++) {
            if (localSearchPheromone[GenerateTour::getArcCode(i, j)] > 1)
                localSearchPheromone[GenerateTour::getArcCode(i, j)] =
                        localSearchPheromone[GenerateTour::getArcCode(i, j)] * 0.8;
        }
    }
}

/*
 * Decreases all the pheromones being used for the weighted random number generator used in the local search.
 * Used for clustered processing.
 */
void localSearch::decreaseLocalSearchPheromoneCluster() {
    for (int i = 0; i < KMeansClustering::numOfClusters; i++) {
        for (int j = i + 1; j < KMeansClustering::numOfClusters; j++) {
            if (localSearchPheromoneCluster[GenerateTour::getArcCode(i, j)] > 1)
                localSearchPheromoneCluster[GenerateTour::getArcCode(i, j)] =
                        localSearchPheromone[GenerateTour::getArcCode(i, j)] * 0.8;
        }
    }
}

/*
 * Gets the total number of pheromones in the system, used for weighted random number generator.
 */
int localSearch::getTotalWeight() {
    int totalWeight = 0;
    for (int i = 0; i <= NUM_OF_CUSTOMERS; i++) {
        for (int j = i + 1; j <= NUM_OF_CUSTOMERS; j++) {
            totalWeight += localSearchPheromone[GenerateTour::getArcCode(i, j)];
        }
    }
    return totalWeight;
}

/*
 * Gets the total number of pheromones in the system, used for weighted random number generator.
 * Used for clustered processing.
 */
int localSearch::getTotalWeightCluster() {
    int totalWeight = 0;
    for (int i = 0; i < KMeansClustering::numOfClusters; i++) {
        for (int j = i + 1; j < KMeansClustering::numOfClusters; j++) {
            totalWeight += localSearchPheromoneCluster[GenerateTour::getArcCode(i, j)];
        }
    }
    return totalWeight;
}

/*
 * Generates a random number pair (x and y) based on the local search pheromones.
 */
std::vector<int> localSearch::getRandomNumber() {
    std::vector<int> xy(2);
    int totalWeight = getTotalWeight();
    xy.operator[](0) = -1;
    xy.operator[](1) = -1;
    int val = rand() % (totalWeight - 1);
    totalWeight = 0;
    for (int i = 0; i <= NUM_OF_CUSTOMERS; i++) {
        for (int j = i + 1; j <= NUM_OF_CUSTOMERS; j++) {
            totalWeight += localSearchPheromone[GenerateTour::getArcCode(i, j)];
            if (val <= (totalWeight)) {
                xy.operator[](0) = i;
                xy.operator[](1) = j;
                return xy;
            }
        }
    }
    return xy;
}

/*
 * Generates a random number pair (x and y) based on the local search pheromones.
 * Used for clustered processing.
 */
std::vector<int> localSearch::getRandomNumberCluster() { //type either x (0) or y (1).
    std::vector<int> xy(2);
    int totalWeight = getTotalWeightCluster();
    xy.operator[](0) = -1;
    xy.operator[](1) = -1;
    int val = rand() % (totalWeight - 1);
    totalWeight = 0;
    for (int i = 0; i < KMeansClustering::numOfClusters; i++) {
        for (int j = i + 1; j < KMeansClustering::numOfClusters; j++) {
            totalWeight += localSearchPheromoneCluster[GenerateTour::getArcCode(i, j)];
            if (val <= (totalWeight)) {
                xy.operator[](0) = i;
                xy.operator[](1) = j;
                return xy;
            }
        }
    }
    return xy;
}

/*
 * Exchange local search.
 * Switches two customers based on randomly selected indexes.
 */
void localSearch::randomLocalSearch(int *bestRoute) {
    int *tempRoute = new int[NUM_OF_CUSTOMERS + 1];
    double route_length = GenerateTour::getRouteLength(bestRoute);
    double new_route_length = route_length;
    int iters = 0, x, y;

    while (new_route_length >= route_length && iters < randomSearchIteration) {
        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
            tempRoute[index] = bestRoute[index];
        iters++;
        x = rand() % NUM_OF_CUSTOMERS;
        y = ((rand() % 5) + 1 + x);
        if (y > NUM_OF_CUSTOMERS) {
            y = NUM_OF_CUSTOMERS;
        }

        int size = (y - x) + 1;
        int *routeTemp = new int[size];
        for (int index = 0; index < size; index++)
            routeTemp[index] = -1;
        int tempIndex = 0;

        for (int index = y; index >= x; index--) {
            routeTemp[tempIndex] = tempRoute[index];
            tempIndex++;
        }
        tempIndex = 0;
        for (int index = x; index <= y; index++) {
            tempRoute[index] = routeTemp[tempIndex];
            tempIndex++;
        }
        delete[] routeTemp;
        //2-opt local search.
//        twoOptLocalPheromoneAddonSearch(tempRoute);
        new_route_length = GenerateTour::getRouteLength(tempRoute);
    }
    if (new_route_length < route_length) {
        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
            bestRoute[index] = tempRoute[index];
    }
    delete[] tempRoute;
}

/*
 * Exchange local search.
 * Switches two customers based on a weighted average determined by pheromones.
 */
void localSearch::randomPheromoneLocalSearch(int *bestRoute) {
    int *tempRoute = new int[NUM_OF_CUSTOMERS + 1];
    double route_length = GenerateTour::getRouteLength(bestRoute);
    double new_route_length = route_length;
    int iters = 0, x, y;
    std::vector<int> xy(2);
    while (new_route_length >= route_length && iters < randomSearchIteration) { //(rand()%5)+10)
        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
            tempRoute[index] = bestRoute[index];
        iters++;

        xy = getRandomNumber();
        if (xy.operator[](0) < xy.operator[](1)) {
            x = xy.operator[](0);
            y = xy.operator[](1);
        } else {
            x = xy.operator[](1);
            y = xy.operator[](0);
        }

        int size = (y - x) + 1;
        int *routeTemp = new int[size];
        for (int index = 0; index < size; index++)
            routeTemp[index] = -1;
        int tempIndex = 0;

        for (int index = y; index >= x; index--) {
            routeTemp[tempIndex] = tempRoute[index];
            tempIndex++;
        }
        tempIndex = 0;
        for (int index = x; index <= y; index++) {
            tempRoute[index] = routeTemp[tempIndex];
            tempIndex++;
        }
        delete[] routeTemp;
        new_route_length = GenerateTour::getRouteLength(tempRoute);
    }
    decreaseLocalSearchPheromone();
    if (new_route_length < route_length) {
        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
            bestRoute[index] = tempRoute[index];
        localSearchPheromone[GenerateTour::getArcCode(x, y)] =
                localSearchPheromone[GenerateTour::getArcCode(x, y)] + (int) ((route_length - new_route_length));
    }
    delete[] tempRoute;

}

/*
 * Exchange local search with 2-opt local search.
 * Switches two customers based on a weighted average determined by pheromones.
 * Uses basic route length to determine which route is better.
 */
//void localSearch::randomPheromoneLocalSearchWithTwoOpt(int *bestRoute) {
//    int *tempRoute = new int[NUM_OF_CUSTOMERS + 1];
//    double route_length = getBasicLength(bestRoute);
//    double new_route_length = route_length;
//    int iters = 0, x, y;
//    std::vector<int> xy(2);
//    while (new_route_length >= route_length && iters < randomSearchIteration) { //(rand()%5)+10)
//        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
//            tempRoute[index] = bestRoute[index];
//        iters++;
//        //x = rand() % NUM_OF_CUSTOMERS;
//        //y = ((rand() % 5) + 1 + x);
//
//        xy = getRandomNumber();
//        if (xy.operator[](0) < xy.operator[](1)) {
//            x = xy.operator[](0);
//            y = xy.operator[](1);
//        } else {
//            x = xy.operator[](1);
//            y = xy.operator[](0);
//        }
//
////        y = getRandomNumber(1);
////        int tempx;
////        if (y < x) {
////            tempx = x;
////            x = y;
////            y = tempx;
////        }
//        //printf("X = %d, Y = %d\n", x, y); //DEBUGGING
//
//        int size = (y - x) + 1;
//        int *routeTemp = new int[size];
//        for (int index = 0; index < size; index++)
//            routeTemp[index] = -1;
//        int tempIndex = 0;
//
//        for (int index = y; index >= x; index--) {
//            routeTemp[tempIndex] = tempRoute[index];
//            tempIndex++;
//        }
//        tempIndex = 0;
//        for (int index = x; index <= y; index++) {
//            tempRoute[index] = routeTemp[tempIndex];
//            tempIndex++;
//        }
//        delete[] routeTemp;
//        //2-Opt As well
//        twoOptLocalPheromoneAddonSearch(tempRoute);
//        new_route_length = getBasicLength(tempRoute);
//        //DEBUGGING
////        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
////            printf("%d, ",tempRoute[index]);
////        printf("\n");
//    }
//    decreaseLocalSearchPheromone();
//    if (new_route_length < route_length) {
//        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
//            bestRoute[index] = tempRoute[index];
//        localSearchPheromone[getArcCode(x, y)] =
//                localSearchPheromone[getArcCode(x, y)] + (int) ((route_length - new_route_length));
//    }
//    //printLocalSearchPheromones();
//    delete[] tempRoute;
//
//}

/*
 * Exchange local search with 2-opt local search.
 * Switches two customers based on a weighted average determined by pheromones.
 * Uses generate tour to determine which route is better.
 * Used for clustered processing.
 */
void localSearch::randomPheromoneLocalSearchWithTwoOptCluster(int *bestRoute) {
    int *tempRoute = new int[KMeansClustering::numOfClusters];
    double route_length = KMeansClustering::getRouteLength(bestRoute);
    double new_route_length = route_length;
    int iters = 0, x, y;
    std::vector<int> xy(2);
    while (new_route_length >= route_length && iters < randomSearchIteration) { //(rand()%5)+10)
        for (int index = 0; index < KMeansClustering::numOfClusters; index++)
            tempRoute[index] = bestRoute[index];
        iters++;

        xy = getRandomNumberCluster();
        if (xy.operator[](0) < xy.operator[](1)) {
            x = xy.operator[](0);
            y = xy.operator[](1);
        } else {
            x = xy.operator[](1);
            y = xy.operator[](0);
        }

        int size = (y - x) + 1;
        int *routeTemp = new int[size];
        for (int index = 0; index < size; index++)
            routeTemp[index] = -1;
        int tempIndex = 0;

        for (int index = y; index >= x; index--) {
            routeTemp[tempIndex] = tempRoute[index];
            tempIndex++;
        }
        tempIndex = 0;
        for (int index = x; index <= y; index++) {
            tempRoute[index] = routeTemp[tempIndex];
            tempIndex++;
        }
        delete[] routeTemp;

        //2-Opt As well
        //twoOptLocalPheromoneAddonSearch(tempRoute);

        new_route_length = KMeansClustering::getRouteLength(tempRoute);
    }
    decreaseLocalSearchPheromoneCluster();
    if (new_route_length < route_length) {
        for (int index = 0; index < KMeansClustering::numOfClusters; index++)
            bestRoute[index] = tempRoute[index];

        localSearchPheromoneCluster[GenerateTour::getArcCode(x, y)] =
                localSearchPheromoneCluster[GenerateTour::getArcCode(x, y)] + (int) ((route_length - new_route_length));
    }
    delete[] tempRoute;

}

/*
 * Exchange local search with 2-opt local search.
 * Switches two customers based on a weighted average determined by pheromones.
 * Uses generate tour to determine which route is better.
 */
void localSearch::randomPheromoneLocalSearchWithTwoOpt(int *bestRoute) {
    int *tempRoute = new int[NUM_OF_CUSTOMERS + 1];
    double route_length = GenerateTour::getRouteLength(bestRoute);
    double new_route_length = route_length;
    int iters = 0, x, y;
    std::vector<int> xy(2);
    while (new_route_length >= route_length && iters < randomSearchIteration) { //(rand()%5)+10)
        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
            tempRoute[index] = bestRoute[index];
        iters++;

        xy = getRandomNumber();
        if (xy.operator[](0) < xy.operator[](1)) {
            x = xy.operator[](0);
            y = xy.operator[](1);
        } else {
            x = xy.operator[](1);
            y = xy.operator[](0);
        }

        int size = (y - x) + 1;
        int *routeTemp = new int[size];
        for (int index = 0; index < size; index++)
            routeTemp[index] = -1;
        int tempIndex = 0;

        for (int index = y; index >= x; index--) {
            routeTemp[tempIndex] = tempRoute[index];
            tempIndex++;
        }
        tempIndex = 0;
        for (int index = x; index <= y; index++) {
            tempRoute[index] = routeTemp[tempIndex];
            tempIndex++;
        }
        delete[] routeTemp;

        //2-Opt As well
        twoOptLocalPheromoneAddonSearch(tempRoute);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    new_route_length = GenerateTour::getRouteLength(tempRoute);
    }
    decreaseLocalSearchPheromone();
    if (new_route_length < route_length) {
        for (int index = 0; index <= NUM_OF_CUSTOMERS; index++)
            bestRoute[index] = tempRoute[index];
        localSearchPheromone[GenerateTour::getArcCode(x, y)] =
                localSearchPheromone[GenerateTour::getArcCode(x, y)] + (int) ((route_length - new_route_length));
    }
    delete[] tempRoute;
}
