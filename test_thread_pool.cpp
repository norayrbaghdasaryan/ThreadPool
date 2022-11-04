#include "gtest/gtest.h"
#include "thread_pool.hpp"
#include <iostream>
#include <fstream>

void f(int n) {
    std::ofstream fout("/dev/null");
    std::cout << "Thread " << n << " started" << std::endl;
    for(int i = 0; i < n * n * n; ++i) {
        fout << i <<' ';
    }
    std::cout << "Thread " << n << " ended" << std::endl;
}

TEST(ThreadPool, execution)
{
    ThreadPool pool;
    for(int i = 0; i < 100; ++i) {
        pool.addJob([i] { return f(i); });
    }
}