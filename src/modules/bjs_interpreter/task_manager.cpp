#include "task_manager.h"
#include <string.h>

TaskManager::TaskManager() { tasks.reserve(5); }

TaskManager::~TaskManager() {
    for (auto task : tasks) {
        if (task) {
            task->terminate();
            delete task;
        }
    }
    tasks.clear();
}

Task *TaskManager::getTaskById(uint8_t id) {
    if (id < tasks.size()) { return tasks[id]; }
    return nullptr;
}

Task *TaskManager::getTaskByName(const char *name) {
    for (auto task : tasks) {
        if (task && task->getName() && strcmp(task->getName(), name) == 0) { return task; }
    }
    return nullptr;
}

uint8_t TaskManager::registerTask(Task *task) {
    if (!task) return -1;

    tasks.push_back(task);
    return tasks.size() - 1;
}

bool TaskManager::unregisterTask(uint8_t taskId) {
    if (taskId == -1) return false;
    auto task = tasks[taskId];
    task->terminate();
    task = NULL;
    return true;
}
