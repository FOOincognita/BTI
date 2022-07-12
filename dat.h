#ifndef _DAT_H
#define _DAT_H

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <stdexcept>
#include <bitset>
#include <iomanip>

auto CONV = 0.00017166154;

enum class LaLo {LATITUDE, LONGITUDE};
enum class Parity {POS, NEG};

struct data {
    LaLo latLong; 
    std::string bin;
    double b1428;
    Parity par29; 
    Parity par32;
};




#endif