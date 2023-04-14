#include"ThreadPool.h"
#include"ThreadPool.cpp"
#include<thread>

void task(void *arg) {
	int num = *(int *)arg;
	std::cout << "corrent id is: " << std::this_thread::get_id() << " :" << num <<"working" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main(){

    ThreadPool<int>* tp = new ThreadPool<int>(4,30);
    //Task<int>* mytask = new myTask(1);

    for(int i = 0; i < 100; i++){
        int* num = new int(i+100);
        std::cout<<*num<<"\t"<<i<<std::endl;
        tp->addTask(Task<int>(task,(void*)num));
    }
    sleep(20);
    
    delete tp;
    return 0;
}
