#!/usr/bin/env python3
# A simple unit test example. Replace by your own tests

import sys
import unittest
from a1ece650 import *

class MyTest(unittest.TestCase):
    
    def test_inter(self):
        p1 = Point (0,0)
        p2 = Point (1,1)
        p3 = Point (0,0)
        p4 = Point (0,1)

        l1 = Line (p1, p2)
        l2 = Line (p3, p4)

        res = l1.intersect(l2)

        self.assertEqual(res, Point(0,0))

    def test_no_inter(self):
        p1 = Point (0,0)
        p2 = Point (2,2)
        p3 = Point (5,0)
        p4 = Point (0,5)

        l1 = Line (p1, p2)
        l2 = Line (p3, p4)
        with self.assertRaises(Exception):
            res = l1.intersect(l2)

    def test_inter_mid(self):
        p1 = Point (0,0)
        p2 = Point (4,4)
        p3 = Point (5,0)
        p4 = Point (0,5)

        l1 = Line (p1, p2)
        l2 = Line (p3, p4)

        res = l1.intersect(l2)

        self.assertEqual(res, Point(2.50,2.50))

    def test_parse(self):
        cmd, street_name, cords = parse_input('add "Weber Street" (2,-1) (2,2) (5,5) (5,6) (3,8)')
        self.assertEqual(cmd, 'add')
        self.assertEqual(street_name, 'weber street')

        with self.assertRaises(Exception):
            cmd, street_name, cords = parse_input('add "Weber Street" (2,-1) (2,2(5,5) (5,6) (3,8)')


if __name__ == '__main__':
    unittest.main()
