/*
    Boost Software License - Version 1.0 - August 17th, 2003

    Permission is hereby granted, free of charge, to any person or organization
    obtaining a copy of the software and accompanying documentation covered by
    this license (the "Software") to use, reproduce, display, distribute,
    execute, and transmit the Software, and to prepare derivative works of the
    Software, and to permit third-parties to whom the Software is furnished to
    do so, all subject to the following:

    The copyright notices in the Software and this entire statement, including
    the above license grant, this restriction and the following disclaimer,
    must be included in all copies of the Software, in whole or in part, and
    all derivative works of the Software, unless such copies or derivative
    works are solely in the form of machine-executable object code generated by
    a source language processor.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#ifndef ASYNCQUEUE_H
#define ASYNCQUEUE_H

#include <condition_variable>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace nrg {

template<typename AsyncStyle>
class AsyncQueue {
public:
    AsyncQueue(std::function<void(const std::exception&)> onError)
        : running_(true)
        , onError_(std::move(onError))
        , thread_(std::thread(&AsyncQueue::run, this))
    {
    }
    ~AsyncQueue()
    {
        stop();
        thread_.join();
    }

    void stop()
    {
        std::function<void()> fn([this]() {running_ = false; });
        (*this)(std::move(fn));
    }

    template<typename... Param>
    void operator()(Param&&... param)
    {
        std::lock_guard<std::mutex> lock(guard_);
        AsyncStyle::push(queue_, std::forward<Param>(param)...);
        cond_.notify_one();
    }

private:
    void run()
    {
        std::unique_lock<std::mutex> lock(guard_);
        while (true) {
            while (!AsyncStyle::isEmpty(queue_)) {
                typename AsyncStyle::QueueElement elem = std::move(AsyncStyle::top(queue_));
                if (AsyncStyle::isDue(elem)) {
                    AsyncStyle::pop(queue_);

                    lock.unlock();
                    try {
                        AsyncStyle::execute(elem);
                    }
                    catch(std::exception& ex) {
                        onError_(ex);
                    }
                    lock.lock();
                } else {
                    cond_.wait_for(lock, AsyncStyle::whenIsDue(elem),
                        [this](){ return AsyncStyle::isDue(AsyncStyle::top(queue_)); });
                }
            }
            if (!running_) {
                break;
            }
            cond_.wait(lock);
        }
    }

private:
    std::mutex guard_;
    std::condition_variable cond_;

    typename AsyncStyle::Queue queue_;
    bool running_;

    std::function<void(const std::exception&)> onError_;
    std::thread thread_;
};

}

#endif
