#ifndef TASK_H
#define TASK_H

#include <string>
#include <optional>

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

std::string statusToString(Status status);
std::optional<Status> stringToStatus(const std::string& str);

#endif