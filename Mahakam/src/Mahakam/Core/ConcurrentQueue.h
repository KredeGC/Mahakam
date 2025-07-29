#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Mahakam
{
    template<typename T>
    class ConcurrentQueue
    {
    public:
        T pop()
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            while (m_Queue.empty())
            {
                m_Condition.wait(lock);
            }
            auto item = std::move(m_Queue.front());
            m_Queue.pop();
            return item;
        }

        void pop(T& item)
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            while (m_Queue.empty())
            {
                m_Condition.wait(lock);
            }
            item = std::move(m_Queue.front());
            m_Queue.pop();
        }

        void push(const T& item)
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Queue.push(item);
            lock.unlock();
            m_Condition.notify_one();
        }

        void push(T&& item)
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Queue.push(std::move(item));
            lock.unlock();
            m_Condition.notify_one();
        }

        template<typename... Ts>
        void emplace(Ts&&... args)
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Queue.emplace(std::forward<Ts>(args) ...);
            lock.unlock();
            m_Condition.notify_one();
        }

        bool empty()
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            return m_Queue.empty();
        }

    private:
        std::queue<T> m_Queue;
        std::mutex m_Mutex;
        std::condition_variable m_Condition;
    };
}