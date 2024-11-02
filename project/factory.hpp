#pragma once

#include <iostream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>

#include "graph.hpp"
#include "tree.hpp"

using namespace std;

struct Edge {
    int src, dest, weight;
};

class Algo{
	public:
		virtual Tree execute(Graph& graph) = 0;
		virtual ~Algo() {}
};

class Prim : public Algo{
	private:
		int minKey(const vector<int>& key, const vector<bool>& inMST, int V) {
			int min = INT_MAX, min_index;

			for (int v = 0; v < V; v++) {
				if (!inMST[v] && key[v] < min) {
					min = key[v], min_index = v;
				}
			}

			return min_index;
		}
	public:
	    Tree execute(Graph& graph) override {
			int V = graph.getV(); 

			// vector to store mst
			vector<int> parent(V);

			// values used to pick minimum weight edge in cut
			vector<int> key(V, INT_MAX);

			// in mst or not
			vector<bool> inMST(V, false);

			//choose the first vertex as root
			key[0] = 0;        
			parent[0] = -1;    

			vector<vector<int>> g = graph.getAdj();
			// its obvious that the MST will have V-1 edges
			for (int count = 0; count < V - 1; count++) {
				// pick mnimum key vertex from the set of vertices not yet included in MST using function minKey
				int u = this->minKey(key, inMST, V);

				// add the picked vertex to the MST
				inMST[u] = true;

				
				// update the key and parent index of the adjacent vertices of the picked vertex.
				for (int v = 0; v < V; v++) {
					// graph[u][v] is non-zero only for adjacent vertices
					// update key only if graph[u][v] is smaller than key[v] and v is not yet included in MST
					if (g[u][v] && !inMST[v] && g[u][v] < key[v]) {
						parent[v] = u;
						key[v] = g[u][v];
					}
				}
			}

			// create an adjacency matrix for the directed MST
			vector<vector<int>> mst(V, vector<int>(V, 0));

			// construct the MST graph, directed from parent to child
			for (int i = 1; i < V; i++) {
				mst[parent[i]][i] = g[parent[i]][i]; 
			}
			Tree tree(mst);
			return tree;
		}
};

class Kruskal : public Algo{
	private:
	    // a utility function to find the root of an element with path compression
		int findRoot(vector<int>& parent, int i) {
			if (parent[i] != i)
				parent[i] = this->findRoot(parent, parent[i]);
			return parent[i];
		}

		// a utility function to union two subsets
		void unionSets(vector<int>& parent, vector<int>& rank, int x, int y) {
			int rootX = this->findRoot(parent, x);
			int rootY = this->findRoot(parent, y);

			if (rank[rootX] < rank[rootY]) {
				parent[rootX] = rootY;
			} else if (rank[rootX] > rank[rootY]) {
				parent[rootY] = rootX;
			} else {
				parent[rootY] = rootX;
				rank[rootX]++;
			}
		}
	public:
	    Tree execute(Graph& graph) override {
			int V = graph.getV();  // Number of vertices
			vector<Edge> edges;
			vector<vector<int>> g = graph.getAdj();
			// make a list of all the edges in the graph
			for (int i = 0; i < V; i++) {
				for (int j = 0; j < V; j++) {
					if (g[i][j] != 0 && g[i][j] != INT_MAX) {
						edges.push_back({i, j, g[i][j]});
					}
				}
			}

			// sort the edges in ascending order of their weights
			sort(edges.begin(), edges.end(), [](Edge a, Edge b) {
				return a.weight < b.weight;
			});

			// parent vector to store the parent of each vertex
			vector<int> parent(V);
			vector<int> rank(V, 0);

			for (int i = 0; i < V; i++)
				parent[i] = i;

			// adjacency matrix to store the MST
			vector<vector<int>> mst(V, vector<int>(V, 0));

			// pick edges one by one and add them to the MST if they do not form a cycle
			int edgesInMST = 0;
			for (auto& edge : edges) {
				if (edgesInMST == V - 1)  // again when the mst has V-1 edges stop
					break;

				int u = edge.src;
				int v = edge.dest;

				int rootU = this->findRoot(parent, u);
				int rootV = this->findRoot(parent, v);

				if (rootU != rootV) {
					mst[u][v] = edge.weight;
					this->unionSets(parent, rank, rootU, rootV);
					edgesInMST++;
				}
			}

			Tree tree(mst);
			return tree;
		}
};




class MSTFactory {
    public:
        static std::unique_ptr<Algo> createMST(string type) {
            if (type == "prim") {
                return std::make_unique<Prim>();
            } else if (type == "kruskal") {
                return std::make_unique<Kruskal>();
            } else {
                return nullptr;
            }
        }
};