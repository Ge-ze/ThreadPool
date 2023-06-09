# C++实现线程池

当并发的**线程数量很多**，并且每个线程都是**执行一个时间很短**的任务就结束了，这样频繁创建线程就会大大降低系统的效率，因为频繁创建线程和销毁线程需要时间。

为了使得线程可以复用且提高系统的效率，于是便有了线程池，即执行完一个任务，并不被销毁，可以继续执行其他的任务。

线程池是一种多线程处理形式，处理过程中将任务添加到队列，然后在创建线程后自动启动这些任务。线程池线程都是后台线程。每个线程都使用默认的堆栈大小，以默认的优先级运行，并处于多线程单元中。如果某个线程在托管代码中空闲（如正在等待某个事件）, 则线程池将插入另一个辅助线程来使所有处理器保持繁忙。如果所有线程池线程都始终保持繁忙，但队列中包含挂起的工作，则线程池将在一段时间后创建另一个辅助线程但线程的数目永远不会超过最大值。超过最大值的线程可以排队，但他们要等到其他线程完成后才启动。


### 使用到的技术
 + 使用STL中的queue管理任务队列
 + 在线程池中创建管理者线程，根据任务多少实现线程动态管理
 + 基于条件变量和互斥锁，实现线程间通信

### master分支使用c++11重写了线程池
  + 封装了任务队列
  + 采用可变参模板编程和引用折叠，支持任意函数和参数的任务提交
  + 使用std::function方法更方便地增加对将成员函数和lambda表达式作为参数的支持
  + 使用std::packaged_task来封装任何可以调用的目标，从而用于实现异步的调用
  + 使用std::make_shared<>()智能指针 对std::packaged_task<>对象进行管理
  + 使用条件变量std::condition_variable 和 互斥锁std::mutex，实现线程间的通信 
