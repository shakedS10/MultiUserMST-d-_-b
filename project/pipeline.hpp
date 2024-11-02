#include "active_object.hpp"
#include "graph.hpp"
#include "tree.hpp"

#include <functional>
#include <utility>
#include <iostream>
#include <vector>
#include <thread>
#include <sstream>

using namespace std;

//typedef void (*taskFunc)();
//typedef void (*runTask)(int id, Graph* graph, Tree* mst, bool is_kruskal, stringstream& ss);

using PLtaskFunc = function<void()>;
using runTask = function<void(int& id, stringstream& ss)>;


class PipeLine{
private:
	vector<ActiveObject> pipes;
	
	int n;
public:
    

	PipeLine(int n){
		this->n = n;

		for (int i = 0; i < n; i++){
			this->pipes.push_back(ActiveObject());
		}
	}

	

	stringstream start(runTask func){
		for (int i = 0; i < this->n; i++){
			this->pipes[i].start();
		}
		stringstream ss;
		int id = 0;
		//int tmp = -1;
		this->pipes[id].request([&id, func, &ss] () {func(id, ss);});
		while (1)
		{
			//cout << id << this->n << endl;
			if (this->pipes[id].is_end_task()){
				id++;
				if (id >= this->n) break;
				this->pipes[id].request([&id, func, &ss] () {func(id, ss);});
				//cout << id << this->n << endl;
			}
		}
		//cout << id<<1 << endl;
		return ss;
	}

	void join(){
		for (int i = 0; i < this->n; i++){
			this->pipes[i].setCondition(true);
		}
        for (int i = 0; i < this->n; i++){
            this->pipes[i].join();
        }
    }
};

