//
// Created by colin on 18-5-10.
//

// future example
#include <iostream>             // std::cout
#include <future>               // std::async, std::future
#include <chrono>               // std::chrono::milliseconds

// a non-optimized way of checking for prime numbers:
bool
is_prime(int x) {
    for (int i = 2; i < x; ++i)
        if (x % i == 0)
            return false;
    return true;
}

int
main() {
    // call function asynchronously:
    std::future<bool> fut = std::async(is_prime, 444444443);

    // do something while waiting for function to set future:
    std::cout << "checking, please wait";
    std::chrono::milliseconds span(20);
    while (1) {
        std::future_status ret = fut.wait_for(span);
        if (ret == std::future_status::deferred) {
            std::cout << '$';
        } else if (ret == std::future_status::ready) {
            std::cout << '#';
            break;
        } else if (ret == std::future_status::timeout) {
            std::cout << '#';
        } else {
            std::cout << (int) ret;
        }
    }

    bool x = fut.get();         // retrieve return value

    std::cout << "\n444444443 " << (x ? "is" : "is not") << " prime.\n";

    return 0;
}

