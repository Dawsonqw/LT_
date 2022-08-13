##模块划分：
基于posix封装的线程、锁  
基于Jsoncpp封装的配置解析
基于ucontext封装的协程  
基于线程池的协程调度器  
基于epoll、协程调度器、定时器，采用reactor模型的IO调度器  
基于内联的方式HOOK主要系统调用以及C标准库函数  
基于socket_in封装的IPv4、IPv6、本机套接字地址  
基于socket封装的socket  
基于协程调度器、HOOK函数、socket的TCP服务器框架  


##主要参考：
sylar高性能服务器框架：https://github.com/sylar-yin/sylar  

##引用库：  
jsoncpp: https://github.com/open-source-parsers/jsoncpp

spdlog:  https://github.com/gabime/spdlog 


##编译方法： 
本项目根目录下:		
mkdir build  
cd build  
cmake ..  
make  

进入bin目录即可执行所有测试程序

