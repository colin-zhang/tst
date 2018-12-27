#include <iostream>
#include <vector>
#include <algorithm>

#define OUT std::cout

#define PRINT_CONTANER(v)                            \
    for (auto it = v.begin(); it != v.end(); it++) { \
        OUT << *it << " ";                           \
    }                                                \
    OUT << "\n";                                     \

bool myfunction(int i, int j) { 
    return i < j; 
}

struct myclass {
    bool operator()(int i,int j) { 
        return i > j;
    }
} myobject;

int main(int argc, char const *argv[])
{
    std::vector<int> v;
    v.push_back(10);
    v.push_back(9);
    v.push_back(120);
    v.push_back(33);

    PRINT_CONTANER(v);

    std::sort(v.begin(), v.end());
    PRINT_CONTANER(v);
    std::sort(v.begin(), v.end(), myobject);
    PRINT_CONTANER(v);


    OUT << "min = " << *min_element(v.begin(), v.end()) << " ";
    OUT << "max = " << *max_element(v.begin(), v.end()) << "\n";
    return 0;
}