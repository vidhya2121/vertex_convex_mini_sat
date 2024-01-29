#include "graph.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <chrono>
#include <thread>
// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"

void Graph::set_vertex(unsigned int num_vertex)
{
    no_of_vertex = num_vertex + 1;
    // adjlist = new std::vector<unsigned int>[num_vertex + 1];
    // std::vector<unsigned int>adjlist[num_vertex + 1];
    adjlist.clear();
    adjlist.resize(no_of_vertex + 1);
    vertcies_of_edges.clear();
}

unsigned int Graph::get_vertex()
{
    return no_of_vertex;
}

void Graph::add_edge(unsigned int v_one, unsigned int v_two)
{
    if (v_one <= no_of_vertex && v_two <= no_of_vertex)
    {
        // link the vertex into edge
        adjlist[v_one].push_back(v_two);
        adjlist[v_two].push_back(v_one);
        vertcies_of_edges.push_back(v_one);
        vertcies_of_edges.push_back(v_two);
    }
}

void Graph::erase_edges(int w) {
    std::vector<unsigned int> neigh = adjlist[w];
    for (auto i : neigh)
    {
        adjlist[i].erase(std::remove(adjlist[i].begin(), adjlist[i].end(), w), adjlist[i].end());
    }
    adjlist[w].clear();
}

int Graph::get_highest_degree_v()
{
    int max_deg = 0;
    int high_deg_v;
    for (int i = 1; i <= no_of_vertex; i++)
    {
        int deg = adjlist[i].size();
        if (deg > max_deg)
        {
            max_deg = deg;
            high_deg_v = i;
        }
    }
    if (max_deg == 0)
    {
        return -1;
    }
    return high_deg_v;
}

bool Graph::vertexcover(int k, std::vector<int> &res_cnf_sat)
{

    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    Minisat::Var Literals[no_of_vertex][k];

    for (unsigned int i = 0; i < no_of_vertex; i++)
    {
        for (unsigned int j = 0; j < k; j++)
        {
            Literals[i][j] = solver->newVar();
        }
    }

    // 1) At least one vertex is the ith vertex in the vertex cover:
    for (unsigned int i = 0; i < k; i++)
    {
        Minisat::vec<Minisat::Lit> at_least_one_vertex;
        for (unsigned int j = 0; j < no_of_vertex; j++)
        {
            at_least_one_vertex.push(Minisat::mkLit(Literals[j][i]));
        }
        solver->addClause(at_least_one_vertex);
        at_least_one_vertex.clear();
    }

    // 2) No one vertex can appear twice in a vertex cover.
    for (unsigned int m = 0; m < no_of_vertex; m++)
    {
        for (unsigned int p = 0; p < k; p++)
        {
            for (unsigned int q = 0; q < p; q++)
            {
                solver->addClause(~Minisat::mkLit(Literals[m][p]), ~Minisat::mkLit(Literals[m][q]));
            }
        }
    }

    // 3) No more than one vertex appears in the mth position of the vertex cover.
    for (unsigned int m = 0; m < k; m++)
    {
        for (unsigned int q = 0; q < no_of_vertex; q++)
        {
            for (unsigned int p = 0; p < q; p++)
            {
                solver->addClause(~Minisat::mkLit(Literals[p][m]), ~Minisat::mkLit(Literals[q][m]));
            }
        }
    }

    // 4) Every edge is incident to at least one vertex in the vertex cover.
    for (unsigned int i = 0; i < vertcies_of_edges.size(); i++)
    {
        Minisat::vec<Minisat::Lit> every_edge_one_vertex;
        for (unsigned int p = 0; p < k; p++)
        {
            every_edge_one_vertex.push(Minisat::mkLit(Literals[vertcies_of_edges[i]][p]));
            every_edge_one_vertex.push(Minisat::mkLit(Literals[vertcies_of_edges[i + 1]][p]));
        }
        solver->addClause(every_edge_one_vertex);
        every_edge_one_vertex.clear();
        i++;
    }

    res_cnf_sat.clear();
    bool sat = solver->solve();
    if (sat)
    {
        for (int i = 0; i < no_of_vertex; i++)
        {
            for (int j = 0; j < k; j++)
            {
                if (Minisat::toInt(solver->modelValue(Literals[i][j])) == 0)
                {
                    res_cnf_sat.push_back(i);
                }
            }
        }
        std::sort(res_cnf_sat.begin(), res_cnf_sat.end());

        solver.reset(new Minisat::Solver());
    }
    return sat;
}

