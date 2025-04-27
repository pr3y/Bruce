#ifndef __TASK_MANAGER_H__
#define __TASK_MANAGER_H__

#include <stdint.h>
#include <vector>

class Task {
public:
    virtual const char *getName() = 0;
    virtual void terminate() = 0;
    virtual uint8_t getState() = 0;
};

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    Task *getTaskById(uint8_t id);
    Task *getTaskByName(const char *name);

    uint8_t registerTask(Task *task);
    bool unregisterTask(uint8_t taskId);

private:
    std::vector<Task *> tasks;
};

#endif
