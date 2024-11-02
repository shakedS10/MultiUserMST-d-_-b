#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <sstream>

#include <thread>

//#include "listener.hpp"


using namespace std;

class Proactor;
typedef void (*proactorFunc)(int fd);

class Proactor{
	private:
		int listener;
		vector<thread> threads;
		vector<int> fds;
	public:
	    Proactor(int listener, proactorFunc func);
		~Proactor();

		void addThread(int fd, proactorFunc func);
		void removeThread(thread& t);
		void removeFd(int fd);
		vector<thread>& getThreads();
		vector<int>& getFds();
		int getListener();

};
