#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>

typedef struct{
    int *visited_cities ;
    int *path ;
    int total_cost;
}Ant;

Ant createAnt(int n){
    Ant ant ;
    ant.path = (int*)malloc(n*sizeof(int));
    ant.visited_cities = (int*)malloc(n*sizeof(int));
    ant.total_cost = 0 ;
    return ant ;
}

void freeAnt(Ant *ant){
    free(ant->path);
    free(ant->visited_cities);
}

int **createIntMatrix(int n){
    int **matrix = (int**)malloc(n*sizeof(int*));
    for(int i = 0; i < n ; i++){
        matrix[i] = (int*)malloc(n*sizeof(int));
    }
    return matrix ;
}

double **createDoubleMatrix(int n){
    double **matrix = (double**)malloc(n*sizeof(double*));
    for(int i = 0 ; i < n; i++){
        matrix[i] = (double*)malloc(n*sizeof(double));
    }
    return matrix ;
}

void freeMatrix(void **matrix , int n ){
    for(int i = 0 ; i < n ; i++){
        free(matrix[i]);
    }
    free(matrix);
}

int random_city(int **dist ,double **pheromone ,int current ,int visited[], int n, float alpha , float  beta){
    float *p = (float*)malloc(n*sizeof(float));
    double somme_proba = 0.0 ;  
    for(int i = 0; i < n ; i++){
        if( visited[i] == 0 && dist[current][i] > 0){
            double d = 1.0 / (double)dist[current][i];
            double tau = pheromone[current][i];
            somme_proba += pow(tau,alpha)*pow(d,beta);
        }
    }

    for(int i = 0; i < n ; i++){
        if( visited[i] == 0 && dist[current][i] > 0){
            double d = 1.0 / dist[current][i];
            double tau = pheromone[current][i];
            p[i] = pow(tau,alpha)*pow(d,beta) / somme_proba ;
        }
        else{
            p[i] = 0.0 ;
        }
    }

    double r = (double)rand() / RAND_MAX ;
    double cum = 0.0;
    for(int i = 0 ; i < n ; i++){
        cum += p[i];
        if(r <= cum){
            free(p);
            return i ;
        }
    }

    for(int i = 0 ; i < n ; i++){
        if(visited[i] == 0){
            free(p);
            return i;
        }
    }

    free(p);
    return -1 ;
}

Ant AOC_tsp(int **dist, int n, int max_iteration, float alpha, float beta , double Q){
    
    Ant *ant = (Ant*)malloc(n*sizeof(Ant));
    for(int i = 0 ; i < n ; i++){
        ant[i] = createAnt(n);
    }

    float rho = 0.1;
    double delta_pheromone ;
    int visited_cities[n];
    double **pheromone;
    int current_city ;
    int next_city ;
    
    pheromone = createDoubleMatrix(n);
    for(int i = 0; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            pheromone[i][j] = 1.0 ;
        } 
    }
    
    for(int itr = 0 ; itr < max_iteration ; itr++){
        for(int i = 0; i < n ; i++){
            for(int k = 0; k < n ; k++){
                ant[i].visited_cities[k] = 0;
            }
            int start = rand() % n ;
            ant[i].visited_cities[start] = 1 ;
            ant[i].path[0] = start ;
            ant[i].total_cost = 0 ; 
            current_city = start ;
            
            for(int j = 1 ; j < n ; j++){
                next_city = random_city(dist, pheromone,current_city, ant[i].visited_cities, n, alpha, beta);
            
                ant[i].total_cost += dist[current_city][next_city];
                ant[i].path[j] = next_city ;
                ant[i].visited_cities[current_city] = 1;
                current_city = next_city;

            }
            ant[i].total_cost += dist[current_city][start] ;
        }

        for(int r = 0 ; r < n ; r++){
            for(int c = 0 ; c < n ; c++){
                pheromone[r][c] = (1 - rho)*pheromone[r][c];
            }
        }
        for(int p = 0 ; p < n ; p++){
            delta_pheromone = Q / ant[p].total_cost ;
            for(int k = 0 ; k < n-1 ; k++){
                int current_city = ant[p].path[k];
                int next_city = ant[p].path[k+1] ;
                pheromone[current_city][next_city] += delta_pheromone ;
            }
            int last_city = ant[p].path[n-1];
            int first_city = ant[p].path[0];
            pheromone[last_city][first_city] += delta_pheromone ;
        }
    }

    int min_cost = ant[0].total_cost ;
    int min_index = 0 ;
    for(int i = 1; i < n ; i++){
        if(ant[i].total_cost < min_cost){
            min_cost = ant[i].total_cost ;
            min_index = i ;
        }
    }

    Ant bestAnt = createAnt(n);
    bestAnt.total_cost = ant[min_index].total_cost;
    for(int i = 0 ; i < n ; i++){
        bestAnt.path[i] = ant[min_index].path[i];
    }

    freeMatrix((void**)pheromone, n);
    for(int i = 0 ; i < n ; i++){
        freeAnt(&ant[i]);
    }

    return bestAnt;
}

