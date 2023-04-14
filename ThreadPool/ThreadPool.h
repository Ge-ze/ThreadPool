#ifndef THREADPOOL_H
#define THREADPOOL_H


#include"TaskQueue.h"
#include"TaskQueue.cpp"
#include<iostream>
#include<string>
#include<string.h>
#include<unistd.h>

template<typename T>
class ThreadPool
{
public:
    //创建线程池并初始化
    ThreadPool(int min,int max);    
    //销毁线程池
    ~ThreadPool();
    //给线程池添加任务
    void addTask(Task<T> task);
    //获取线程池中工作的线程个数
    int getBusyNum();
    //获取线程池中或者的线程个数
    int getLiveNum();

private:
    //工作的线程任务函数    
    static void* worker(void* args);
    //管理者任务线程函数
    static void* manager(void* args);
    //线程退出
    void threadExit();

private:
    //任务队列
    TaskQueue<T>* taskQ = nullptr;
    //管理者线程id
    pthread_t managerID;
    //工作的线程id
    pthread_t* threadIDs = nullptr;           
    //最小线程数量
    int minNum;
    //最大线程数量
    int maxNum;
    //正在工作的线程数
    int busyNum;
    //存活的的线程数
    int liveNum;
    //要销毁的线程数
    int exitNum;

    //线程池互斥锁
    pthread_mutex_t mutexPool;
    //信号变量->任务队列是否为空
    pthread_cond_t notEmpty;

    static const int NUMBER = 2;

    //是否关闭线程池
    bool shutdown;
};    

#endif  