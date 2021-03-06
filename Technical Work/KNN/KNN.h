#ifndef TESTSAMPLECODE_KNN_H
#define TESTSAMPLECODE_KNN_H
#include "../LocalSearchAndGenerateTour/LocalSearches.h"
#include <map>
#include <iostream>
#include <string>
#include <stdio.h>
#include <random>
#include <climits>
class KNN {
private:
    int *v;
    int*filter;
    int **neighbours;
    localSearch* LS;
public:

    void generateRoute(int);
    int findNextClosestNode(int);
    KNN(int);
    int *getKNN(int, int);
    void printRoute();
    ~KNN();
};


#endif //TESTSAMPLECODE_KNN_H
