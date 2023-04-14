#include"TaskQueue.h"

template<typename T>
TaskQueue<T>::TaskQueue(){
    //初始化互斥锁
    pthread_mutex_init(&m_mutex,NULL);
}

template<typename T>
TaskQueue<T>::~TaskQueue(){
    //销毁互斥锁
    pthread_mutex_destroy(&m_mutex);
}

template<typename T>
void TaskQueue<T>::addTask(Task<T>& task){
    pthread_mutex_lock(&m_mutex);
    m_taskQ.push(task);
    pthread_mutex_unlock(&m_mutex);

}

template<typename T>
void TaskQueue<T>::addTask(callback func, void* arg)
{
    pthread_mutex_lock(&m_mutex);
    Task<T> task;
    task.function = func;
    task.arg = arg;
    m_taskQ.push(task);
    pthread_mutex_unlock(&m_mutex);
}

//template<typename T>
//void TaskQueue<T>::addTask(callback f,void* args){
//    pthread_mutex_lock(&m_mutex);
//    m_taskQ.push(Task<T>(f,args));
//    pthread_mutex_unlock(&m_mutex);
//}

template<typename T>
Task<T> TaskQueue<T>::takeTask(){
    Task<T> t;
    pthread_mutex_lock(&m_mutex);
    if(!m_taskQ.empty()){
        t = m_taskQ.front();
        m_taskQ.pop();
    }
    pthread_mutex_unlock(&m_mutex);
    return t;
}
