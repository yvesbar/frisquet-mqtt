#ifndef __TSQUEUE_H__
#define __TSQUEUE_H__


#include <condition_variable> 
#include <iostream> 
#include <mutex> 
#include <queue> 

// Thread-safe queue
template <typename T> 
class TSQueue { 
    private:
        std::queue<T> m_queue; 
        std::mutex m_mutex; 
        std::condition_variable m_cond; 

    public: 
        bool empty() {
            std::unique_lock<std::mutex> lock(m_mutex); 
            bool isEmpty = m_queue.empty(); 
            m_cond.notify_one(); 
            return isEmpty;
        }
        void push(T item) { 
            std::unique_lock<std::mutex> lock(m_mutex); 
            m_queue.push(item); 
            m_cond.notify_one(); 
        } 

        // Pops an element off the queue 
        T pop() { 
            std::unique_lock<std::mutex> lock(m_mutex); 
            m_cond.wait(lock, [this]() { return !m_queue.empty(); }); 
            T item = m_queue.front(); 
            m_queue.pop(); 
            return item; 
        } 
}; 

#endif