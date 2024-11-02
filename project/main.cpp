#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "proactor.hpp"

#include "graph.hpp"
#include "tree.hpp"

#include "factory.hpp"
#include "leader_follower.hpp"
#include "active_object.hpp"
#include "pipeline.hpp"

#include <mutex>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>




using namespace std;


bool is_pipeline = false;
bool closed = false;
//Reactor* reactor;
std::unique_ptr<Proactor> proactor;
LF lf(5);
mutex mtx;

void* get_in_addr(struct sockaddr *sa)
{
        if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int Listener(int nport = 9035) {
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv, fd;


    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    //std::to_string(port).c_str()

    string port_str = to_string(nport);
    if ((rv = getaddrinfo(NULL, port_str.c_str(), &hints, &ai)) != 0) {
        cerr << "selectserver: " << gai_strerror(rv) << endl;
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd < 0) { 
            continue;
        }
        
        // Lose the pesky "address already in use" error message
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(fd, p->ai_addr, p->ai_addrlen) < 0) {
            close(fd);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    if (p == NULL) return -1;

    // Listen
    listen(fd, 10);
    return fd;
}

void calc(int& id, Graph& graph, Tree& mst, string algo, stringstream& ss){
    //cout << id << endl;
    if (id == 0) {
        MSTFactory factory;
        mst = factory.createMST(algo)->execute(graph);
        mst.floydWarshall();
        ss << "MST: " << endl;
        for (int i = 0; i < mst.getV(); i++) {
            for (int j = 0; j < mst.getV(); j++) {
                
                ss << std::to_string(mst.getWeight(i, j)) << " ";
            }
            ss << endl;
        }
        
	}
	else if (id == 1){ 
        ss << "total weight: " << mst.calc_weight() << endl;
    } else if (id == 2) {
        ss << "avg Dis: " << mst.avgDistance() << endl;
    } else if (id == 3) {
        ss << "max Dis: " << mst.maxDistance() << endl;
    } else if (id == 4) {
        ss << "min Dis: " << mst.minDistance() << endl;
    }
    //id++;
    
}

bool calcAll(Graph& graph, Tree& mst, string algo, stringstream& ss, int& checkFinished){
    this_thread::sleep_for(chrono::seconds(1));
    MSTFactory factory;
    mst = factory.createMST(algo)->execute(graph);
    mst.floydWarshall();
    ss << "MST: " << endl;
    for (int i = 0; i < mst.getV(); i++) {
        for (int j = 0; j < mst.getV(); j++) {
            ss << std::to_string(mst.getWeight(i, j)) << " ";
        }
        ss << endl;
    }
    ss << "total weight: " << mst.calc_weight() << endl;
    ss << "avg Dis: " << mst.avgDistance() << endl;
    ss << "max Dis: " << mst.maxDistance() << endl;
    ss << "min Dis: " << mst.minDistance() << endl;
    checkFinished = 1;
    return true;
}


void proccessCommand(string line, Graph& graph, Tree& mst, bool& is_graph, int fd, stringstream& ss) {
    cout << "ProccessCommand: " << line << endl;
    ss.str("");
    if(line[0] == 'g'){
        int m = stoi(line.substr(line.find(" ") + 1, line.find(",")));
        int n = stoi(line.substr(line.find(",") + 1, line.length()));
        vector<vector<int>> newGraph(n, vector<int>(n, 0));

        graph = Graph(m, n);

        char buf[256];
        int nbytes;
        cout << "format: 'a' 'b' 'w'" << endl;
        for(int i = 0; i < m; i++){
            nbytes = recv(fd, buf, sizeof(buf), 0);
            
            string s1 = buf;
            int f1 = s1.find(" ");
            int f2 = s1.find(" ", s1.find(" ") + 1);
            int a = stoi(s1.substr(0, f1));
            int b = stoi(s1.substr(f1 + 1, f2));
            int w = stoi(s1.substr(f2 + 1, s1.length()));
            

            if(a != b){
                cout << "a: " << a << endl;
                cout << "b: " << b << endl;
                cout << "w: " << w << endl;
                
                graph.addEdge(a, b, w);
            }
            else{
                i--;
            }
        }
        is_graph = true;
        ss << "new graph: " << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                
                ss << graph.getWeight(i, j) << " ";
            }
            ss << endl;
        }

    }
    else if (line[0] == 'p'){
        ss.str("");
        ss.clear();
        if (is_graph) {
            if (!is_pipeline){
                int checkFinished = 0;
                auto func = [&graph, &mst, &ss, &checkFinished] () {
                    return calcAll(graph, mst, "prim", ss, checkFinished);
                };
                lf.addTask(func);
                while (checkFinished == 0);
                
            } else {
                PipeLine p(5);
                
                auto func = [&graph, &mst](int& i, stringstream& ss1){
                    calc(i, graph, mst, "prim", ss1);
                };
                ss << p.start(func).str();
                p.join();
            }
        }
        
    }

    else if (line[0] == 'k'){
        ss.str("");
        ss.clear();
        if (is_graph) {
            if (!is_pipeline){
                int checkFinished = 0;
                auto func = [&graph, &mst, &ss, &checkFinished] () {
                    return calcAll(graph, mst, "kruskal", ss, checkFinished);
                };
                lf.addTask(func);
                while (checkFinished == 0);
            } else {
                PipeLine p(5);
                auto func = [&graph, &mst](int& i, stringstream& ss1){
                    calc(i, graph, mst, "kruskal", ss1);
                };
                ss << p.start(func).str();
                p.join();
            }
        }
    }

    else if(line[0] == 'n'){
        int f1 = line.find(" ");
        int f2 = line.find(" ", f1 + 1);
        int f3 = line.find(" ", f2 + 1);
        int a = stoi(line.substr(f1 + 1, f2));
        int b = stoi(line.substr(f2 + 1, f3));
        int w = stoi(line.substr(f3 + 1, line.length()));
        if (is_graph) {
            graph.addEdge(a, b, w);
            ss << "new edge: " << a << " " << b << " " << w << endl;
        }
        
    }

    else if(line[0] == 'r'){
        int f1 = line.find(" ");
        int f2 = line.find(" ", f1 + 1);
        int a = stoi(line.substr(f1 + 1, f2));
        int b = stoi(line.substr(f2 + 1, line.length()));
        if (is_graph) {
            graph.removeEdge(a, b);
            ss << "remove edge: " << a << " " << b << endl;
        }
        
    }

    else if(line[0] == 'q'){
        close(fd);
        cout << "Server closed bye bye ^_^" << endl;
    }
    else if(line[0] == 'w'){
        closed = true;
        close(fd);
        //proactor->removeFd(fd);
        cout << "Server closed bye bye ^_^" << endl;
        //exit(0);
    }

    else{
        ss << "Invalid command" << endl;
    }

}

