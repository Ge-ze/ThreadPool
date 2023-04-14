#include"ThreadPool.h"
 
template<typename T>
ThreadPool<T>::ThreadPool(int min,int max){
 
do{
    //初始化任务队列
    taskQ = new TaskQueue<T>();
    if(taskQ == nullptr){
        std::cout<<"new TaskQueue fail ..."<<std::endl;
    }

    //初始化线程池
    minNum = min;
    maxNum = max;
    liveNum = minNum;
    busyNum = 0;
    shutdown = false;

    //根据最大上限给线程数组分配内存
    threadIDs = new pthread_t[max];
    if(threadIDs == nullptr){
        std::cout<<"new pthead_t[] fail ..."<<std::endl;
        break;
    }
    memset(threadIDs,0,sizeof(pthread_t)*max);

    //初始化互斥锁和条件变量
    if(pthread_mutex_init(&mutexPool,NULL) != 0 
    || pthread_cond_init(&notEmpty,NULL) != 0){
        std::cout<<"init mutex or condition fail ..."<<std::endl;
        break;
    }

    //创建管理者线程
    pthread_create(&managerID,NULL,manager,this);

    //创建线程
    for(int i = 0; i < minNum; ++i){
        //将实例化对象的地址传递给静态方法worker,使其可以访问非静态成员变量和方法
        pthread_create(&threadIDs[i],NULL,worker,this);
        std::cout<<"create child pthread,id:"<<std::to_string(threadIDs[i])<<std::endl;
    }
    return;
}while(0);

    //初始化失败则释放资源
    if(threadIDs)delete[] threadIDs;
    if(taskQ)delete taskQ;

}

template<typename T>
ThreadPool<T>::~ThreadPool(){
    shutdown = true;
    //销毁管理者进程
    pthread_join(managerID,NULL);
    //唤醒所有工作进程
    for(int i = 0; i < liveNum; ++i){
        pthread_cond_signal(&notEmpty);
    }

    //taskQ->~TaskQueue();
    if(taskQ)delete taskQ;
    if(threadIDs) delete[] threadIDs;
    pthread_mutex_destroy(&mutexPool);
    pthread_cond_destroy(&notEmpty);

}

template<typename T>
void ThreadPool<T>::addTask(Task<T> task){

    pthread_mutex_lock(&mutexPool);
    //判断是否关闭线程池
    if(shutdown){
        pthread_mutex_unlock(&mutexPool);
        return;
    }
    //在线程池的任务队列添加任务
    taskQ->addTask(task);
    //唤醒工作线程
    pthread_cond_signal(&notEmpty);
    pthread_mutex_unlock(&mutexPool);
}

template<typename T>
int ThreadPool<T>::getBusyNum(){
    int num = 0;
    pthread_mutex_lock(&mutexPool);
    num = busyNum;
    pthread_mutex_unlock(&mutexPool);
    return num;
}

template<typename T>
int ThreadPool<T>::getLiveNum(){
    int num = 0;
    pthread_mutex_lock(&mutexPool);
    num = liveNum;
    pthread_mutex_unlock(&mutexPool);
    return num;
}

template<typename T>
void* ThreadPool<T>:: worker(void* args){
    //指针类型转化
    ThreadPool<T>* pool = static_cast<ThreadPool<T>*>(args);

    while(true){
        //对线程池加锁
        pthread_mutex_lock(&pool->mutexPool);

        //判断任务队列是否为空
        while(pool->taskQ->taskNumber() == 0 && pool->shutdown == false){
            std::cout << "thread " << std::to_string(pthread_self()) << " waiting..." << std::endl;
            //阻塞线程
            pthread_cond_wait(&pool->notEmpty,&pool->mutexPool);

            //解除阻塞后，线程是否销毁
            if(pool->exitNum > 0){ 
                pool->exitNum--;
                if(pool->liveNum > pool->minNum){
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexPool);
                    pool->threadExit();
                }
            }
        }

        //判断线程池是否关闭
        if(pool->shutdown){
            pthread_mutex_unlock(&pool->mutexPool);
            pool->threadExit();
        }

        //从任务队列中取出任务
        Task<T> task = pool->taskQ->takeTask();
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexPool);
        //执行任务
        std::cout<<"thread "<<std::to_string(pthread_self())<<" start working..."<<std::endl;
        //task.~Task();
        task.function(task.args);
        delete task.args;
        task.args = nullptr;


        //任务处理结束
        std::cout<<"thread "<<std::to_string(pthread_self())<<" end working..."<<std::endl;
        //互斥修改工作线程数
        pthread_mutex_lock(&pool->mutexPool);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexPool);
  
    }

    return nullptr;

}

template<typename T>
void* ThreadPool<T>::manager(void* args){
    ThreadPool<T>* pool = static_cast<ThreadPool<T>*>(args);

    while(!pool->shutdown){
        //每隔3S检测一次
        sleep(3);
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->taskQ->taskNumber();
        int liveNum = pool->liveNum;
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexPool);

         
        //任务数>存活线程数&&存活线程数<最大线程数
        if(queueSize > liveNum && liveNum < pool->maxNum){
            //创建线程
            pthread_mutex_lock(&pool->mutexPool);
            int num = 0;
            for(int i = 0; i < pool->maxNum && pool->liveNum < pool->maxNum && num < NUMBER; ++i){
                if(pool->threadIDs[i] == 0){
                    pthread_create(&pool->threadIDs[i],NULL,worker,pool);
                    num++;
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }

        //销毁多余线程
        //忙线程数*2 < 存活线程数 && 存活线程数 > 最小线程数
        if(busyNum * 2 < liveNum &&  liveNum > pool->minNum){
            pthread_mutex_lock(&pool->mutexPool);
            pool->exitNum = NUMBER;
            pthread_mutex_unlock(&pool->mutexPool);

            for(int i = 0; i < NUMBER; ++i){
                pthread_cond_signal(&pool->notEmpty);
            }
        }

    }
    return nullptr;
}

template<typename T>
void ThreadPool<T>::threadExit(){
    pthread_t tid = pthread_self();
    for(int i = 0; i < maxNum; ++i){
        if(tid == threadIDs[i]){
            std::cout<<"threadExit function thread "<<std::to_string(tid)<<" exiting ..."<<std::endl;
            threadIDs[i] = 0;
            break;
        }
    }
    //退出当前线程
    pthread_exit(NULL);
}