void Graph::cnf_sat_vc(std::vector<int> &res_cnf_sat)
{
    unsigned int k = 1;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    while (k <= get_vertex())
    {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        int diff = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
        if (diff > 10) {
            // std::cerr << "Timeout in cnf sat" << std::endl;
            break;
        }
        bool sat = vertexcover(k, res_cnf_sat);
        if (sat)
        {
            break;
        }
        k++;
    }
}

void Graph::approx_vc1(std::vector<int> &res_vc1) {
    int high_degree = get_highest_degree_v();
    while (high_degree != -1)
    {
        res_vc1.push_back(high_degree);
        erase_edges(high_degree);
        high_degree = get_highest_degree_v();
    }
    std::sort(res_vc1.begin(), res_vc1.end());
    return;
}

void Graph::approx_vc2(std::vector<int> &res_vc2) {
    res_vc2.push_back(vertcies_of_edges[0]);
    res_vc2.push_back(vertcies_of_edges[1]);
    vertcies_of_edges.erase(vertcies_of_edges.begin(),vertcies_of_edges.begin()+2);

    for(int i=0; i<vertcies_of_edges.size(); i=i+2){
        if (std::count(res_vc2.begin(), res_vc2.end(), vertcies_of_edges[i]) + std::count(res_vc2.begin(), res_vc2.end(), vertcies_of_edges[i+1]) == 0) {
            res_vc2.push_back(vertcies_of_edges[i]);
            res_vc2.push_back(vertcies_of_edges[i+1]);
        }
    }
    std::sort(res_vc2.begin(), res_vc2.end());
    return;
}

bool parse_line(const std::string &line, char &cmd, unsigned int &num_ver, std::string &edge_str, unsigned int &src, unsigned int &dst, std::string &err_msg)
{
    std::istringstream input(line);

    std::ws(input);

    if (input.eof())
    {
        err_msg = "Empty Command";
        return false;
    }

    char ch;
    input >> ch;

    if (input.fail())
    {
        err_msg = "input fail to read";
        return false;
    }

    if (ch == 'V')
    {
        unsigned int n;
        input >> n;
        if (input.fail())
        {
            err_msg = "Missing or bad argument";
            return false;
        }
        std::ws(input);
        if (!input.eof())
        {
            err_msg = "Unexpected argument";
            return false;
        }
        cmd = ch;
        num_ver = n;
        return true;
    }
    else if (ch == 'E')
    {
        std::string edge;
        input >> edge;
        if (input.fail())
        {
            err_msg = "Missing or bad argument";
            return false;
        }
        cmd = ch;
        edge_str = edge;
        return true;
    }
    else if (ch == 's')
    {
        unsigned int s;
        unsigned int d;
        input >> s;
        input >> d;
        if (input.fail())
        {
            err_msg = "Missing or bad argument";
            return false;
        }
        std::ws(input);
        if (!input.eof())
        {
            err_msg = "Unexpected argument";
            return false;
        }
        cmd = ch;
        src = s;
        dst = d;
        return true;
    }
    else
    {
        err_msg = "unknown command";
        return false;
    }
}

void display(std::vector<int> res_cnf,std::vector<int> res_vc1,std::vector<int> res_vc2)
{
    std::cout << "CNF-SAT-VC: ";
    if(res_cnf.size() == 0){
        std::cout << "TIMEOUT";
    }
    else{
        for (int i = 0; i < res_cnf.size(); i++)
        {
            if (res_cnf.size() - 1 == i)
            {
                std::cout << res_cnf[i];
            }
            else
            {
                std::cout << res_cnf[i] << ",";
            }
        }
    }
    
    std::cout << std::endl;
    std::cout << "APPROX-VC-1: ";
    for (int i = 0; i < res_vc1.size(); i++)
    {
        if (res_vc1.size() - 1 == i)
        {
            std::cout << res_vc1[i] << std::endl;
        }
        else
        {
            std::cout << res_vc1[i] << ",";
        }
    }
    std::cout << "APPROX-VC-2: ";
    for (int i = 0; i < res_vc2.size(); i++)
    {
        if (res_vc2.size() - 1 == i)
        {
            std::cout << res_vc2[i] << std::endl;
        }
        else
        {
            std::cout << res_vc2[i] << ",";
        }
    }
}
