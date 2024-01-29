#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <unistd.h>
#include <fstream>

class Point {
    int x;
    int y;
    public:
        Point(int x_cor, int y_cor) {
            x = x_cor;
            y = y_cor;
        }
        void printPoint() {
            std::cout << " (" << x << "," << y << ")";
        }
        bool samePoints(Point p2) {
            if (x == p2.x && y == p2.y) {
                return true;
            }
            return false;
        }
        int orientation(Point p, Point q, Point r) {
            float val = (float(q.y - p.y) * (r.x - q.x)) - (float(q.x - p.x) * (r.y - q.y));
            if (val > 0) {
                return 1;
            }
            else if (val < 0) {
                return 2;
            }
            else {
                return 0;
            }
        }
        bool onSegment(Point p, Point q, Point r) {
            if ( (q.x <= std::max(p.x, r.x)) && (q.x >= std::min(p.x, r.x)) && 
                (q.y <= std::max(p.y, r.y)) and (q.y >= std::min(p.y, r.y))) {
                return true;
            }
            return false;
        }
        bool parallel(Point p0, Point p1, Point p2, Point p3) {
            if (p1.x != p0.x and p3.x != p2.x) {
                float m1 = float(p1.y - p0.y) / float(p1.x - p0.x);
                float m2 = float(p3.y - p2.y) / float(p3.x - p2.x);
                
                if (m1==m2) {
                    // std::cout << "slope " << m1 << " " << m2 << std::endl;
                    // std::cout << "slop(" << p1.x << ',' << p1.y << ')'
                    //     << '(' << p2.x << ',' << p2.y << ')'
                    //     << '(' << p3.x << ',' << p3.y << ')'
                    //     << '(' << p0.x << ',' << p0.y << ')' << std::endl;
                    return true;
                } else {
                    return false;
                }
            }
            else if (p1.x != p0.x or p3.x != p2.x)
                return false;
            else
                return true;
        }

        int colli(Point p1,Point p2,Point p3) {
            int area = p1.x*p2.y + p2.x*p3.y + p3.x*p1.y - p1.x*p3.y - p2.x*p1.y - p3.x*p2.y;
            // std::cout << "area is " << area << std::endl;
            
            return area;
        }

        bool overlap(Point p1, Point p2, Point p3, Point p4) {
            if (parallel(p1,p2,p3,p4)) {
                if (colli(p1,p2,p3) == 0) {
                    // std::cout << "3(" << p1.x << ',' << p1.y << ')'
                    //     << '(' << p2.x << ',' << p2.y << ')'
                    //     << '(' << p3.x << ',' << p3.y << ')'
                    //     << '(' << p4.x << ',' << p4.y << ')'
                    //     << std::endl;
                    return true;
                }
                if (colli(p1,p2,p4) == 0) {
                    // std::cout << "4(" << p1.x << ',' << p1.y << ')'
                    //     << '(' << p2.x << ',' << p2.y << ')'
                    //     << '(' << p3.x << ',' << p3.y << ')'
                    //     << '(' << p4.x << ',' << p4.y << ')'
                    //     << std::endl;
                    return true;
                }
            }
            return false;
        }
};

class Street {
    std::string name;
    std::vector<Point> points;
    public:
        Street(std::string street) {
            name = street;
        }
        std::string getName() {
            return name;
        }
        std::vector<Point> getPoints() {
            return points;
        }
        void addPoint(Point p) {
            points.push_back(p);
        }
        void clear() {
            points.clear();
        }
        Point getLastPoint() {
            return points[points.size() - 1];
        }
        bool isValidPoint(Point p) {
            if (points.size() == 0) {
                return true;
            }
            for (int i = 0; i < points.size(); i++) {
                if (points[i].samePoints(p)) {
                    return false;
                }
            }
            if (points.size() < 3) {
                return true;
            }
            for (int i = 0; i < points.size() - 2; i++) {
                if (intersect(points[i], points[i+1], points[points.size()-1],p)) {
                        
                        return false;
                    }
            }
            return true;
        }
        void printStreet() {
            std::cout << "add \"" << name << "\"";
            for (int i = 0; i < points.size(); i++) {
                points[i].printPoint();
            }
        }
        bool isValid(Point p3, Point p4) {
            for (int i = 0; i < points.size() - 1; i++) {
                Point p1 = points[i];
                Point p2 = points[i+1];
                if (p1.overlap(p1,p2,p3,p4)) {
                    return false;
                }
            }
            return true;
        }
        bool intersectExists(Street s2) {
            for (int i = 0; i < points.size()-1; i++) {
                for (int j = 0; j < s2.points.size()-1; j++) {
                    if (intersect(points[i], points[i+1], s2.points[j], s2.points[j+1])) {
                       return true;
                    }
                }
            }
            return false;
        }
        bool intersect(Point p1, Point q1, Point p2, Point q2) {
            int o1 = p1.orientation(p1,q1,p2);
            int o2 = p1.orientation(p1,q1,q2);
            int o3 = p1.orientation(p2,q2,p1);
            int o4 = p1.orientation(p2,q2,q1);
            if ((o1 != o2) && (o3 != o4))
                return true;
            if ((o1 == 0) && p1.onSegment(p1, p2, q1))
                return true;
            if ((o2 == 0) && p1.onSegment(p1, q2, q1))
                return true;
            if ((o3 == 0) && p1.onSegment(p2, p1, q2))
                return true;
            if ((o4 == 0) && p1.onSegment(p2, q1, q2))
                return true;
            return false;
        }
};
// https://richard.esplins.org/static/downloads/linux_book.pdf
int getRandomNumber(int min, int max) {
    std::ifstream urandom("/dev/urandom");
    if (urandom.fail()) {
        std::cerr << "Error: cannot open /dev/urandom\n";
        return 1;
    }
    unsigned int rand_num = 42;
    urandom.read((char *)&rand_num, sizeof(int));
    return min + (rand_num % (max - min + 1));
}
// https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
std::string getRandomName(const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}


