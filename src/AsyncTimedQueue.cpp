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

#include <executor/AsyncTimedQueue.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace nrg {

namespace {
struct TimedFunction {
    std::chrono::steady_clock::time_point when;
    std::function<void()> fn;
};

bool operator>(const TimedFunction& lhs, const TimedFunction& rhs)
{
    return lhs.when > rhs.when;
}

}

class AsyncTimedQueue::Impl {
public:
    Impl(std::function<void(const std::exception&)> onError);
    ~Impl();

    void stop();
    void push(const std::chrono::steady_clock::time_point& when, std::function<void()> fn);

private:
    void run();
    bool queueIsDue() const;

private:
    std::thread thread_;

    std::mutex guard_;
    std::condition_variable cond_;

    std::priority_queue<TimedFunction, std::vector<TimedFunction>, std::greater<TimedFunction>> queue_;

    std::function<void(const std::exception&)> onError_;
    bool running_;
};

AsyncTimedQueue::Impl::Impl(std::function<void(const std::exception&)> onError)
    : thread_(std::thread(&AsyncTimedQueue::Impl::run, this))
    , onError_(std::move(onError))
    , running_(true)
{
}

AsyncTimedQueue::Impl::~Impl()
{
    stop();
    thread_.join();
}

void AsyncTimedQueue::Impl::stop()
{
    push(std::chrono::steady_clock::now(), [this]() { running_ = false; });
}

void AsyncTimedQueue::Impl::push(const std::chrono::steady_clock::time_point& when, std::function<void()> fn)
{
    std::unique_lock<std::mutex> lock(guard_);
    queue_.emplace(TimedFunction{when, std::move(fn)});
    cond_.notify_one();
}

void AsyncTimedQueue::Impl::run()
{
    std::unique_lock<std::mutex> lock(guard_);
    while (true) {
        while (queueIsDue()) {
            // this one is due!
            TimedFunction tf = std::move(queue_.top());
            queue_.pop();

            lock.unlock();
            try {
                tf.fn();
            } catch(const std::exception& ex) {
                onError_(ex);
            }
            lock.lock();
        }
        if (!running_) {
            break;
        }

        if (!queue_.empty()) {
            cond_.wait_until(lock, queue_.top().when, [this](){return queueIsDue();});
        } else {
            cond_.wait(lock);
        }
    }
}

bool AsyncTimedQueue::Impl::queueIsDue() const
{
    return !queue_.empty() &&
        queue_.top().when <= std::chrono::steady_clock::now() &&
        running_;
}

AsyncTimedQueue::AsyncTimedQueue(std::function<void(const std::exception&)> onError)
    : impl_(std::make_shared<Impl>(std::move(onError)))
{
}

void AsyncTimedQueue::stop()
{
    impl_->stop();
}

void AsyncTimedQueue::push(const std::chrono::steady_clock::time_point& when, std::function<void()> fn)
{
    impl_->push(when, std::move(fn));
}

}
