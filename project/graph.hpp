#pragma once
#include <iostream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


using namespace std;


class Graph
{
    vector<vector<int>> adj;
    int V;

    public:
        Graph(int m, int n): V(n){
            this->adj = vector<vector<int>>(n, vector<int>(n, 0));
            
        }

        Graph() = default;

        Graph(const vector<vector<int>> other){
			adj = other;
            V = other.size();
		}

        void addEdge(int a, int b, int weight)  {
            this->adj[a][b] = weight;
        }

        void removeEdge(int a, int b)  {
            this->adj[a][b] = 0;
        }

        int getWeight(int a, int b) const {
            return this->adj[a][b];
        }

        vector<vector<int>> getAdj() const {
            return this->adj;
        }

        int getV() const {
            return this->V;
        }

        void print(){
            for(int i = 0; i < V; i++){
                for(int j = 0; j < V; j++){
                    cout << adj[i][j] << " ";
                }
                cout << endl;
            }
        }
};