void proactor_function(int fd) {
    Graph graph;
    Tree mst;
    bool is_graph;
    stringstream ss;
    for (;;) {
        
        char buf[256];
        memset(buf, 0, sizeof(buf));  // Clear buffer before use
        int nbytes = recv(fd, buf, sizeof(buf), 0);


        if (nbytes <= 0) {
            if (nbytes == 0) cout << "pollserver: socket " << fd << " hung up" << endl;
            else cerr << "recv";
            close(fd);
            //proactor->removeFd(fd);
            return;
        } else {
            cout << "Received command: " << buf << endl;
            proccessCommand(buf, graph, mst, is_graph, fd, ss);
            
            string response = ss.str();
            if (closed) break;
            if (response == "") break;
            if (send(fd, response.c_str(), response.length(), 0) == -1) {
                cerr << "send error" << endl;
                return;
            }

            ss.str("");
            ss.clear();
        }
    }
}

void proactor_accept(int fd) {
    for (;;) {
        if (closed) return;
        struct sockaddr_storage remoteaddr; // Client address
        socklen_t addrlen;
        addrlen = sizeof(remoteaddr);
        
        int newfd = accept(fd, (struct sockaddr *)&remoteaddr, &addrlen);

        if (newfd == -1) {
            cerr << "accept";
            continue;
        }
        
        proactor->addThread(newfd, proactor_function);
    }
}


    


int main(int argc, char* argv[]) {

    int nport = 9035;
    int opt;
    while ((opt = getopt(argc, argv, "l:p:")) != -1) {
        switch (opt) {
            case 'p'://pipeline
                nport = stoi(optarg);
                is_pipeline = true;
                break;
            case 'l'://leader follower
                nport = stoi(optarg);
                
                is_pipeline = false;
                break;
            default:
                cerr << "Usage: " << argv[0] << " [-p pipeline] [-l leader-follower]" << endl;
                exit(1);
        }
    }

    int listener = Listener(nport);

    if (listener == -1) {
        cerr << "error getting listening socket" << endl;
        exit(1);
    }

    cout <<"Functions: " << endl;
    cout << "g m n: create a new graph with m edges and n vertices" << endl;
    cout << "p: calculate the minimum spanning tree using Prim's algorithm" << endl;
    cout << "k: calculate the minimum spanning tree using Kruskal's algorithm" << endl;
    cout << "n a b w: add a new edge between vertices a and b with weight w" << endl;
    cout << "r a b: remove the edge between vertices a and b" << endl;
    cout << "c: calculate the total weight, average distance, maximum distance, and minimum distance of the MST" << endl;
    cout << "those statistics are calculated using design patterns chosen when starting the server" << endl;
    cout << "w: quit and close the server	(︶︹︶) " << endl;

   // reactor = new Reactor(listener, reactor_accept);
    proactor = std::make_unique<Proactor>(listener, proactor_accept);
    if (!is_pipeline) lf.start();
    while(!closed);
    lf.close();
    lf.join();
    return 0;

}