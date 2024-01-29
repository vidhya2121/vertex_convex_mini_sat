#include <unistd.h>
#include <iostream>
#include <vector>
#include <signal.h>
#include <sys/wait.h>

int rgen(int argc, char **argv) {   
    argv[0] = (char *) "rgen";
    execv("rgen", argv);
    return 0;
}

int pyth() {   
    char *args[3];
    args[0] = (char *) "python3";
    args[1] = (char *) "ece650-a1.py";
    args[2] = nullptr;
    execvp("python3", args);
    return 0;
}


int send() {
    while(!std::cin.eof()) {
        std::string line;
        std::getline(std::cin, line);
        if (line.size() > 0) {
            std::cout <<line << std::endl;
        }
    }
    return 0;
}

int cppp() {   
    char *args[2];
    args[0] = (char *) "ece650-a2";
    args[1] = nullptr;
    execv("ece650-a2", args);
    return 0;
}

void validate(int argc, char **argv) {
    std::string s_value, n_value, l_value, c_value;
    int o;

    opterr = 0;
    while ((o = getopt(argc, argv, "s:n:l:c:")) != -1) {
        switch(o) {
            case 's':
                if (optarg != NULL) {
                    s_value = optarg;
                    int s = atoi(s_value.c_str());
                    if (s < 2) {
                        std::cerr << "Error: arg s should be >=2" << std::endl;
                        exit(1);
                    }
                }
                break;
            case 'n':
                if (optarg != NULL) {
                    n_value = optarg;
                    int n = atoi(n_value.c_str());
                    if (n < 1) {
                        std::cerr << "Error: arg n should be >=1" << std::endl;
                        exit(1);
                    }
                }
                break;
            case 'l':
                if (optarg != NULL) {
                    l_value = optarg;
                    int l = atoi(l_value.c_str());
                    if (l < 5) {
                        std::cerr << "Error: arg l should be >=5" << std::endl;
                        exit(1);
                    }
                }
                break;
            case 'c':
                if (optarg != NULL) {
                    l_value = optarg;
                    int c = atoi(l_value.c_str());
                    if (c < 1) {
                        std::cerr << "Error: arg c should be >=1" << std::endl;
                        exit(1);
                    }
                }
                break;
            default:
                break;

        }
    }
}

int main (int argc, char **argv) {
    validate(argc, argv);
    int regentoa1[2];
    pipe(regentoa1);
    std::vector<pid_t> child;
    int a1toa2[2];
    pipe(a1toa2);

    auto pid = fork();
   
    if (pid == 0) {
        dup2(regentoa1[1], STDOUT_FILENO);
        close(regentoa1[0]);
        close(regentoa1[1]);
        return rgen(argc, argv);
    }
    child.push_back(pid);
    pid = fork();
   

    if (pid == 0) {
        dup2(regentoa1[0], STDIN_FILENO);
        close(regentoa1[0]);
        close(regentoa1[1]);

        dup2(a1toa2[1], STDOUT_FILENO);
        close(a1toa2[0]);
        close(a1toa2[1]);
        return pyth();
    } 

    child.push_back(pid);
    pid = fork();
    if (pid == 0) {
        dup2(a1toa2[0], STDIN_FILENO);
        close(a1toa2[0]);
        close(a1toa2[1]);
        return cppp();
    }
    child.push_back(pid);

    pid = fork();
    if (pid == 0) {
        dup2(a1toa2[1], STDOUT_FILENO);
        close(a1toa2[0]);
        close(a1toa2[1]);
        return send();
    }
    child.push_back(pid);
    
    int status;
    wait(&status);
    for (pid_t k : child) {
        int status;
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }

    return 0;
}
