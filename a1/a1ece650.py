#!/usr/bin/env python3
import sys
import shlex
import ast
from collections import defaultdict
from copy import deepcopy
import re
import math

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    def __str__ (self):
        return '(' + str(self.x) + ',' + str(self.y) + ')'
    def __eq__(self, p2):
        if self.x == p2.x and self.y == p2.y:
            return True
        return False

class Line:
    def __init__(self, p1, p2):
        self.src = p1
        self.dst = p2
    def __str__(self):
        return str(self.src) + '-->' + str(self.dst)

    def getPoints(self):
        return self.src, self.dst

    # The following 3 methods are taken from the website https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/

    def onSegment(self, p, q, r):
        if ( (q.x <= max(p.x, r.x)) and (q.x >= min(p.x, r.x)) and 
            (q.y <= max(p.y, r.y)) and (q.y >= min(p.y, r.y))):
            return True
        return False

    def orientation(self, p, q, r):
        val = (float(q.y - p.y) * (r.x - q.x)) - (float(q.x - p.x) * (r.y - q.y))
        if (val > 0):
            return 1
        elif (val < 0):
            return 2
        else:
            return 0

    def doIntersect(self,l2):
        p1 = self.src
        q1 = self.dst
        p2 = l2.src
        q2 = l2.dst
        
        o1 = self.orientation(p1, q1, p2)
        o2 = self.orientation(p1, q1, q2)
        o3 = self.orientation(p2, q2, p1)
        o4 = self.orientation(p2, q2, q1)
        if ((o1 != o2) and (o3 != o4)):
            return True
        if ((o1 == 0) and self.onSegment(p1, p2, q1)):
            return True
        if ((o2 == 0) and self.onSegment(p1, q2, q1)):
            return True
        if ((o3 == 0) and self.onSegment(p2, p1, q2)):
            return True
        if ((o4 == 0) and self.onSegment(p2, q1, q2)):
            return True
        return False

    # this following method is taken from https://git.uwaterloo.ca/ece650-1221/py/-/blob/master/py/intersect.py
    def intersect (self, l2):
        if not self.doIntersect(l2):
            raise Exception('lines do not intersect')
        x1, y1 = self.src.x, self.src.y
        x2, y2 = self.dst.x, self.dst.y
        x3, y3 = l2.src.x, l2.src.y
        x4, y4 = l2.dst.x, l2.dst.y
        xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))
        xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
        xcoor =  xnum / xden

        ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)
        yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4)
        ycoor = ynum / yden

        xcoor = int(xcoor) if int(xcoor) == xcoor else round(xcoor,2)
        ycoor = int(ycoor) if int(ycoor) == ycoor else round(ycoor,2)

        return Point(xcoor, ycoor)

class Street:
    def __init__(self):
        self.street_names = []
        self.street_dict = {}
        self.street_lines = {}
        self.point_street = {}
        self.street_dict_with_inter = {}

    def add_street(self, name, cords):
        if name in self.street_names:
            raise Exception('cannot add an old street')
        self.street_names.append(name)
        self.street_dict[name] = cords
        self.street_dict_with_inter[name] = cords

        lines = []
        for i in range(0,len(cords) - 1):
            line = Line(cords[i], cords[i+1])
            lines.append(line)
        
        self.street_lines[name] = lines

        for i in cords:
            self.point_street[(i.x,i.y)] = name

    def setCurrStreets(self):
        self.street_dict_with_inter = deepcopy(self.street_dict)

    def addInterPoint(self, name, p1, inter, p2):
        temp = self.street_dict_with_inter[name]
        for p in temp:
            if p == inter:
                return
        first = -1
        second = -1
        for i, point in enumerate(temp):
            if point == p1:
                first = i
            if point == p2:
                second = i
        (first, second) = (second, first) if first > second else (first, second)
        offset = 0
        if abs(first - second) > 1:
            between_inter = temp[first+1:second]
            dist_inter_first = math.dist([inter.x,inter.y],[temp[first].x, temp[first].y])
            for poin in between_inter:
                dist_poin_first = math.dist([poin.x,poin.y],[temp[first+offset].x, temp[first+offset].x])
                if dist_poin_first > dist_inter_first:
                    break
                offset = offset + 1

            temp.insert(first+offset+1 , inter)
        else:
            temp.insert(first+1,inter)
        self.street_dict_with_inter[name] = temp
    
    def mod_street(self, name, cords):
        if name not in self.street_names:
            raise Exception('cannot modify a new street')
        self.street_dict[name] = cords

        lines = []
        for i in range(0,len(cords) - 1):
            line = Line(cords[i], cords[i+1])
            lines.append(line)
        
        self.street_lines[name] = lines

        for i in cords:
            self.point_street[(i.x,i.y)] = name

    def rm_street(self, name):
        if name not in self.street_names:
            raise Exception('cannot remove a new street')
        self.street_names.remove(name)
        del self.street_dict[name]
        del self.street_lines[name]
        self.point_street = {key:val for key, val in self.point_street.items() if val != name}

# add "Weber Street" (2,-1) (2,2) (5,5) (5,6) (3,8)
# add "King Street S" (4,2) (4,8)  
# add "Davenport Road" (1,4) (5,8)
# mod "Weber Street" (2,1) (2,2)
# rm "King Street S"

