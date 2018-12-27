//
// Created by colin on 18-5-10.
//

#include <iostream>       // std::cout
#include <functional>     // std::ref
#include <thread>         // std::thread
#include <future>         // std::promise, std::future

void print_int(std::future<int>& fut) {
    std::cout << "get " << std::endl;
    int x = fut.get(); // 获取共享状态的值.
    std::cout << "value: " << x << '\n'; // 打印 value: 10.
}

int p1 ()
{
    std::promise<int> prom; // 生成一个 std::promise<int> 对象.
    std::future<int> fut = prom.get_future(); // 和 future 关联.
    std::thread t(print_int, std::ref(fut)); // 将 future 交给另外一个线程t.

    std::this_thread::sleep_for(std::chrono::seconds(3));
    prom.set_value(10); // 设置共享状态的值, 此处和线程t保持同步.
    t.join();
    return 0;
}


std::promise<int> prom;

void print_global_promise () {
    std::future<int> fut = prom.get_future();
    int x = fut.get();
    std::cout << "value: " << x << '\n';
}

int p2()
{
    std::thread th1(print_global_promise);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    prom.set_value(10);
    th1.join();

    prom = std::promise<int>();    // prom 被move赋值为一个新的 promise 对象.

    std::thread th2 (print_global_promise);
    prom.set_value (20);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    th2.join();

    return 0;
}

int main()
{
    p2();
}