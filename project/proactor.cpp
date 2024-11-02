#include "proactor.hpp"

using namespace std;

Proactor::Proactor(int listener, proactorFunc func) {
	this->listener = listener;
	this->addThread(listener, func);
	this->threads[0].detach();
}

Proactor::~Proactor() {
	for (int i = 0; i < this->threads.size(); i++) {
		this->removeThread(this->threads[i]);
	}
}



void Proactor::addThread(int fd, proactorFunc func){
    this->fds.push_back(fd);
	this->threads.push_back(thread(func, fd));
}
void Proactor::removeThread(thread& t){
	int i = 0;
	for (; i < this->threads.size(); i++){
		if (addressof(this->threads[i]) == addressof(t)) break;
	}
	this->threads.erase(this->threads.begin() + i);
	this->fds.erase(this->fds.begin() + i);
	t.join();
}

void Proactor::removeFd(int fd){
	int i = 0;
	for (; i < this->fds.size(); i++){
		if (this->fds[i] == fd) break;
	}
	this->threads[i].join();
	this->threads.erase(this->threads.begin() + i);
	this->fds.erase(this->fds.begin() + i);
	
}


vector<thread>& Proactor::getThreads(){

	return this->threads;
}

vector<int>& Proactor::getFds(){
	return this->fds;
}

int Proactor::getListener(){
	return this->listener;
}