def parse_input(line):
    rr = re.compile(r'(add|mod|rm|gg)([ ]+"[ ]*[a-zA-z][a-zA-z ]+")?(([ ]+\([ ]*[-]?[0-9]+[ ]*,[ ]*[-]?[0-9]+[ ]*\))*)$')
    ans = rr.match(line.strip())
    cmd = None
    street_name = None
    cords = []
    if ans:
        cmd=ans.group(1).strip()
        if cmd == 'add' or cmd == 'mod':
            rr = re.compile(r'(add|mod|rm|gg)([ ]+"[ ]*[a-zA-z][a-zA-z ]+"){1}(([ ]+\([ ]*[-]?[0-9]+[ ]*,[ ]*[-]?[0-9]+[ ]*\))*)$')
            ans = rr.match(line.strip())
            if not ans:
                raise Exception('invalid arguments given for ' + cmd + ' operation')
            street_name=ans.group(2).strip().replace('"','').lower()
            cordinates=ans.group(3).strip()
            cordsList=re.findall(r'\([ ]*[-]?[0-9]+[ ]*,[ ]*[-]?[0-9]+[ ]*\)', cordinates)
            cordsList = [c.replace(' ','') for c in cordsList]
            if len(cordsList) < 2:
                raise Exception('invalid number of cordinates given')
            for c in cordsList:
                point_tuple = ast.literal_eval(c)
                if Point(point_tuple[0], point_tuple[1]) in cords:
                    raise Exception('duplicate arguments given for same street')
                cords.append(Point(point_tuple[0], point_tuple[1]))
            
        elif cmd == 'rm':
            rr = re.compile(r'(add|mod|rm|gg)([ ]+"[ ]*[a-zA-z][a-zA-z ]+"){1}$')
            ans = rr.match(line.strip())
            if not ans:
                raise Exception('invalid arguments for rm operation')
            street_name=ans.group(2).strip().replace('"','').lower()
        elif cmd == 'gg':
            rr = re.compile(r'(add|mod|rm|gg)$')
            ans = rr.match(line.strip())
            if not ans:
                raise Exception('invalid arguments for gg operation')
        else:
            raise Exception('given cmd isinvalid')
     
    else:
        raise Exception('the given input format is invalid')
    return cmd, street_name, cords



# YOUR CODE GOES HERE


def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    streets = Street()
    vertices = []
    intersections = []
    inter_points = defaultdict(list)
    comp_streets = []
    while True:
        line = sys.stdin.readline()
        if line == "":
            break
        if line.strip() == "":
            print('Error: ' + 'empty inputs', file=sys.stderr)
            continue
        cmd = None 
        street_name = None
        cords = None
        try:
            cmd, street_name, cords = parse_input(line)
        except Exception as e:
            print('Error: ' + str(e), file=sys.stderr)
            continue

        try:
            if cmd == 'add' or cmd == 'mod' or cmd == 'rm':
                if cmd == 'add':
                    streets.add_street(street_name, cords)
                elif cmd == 'mod':
                    streets.mod_street(street_name, cords)
                else:
                    streets.rm_street(street_name)
                streets.setCurrStreets()
                vertices = []
                intersections = []
                inter_points = defaultdict(list)
                comp_streets = []
                for i in streets.street_names:
                    for j in streets.street_names:
                        if i != j:
                            if (i,j) not in comp_streets and (j,i) not in comp_streets:
                                comp_streets.append((i,j))
                                st1 = streets.street_lines[i]
                                st2 = streets.street_lines[j]
                                for l1 in st1:
                                    for l2 in st2:
                                        try:
                                            inter = l1.intersect(l2)
                                            inter_points[(inter.x, inter.y)].append(i)
                                            inter_points[(inter.x, inter.y)].append(j)
                                            intersections.append((inter.x, inter.y))
                                            if (inter.x, inter.y) not in vertices:
                                                vertices.append((inter.x, inter.y))
                                            p1, p2 = l1.getPoints()
                                            p3, p4 = l2.getPoints()
                                            streets.addInterPoint(i,p1,inter,p2)
                                            streets.addInterPoint(j,p3,inter,p4)
                                            if (p1.x,p1.y) not in vertices:
                                                vertices.append((p1.x,p1.y))
                                            if (p2.x,p2.y) not in vertices:
                                                vertices.append((p2.x,p2.y))
                                            if (p3.x,p3.y) not in vertices:
                                                vertices.append((p3.x,p3.y))
                                            if (p4.x,p4.y) not in vertices:
                                                vertices.append((p4.x,p4.y))
                                        except Exception as e:
                                            pass

            if cmd == 'gg':
                print("V = {", file=sys.stdout)
                for i in range(len(vertices)):
                    
                    print("  " + str(i + 1) + ":", vertices[i], file=sys.stdout)
                print("}", file=sys.stdout)

                edges = []
                
                for name,cords in streets.street_dict_with_inter.items():
                    for i in range(len(cords)-1):
                        p1 = cords[i]
                        p2 = cords[i+1]
                        if (p1.x,p1.y) in intersections or (p2.x,p2.y) in intersections:
                            edges.append((vertices.index((p1.x,p1.y))+1, vertices.index((p2.x,p2.y))+1))
                            # edges.append(((p1.x,p1.y), (p2.x,p2.y)))
                

                print("E = {", file=sys.stdout)
                for i in range(len(edges)):
                    if i == len(edges) - 1:
                        print("  <" + str(edges[i][0]) + "," + str(edges[i][1]) + ">", file=sys.stdout)
                    else:
                        print("  <" + str(edges[i][0]) + "," + str(edges[i][1]) + ">,", file=sys.stdout)
                print("}", file=sys.stdout)


                # for name,x_x in streets.street_dict_with_inter.items():
                #     for x in x_x:
                #         print(x , end=" ")
                #     print(name)
        
        except Exception as exp:
            print('Error: ' + str(exp), file=sys.stderr)

        
                
        # print("read a line:", line)

    # print("Finished reading input")
    # return exit code 0 on successful termination
    sys.exit(0)


if __name__ == "__main__":
    main()
