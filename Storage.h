#ifndef STORAGE_H
#define STORAGE_H

#include "Task.h"
#include <vector>

bool saveTasks(const std::vector<Task>&TaskList);
bool loadTasks(std::vector<Task>&tasks);

#endif