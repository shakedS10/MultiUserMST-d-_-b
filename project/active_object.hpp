#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>


using namespace std;

//typedef void (*PLtaskFunc)();
using PLtaskFunc = function<void()>;

class ActiveObject{
	private:
		thread task;
		queue<function<void()>> tasks;
		bool quit;
		bool end_task;
		//mutex mtx;


		void run(){
			while (!this->quit){
				PLtaskFunc func = this->getTask();
				if (func != nullptr) {
					//this->mtx.lock();
					this->end_task = false;
					func();
					//cout << -1 << endl;
					this->end_task = true;
					//this->mtx.unlock();
				}
			}
		}
	public:
	    ActiveObject(){
			this->quit = false;
			this->end_task = false;
		}
		void join(){
			this->task.join();
		}
		bool is_end_task() { return this->end_task; }
		void setCondition(bool quit){
            this->quit = quit;
        }

		void start(){
			this->task = thread([this]() {this->run();});
		}
		void request(PLtaskFunc task){
			this->tasks.push(task);
		}

		PLtaskFunc getTask(){
			if (this->tasks.size() == 0) return nullptr;
			PLtaskFunc func = this->tasks.front();
			this->tasks.pop();
			return func;
		}



};

