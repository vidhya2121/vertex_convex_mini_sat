#include<string>
#include<vector>
#include <memory>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"

class Graph
{
    unsigned int no_of_vertex;
    std::vector<std::vector<unsigned int>> adjlist;
    std::vector<unsigned int> resultant_path;
    std::vector<int> vertcies_of_edges;

public:
    
    void set_vertex(unsigned int num_vertex);

    unsigned int get_vertex();

    void add_edge(unsigned int v_one, unsigned int v_two);

    void erase_edges(int w);

    int get_highest_degree_v();

    bool vertexcover(int k, std::vector<int> &res_cnf_sat);

    void cnf_sat_vc(std::vector<int> &res_cnf_sat);

    void approx_vc1(std::vector<int> &res_vc1);

    void approx_vc2(std::vector<int> &res_vc2);
};

bool parse_line(const std::string &line, char &cmd, unsigned int &num_ver, std::string &edge_str, unsigned int &src, unsigned int &dst, std::string &err_msg);

void display(std::vector<int> res_cnf,std::vector<int> res_vc1,std::vector<int> res_vc2);