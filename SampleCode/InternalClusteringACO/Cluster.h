#ifndef TESTSAMPLECODE_CLUSTER_H
#define TESTSAMPLECODE_CLUSTER_H

#include "../LocalSearchAndGenerateTour/LocalSearches.h"

class Cluster {
private:
    struct Node{
        int* customers;
        double distance;
        int sizeOfCluster;
        double demand;
        std::string displayNode();
        void getTotalDistance();
        void getTotalDemand();
    };
    std::vector<std::pair<double,double>*>* centroids;
    bool * visited;

    void initialCentroids();
    void displayCentroids();
    void displayClusters();
    void clusterAroundCentroids();
    bool generateCentroidsBasedOnClusters();
    double getXYDistance(double,double, int);
public:
    std::vector<struct Node*>* clusters;
    int numOfClusters;
    Cluster();
    ~Cluster();
    void createClusters();


};


#endif //TESTSAMPLECODE_CLUSTER_H
