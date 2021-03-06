#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<cstring>
#include<math.h>
#include<fstream>
#include<time.h>
#include<limits.h>

#include "EVRP.hpp"

using namespace std;

char* problem_instance;          //Name of the instance
struct node *node_list;     //List of nodes with id and x and y coordinates
int *cust_demand;                //List with id and customer demands
bool* charging_station;
double **distances;              //Distance matrix
int problem_size;                //Problem dimension read
double energy_consumption;

int DEPOT;                       //depot id (usually 0)
int NUM_OF_CUSTOMERS;       //Number of customers (excluding depot)
int ACTUAL_PROBLEM_SIZE;        //Tottal number of customers, charging stations and depot
double OPTIMUM;
int NUM_OF_STATIONS;
int BATTERY_CAPACITY;       //maximum energy of vehicles
int MAX_CAPACITY;           //capacity of vehicles
int MIN_VEHICLES;



double evals;
double current_best;

FILE* jsonNodes;

/*
 * Stores the node data to a file so that it can be used for visualisation.
 * Not part of the competition framework.
 */
void storeNodes(){
    printf("saving....\n");
    if ((jsonNodes = fopen(R"(..\Data\storeNodes.txt)","w")) == NULL) { printf("ERROR\n");}
    for (int index=0;index<ACTUAL_PROBLEM_SIZE;index++){
        fprintf(jsonNodes,"%d %f %f",node_list[index].id,node_list[index].x,node_list[index].y);
        if(index == DEPOT)
            fprintf(jsonNodes," %s","d");
        else if(charging_station[index])
            fprintf(jsonNodes," %s","s");
        else
            fprintf(jsonNodes," %s","c");
        fprintf(jsonNodes,"\n");
    }
    fclose(jsonNodes);
}

node getNodeInfo(int index){
    return node_list[index];
}


/****************************************************************/
/*Compute and return the euclidean distance of two objects      */
/****************************************************************/
double euclidean_distance(int i, int j) {
    double xd,yd;
    double r = 0.0;
    xd = node_list[i].x - node_list[j].x;
    yd = node_list[i].y - node_list[j].y;
    r  = sqrt(xd*xd + yd*yd);
    return r;
}

/****************************************************************/
/*Compute the distance matrix of the problem instance           */
/****************************************************************/
void compute_distances(void) {
    int i, j;
    for(i = 0; i < ACTUAL_PROBLEM_SIZE; i++){
        for(j = 0; j < ACTUAL_PROBLEM_SIZE; j++){
            distances[i][j] = euclidean_distance(i,j);
        }
    }
}


/****************************************************************/
/*Generate and return a two-dimension array of type double      */
/****************************************************************/
double ** generate_2D_matrix_double(int n, int m){
    double **matrix;

    matrix = new double*[n];
    for ( int i = 0 ; i < n ; i++ ) {
        matrix[i] = new double[m];
    }
    //initialize the 2-d array
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++) {
            matrix[i][j] = 0.0;
        }
    }
    return matrix;
}

/*
 * Displays the node data at the beginning of the program.
 * Not part of the competition framework.
 */
void displayNodeData(void){
    for (int index=0; index < ACTUAL_PROBLEM_SIZE; index++){
        printf("Node %d at %f,%f",node_list[index].id,node_list[index].x,node_list[index].y);
        if (charging_station[index])
            printf(" Charging station");
        if (index == DEPOT)
            printf(" Depot");
        printf("\n");
    }
}