int main(int argc, char **argv) {

    // Point p1(4,4);
    // Point p2(4,7);
    // Point p3(4,2);
    // Point p4(4,8);

    // bool over = p1.overlap(p1,p2,p3,p4);
    // std::cout << over << std::endl;

    int c = 20;
    int s = 10;
    int n = 5;
    int l = 5;
    std::string s_value, n_value, l_value, c_value;
    int o;

    opterr = 0;

    while ((o = getopt(argc, argv, "s:n:l:c:")) != -1) {
        switch(o) {
            case 's':
                if (optarg != NULL) {
                    s_value = optarg;
                    s = atoi(s_value.c_str());
                }
                break;
            case 'n':
                if (optarg != NULL) {
                    n_value = optarg;
                    n = atoi(n_value.c_str());
                }
                break;
            case 'l':
                if (optarg != NULL) {
                    l_value = optarg;
                    l = atoi(l_value.c_str());
                }
                break;
            case 'c':
                if (optarg != NULL) {
                    c_value = optarg;
                    c = atoi(c_value.c_str());
                }
                break;
            default:
                break;

        }
    }

    if (s < 2 || n < 1 || l < 5 || c < 1) {
        std::cerr << "Error, invalid args value given " << std::endl;
        exit(1);
    }

    // std::cout << s << n << l << c << std::endl;

    int retry = 0;
    std::vector<Street> streets;
    int num_streets;
    // int wait_sec;
    while(true) {
        num_streets = getRandomNumber(2,s);
        // std::cout << "streets # " << num_streets << std::endl;
        for (int m = 0; m < streets.size(); m++) {
            std::cout << "rm \"" << streets[m].getName() << "\"" << std::endl;
            streets[m].clear();
        }
        streets.clear();
        for (int i = 0; i < num_streets; i++) {
            std::string name = getRandomName(10);
            Street s(name);
            int line_segs = getRandomNumber(1,n);
            // std::cout << "line seg # " << line_segs << std::endl;
            for (int j = 0; j <= line_segs; j++) {
                if (retry > 25) {
                    std::cerr << "Error: " << "Too many retry" << std::endl;
                    std::cout << "Error: Too many retries" << std::endl;
                    exit(1);
                }
                int x = getRandomNumber(-c, c);
                int y = getRandomNumber(-c, c);
                bool valid = s.isValidPoint(Point(x,y));
                if (!valid) {
                    // std::cout << "invalid " << i << "th line " << j << " : " <<  x << "," << y << std::endl;
                    j--;
                    retry++;
                    continue;
                }
                if (i > 0 and j > 0) {
                    bool flag = 0;
                    // std::cout << i << " i " << std::endl;
                    for (int f = i-1; f >= 0; f--) {
                        Point src = s.getLastPoint();
                        Point dst = Point(x,y);
                        // std::cout << f << std::endl;
                        Street prev = streets[f];
                        if (!prev.isValid(src,dst)) {
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 1) {
                        j--;
                        retry++;
                        continue;
                    }
                }
                s.addPoint(Point(x,y));
            }
            streets.push_back(s);
        }
        // std::cout << "\n\n\n";
        bool intersectExists = false;
        for (int i = 0; i < streets.size() - 1; i++) {
            if (streets[i].intersectExists(streets[i+1])) {
                intersectExists = true;
            }
        }
        if (intersectExists == false) {
            for (int m = 0; m < streets.size(); m++) {
                streets[m].clear();
            }
            streets.clear();
            retry = 0;
            num_streets = 0;
            continue;
        }
        for (int j = 0; j < streets.size(); j++) {
            streets[j].printStreet();
            std::cout <<std::endl;
        }
        std::cout << "gg" << std::endl;
        int wait_time = getRandomNumber(5,l);
        sleep(wait_time);
        retry = 0;
        num_streets = 0;
    }

}

