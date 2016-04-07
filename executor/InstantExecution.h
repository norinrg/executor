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

#ifndef NRG_INSTANT_H
#define NRG_INSTANT_H

#include <chrono>
#include <functional>
#include <queue>
#include <utility>

namespace nrg {

struct InstantExecution {
    using QueueElement = std::function<void()>;
    using Queue = std::queue<QueueElement>;

    static void push(Queue& queue, QueueElement elem)
    {
        queue.push(std::move(elem));
    }

    template<typename FN, typename... Param>
    static void push(Queue& queue, FN fn, Param&&... param)
    {
        QueueElement elem = [=]() { fn(param...); };
        push(queue, elem);
    }

    static bool isEmpty(const Queue& queue)
    {
        return queue.empty();
    }

    static QueueElement& top(Queue& queue)
    {
        return queue.front();
    }

    static void pop(Queue& queue)
    {
        queue.pop();
    }

    static bool isDue(const QueueElement& elem)
    {
        return true;
    }

    static void execute(const QueueElement& elem)
    {
        elem();
    }

    static std::chrono::steady_clock::duration whenIsDue(const QueueElement& elem)
    {
        return std::chrono::milliseconds(0);
    }

};

}

#endif