/****************************************************************/
/* Read the variables instance and generate the initial object    */
/* vector.                                                      */
/****************************************************************/
std::vector<double>* read_variables(std::string filename,int input){
    auto vars = new std::vector<double>();

    int i;
    char line[CHAR_LEN];
    char * keywords;
    char Delimiters[] = " :=\n\t\r\f\v";
    ifstream fin("../"+filename);

    //ANT COLONY VARS
    int ants,iter,prob,q,randIter,twoOptIter,cluster;
    double phero,alpha,beta,pBest;

    //GENETIC ALGORITHM VARS
    int pop,gen,mut,sel,cros,mutSel;

    while((fin.getline(line, CHAR_LEN-1))){
        printf("line: %s\n",line);

        if(!(keywords = strtok(line, Delimiters)))
            continue;
        if(!strcmp(keywords, "ANTS")){
            if(!sscanf(strtok(NULL, Delimiters), "%d", &ants)){
                cout<<"ANTS error"<<endl;
                exit(0);
            }
        }
        else if(!strcmp(keywords, "ITERATIONS")){
            if(!sscanf(strtok(NULL, Delimiters), "%d", &iter)){
                cout<<"ITERATIONS error"<<endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "PROBABILITY")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &prob)){
                cout << "PROBABILITY error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "PHEROMONE")){
            if(!sscanf(strtok(NULL,Delimiters), "%lf", &phero)){
                cout << "PHEROMONE error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "Q")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &q)){
                cout << "Q error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "ALPHA")){
            if(!sscanf(strtok(NULL,Delimiters), "%lf", &alpha)){
                cout << "ALPHA error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "BETA")){
            if(!sscanf(strtok(NULL,Delimiters), "%lf", &beta)){
                cout << "BETA error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "RANDOMITER")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &randIter)){
                cout << "RANDOMITER error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "TWOOPTITER")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &twoOptIter)){
                cout << "TWOOPTITER error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "PBEST")){
            if(!sscanf(strtok(NULL,Delimiters), "%lf", &pBest)){
                cout << "PBEST error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "CLUSTER")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &cluster)){
                cout << "CLUSTER error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "POP")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &pop)){
                cout << "POP error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "GEN")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &gen)){
                cout << "GEN error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "MUT")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &mut)){
                cout << "MUT error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "SEL")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &sel)){
                cout << "SEL error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "CROS")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &cros)){
                cout << "CROS error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "MUTSEL")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &mutSel)){
                cout << "MUTSEL error" << endl;
                exit(0);
            }
        }
    }
    fin.close();
    if(randIter != 0){
    if(input == 5 || (7 <= input && input <= 10)){
        vars->push_back(ants);
        vars->push_back(iter);
        vars->push_back(prob);
        vars->push_back(phero);
        vars->push_back(q);
        vars->push_back(alpha);
        vars->push_back(beta);
        if(input==10){
            vars->push_back(cluster);
        }
        if(input==9){
            vars->push_back(pBest);
        }
    }
    else if(input == 6){
        vars->push_back(pop);
        vars->push_back(gen);
        vars->push_back(mut);
        vars->push_back(sel);
        vars->push_back(cros);
        if(cros == 1)
            vars->push_back(mutSel);
        else
            vars->push_back(0);
    }

    vars->push_back(randIter);
    vars->push_back(twoOptIter);
    return vars;
    }else{
        return nullptr;
    }
}


/****************************************************************/
/* Read the problem instance and generate the initial object    */
/* vector.                                                      */
/****************************************************************/
void read_problem(char* filename){
    int i;
    char line[CHAR_LEN];
    char * keywords;
    char Delimiters[] = " :=\n\t\r\f\v";
    ifstream fin(filename);
    while((fin.getline(line, CHAR_LEN-1))){

        if(!(keywords = strtok(line, Delimiters)))
            continue;
        if(!strcmp(keywords, "DIMENSION")){
            if(!sscanf(strtok(NULL, Delimiters), "%d", &problem_size)){
                cout<<"DIMENSION error"<<endl;
                exit(0);
            }
        }
        else if(!strcmp(keywords, "EDGE_WEIGHT_TYPE")){
            char * tempChar;
            if(!(tempChar=strtok(NULL, Delimiters))){
                cout<<"EDGE_WEIGHT_TYPE error"<<endl;
                exit(0);
            }
            if(strcmp(tempChar, "EUC_2D")){
                cout<<"not EUC_2D"<<endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "CAPACITY")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &MAX_CAPACITY)){
                cout << "CAPACITY error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "VEHICLES")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &MIN_VEHICLES)){
                cout << "VEHICLES error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "ENERGY_CAPACITY")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &BATTERY_CAPACITY)){
                cout << "ENERGY_CAPACITY error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "ENERGY_CONSUMPTION")){
            if(!sscanf(strtok(NULL,Delimiters), "%lf", &energy_consumption)){
                cout << "ENERGY_CONSUMPTION error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "STATIONS")){
            if(!sscanf(strtok(NULL,Delimiters), "%d", &NUM_OF_STATIONS)){
                cout << "STATIONS error" << endl;
                exit(0);
            }
        }
        else if (!strcmp(keywords, "OPTIMAL_VALUE")){
            if(!sscanf(strtok(NULL,Delimiters), "%lf", &OPTIMUM)){
                cout << "OPTIMAL_VALUE error" << endl;
                exit(0);
            }
        }
        else if(!strcmp(keywords, "NODE_COORD_SECTION")){
            if(problem_size!=0){
                /*prroblem_size is the number of customers plus the depot*/
                NUM_OF_CUSTOMERS = problem_size - 1;
                ACTUAL_PROBLEM_SIZE = problem_size + NUM_OF_STATIONS;

                node_list = new node[ACTUAL_PROBLEM_SIZE];

                for(i=0; i < ACTUAL_PROBLEM_SIZE; i++){
                    //store initial objects
                    fin>>node_list[i].id;
                    fin>>node_list[i].x>>node_list[i].y;
                    node_list[i].id -=1;
                }
                //compute the distances using initial objects
                distances = generate_2D_matrix_double(ACTUAL_PROBLEM_SIZE, ACTUAL_PROBLEM_SIZE);
            } else {
                cout << "wrong problem instance file" << endl;
                exit(1);
            }
        }
        else if(!strcmp(keywords, "DEMAND_SECTION")){
            if(problem_size!=0){

                int temp;
                //masked_demand = new int[problem_size];
                cust_demand = new int[ACTUAL_PROBLEM_SIZE];
                charging_station = new bool[ACTUAL_PROBLEM_SIZE];
                for(i = 0; i < problem_size; i++){
                    fin >> temp;
                    fin >> cust_demand[temp-1];
                }

                //initialize the charging stations.
                //the depot is set to a charging station in the DEPOT_SECTION
                for(i = 0; i < ACTUAL_PROBLEM_SIZE; i++){
                    if(i < problem_size) {
                        charging_station[i] = false;
                    } else {
                        charging_station[i] = true;
                        cust_demand[i] = 0;
                    }
                }
            }
        }
        else if(!strcmp(keywords, "DEPOT_SECTION")){
            fin >> DEPOT;
            DEPOT-=1;
            charging_station[DEPOT] = true;
        }

    }
    fin.close();
    displayNodeData();
    storeNodes();
    if(ACTUAL_PROBLEM_SIZE == 0) {
        cout << "wrong problem instance file" << endl;
        exit(1);
    } else {
        compute_distances();
    }

}


/****************************************************************/
/* Returns the solution quality of the solution. Taken as input */
/* an array of node indeces and its length                      */
/****************************************************************/
double fitness_evaluation(int *routes, int size) {
    int i;
    double tour_length = 0.0;

    //the format of the solution that this method evaluates is the following
    //Node id:  0 - 5 - 6 - 8 - 0 - 1 - 2 - 3 - 4 - 0 - 7 - 0
    //Route id: 1 - 1 - 1 - 1 - 2 - 2 - 2 - 2 - 2 - 3 - 3 - 3
    //this solution consists of three routes:
    //Route 1: 0 - 5 - 6 - 8 - 0
    //Route 2: 0 - 1 - 2 - 3 - 4 - 0
    //Route 3: 0 - 7 - 0
    for (i = 0; i < size-1; i++)
        tour_length += distances[routes[i]][routes[i+1]];
    if(tour_length < current_best)
        current_best = tour_length;

    //adds complete evaluation to the overall fitness evaluation count
    evals++;

    return tour_length;
}

/****************************************************************/
/* Outputs the routes of the solution. Taken as input           */
/* an array of node indeces and its length                      */
/****************************************************************/
void print_solution(int *routes, int size) {
    int   i;

    for( i = 0 ; i < size; i++ ) {
        cout << routes[i] <<  " , ";
    }
}


/****************************************************************/
/* Validates the routes of the solution. Taken as input         */
/* an array of node indeces and its length                      */
/****************************************************************/
void check_solution(int *t, int size){
    int i, from, to;
    double energy_temp = BATTERY_CAPACITY;
    double capacity_temp = MAX_CAPACITY;
    double distance_temp = 0.0;

    for(i = 0; i < size-1; i++){
        from = t[i];
        to = t[i+1];
        capacity_temp -= get_customer_demand(to);
        energy_temp -= get_energy_consumption(from,to);
        distance_temp += get_distance(from,to);
        if(capacity_temp < 0.0) {
            cout << "error: capacity below 0 at customer " << to <<  endl;
            print_solution(t,size);
            exit(1);
        }
        if(energy_temp < 0.0) {
            cout << "error: energy below 0 from " << from << " to " << to <<  endl;
            print_solution(t,size);
            exit(1);
        }
        if(to == DEPOT) {
            capacity_temp = MAX_CAPACITY;
        }
        if(is_charging_station(to)==true || to==DEPOT){
            energy_temp = BATTERY_CAPACITY;
        }
    }
    if(distance_temp != fitness_evaluation(t,size)) {
        cout << "error: check fitness evaluation" << endl;
    }
}


/****************************************************************/
/* Returns the distance between two points: from and to. Can be */
/* used to evaluate a part of the solution. The fitness         */
/* evaluation count will be proportional to the problem size    */
/****************************************************************/
double get_distance(int from, int to){
    //adds partial evaluation to the overall fitness evaluation count
    //It can be used when local search is used and a whole evaluation is not necessary
    evals += (1.0/ACTUAL_PROBLEM_SIZE);

    return distances[from][to];

}


/****************************************************************/
/* Returns the energy consumed when travelling between two      */
/* points: from and to.                                         */
/****************************************************************/
double get_energy_consumption(int from, int to) {

    /*DO NOT USE THIS FUNCTION MAKE ANY CALCULATIONS TO THE ROUTE COST*/
    return energy_consumption*distances[from][to];

}

/****************************************************************/
/* Returns the demand for a specific customer                   */
/* points: from and to.                                         */
/****************************************************************/
int get_customer_demand(int customer){

    return cust_demand[customer];

}

/****************************************************************/
/* Returns true when a specific node is a charging station;     */
/* and false otherwise                                          */
/****************************************************************/
bool is_charging_station(int node){

    bool flag = false;
    if(charging_station[node] == true)
        flag = true;
    else
        flag = false;
    return flag;

}

/****************************************************************/
/* Returns the best solution quality so far                     */
/****************************************************************/
double get_current_best(){

    return current_best;

}

/*******************************************************************/
/* Reset the best solution quality so far for a new indepedent run */
/*******************************************************************/
void init_current_best(){

    current_best = INT_MAX;

}

/****************************************************************/
/* Returns the current count of fitness evaluations             */
/****************************************************************/
double get_evals(){

    return evals;

}

/****************************************************************/
/* Reset the evaluation counter for a new indepedent run        */
/****************************************************************/
void init_evals(){

    evals = 0;

}



/****************************************************************/
/* Clear the allocated memory                                   */
/****************************************************************/
void free_EVRP(){

    int i;

    delete[] node_list;
    delete[] cust_demand;
    delete[] charging_station;

    for(i = 0; i < ACTUAL_PROBLEM_SIZE; i++) {
        delete[] distances[i];
    }

    delete[] distances;

}
