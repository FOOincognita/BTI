#ifndef _DAT_H
#define _DAT_H

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <stdexcept>
#include <bitset>
#include <iomanip>
#include <cstdint>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unordered_map>

enum class LaLo {LATITUDE, LONGITUDE};
enum class Parity {POS, NEG};

std::unordered_map<char,std::string> hexMap = {
    {'0',"0000"},
    {'1',"0001"},
    {'2',"0010"},
    {'3',"0011"},
    {'4',"0100"},
    {'5',"0101"},
    {'6',"0110"},
    {'7',"0111"},
    {'8',"1000"},
    {'9',"1001"},
    {'a',"1010"},
    {'b',"1011"},
    {'c',"1100"},
    {'d',"1101"},
    {'e',"1110"},
    {'f',"1111"},
    {'A',"1010"},
    {'B',"1011"},
    {'C',"1100"},
    {'D',"1101"},
    {'E',"1110"},
    {'F',"1111"},
};

struct data {
    LaLo latLong; 
    std::string bin;
    long b1428;
    Parity par29; 
    Parity par32;
};

/// Converts hex to decimal
int hexDec(std::string h) {
    int x(0);   
    std::stringstream ss;
    ss << std::hex << h;
    ss >> x;
    return x;
}

int binaryToDecimal(std::string s) {
    std::bitset<32> bits(s);
    int number = bits.to_ulong();
    return number;
}


#endif
