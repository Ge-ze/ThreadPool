#ifndef TASK_H
#define TASK_H

 
using callback = void (*)(void* args);

/**
 * 任务类
*/
template<typename T>
class Task
{
public:
    Task():function(nullptr),args(nullptr){}
    Task(callback fun,void* a){
        this->function = fun;
        this->args = (T*)a;
    }
    inline ~Task(){
        //delete args;         
    };
    virtual void run(){ }

 
    callback function = nullptr;
    T * args = nullptr;
};

#endif
