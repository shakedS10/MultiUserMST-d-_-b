
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>

using namespace std;

using LFtaskFunc = function<bool()>;
// FIXED - changed to atomic to fix crashing issue ~shaked :) 
class LF {
private:
    int numThreads;
    atomic<int> leader;
    atomic<bool> quit;
    mutex mtx;
    vector<thread> followers;
    queue<LFtaskFunc> tasks;

public:
    LF(int numThreads) : numThreads(numThreads), leader(numThreads), quit(false) {}

    void start() {
        for (int i = 0; i < this->numThreads; i++) {
            followers.emplace_back([this, i]() { this->run(i); });
        }
    }

    void join() {
        quit.store(true);

  
        for (auto& t : followers) {
            t.join();
        }
    }

    void addTask(LFtaskFunc task) {
        lock_guard<mutex> lock(mtx);
        tasks.push(task);
    }

    void close() {
        quit.store(true);
    }

private:
    void proccessTask(LFtaskFunc task) {
        task();
    }

    void promoteLeader(int id) {
        leader.store(id);
    }

    bool isLeader(int id) const {
        return leader.load() == id;
    }

    bool hasLeader() const {
        return leader.load() != numThreads;
    }

    void removeLeader() {
        leader.store(numThreads);
    }

    LFtaskFunc getTask() {
        lock_guard<mutex> lock(mtx);
        if (tasks.empty()) {
            return nullptr;
        }
        LFtaskFunc task = tasks.front();
        tasks.pop();
        return task;
    }

    void run(int id) {
        while (!quit.load()) {
            if (!hasLeader()) {
                promoteLeader(id);
            }
            if (isLeader(id)) {
                LFtaskFunc task = getTask();
                if (task) {
                    proccessTask(task);
                }
                removeLeader();
            }
        }
    }
};
