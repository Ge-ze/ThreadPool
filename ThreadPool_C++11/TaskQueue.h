#ifndef TASKQUEUE_H
#define TASKQUEUE_H
#include <mutex>
#include <queue>
 
 
/**
 * 对queue进行二次封装，保证线程并发时对任务队列操作的安全性
*/
template <typename T>
class TaskQueue
{
private:
    //利用模板函数构造队列
    std::queue<T> m_queue;  

    // 访问互斥信号量
    std::mutex m_mutex;  

    

public:
    TaskQueue() {}
    TaskQueue(TaskQueue &&other) {}
    ~TaskQueue() {}

    // 返回队列是否为空
    bool empty()  
    {
        // 互斥信号变量加锁，防止m_queue被改变
        std::unique_lock<std::mutex> lock(m_mutex);  

        return m_queue.empty();
    }

    int size()
    {
        // 互斥信号变量加锁，防止m_queue被改变
        std::unique_lock<std::mutex> lock(m_mutex);  

        return m_queue.size();
    }

    // 队列添加元素
    void enqueue(T &t)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.emplace(t);
    }

    // 队列取出元素
    bool dequeue(T &t)
    {
         // 队列加锁
        std::unique_lock<std::mutex> lock(m_mutex); 

        if (m_queue.empty())
            return false;

        // 取出队首元素，返回队首元素值，并进行右值引用
        t = std::move(m_queue.front());  

        // 弹出入队的第一个元素
        m_queue.pop();  

        return true;
    }
};

#endif