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

#ifndef NRG_ASYNCRESULT_H
#define NRG_ASYNCRESULT_H

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <experimental/optional>
#include <utility>

namespace nrg {

template <typename R>
class AsyncResult {
public:
    AsyncResult() : impl_(std::make_shared<Impl>())
    {
    }

    void setResult(R result)
    {
        impl_->setResult(std::move(result));
    }

    R getResult() const
    {
        return impl_->getResult();
    }

    template <typename Rep, typename Period>
    R getResult(const std::chrono::duration<Rep, Period>& timeout, R Default) const
    {
        std::experimental::optional<R> result = impl_->getResult(timeout);
        if (result) {
            return std::move(*result);
        }
        return std::move(Default);
    }

    template <typename Rep,  typename Period>
    std::experimental::optional<R> getResult(const std::chrono::duration<Rep, Period>& timeout) const
    {
        return impl_->getResult(timeout);
    }

private:
    class Impl;
    std::shared_ptr<Impl> impl_;

    class Impl {
    public:
        void setResult(R result)
        {
            std::lock_guard<std::mutex> lock(guard_);
            result_ = std::move(result);
            cond_.notify_one();
        }

        R getResult() const {
            std::unique_lock<std::mutex> lock(guard_);
            if (result_) {
                return *result_;
            }

            cond_.wait(lock, [this]() { return static_cast<bool>(result_); });
            return *result_;
        }

        template <typename Rep,  typename Period>
        std::experimental::optional<R> getResult(const std::chrono::duration<Rep, Period>& timeout) const
        {
            std::unique_lock<std::mutex> lock(guard_);
            if (result_) {
                return result_;
              }

            cond_.wait_for(lock, timeout, [this]() { return static_cast<bool>(result_); });
            return result_;
        }

    private:
        mutable std::mutex guard_;
        mutable std::condition_variable cond_;

        std::experimental::optional<R> result_;
    };
};

template <>
class AsyncResult<void> {
public:
    AsyncResult() : impl_(std::make_shared<Impl>())
    {
    }

    void setResult()
    {
        impl_->setResult();
    }

    void getResult() const
    {
        return impl_->getResult();
    }

    template <typename Rep, typename Period>
    void getResult(const std::chrono::duration<Rep, Period>& timeout) const
    {
        impl_->getResult(timeout);
    }

private:
    class Impl;
    std::shared_ptr<Impl> impl_;

    class Impl {
    public:
        void setResult()
        {
            std::lock_guard<std::mutex> lock(guard_);
            isSet_ = true;
            cond_.notify_one();
        }

        void getResult() const {
            std::unique_lock<std::mutex> lock(guard_);
            if (!isSet_) {
                cond_.wait(lock, [this]() { return isSet_; });
            }
        }

        template <typename Rep,  typename Period>
        void getResult(const std::chrono::duration<Rep, Period>& timeout) const
        {
            std::unique_lock<std::mutex> lock(guard_);
            if (!isSet_) {
                cond_.wait_for(lock, timeout, [this]() { return isSet_; });
            }
        }

    private:
        mutable std::mutex guard_;
        mutable std::condition_variable cond_;

        bool isSet_ = false;
    };
};

}

#endif
