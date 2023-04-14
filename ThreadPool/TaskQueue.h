#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include<queue>
#include<pthread.h>
#include "Task.h"

/**
 * 任务队列
*/
template<typename T>
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();
    //添加任务
    void addTask(Task<T>& task);

    void addTask(callback f,void* args);
    
    //取出任务
    Task<T> takeTask();

    //获取当前任务个数
    inline int taskNumber()
    {
        return m_taskQ.size();
    }

private:
    //任务队列
    std::queue<Task<T> > m_taskQ;
    //互斥锁
    pthread_mutex_t m_mutex;

};

 #endif










