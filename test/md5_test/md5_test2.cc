#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <iostream>
#include <fstream> 
#include <vector>
#include <string>

#include "clock_time.h"


#define MD5_TEST "634DCA95FAF29D2B711E4D6358D6BBE0"

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

static uint8_t decimal(uint8_t x)
{
    uint8_t b = ((x & 0xf0) >> 6) & 0x01;
    return (b * 10) + ((x & 0x0f)  - b);
}

// static inline uint8_t decimal2(uint8_t x)
// {
//     uint8_t b = ((x & 0xf0) >> 6) & 0x01;
//     if (b) {
//         return (b * 10) + ((x & 0x0f)  - 1);
//     } else {
//         return x & 0x0f;
//     }
// }
static inline uint8_t decimal2(uint8_t input)
{
  if(input >= '0' && input <= '9')
    return input - '0';
  if(input >= 'A' && input <= 'F')
    return input - 'A' + 10;
  if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;
}

static inline uint8_t combine(uint8_t x1, uint8_t x2)
{
    return (decimal(x1) << 4) | decimal(x2);
}

static inline uint8_t combine2(uint8_t x1, uint8_t x2)
{
    return (decimal2(x1) << 4) | decimal2(x2);
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




int main()
{
    struct Md5Int md5;
    printf("%02x\n", decimal('A'));
    printf("%02x\n", decimal('F'));
    printf("%02x\n", decimal('1'));
    printf("%02x\n", decimal('9'));
    printf("%02x\n", combine('8', '9'));
    Md5Str2Int(MD5_TEST, &md5);
    printf("%s -> %lx, %lx\n", MD5_TEST, md5.a, md5.b);
    return 0;
}