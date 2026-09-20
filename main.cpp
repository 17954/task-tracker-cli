#include<iostream>
#include<string>
#include<vector>
#include<optional>
#include<sstream>
#include<ctime>
#include<iomanip>
#include<exception>
#include "Task.h"
#include "Storage.h"

std::vector<Task>TaskList;

std::string getCurrentTime(){
    std::time_t raw=std::time(nullptr);
    std::tm* local=std::localtime(&raw); //传time_t变量的地址
    std::ostringstream out;
    out<<std::put_time(local,"%Y-%m-%d %H:%M:%S"); //日期时间->内存中的输出流
    std::string result=out.str(); //->普通string
    // std::cout 可以接收 << 并输出到终端
    // std::ostringstream 可以接收 <<并输出到内存缓冲区,put_time函数是给输出流使用的
    // std::string 只保存最终的字符内容
    return result;
}

void printTask(const Task &task){
    std::cout<<"id : "<<task.id<<std::endl
        <<"description : "<<task.description<<std::endl
        <<"status : "<<statusToString(task.status)<<std::endl
        <<"createdAt : "<< task.createdAt<<std::endl
        <<"updatedAt : "<< task.updatedAt<<std::endl;
}

std::optional<int> parseId(const std::string&text){
    try{
        std::size_t consumed=0;
        int num=std::stoi(text,&consumed);
        if(consumed!=text.size())
            return std::nullopt;
        if(num<=0)
            return std::nullopt;
        return num;
    }
    catch(const std::exception&){
        return std::nullopt;
    }
}

int getNextId(const std::vector<Task>&TaskList){
    int maxId=0;
    for(const Task&task:TaskList){
        if (task.id>maxId)
            maxId=task.id;
    }
    return maxId+1;
}
Task* findTaskById(int id){
    for(Task&task:TaskList){
        if(task.id==id){
            return &task;
        }
    }
    return nullptr;
}
bool isSupportedDescription(const std::string&text){
    // find_first_of 返回位置，找到特殊字符返回位置，没找到返回std::string::npos
    return text.find_first_of("\"\\\n\r{}")==std::string::npos;
}

int main(int argc,char* argv[]){
    if(argc<2){
        std::cout<<"Usage: .\\task-cli.exe add \"test\""<<std::endl;
        
        return 0;
    }
    if(!loadTasks(TaskList)){
        return 1;
    }
    std::string command=argv[1]; //调用string构造函数，隐式转换
    //也可以 string command(argv[1]);
    if(command=="add"){
        
        if(argc!=3){
            std::cout<<"Please add a description."<<std::endl;
            return 0;
        }
        std::string desc=argv[2];
        if(desc.empty()){
            std::cerr<<"Description cannot be empty.\n";
            return 1;
        }
        if(!isSupportedDescription(desc)){
            std::cerr<<"Description contains unsupported characters.\n";
            return 1;
        }
        std::string NowTime=getCurrentTime();
        Task task;
            
        task.id=getNextId(TaskList);
        task.description=desc;
        task.status=Status::Todo;
        task.createdAt=task.updatedAt=NowTime;
        TaskList.push_back(task);
        
        if (!saveTasks(TaskList)) {
            return 1;
        }

        std::cout<<"Task added successfully. ID:"<<task.id<<"\n";
    }
    else if(command=="list"){
        if(TaskList.empty()){
            std::cout<<"No tasks found.\n";
        }
        if(argc==2){
            for(const Task&task:TaskList){
                printTask(task);
            }
        }
        else if(argc==3){
            auto status=stringToStatus(argv[2]);
            if(!status){
                std::cerr<<"Invalid status.\n";
                return 1;
            }
            for(const Task&task:TaskList){
                if(task.status==*status)
                    printTask(task);
            }
        }
        else{
            std::cerr<<"Wrong usage.\n";
        }
    }
    else if(command=="mark-done"){
        if(argc==3){
            auto id=parseId(argv[2]);
            if(!id){
                std::cerr<<"Invalid id.\n";
                return 1;
            }
            Task*task=findTaskById(*id);
            if(task==nullptr){
                std::cerr<<"Task not found.\n";
                return 1;
            }
            task->status=Status::Done;
            task->updatedAt=getCurrentTime();
                
            if (!saveTasks(TaskList)) {
                return 1;
            }
            std::cout<<"Mark-done success.\n";
                
        }
        else{
            std::cerr<<"Wrong usage.\n";
        }
    }
    else if(command=="mark-in-progress"){
        if(argc==3){
            auto id=parseId(argv[2]);
            if(!id){
                std::cerr<<"Invalid id.\n";
                return 1;
            }
            Task*task=findTaskById(*id);
            if(task==nullptr){
                std::cerr<<"Task not found.\n";
                return 1;
            }
            task->status=Status::InProgress;
            task->updatedAt=getCurrentTime();
                
            if (!saveTasks(TaskList)) {
                return 1;
            }
            std::cout<<"Mark-in-progress success.\n";               
        }
        else{
            std::cerr<<"Wrong usage.\n";
        }
    }
    else if(command=="update"){
        if(argc==4){
            auto id=parseId(argv[2]);
            if(!id){
                std::cerr<<"Invalid id.\n";
                return 1;
            }
            std::string newDesc=argv[3];
            if(newDesc.empty()){
                std::cerr<<"Description cannot be empty.\n";
                return 1;
            }
            if(!isSupportedDescription(newDesc)){
                std::cerr<<"Description contains unsupported characters.\n";
                return 1;
            }
            Task*task=findTaskById(*id);
            if(task==nullptr){
                std::cerr<<"Task not found.\n";
                return 1;
            }            

            task->description=newDesc;
            task->updatedAt=getCurrentTime();
            if (!saveTasks(TaskList)) {
                return 1;
            }
            std::cout<<"Update success.\n";
                
        }
        else{
            std::cerr<<"Wrong usage.\n";
        }
    }
    else if(command=="delete"){
        if(argc==3){
            auto id=parseId(argv[2]);
            if(!id){
                std::cerr<<"Invalid id.\n";
                return 1;
            }
            bool found=false;
            //需要知道task在vector中的位置，改用迭代器
            for(auto it=TaskList.begin();it!=TaskList.end();it++){
                if(it->id==*id){
                    found=true;
                    TaskList.erase(it);
                    break;
                }
            }
            if(!found)
                std::cerr<<"Task not found.\n";
            else{
                if (!saveTasks(TaskList)) {
                    return 1;
                }
                std::cout<<"Delete success.\n";
            }
                
        }
        else{
            std::cerr<<"Wrong usage.\n";
        }
    }
    else{
        std::cout<<"Unknown command."<<std::endl;
        return 0;
    }

    return 0;
}