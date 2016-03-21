#include <executor/AsyncExecutor.h>
#include <executor/AsyncTimedExecutor.h>
#include <executor/Executor.h>
#include <executor/SyncExecutor.h>

#include <iostream>
#include <thread>

class X {
public:
    void f(int) {}
    void c() const {}
};

std::chrono::steady_clock::time_point now()
{
    return std::chrono::steady_clock::now();
}

class TimedCaller {
public:
    TimedCaller() : started_(now())
    {
    }

    void operator()() const
    {
        std::cerr << "called after " << std::chrono::duration_cast<std::chrono::milliseconds>(now()-started_).count() << "\n";
    }

private:
    std::chrono::steady_clock::time_point started_;
};

/*
template <typename T>
class Q {
public:
    Q(const std::shared_ptr<T>& t) : t_(t) {}

    void start()
    {
        q_.start();
    }

    template<typename Fn, typename... P>
    void push(Fn fn, P&& ...p)
    {
        WorkerQueue::WorkerFunction fkt =
            [this, fn, p...]() {
                if (std::shared_ptr<T> t = t_.lock()) {
                    ((*t).*fn)(std::forward<P...>(p)...);
                }
            };
        q_.push(fkt);
        //!!!fkt = std::bind(&Q::execute, this, fn, std::forward<P...>(p)...);
        q_.push(fkt);
    }

private:
    template<typename Fn, typename... P>
    void execute(Fn fn, P&& ...p)
    {
        if (std::shared_ptr<T> t = t_.lock()) {
            ((*t).*fn)(std::forward<P...>(p)...);
        }
    }

private:
    WorkerQueue q_;
    std::weak_ptr<T> t_;
};

template <typename T>
class ObjectQueue {
public:
    ObjectQueue(const std::shared_ptr<T>& t, WorkerQueue queue = WorkerQueue())
        : t_(t)
        , queue_(queue)
    {}

    template<typename Fn, typename... P>
    void push(Fn fn, P&& ...p)
    {
        execute(queue_, fn, t_, std::forward<P>(p)...);
    }

    WorkerQueue queue() const
    {
        return queue_;
    }

private:
    template<typename Fn, typename... P>
    static void execute(WorkerQueue queue, Fn fn, std::weak_ptr<T> t, P&& ...p)
    {
        auto wf = [t, fn, p...]() {
            if (std::shared_ptr<T> obj = t.lock()) {
                ((*obj).*fn)(std::forward<P>(p)...);
            }
        };
        queue.push(wf);
    }

private:
    WorkerQueue queue_;
    std::weak_ptr<T> t_;
};

*/

















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

void print(std::ostream& stream, int val)
{
    stream << val << "\n";
}

/*
long now()
{
    boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::universal_time();
    boost::gregorian::date epoch(1970, 1, 1);
    return (t1 - boost::posix_time::ptime(epoch)).total_milliseconds();
}
*/

/*
WorkerQueue fn()
{
    std::shared_ptr<X> x = std::make_shared<X>();
    ObjectQueue<X> qx(x);

    qx.push(&X::f, 42);

    return qx.queue();
    //qx.push(&X::c);
}
*/

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
    ex(print, std::ref(std::cerr), 22);
}

void testTimed()
{
    nrg::Executor<nrg::AsyncTimedExecutor> ex(onError);
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

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main()
{
    //test<nrg::SyncExecutor>();
    //test<nrg::AsyncExecutor>();

    //test<nrg::AsyncTimedExecutor>();
    testTimed();
}
