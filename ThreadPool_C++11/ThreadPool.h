#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>
#include"TaskQueue.h"



class ThreadPool
{
public:
    // 线程池构造函数   创建工作线程个数
    ThreadPool(const int n_threads = 4)
        : worker_threads(std::vector<std::thread>(n_threads)),
         m_shutdown(false){}

    //禁用了复制和移动构造函数，避免了对象被误复制和移动导致线程池出现异常 
    ThreadPool(const ThreadPool &) = delete;

    ThreadPool(ThreadPool &&) = delete;

    //禁用了复制和移动赋值运算符，避免了对象被误复制和移动导致线程池出现异常
    ThreadPool &operator=(const ThreadPool &) = delete;

    ThreadPool &operator=(ThreadPool &&) = delete;

    // 初始化线程池
    void init()
    {        
        for (int i = 0; i < worker_threads.size(); ++i)
        {
            // 分配工作线程
            worker_threads.at(i) = std::thread(ThreadWorker(this, i));  
        }        
        
    }

    //等待线程完成任务并关闭线程池
    void shutdown()
    {
        m_shutdown = true;

        // 通知，唤醒所有工作线程
        m_conditional_lock.notify_all();  
        for (int i = 0; i < worker_threads.size(); ++i)
        {
            // 判断线程是否在等待
            if (worker_threads.at(i).joinable())  
            {
                // 将线程加入到等待队列
                worker_threads.at(i).join();  
            }
        }
    }
 
    //提交任务 可变参模板 
    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        // Create a function with bounded parameter ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...); // 连接函数和参数定义，特殊函数类型，避免左右值错误

        
        //c++11中auto只能推导变量类型，decltype可以推导表达式的类型 故二者结合推导函数返回值类型
        //使用std::make_shared<>()智能指针 对std::packaged_task<>对象进行管理
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // 打包void（）类型函数
        std::function<void()> warpper_func = [task_ptr]()
        {
            (*task_ptr)();
        };

        // 队列通用安全封包函数，并压入任务队列
        taskQueue.enqueue(warpper_func);

        // 唤醒一个等待中的线程
        m_conditional_lock.notify_one();

        // 返回先前注册的任务指针
        return task_ptr->get_future();
    }

      
 
private:
    // 线程池是否关闭
    bool m_shutdown;  

    // 执行函数安全队列，即任务队列
    TaskQueue<std::function<void()>> taskQueue;  

    // 工作线程队列
    std::vector<std::thread> worker_threads; 

    // 线程休眠锁互斥变量
    std::mutex m_conditional_mutex;  

    // 线程环境锁，可以让线程处于休眠或者唤醒状态
    std::condition_variable m_conditional_lock; 
    

private:
    // 内置线程工作类
    class ThreadWorker  
    {
    private:
        int m_id; // 工作id

        ThreadPool *m_pool; // 所属线程池
    public:
        // 构造函数
        ThreadWorker(ThreadPool *pool, const int id) : m_pool(pool), m_id(id){}

        // 重载()操作
        void operator()()
        {
            // 定义基础函数类func
            std::function<void()> func; 

            // 是否正在取出队列中元素
            bool dequeued;  

            while (!m_pool->m_shutdown)
            {
                {
                    // 为线程环境加锁，互访问工作线程的休眠和唤醒
                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);

                    // 如果任务队列为空，阻塞当前线程
                    if (m_pool->taskQueue.empty())
                    {
                        m_pool->m_conditional_lock.wait(lock); // 等待条件变量通知，开启线程
                    }

                    // 取出任务队列中的元素
                    dequeued = m_pool->taskQueue.dequeue(func);
                }

                // 如果成功取出，执行工作函数
                if (dequeued)
                    func();
            }
        }
    };



};





#endif