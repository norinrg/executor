/*
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <executor/AsyncExecutor.h>
#include <executor/AsyncResult.h>
#include <executor/Executor.h>
#include <executor/InstantExecution.h>
#include <executor/PrioExecution.h>
#include <executor/SyncExecutor.h>
#include <executor/TimedExecution.h>


#include <logging/Logging.h>

#include <iostream>
#include <thread>

std::chrono::steady_clock::time_point now()
{
    return std::chrono::steady_clock::now();
}

class TimedCaller {
public:
    TimedCaller(const std::string& s) : msg_(s), started_(now())
    {}

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
/*
template<typename T>
void bye(nrg::Executor<T>& ex)
{
    ex.stop();
}
*/
void noParam()
{
    std::cerr << "Well done\n";
}

void onError(const nrg::detail::ExecutionBase::ExceptionType& ex)
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
    typename ExecutionMode::Function f = noParam;
    ex(f);

    ex(noParam);
    ex(print2, 42, "hallo");
    ex(exception);

    ex(print<int>, std::ref(std::cerr), 25);
}

void withResult(nrg::AsyncResult<int> ar1, nrg::AsyncResult<int> ar2, nrg::AsyncResult<int> ar3, int Default)
{
    ar1.setResult(Default);
    // ar2 is not set, will be set to a default from the other side
    // ar3 is not set, will be checked by a std::optional<int>
}

void roundTrip(nrg::AsyncResult<int> ar4)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ar4.setResult(0);
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

    ex(exception);

    nrg::AsyncResult<int> ar1;
    nrg::AsyncResult<int> ar2;
    nrg::AsyncResult<int> ar3;

    ex(withResult, ar1, ar2, ar3, 42);
    int res1 = ar1.getResult();
    int res2 = ar2.getResult(std::chrono::milliseconds(0), 43);
    std::experimental::optional<int> res3 = ar3.getResult(std::chrono::milliseconds(0));
    std::cerr <<  "ar1: " <<  res1 << ",  ar2: " << res2 << ",  res3: " << (res3 ? *res3 : 0) << "\n";

    auto t1 = now();

    nrg::AsyncResult<int> ar4;
    ex(roundTrip, ar4);
    ar4.getResult(std::chrono::milliseconds(200), 99);
    auto dur = now()-t1;
    std::cout <<  "ar4 duration: " << std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count() << " ns\n";

    ex(print<int>, std::ref(std::cerr), 251);
    ex(print<int>, std::ref(std::cerr), 252);
    ex(print<int>, std::ref(std::cerr), 253);
    ex(print<int>, std::ref(std::cerr), 254);
    ex(print<int>, std::ref(std::cerr), 255);
    ex(print<int>, std::ref(std::cerr), 256);
    ex(TimedCaller("no timeout"));
    ex(print<std::string>, std::ref(std::cerr), title + std::string(" -> done\n"));

    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

template<typename T>
std::chrono::steady_clock::duration measure(const char* title,  int count)
{
    auto n = now();
    for (int i = 0; i != count; ++i) {
        test1<T>(title);
    }
    return now()-n;
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

void testTimed()
{
    nrg::Executor<nrg::AsyncExecutor<nrg::TimedExecution>> ex(onError);

    ex(now()+std::chrono::milliseconds(100), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(0), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(10), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(20), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(30), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(40), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(50), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(60), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(70), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(80), TimedCaller(""));
    ex(now()+std::chrono::milliseconds(90), TimedCaller(""));

    //std::this_thread::sleep_for(std::chrono::seconds(2));
    ex(print<int>, std::ref(std::cerr), 2212);
    ex(print2, 42, "hallo123");
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main()
{
    test1<nrg::SyncExecutor>("SyncExecution");
    test1<nrg::AsyncExecutor<>>("Async-InstantExecution (default)");
    test1<nrg::AsyncExecutor<nrg::InstantExecution>>("Async-InstantExecution");
    test1<nrg::AsyncExecutor<nrg::TimedExecution>>("Async-TimedExecution");
    test1<nrg::AsyncExecutor<nrg::PrioExecution>>("Async-PrioExecution");

    testAsyncTimed();

    auto t1 = measure<nrg::SyncExecutor>("SyncExecution", 2);
    auto t2 = measure<nrg::AsyncExecutor<nrg::InstantExecution>>("Async-InstantExecution", 2);
    auto t3 = measure<nrg::AsyncExecutor<nrg::TimedExecution>>("Async-TimedExecution", 2);
    auto t4 = measure<nrg::AsyncExecutor<nrg::PrioExecution>>("Async-PrioExecution", 2);

    using namespace std::chrono;
    std::cout << "SynExecution: " << duration_cast<milliseconds>(t1).count() << " ms\n";
    std::cout << "Async-InstantExecution: " << duration_cast<milliseconds>(t2).count() << " ms\n";
    std::cout << "Async-TimedExecution: " << duration_cast<milliseconds>(t3).count() << " ms\n";
    std::cout << "Async-PrioExecution: " << duration_cast<milliseconds>(t4).count() << " ms\n";

    test<nrg::AsyncExecutor<nrg::TimedExecution>>();

    testTimed();
}
