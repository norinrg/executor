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

#include "AsyncQueue.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

class AsyncQueue::Impl {
public:
    Impl(std::function<void(const std::exception&)> onError);
    ~Impl();

    void stop();
    void push(std::function<void()> fn);

private:
    void run();

private:
    std::thread thread_;
    
    std::mutex guard_;
    std::condition_variable cond_;
    
    std::queue<std::function<void()>> queue_;
    std::function<void(const std::exception&)> onError_;
    bool running_;
};

AsyncQueue::Impl::Impl(std::function<void(const std::exception&)> onError)
    : thread_(std::thread(&AsyncQueue::Impl::run, this))
    , onError_(std::move(onError))
    , running_(true)
{
}

AsyncQueue::Impl::~Impl()
{
    stop();
    thread_.join();
}

void AsyncQueue::Impl::stop()
{
    push([this]() { running_ = false; });
}

void AsyncQueue::Impl::push(std::function<void()> fn)
{
    std::unique_lock<std::mutex> lock(guard_);
    queue_.push(fn);
    cond_.notify_one();
}

void AsyncQueue::Impl::run()
{
    std::unique_lock<std::mutex> lock(guard_);
    while (running_) {
        while (!queue_.empty() && running_) {
            std::function<void()> fn = std::move(queue_.front());
            queue_.pop();

            lock.unlock();
            try {
                fn();
            } catch(const std::exception& ex) {
                onError_(ex);
            }
            lock.lock();
        }
        if (!running_) {
            break;
        }
        cond_.wait(lock);
    }
}

AsyncQueue::AsyncQueue(std::function<void(const std::exception&)> onError)
    : impl_(std::make_shared<Impl>(std::move(onError)))
{
}

void AsyncQueue::stop()
{
    impl_->stop();
}

void AsyncQueue::push(std::function<void()> fn)
{
    impl_->push(std::move(fn));
}
