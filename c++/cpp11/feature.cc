// future example
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds

#include <math.h>

// a non-optimized way of checking for prime numbers:
bool is_prime(int x) {
    for (int i = 2; i <= sqrt(i); i++) {
        if (x % i == 0) {
            return false;
        }
    }
    return true;
}

int main ()
{
    // call function asynchronously:
    std::future<bool> fut = std::async (is_prime,44444443); 

    // do something while waiting for function to set future:
    std::cout << "checking, please wait";
    std::chrono::milliseconds span (100);
    while (fut.wait_for(span)==std::future_status::timeout) {
        std::cout << '.' << std::flush;
    }

    bool x = fut.get();     // retrieve return value

    std::cout << "xxx" << std::endl;
    std::cout << "\n444444443 " << (x?"is":"is not") << " prime.\n";
    return 0;
}