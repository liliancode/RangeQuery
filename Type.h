#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iostream>
#include <bitset>
#include <thread>
#include <math.h> 
#include <sstream>
#include <cstring>

#define V5D_5T 0          // Variant   0: 5D      1: 5T
#define Filter5D5T        (0b1111111111 >> V5D_5T) 

 
using namespace std;
using namespace std::chrono;


class Horloge
{
	steady_clock::time_point clock_begin;
    public:
	void Start() { clock_begin = steady_clock::now(); }
	double TimeElapsedSeconds() { return duration_cast<milliseconds>(steady_clock::now() - clock_begin).count()*0.001; }
};

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;


typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;


typedef u16 IDMov;  // must be u16 u32 or u64
 
 

#define Fori(a) for (size_t i = 0 ; i < (a) ; i++)
#define Fori32(a) for (int i = 0 ; i < (a) ; i++)
#define Forj(a) for (size_t j = 0 ; j < (a) ; j++)
#define Forj32(a) for (int j = 0 ; j < (a) ; j++)

char WaitForKey();
// add spaces to obtain a string of size len
string k(string in, int len);
string k(int in, int len);

const int Dx[] = { 1, 0, 1, -1 };  // directions :  right, up, upper right, upper left (origin lower left)
const int Dy[] = { 0, 1, 1,  1 };
   
extern double CPUSpeed;

typedef  float Ptype;  // type des probabilités

 