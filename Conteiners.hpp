#pragma once
#include<queue>
#include<thread>
#include<mutex>
#include<chrono>



template<typename T>
class AsyncQueue
{
public:
    AsyncQueue()=default;
    AsyncQueue(const AsyncQueue&)=delete;
    AsyncQueue& operator = (const AsyncQueue&)=delete;

    int size()
    {
        std::scoped_lock<std::mutex> lock(mutexQueue);
        return _queue.size();
    }
    const T& front()
    {
        std::scoped_lock<std::mutex> lock(mutexQueue);
        return _queue.front();
    }


    const T& back()
    {
        std::scoped_lock<std::mutex> lock(mutexQueue);
        return _queue.back();
    }

    void push_back(const T& item)
    {
        std::scoped_lock<std::mutex> lock(mutexQueue);
        _queue.emplace_back(std::move(item));


        std::unique_lock<std::mutex> uniqLock(mutexBlocking);
        cv.notify_all();
    }

    T pop_back()
    {
        std::scoped_lock<std::mutex> lock(mutexQueue);
        T back_item=_queue.back();;
        _queue.pop_back();
        return back_item;
    }

    void push_front(const  T& item)
    {
        std::scoped_lock<std::mutex> lock(mutexQueue);
        _queue.emplace_back(std::move(item));


        std::unique_lock<std::mutex> uniqLock(mutexBlocking);
        cv.notify_all();
    }

    T pop_front()
    {
        std::scoped_lock<std::mutex> lock(mutexQueue);
        T front_item=_queue.front();;
        _queue.pop_front();
        return front_item;
    }


    bool empty()
    {
        std::scoped_lock<std::mutex> lock(mutexQueue);
        return _queue.empty();
    }

    void wait()
    {
        while (empty())
        {
            std::unique_lock<std::mutex> uniqLock(mutexBlocking);
            cv.wait(uniqLock);
        }
        
    }

    void clear()
    {
        std::scoped_lock<std::mutex> lock(mutexQueue);
        _queue.clear();
    }

protected:
    std::deque<T> _queue;
    std::mutex mutexQueue;
    std::mutex mutexBlocking;
    std::condition_variable cv;
};