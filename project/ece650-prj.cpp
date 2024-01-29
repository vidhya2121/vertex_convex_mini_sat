#include <iostream>
#include <sstream>
#include <vector>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "graph.hpp"

typedef struct values_cnf
{
    std::vector<int> res_cnf;
    Graph g;
} valuest_cnf;

typedef struct values_vc1
{
    std::vector<int> res_vc1;
    Graph g;
} valuest_vc1;

typedef struct values_vc2
{
    std::vector<int> res_vc2;
    Graph g;
} valuest_vc2;

void *vccnf(void *data)
{
    valuest_cnf *vals = static_cast<valuest_cnf *>(data);
    vals->g.cnf_sat_vc(vals->res_cnf);
    return nullptr;
}

void *vc1(void *data)
{
    valuest_vc1 *vals = static_cast<valuest_vc1 *>(data);
    vals->g.approx_vc1(vals->res_vc1);
    return nullptr;
}

void *vc2(void *data)
{
    valuest_vc2 *vals = static_cast<valuest_vc2 *>(data);
    vals->g.approx_vc2(vals->res_vc2);
    return nullptr;
}

int main(int argc, char **argv)
{

    Graph grh;

    // read from stdin until EOF
    while (!std::cin.eof())
    {

        // read a line of input until EOL and store in a string
        std::string line;
        std::getline(std::cin, line);

        // if nothing was read, go to top of the while to check for eof
        if (line.size() == 0)
        {
            continue;
        }

        char cmd;
        unsigned int ver_num;
        std::string edge_str;
        unsigned int src;
        unsigned int dst;
        std::string err_msg;
        int flag = 0;
        if (parse_line(line, cmd, ver_num, edge_str, src, dst, err_msg))
        {
            switch (cmd)
            {
            case 'V':
                if (ver_num <= 0)
                {
                    std::cout << "Error: "
                              << "vertex is out of range" << std::endl;
                }
                else
                {
                    grh.set_vertex(ver_num);
                }
                break;
            case 'E':
                std::istringstream input_edge(edge_str);

                unsigned int v_one;
                unsigned int v_two;
                char next_char;
                unsigned int vertex_value;

                input_edge >> next_char; //read '{'
                while (next_char != '}')
                {
                    input_edge >> next_char; //either read '<' or '}'
                    if (next_char == '}')
                    { // check if no edge is given
                        break;
                    }
                    input_edge >> vertex_value;
                    v_one = vertex_value;
                    input_edge >> next_char;
                    input_edge >> vertex_value;
                    v_two = vertex_value;
                    input_edge >> next_char;
                    input_edge >> next_char;

                    if (v_one > ver_num || v_two > ver_num || v_one < 1 || v_two < 1)
                    {
                        std::cout << "Error: "
                                  << "vertices are out of range" << std::endl;
                        flag = 1;
                        break;
                    }
                    else
                    {
                        grh.add_edge(v_one, v_two); //add the edge in the adjacency list
                    }
                }
                break;
            }
        }
        else
        {
            std::cout << "Error: " << err_msg << std::endl;
            flag = 1;
        }
        if (cmd == 'E' && flag == 0)
        {
            pthread_t t1, t2, t3;
            
            valuest_vc1 vals1 = {.res_vc1 = {}, .g = grh};
            valuest_vc2 vals2 = {.res_vc2 = {}, .g = grh};
            valuest_cnf valsc = {.res_cnf = {}, .g = grh};

            pthread_create(&t1, NULL, vccnf, &valsc);
            pthread_create(&t2, NULL, vc1, &vals1);
            pthread_create(&t3, NULL, vc2, &vals2);

            pthread_join(t1, NULL);
            pthread_join(t2, NULL);
            pthread_join(t3, NULL);

            display(valsc.res_cnf, vals1.res_vc1, vals2.res_vc2);

        }
    }
    return 0;
}