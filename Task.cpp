#include "Task.h"

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