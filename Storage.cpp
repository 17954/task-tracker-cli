#include "Storage.h"
#include <iostream>
#include <fstream>
#include <sstream>

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
bool loadTasks(std::vector<Task>&tasks){
    std::ifstream file("tasks.json");
    if(!file.is_open()){
        std::vector<Task>emptyTasks;
        if(!saveTasks(emptyTasks)){
            return false;
        }
        tasks=emptyTasks;
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
    tasks=loadedTasks;
    return true;
}