// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <list>

// Reference https://www.geeksforgeeks.org/shortest-path-unweighted-graph/
class Graph {
    std::vector<std::vector<int>> adj;
    int v;
    public:
        void setV(int v);
        int getV();
        void addEdge(int src, int dst);
        bool bfs(int src, int dst, int pred[]);
        void printShortestDistance(int src, int dst);
};


void Graph::setV(int v_) {
    v = v_;
    for (int i=0; i<adj.size(); i++) {
        adj[i].clear();
    }
    adj.clear();
    for(int i = 0; i<=v; i++) {
        std::vector<int> vertices;
        adj.push_back(vertices);
    }
}

int Graph::getV() {
    return v;
}

void Graph::addEdge(int src, int dst) {
    adj[src].push_back(dst);
    adj[dst].push_back(src);
}

bool Graph::bfs(int src, int dst,int parent[]) {
    std::list<int> queue;
    bool visited[v+1];
    for (int i = 1; i <= v; i++) {
        visited[i] = false;
        parent[i] = -1;
    }
    visited[src] = true;
    queue.push_back(src);
    while (!queue.empty()) {
        int node = queue.front();
        queue.pop_front();
        std::vector<int> neighbours = adj[node];
        //std::reverse(neighbours.begin(),neighbours.end());
        for (auto x : neighbours) {
            if (visited[x] == false) {
                visited[x] = true;
                parent[x] = node;
                queue.push_back(x);
                if (x == dst)
                    return true;
            }
        }
    }
    return false;
}

void Graph::printShortestDistance(int src, int dst) {
    int parent[v+1];
    
    if (src == dst) {
        std::cout << src << std::endl;
        return;
    }
    if (bfs(src, dst, parent) == false) {
        std::cerr << "Error: src and dst not connected\n";
        return;
    }
    std::vector<int> path;
    for (int i = dst; i != -1; i = parent[i]) {
        path.push_back(i);
    }
    for (int i = path.size() - 1; i >= 0; i--) {
        if (i == 0) {
            std::cout << path[i] << std::endl;
        }
        else 
            std::cout << path[i] << "-";
    }
}

bool process_input (std::string line, std::string &err_msg, Graph &g) {
    std::string cmd;
    std::istringstream input(line); 
    std::ws(input);
    if (input.eof()) {
        err_msg = "Empty command";
        return false;
    }
    input >> cmd;
    if (cmd == "V") {
        int v;
        if (input.eof()) {
            err_msg = "V is not followed by a number";
            return false;
        }
        std::ws(input);
        input >> v;
        std::ws(input);
        if (!input.eof()) {
            err_msg = "V is not followed by more than one number";
            return false;
        }
        if (v < 1) {
            err_msg = "Invalid number of vertices";
            return false;
        }
        
        g.setV(v);
        std::cout << line << std::endl;
    } else if (cmd == "s") {
        int src = 0;
        int dst = 0;
        if (input.eof()) {
            err_msg = "s is not followed by two numbers";
            return false;
        }
        input >> src;
        std::ws(input);
        if (input.eof()) {
            err_msg = "s is not followed by two numbers";
            return false;
        }
        input >> dst;
        std::ws(input);
        if (!input.eof()) {
            err_msg = "s is followed by more than two numbers";
            return false;
        }
        if (src < 0 || src > g.getV() || dst < 0 || dst > g.getV()) {
            err_msg = "invalid src dst given";
            return false;
        }
        g.printShortestDistance(src,dst);
    } else if (cmd == "E") {
        if (input.eof()) {
            err_msg = "E is not followed by a list of numbers";
            return false;
        }
        g.setV(g.getV());
        //std::string edges = input.get();
        std::string edges;
        input >> edges;
        if (edges.at(0) != '{' || edges.at(edges.length()-1) != '}') {
            err_msg = "E is not followed by a list of numbers";
            return false;
        }
        for (auto i=1; i<edges.size();i++) {
            if (edges[i] == '<') {
                i++;
                std::string left = "";
                while (i<edges.size() && edges[i] != ',') {
                    left = left + edges[i];
                    i++;
                }
                i++;
                std::string right = "";
                while (i<edges.size() && edges[i] != '>') {
                    right = right + edges[i];
                    i++;
                }
                i++;
                std::stringstream v1(left);
                std::stringstream v2(right);
                int v1_ = 0;
                int v2_ = 0;
                v1 >> v1_;
                v2 >> v2_;
                if (v1_ < 1 || v2_ < 1 || v1_ > g.getV() || v2_ > g.getV()) {
                    err_msg = "invalid edge given";
                    return false;
                }
                g.addEdge(v1_,v2_);
            }
        }
        std::cout << line << std::endl;        
    } else {
        err_msg = "invalid input parameter";
        return false;
    }
    return true;
}


int main(int argc, char** argv) {
    std::string err_msg;
    Graph g;
    while (!std::cin.eof()) {
        // read a line of input until EOL and store in a string
        std::string line;
        std::getline(std::cin, line);

        if (line.size() == 0) {
            continue;
        }
        if (line.find("Error") != std::string::npos) {
            std::cout << "Error: Max retries exceeded" << std::endl;
            exit(1);
        }
        bool proccessed = process_input(line, err_msg, g);

        if (proccessed == false) {
            std::cerr << "Error: " << err_msg << std::endl;
            continue;
        }

    }
    
}


/*
V 15
E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}
s 2 10
2-8-10
V 5
E {<1,3>,<3,2>,<3,4>,<4,5>,<5,2>}
s 5 1
5-2-3-1

V 5
E {<1,3>,<1,2>,<3,4>,<4,5>,<5,2>}
s 5 1
5-2-1

   E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}
E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}
E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}   
E {<2,6 >,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}
E {<2,6 >,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>,<>}
E {<2,6>,< 2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>,<>}
E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}
E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,{10,8}}
E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>@}
E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>1234}

*/

