#ifndef TASK_H
#define TASK_H

#include <string>

enum class Status{
	Todo,
	InProgress,
	Done
};
struct Task{
	int id;
	std::string description;
	Status status;
	std::string createdAt;
	std::string updatedAt;
};

#endif