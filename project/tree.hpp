#pragma once
#include <iostream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>

#include "graph.hpp"


using namespace std;


class Tree
{
    private:
        vector<vector<int>> adj;
        int V;
        vector<vector<int>> dist;
        int ready;
	

    public:
        
        Tree(int m, int n): V(n){
            adj = vector<vector<int>>(n, vector<int>(n, 0));
            cout << "format: 'a' 'b' " << endl;
            string line;
            for(int i = 0; i < m; i++){
                getline(cin, line);
                int a = stoi(line.substr(0, line.find(" ")));
                cout << "a: " << a << endl;
                int b = stoi(line.substr(line.find(" ") + 1, line.length()));
                cout << "b: " << b << endl;
                if(a != b){
                    cout << "enter weight: "<<endl;
                    int w;
                    cin>>w;
                    adj[a][b] = w;
                }
                else{
                    i--;
                }
            }
            this->ready = 0;
        }

		Tree(): ready(0) {}

		Tree(const Graph& other){
			adj = other.getAdj();
            V = other.getV();
            this->ready = 0;
		}

		Tree(const Tree& other){
			adj = other.getAdj();
            V = other.getV();
            this->ready = 0;
		}

		Tree(const vector<vector<int>> other){
			adj = other;
            V = other.size();
            this->ready = 0;
		}

		//Tree& operator=(const Tree

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

        bool is_ready(){return this->ready != 0;}

		int calc_weight(){
			int weight = 0;
            for(int i = 0; i < V; i++){
                for(int j = i+1; j < V; j++){
                    if(adj[i][j] != 0){
                        weight += adj[i][j];
                    }
                }
            }
            return weight;
		}

		void floydWarshall() {

			// Distance matrix initialized with the input graph's adjacency matrix
			this->dist = this->adj;

			for (int i = 0; i < this->dist.size(); i++){
				for (int j = 0; j < this->dist[i].size(); j++) {
                    if (i == j) {
                        this->dist[i][j] = 0;
                    } else {
						if (this->dist[i][j] == 0)
							this->dist[i][j] = INT_MAX;
                    }
                }
			}

			// Floyd-Warshall algorithm
			for (int k = 0; k < this->V; k++) {
				for (int i = 0; i < this->V; i++) {
					for (int j = 0; j < this->V; j++) {
						if (this->dist[i][k] != INT_MAX && this->dist[k][j] != INT_MAX && this->dist[i][k] + this->dist[k][j] < this->dist[i][j]) {
							this->dist[i][j] = this->dist[i][k] + this->dist[k][j];
						}
					}
				}
			}
            this->ready = 1;
		}

		int minDistance() {
			int min = INT_MAX;
            for (int v = 0; v < this->dist.size(); v++) {
				for (int i = 0; i < this->dist[v].size(); i++) {
					if (this->dist[v][i] < min && i != v){
						min = this->dist[v][i];
					}
				}
            }
            return min;
		}

		int maxDistance() {
			int max = INT_MIN;
            for (int v = 0; v < this->dist.size(); v++) {
				for (int i = 0; i < this->dist[v].size(); i++) {
					if (this->dist[v][i] > max && this->dist[v][i] != INT_MAX){
						max = this->dist[v][i];
					}
				}
            }
            return max;
		}

		int avgDistance() {
			int sum = 0;
			int count = 0;
            for (int v = 0; v < this->V; v++) {
				for (int i = v; i < this->V; i++) {
					if (this->dist[v][i] != INT_MAX && v != i){
						sum += this->dist[v][i];
						count++;
					}
				}
            }
            return sum / count;
		}

        Tree& operator=(const Tree& other){
            adj = other.getAdj();
            V = other.getV();
            return *this;
        }
};