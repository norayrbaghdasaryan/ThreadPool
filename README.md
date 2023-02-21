# ThreadPool
Thread Pool class implemented with `C++` and `Make` under `Linux` platform.

Pool provides simple interface. Has `addJob` function, which pushes passed functor in waiting jobs queue.

### Usage

```c++
constexpr size_t THREAD_COUNT = 25;
// create thread pool consisting of 25 threads
ThreadPool pool (THREAD_COUNT);
constexpr size_t JOB_COUNT = 1000;
std::promise<int> p[JOB_COUNT];
// add jobs in thread pool queue
for (int i = 0; i < JOB_COUNT; ++i) {
    pool.addJob([&p, i]() {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // do some computations
        p[i].set_value(i);
    });
}
for (int i = 0; i < JOB_COUNT; ++i) {
    std::future f = p[i].get_future();
    int result = f.get(); // result = i
}
```
