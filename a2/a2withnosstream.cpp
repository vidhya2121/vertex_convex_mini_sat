// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <ctype.h>

class Graph {
    std::vector<int> *adj;
    int v;
    public:
        void setV(int v);
        int getV();
        void addEdge(int src, int dst);
        void printGraph();
        bool bfs(int src, int dst, int pred[]);
        void printShortestDistance(int src, int dst);
};


void Graph::setV(int v_) {
    v = v_;
    adj = new std::vector<int>[v+1];
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
        std::reverse(neighbours.begin(),neighbours.end());
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
        std::cout << "Error: src and dst not connected\n";
        return;
    }
    std::vector<int> path;
    for (int i = dst; i != -1; i = parent[i]) {
        path.push_back(i);
    }
    // printing path from source to destination
    for (int i = path.size() - 1; i >= 0; i--) {
        if (i == 0) {
            std::cout << path[i] << std::endl;
        }
        else 
            std::cout << path[i] << "-";
    }
}

bool check(char x) {
    if (isdigit(x)){
        return true;
    }else{
        return false;
    }
}

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool process_input (std::string line, std::string &err_msg, Graph &g) {
    if (line.length() < 1) {
        err_msg = "Empty command";
        return false;
    }
    std::string trim_line = trim(line);
    if (trim_line.at(0) == 'V') {
        std::string vertex = "";
        if (trim_line.at(1) != ' ') {
            err_msg = "V is not followed by a space";
            return false;
        }
        for (int i = 2; i < trim_line.length(); i++) {
            if (!isdigit(trim_line.at(i))) {
                err_msg = "Invalid number of vertices";
                return false;
            }
            vertex += trim_line.at(i);
        }
        int v = std::stoi(vertex);
        if (v < 1) {
            err_msg = "Invalid number of vertices";
            return false;
        }
        g.setV(v);
    } else if (trim_line.at(0) == 's') {
        if (trim_line.at(1) != ' ') {
            err_msg = "s is not followed by space";
            return false;
        }
        int i = 2;
        std::string source = "";
        while (i < trim_line.length() && trim_line.at(i) != ' ') {
            if (!isdigit(trim_line.at(i))) {
                err_msg = "s is not followed by two numbers";
                return false;
            }
            source += trim_line.at(i);
            i++;
        }
        if (i >= trim_line.length()) {
            err_msg = "s is not followed by two numbrs";
            return false;
        }
        i++;
        std::string dest = "";
        while (i < trim_line.length()) {
            if (!isdigit(trim_line.at(i))) {
                err_msg = "s is not followed by two numbrs";
                return false;
            }
            dest += trim_line.at(i);
            i++;
        }
        int src = std::stoi(source);
        int dst = std::stoi(dest);
        if (src < 0 || src > g.getV() || dst < 0 || dst > g.getV()) {
            err_msg = "invalid src dst given";
            return false;
        }
        g.printShortestDistance(src,dst);
    } else if (trim_line.at(0) == 'E') {
        int v_n = g.getV();
        g.setV(v_n);
        if (trim_line.at(1) != ' ') {
            err_msg = "E is not followed by space";
            return false;
        }
        if (trim_line.at(2) != '{' || trim_line.at(trim_line.length()-1) != '}') {
            err_msg = "E is not enclosed in { }";
            return false;
        }
        for (auto i=3; i<trim_line.length();i++) {
            if (trim_line.at(i) == '<') {
                i++;
                std::string left = "";
                while (trim_line.at(i) != ',') {
                    if (!isdigit(trim_line.at(i))) {
                        err_msg = "invalid characters in edges";
                        return false;
                    }
                    left = left + trim_line.at(i);
                    i++;
                }
                i++;
                std::string right = "";
                while (trim_line.at(i) != '>') {
                    if (!isdigit(trim_line.at(i))) {
                        err_msg = "invalid characters in edges";
                        return false;
                    }
                    right = right + trim_line.at(i);
                    i++;
                }
                i++;
                int v1_ = std::stoi(left);
                int v2_ = std::stoi(right);
                if (v1_ < 1 || v2_ < 1 || v1_ > g.getV() || v2_ > g.getV()) {
                    err_msg = "invalid edge given";
                    return false;
                }
                g.addEdge(v1_,v2_);
            } else {
                if (trim_line.at(i) == ',') {
                    i++;
                }
                err_msg = "invalid number of >";
                return false;
            }
        }

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
        bool proccessed = process_input(line, err_msg, g);

        if (proccessed == false) {
            std::cout << "Error: " << err_msg << std::endl;
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
*/