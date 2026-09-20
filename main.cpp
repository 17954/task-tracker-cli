#include<iostream>
#include<string>
#include<ctime>
#include<iomanip>
#include<sstream>
#include<fstream>
#include<vector>
#include<optional>
#include<exception>
#include "Task.h"


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
std::string statusToString(Status status){
    switch (status){
        case Status::Todo:
            return "todo";
        case Status::InProgress:
            return "in-progress";
        case Status::Done:
            return "done";
        default:
            return "";
    }
}
std::optional<Status> stringToStatus(const std::string& str){
    if(str=="todo") 
        return Status::Todo;
    else if(str=="in-progress")
        return Status::InProgress;
    else if(str=="done")
        return Status::Done;
    else
        return std::nullopt;
}
void printTask(const Task &task){
    std::cout<<"id : "<<task.id<<std::endl
        <<"description : "<<task.description<<std::endl
        <<"status : "<<statusToString(task.status)<<std::endl
        <<"createdAt : "<< task.createdAt<<std::endl
        <<"updatedAt : "<< task.updatedAt<<std::endl;
}
std::optional<std::string> extractStringField(const std::string&object,const std::string&key){
    std::string target= "\""+key+"\":\"";
    std::size_t pos=object.find(target);
    if(pos==std::string::npos)
        return std::nullopt;
    std::size_t start=pos+target.size();
    std::size_t end=object.find('"',start);
    if(end==std::string::npos)
        return std::nullopt;
    return object.substr(start,end-start);
}
std::optional<int> extractIntField(const std::string&object,const std::string&key){
    std::string target= "\""+key+"\":";
    std::size_t pos=object.find(target);
    if(pos==std::string::npos)
        return std::nullopt;
    std::size_t start=pos+target.size();
    std::size_t end=object.find_first_of(",}",start);
    if(end==std::string::npos)
        return std::nullopt;
    std::string numberText=object.substr(start,end-start);
    try{
        std::size_t consumed=0;
        int num=std::stoi(numberText,&consumed);
        if(consumed!=numberText.size())
            return std::nullopt;
        return num;
    }
    catch(const std::exception&){
        return std::nullopt;
    }
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
std::optional<Task> parseTask(const std::string&object){
    auto id=extractIntField(object,"id");
    auto description=extractStringField(object,"description");
    auto statusString=extractStringField(object,"status");
    auto createdAt=extractStringField(object,"createdAt");
    auto updatedAt=extractStringField(object,"updatedAt");
    if(!id || !description||!statusString ||!createdAt|| !updatedAt)
        return std::nullopt;
    auto status=stringToStatus(*statusString);
    if(!status)
        return std::nullopt;
    Task task;
    task.id=*id;
    task.description=*description;
    task.status=*status;
    task.createdAt=*createdAt;
    task.updatedAt=*updatedAt;
    return task;
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
bool saveTasks(const std::vector<Task>&TaskList){
    std::ofstream file("tasks.json");
    if(!file.is_open()){
        std::cerr<<"Failed to open tasks.json\n";
        return false;
    }
    file<<"[\n"; //std::endl还会强制刷新文件输出缓冲区，\n只写换行
    for(std::size_t i=0;i<TaskList.size();i++){
        file<<"    {\n";
        file<<"    \"id\":"<<TaskList[i].id<<",\n";
        file<<"    \"description\":\""<<TaskList[i].description<<"\",\n";
        file<<"    \"status\":\""<<statusToString(TaskList[i].status)<<"\",\n";
        file<<"    \"createdAt\":\""<<TaskList[i].createdAt<<"\",\n";
        file<<"    \"updatedAt\":\""<<TaskList[i].updatedAt<<"\"\n";
        file<<"    }";
        if(i+1<TaskList.size())
            file<<",\n";
        else
            file<<"\n";
    }

    file<<"]\n";
    file.close();

    if(!file){
        std::cerr<<"Failed to write tasks.json\n";
        return false;
    }
    return true;
}
bool loadTasks(){
    std::ifstream file("tasks.json");
    if(!file.is_open()){
        std::vector<Task>emptyTasks;
        if(!saveTasks(emptyTasks)){
            return false;
        }
        TaskList=emptyTasks;
        return true;
    }
    std::ostringstream buffer;
    buffer<<file.rdbuf();// file是输入文件流对象，rdbuf()取得它底层的输入缓冲区
    std::string json;
    json=buffer.str();
    //文件流不保存“文件内容这个字符串”，保存的是已打开的文件，当前读到的位置，读文件的能力和状态
    // std::string只能保存字符，不能从文件中持续读取
    //因此需要ifstream file->ostringstream buffer->buffer.str()
    
    //加入整体数组检查
    std::size_t first=json.find_first_not_of(" \t\r\n");
    std::size_t last=json.find_last_not_of(" \t\r\n");
    if(first==std::string::npos||json[first]!='['||json[last]!=']'){
        std::cerr<<"Invalid JSON format.\n";
        return false;
    }
    std::vector<Task> loadedTasks;
    std::size_t searchStart=0;
    while(true){
        std::size_t leftPos=json.find('{',searchStart);
        if(leftPos==std::string::npos){
            break;
        }
        std::size_t rightPos=json.find('}',leftPos);
        if(rightPos==std::string::npos){
            std::cerr<<"json is incomplete\n";
            return false;
        }
        std::string content=json.substr(leftPos,rightPos-leftPos+1);

        std::optional<Task> result=parseTask(content);
        if(result!=std::nullopt){
            loadedTasks.push_back(*result);
        }
        else{
            std::cerr<<"failed to parse task.\n";
            return false;
        }
            
        searchStart=rightPos+1;
    }
    TaskList=loadedTasks;
    return true;
}
int main(int argc,char* argv[]){
    if(argc<2){
        std::cout<<"Usage: .\\task-cli.exe add \"test\""<<std::endl;
        
        return 0;
    }
    if(!loadTasks()){
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