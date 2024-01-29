// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include <memory>
// Reference https://www.geeksforgeeks.org/shortest-path-unweighted-graph/
class Graph
{
    std::vector<std::vector<int>> adj;
    std::vector<int> edge_list;
    int v;

public:
    void setV(int v);
    int getV();
    void addEdge(int src, int dst);
    std::vector<int> getEdges();
    void printGraph();
    bool vertexCover(int num_cover);
};

void Graph::setV(int v_)
{
    v = v_;
    for (unsigned int i = 0; i < adj.size(); i++)
    {
        adj[i].clear();
    }
    adj.clear();
    edge_list.clear();
    for (int i = 0; i <= v; i++)
    {
        std::vector<int> vertices;
        adj.push_back(vertices);
    }
}

int Graph::getV()
{
    return v;
}

std::vector<int>  Graph::getEdges() {
    return edge_list;
}

void Graph::addEdge(int src, int dst)
{
    adj[src].push_back(dst);
    adj[dst].push_back(src);
    edge_list.push_back(src);
    edge_list.push_back(dst);
}

void Graph::printGraph()
{
    for (int i = 0; i < adj.size(); i++)
    {
        for (int j = 0; j < adj[i].size(); j++)
        {
            std::cout << adj[i][j] << " ";
        }
        std::cout << std::endl;
    }
    for (int i = 0; i < edge_list.size(); i++)
    {
        std::cout << edge_list[i] << " ";
    }
    std::cout << std::endl;
}

bool Graph::vertexCover(int num_cover)
{
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    Minisat::Lit literals[v][num_cover];
    // bool addClause_(vec<Lit>& ps);
    //  Reduction uses n*k literals n(vertices), k(covers)
    int n = v;
    int k = num_cover;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < k; j++)
        {
            literals[i][j] = Minisat::mkLit(solver->newVar());
        }
    }
    // Atleast one vertex in a vertex cover
    Minisat::vec<Minisat::Lit> lits;
    for (int i = 0; i < k; i++)
    {
        lits.clear();
        for (int j = 0; j < n; j++)
        {
            lits.push(literals[j][i]);
        }
        solver->addClause(lits);
    }

    // no one vertex can appear twice in a vertex cover
    for (int m = 0; m < n; m++)
    {
        for (int p = 0; p < k - 1; p++)
        {
            for (int q = p + 1; q < k; q++)
            {
                solver->addClause(~literals[m][p], ~literals[m][q]);
            }
        }
    }

    // No more than one vertex in each cover
    for (int m = 0; m < k; m++)
    {
        for (int p = 0; p < n - 1; p++)
        {
            for (int q = p + 1; q < n; q++)
            {
                solver->addClause(~literals[p][m], ~literals[q][m]);
            }
        }
    }

    // Every edge incident to atleast one vertex
    for (int i = 0; i < edge_list.size(); i = i + 2)
    {
        int j = i + 1;
        lits.clear();
        for (int m = 0; m < k; m++)
        {
            lits.push(literals[edge_list[i]-1][m]);
            lits.push(literals[edge_list[j]-1][m]);
        }
        solver->addClause(lits);
    }

    bool sat = solver->solve();
    if (sat) {
        std::vector<int> res;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < k; j++)
            {
                int b = Minisat::toInt(solver->modelValue(literals[i][j]));
                if (b == 0){
                    res.push_back(i+1);
                }
                // std::cout << b << "  ";
            }
            // std::cout << std::endl;
        }
        std::sort(res.begin(), res.end());
        for (int c = 0; c < res.size(); c++) {
            std::cout << res[c] << " ";
        }
        std::cout << std::endl;
    }
    
    return sat;
}

bool process_input(std::string line, std::string &err_msg, Graph &g)
{
    std::string cmd;
    std::istringstream input(line);
    std::ws(input);
    if (input.eof())
    {
        err_msg = "Empty command";
        return false;
    }
    input >> cmd;
    if (cmd == "V")
    {
        int v;
        if (input.eof())
        {
            err_msg = "V is not followed by a number";
            return false;
        }
        std::ws(input);
        input >> v;
        std::ws(input);
        if (!input.eof())
        {
            err_msg = "V is not followed by more than one number";
            return false;
        }
        if (v < 1)
        {
            err_msg = "Invalid number of vertices";
            return false;
        }

        g.setV(v);
    }
    else if (cmd == "E")
    {
        if (input.eof())
        {
            err_msg = "E is not followed by a list of numbers";
            return false;
        }
        g.setV(g.getV());
        //std::string edges = input.get();
        std::string edges;
        input >> edges;
        if (edges.at(0) != '{' || edges.at(edges.length() - 1) != '}')
        {
            err_msg = "E is not followed by a list of numbers";
            return false;
        }
        for (auto i = 1; i < edges.size(); i++)
        {
            if (edges[i] == '<')
            {
                i++;
                std::string left = "";
                while (i < edges.size() && edges[i] != ',')
                {
                    left = left + edges[i];
                    i++;
                }
                i++;
                std::string right = "";
                while (i < edges.size() && edges[i] != '>')
                {
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
                if (v1_ < 1 || v2_ < 1 || v1_ > g.getV() || v2_ > g.getV())
                {
                    err_msg = "invalid edge given";
                    return false;
                }
                g.addEdge(v1_, v2_);
            }
        }
        if (g.getEdges().size() < 1) {
            err_msg = "zero edges, empty graph, empty cover";
            return false;
        }
        for (int i=1; i<g.getV()+1; i++) {
            bool sat = g.vertexCover(i);
            if (sat) {
                break;
            }
        }
    }
    else
    {
        err_msg = "invalid input parameter";
        return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    std::string err_msg;
    Graph g;
    while (!std::cin.eof())
    {
        // read a line of input until EOL and store in a string
        std::string line;
        std::getline(std::cin, line);

        if (line.size() == 0)
        {
            continue;
        }
        bool proccessed = process_input(line, err_msg, g);

        if (proccessed == false)
        {
            if (err_msg == "zero edges, empty graph, empty cover") {
                std::cout << std::endl;
                continue;
            }
            std::cerr << "Error: " << err_msg << std::endl;
            continue;
        }
    }
}
