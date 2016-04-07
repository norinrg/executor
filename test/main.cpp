#include <executor/AsyncExecutor.h>
#include <executor/Executor.h>
#include <executor/InstantExecution.h>
#include <executor/SyncExecutor.h>
#include <executor/TimedExecution.h>

#include <iostream>
#include <thread>

std::chrono::steady_clock::time_point now()
{
    return std::chrono::steady_clock::now();
}

class TimedCaller {
public:
    TimedCaller(const std::string& s) : msg_(s), started_(now())
    {
    }

    void operator()() const
    {
        std::cerr << msg_ << ": called after " << std::chrono::duration_cast<std::chrono::milliseconds>(now()-started_).count() << " ms\n";
    }

private:
    std::string msg_;
    std::chrono::steady_clock::time_point started_;
};

void printInt(int i)
{
    std::cerr << "int: " << i << std::endl;
}

void printStr(const std::string& s)
{
    std::cerr << "string: " << s << std::endl;
}

void print2(int i, const std::string& s)
{
    std::cerr <<"int: " << i << ", string: " << s << std::endl;
}

void printDbl(double d)
{
    std::cerr << "double: " << d << std::endl;
}

template<typename T>
void print(std::ostream& stream, const T& val)
{
    stream << val << "\n";
}

template<typename T>
void bye(nrg::Executor<T>& ex)
{
    ex.stop();
}

void noParam()
{
    std::cerr << "Well done\n";
}

void onError(const std::exception& ex)
{
    std::cerr << "Exception: " << ex.what() << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void exception()
{
    throw std::runtime_error("Ach du Schreck!");
}

template<typename ExecutionMode>
void test()
{
    nrg::Executor<ExecutionMode> ex(onError);
    std::function<void()> f = noParam;
    ex(f);

    ex(noParam);
    ex(print2, 42, "hallo");
    ex(exception);

    ex(print<int>, std::ref(std::cerr), 25);
}

template<typename ExecutionMode>
void test1(const char* title)
{
    nrg::Executor<ExecutionMode> ex(onError);
    int i = 42;
    ex(print<std::string>, std::ref(std::cerr), title + std::string(" -> start"));
    ex(print2, ++i, "hallo1");
    ex(print2, ++i, "hallo2");
    ex(print2, ++i, "hallo3");
    ex(print2, ++i, "hallo4");
    ex(print2, ++i, "hallo5");
    ex(print2, ++i, "hallo6");

    ex(print<int>, std::ref(std::cerr), 251);
    ex(print<int>, std::ref(std::cerr), 252);
    ex(print<int>, std::ref(std::cerr), 253);
    ex(print<int>, std::ref(std::cerr), 254);
    ex(print<int>, std::ref(std::cerr), 255);
    ex(print<int>, std::ref(std::cerr), 256);
    ex(TimedCaller("no timeout"));
    ex(print<std::string>, std::ref(std::cerr), title + std::string(" -> done\n"));

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void testAsyncTimed()
{
    nrg::Executor<nrg::AsyncExecutor<nrg::TimedExecution>> ex;

    using namespace std::literals::chrono_literals;

    ex(print<const char*>, std::ref(std::cerr), "no timeout");
    ex(100ms, print<const char*>, std::ref(std::cerr), "after 100ms");

    ex(now()+100ms, print<const char*>, std::ref(std::cerr), "after 100ms(again)");

    ex(TimedCaller("no timeout"));
    ex(200ms, TimedCaller("after 200ms"));
    ex(now()+200ms, TimedCaller("after 200ms(again)"));

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

/*
void f1(nrg::Executor<nrg::AsyncExecutor<nrg::TimedExecution>> ex)
{
    auto dt = std::chrono::milliseconds(100);

    for (int i = 0; i != 1000; ++i) {
        ex(now()+dt, TimedCaller());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100*1000));
}

void f2(nrg::Executor<nrg::AsyncExecutor<nrg::TimedExecution>> ex)
{
    for (int i = 0; i != 100; ++i) {
        ex(TimedCaller());
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
*/
/*
void testTimed()
{
    nrg::Executor<nrg::AsyncExecutor<nrg::TimedExecution>> ex(onError);

    ex(now()+std::chrono::milliseconds(0), TimedCaller());
    ex(now()+std::chrono::milliseconds(10), TimedCaller());
    ex(now()+std::chrono::milliseconds(20), TimedCaller());
    ex(now()+std::chrono::milliseconds(30), TimedCaller());
    ex(now()+std::chrono::milliseconds(40), TimedCaller());
    ex(now()+std::chrono::milliseconds(50), TimedCaller());
    ex(now()+std::chrono::milliseconds(60), TimedCaller());
    ex(now()+std::chrono::milliseconds(70), TimedCaller());
    ex(now()+std::chrono::milliseconds(80), TimedCaller());
    ex(now()+std::chrono::milliseconds(90), TimedCaller());
    ex(now()+std::chrono::milliseconds(100), TimedCaller());

    //std::this_thread::sleep_for(std::chrono::seconds(2));
    ex(print<int>, std::ref(std::cerr), 2212);
    ex(print2, 42, "hallo123");
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
*/
int main()
{
    //test1<nrg::SyncExecutor>("SyncExecution");
    //test1<nrg::AsyncExecutor<>>("Async-InstantExecution (default)");
    //test1<nrg::AsyncExecutor<nrg::InstantExecution>>("Async-InstantExecution");
    //test1<nrg::AsyncExecutor<nrg::TimedExecution>>("Async-TimedExecution");

    testAsyncTimed();
    //test<nrg::AsyncExecutor<nrg::TimedExecution>>();

    //testTimed();
    //int i=3;
    //nrg::Executor<nrg::AsyncTimedExecutor> ex(onError);
    //std::thread t1(f1, ex);
    //std::thread t2(f2, ex);

    //t1.join();
    //t2.join();
}
