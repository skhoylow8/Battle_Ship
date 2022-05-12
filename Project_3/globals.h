//
//  globals.h
//  Project_3
//
//  Created by Setareh Khoylow on 5/4/22.
//

#ifndef globals_h
#define globals_h

#include <random>

const int MAXROWS = 10;
const int MAXCOLS = 10;

enum Direction {
    HORIZONTAL, VERTICAL
};

class Point
{
  public:
    Point() : r(0), c(0) {}
    Point(int rr, int cc) : r(rr), c(cc) {}
    int r;
    int c;
};

  // Return a uniformly distributed random int from 0 to limit-1
inline int randInt(int limit)
{
    static std::random_device rd;
    static std::mt19937 generator(rd());
    if (limit < 1)
        limit = 1;
    std::uniform_int_distribution<> distro(0, limit-1);
    return distro(generator);
}

#endif /* globals_h */
