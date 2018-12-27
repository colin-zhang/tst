#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <iostream>
#include <fstream> 
#include <vector>
#include <string>

#include "clock_time.h"


#define MD5_TEST "634DCA95FAF29D2B711E4D6358D6Bae0"

struct Md5Int
{
    uint64_t a;
    uint64_t b;
    Md5Int()
    : a(0)
    , b(0)
    {
    }
};

// define function:
// ['1', ..., '9'] -> [0x01, ..., 0x09] 
// ['A', ..., 'F'] -> [0x0a, ..., 0x0f]
// ['a', ..., 'f'] -> [0x0a, ..., 0x0f]
static uint8_t xx[256] = {0, 9, 0,};
static inline uint8_t decimal(uint8_t x)
{
    return xx[x >> 6] + (x & 0x0f);
}

static inline uint8_t decimal2(uint8_t x) 
{
    return ((x >> 6)* 10) + ((x & 0x0f) - (x >> 6));
}

static uint8_t decimal3(uint8_t x)
{
    if (x & 0x40) {
        return 9 + x & 0x0f;
    } else {
        return x & 0x0f;
    }
}

static uint8_t decimal4(uint8_t x)
{
    if (x >= '0' && x <= '9') {
        return x - '0';
    } else if(x >= 'A' && x <= 'F') {
        return x - 'A' + 10;
    } else if(x >= 'a' && x <= 'f') {
        return x - 'a' + 10;
    } else {
        return 0;
    }
}

// static inline uint8_t decimal2(uint8_t x)
// {
//     if ((x >= '0' && x <= '9') ||
//         (x >= 'A' && x <= 'F') ||
//         (x >= 'a' && x <= 'f')) {
//         return xx[x >> 6] + (x & 0x0f);
//     } else {
//         return 0;
//     }
// }


static inline uint8_t combine(uint8_t x1, uint8_t x2)
{
    return (decimal(x1) << 4) | decimal(x2);
}

static inline uint8_t combine2(uint8_t x1, uint8_t x2)
{
    return (decimal2(x1) << 4) | decimal2(x2);;
}

int Md5Str2Int(const char* md5_str, Md5Int* md5_int)
{
    uint64_t md5_part = 0;
    uint8_t* x = reinterpret_cast<uint8_t*>(md5_int);
    for (int i = 0, j = 0; i < 32; i += 8, j += 4) {
        x[j ] = combine(md5_str[i], md5_str[i + 1]);
        x[j + 1] = combine(md5_str[i + 2], md5_str[i + 3]);
        x[j + 2] = combine(md5_str[i + 4], md5_str[i + 5]);
        x[j + 3] = combine(md5_str[i + 6], md5_str[i + 7]);
    }
    return 0;
}


int Md5Str2Int2(const char* md5_str, Md5Int* md5_int)
{
    uint64_t md5_part = 0;
    uint8_t* x = reinterpret_cast<uint8_t*>(md5_int);
    for (int i = 0, j = 0; i < 32; i += 8, j += 4) {
        x[j ] = combine2(md5_str[i], md5_str[i + 1]);
        x[j + 1] = combine2(md5_str[i + 2], md5_str[i + 3]);
        x[j + 2] = combine2(md5_str[i + 4], md5_str[i + 5]);
        x[j + 3] = combine2(md5_str[i + 6], md5_str[i + 7]);
    }    
    // for (int i = 0, j = 0; i < 32; i += 2, j++) {
    //     x[j ] = combine2(md5_str[i], md5_str[i + 1]);
    // }
    return 0;
}

int test()
{
    struct Md5Int md5;
    printf("%02x\n", decimal2('A'));
    printf("%02x\n", decimal2('F'));
    printf("%02x\n", decimal2('1'));
    printf("%02x\n", decimal2('9'));
    printf("%02x\n", combine2('8', '9'));
    Md5Str2Int2(MD5_TEST, &md5);
    printf("%s -> %lx, %lx\n", MD5_TEST, md5.a, md5.b);
    return 0;
}

void test1(std::vector<std::string>& md5_v, int n)
{
    struct Md5Int md5;
    printf("md5_v size = %lu\n", md5_v.size() * n);
    ClockTime clock_time(true);
    clock_time.GatherNow();
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < md5_v.size(); i++) {
            Md5Str2Int(md5_v[i].c_str(), &md5);
        }
    }
    clock_time.PrintDuration();
}

void test2(std::vector<std::string>& md5_v, int n)
{
    struct Md5Int md5;
    printf("md5_v size = %lu\n", md5_v.size() * n);
    ClockTime clock_time(true);
    clock_time.GatherNow();
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < md5_v.size(); i++) {
            Md5Str2Int2(md5_v[i].c_str(), &md5);
        }
    }
    clock_time.PrintDuration();
}

int main(int argc, char * argv[])
{
    char buffer[256];
    std::vector<std::string> md5_v;
    std::ifstream in("md5.txt");  
    if (! in.is_open())  { 
        std::cout << "Error opening file \n"; 
        exit (1); 
    }  
    while (!in.eof()) {  
        in.getline (buffer,100);  
        md5_v.push_back(buffer);
        //std::cout << buffer << std::endl;  
    }
    
    test1(md5_v, 100);
    test2(md5_v, 100);
    
    test();
    return 0;
}

