#ifndef __TASK_MANAGER_H__
#define __TASK_MANAGER_H__

#include <stdint.h>
#include <vector>

class Task {
public:
    virtual ~Task() {};
    virtual const char *getName() = 0;

    virtual void toForeground() = 0;
    virtual void toBackground() = 0;

    virtual void terminate(bool waitForTermination = false) = 0;
};

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    Task *getTaskById(int8_t id);
    Task *getTaskByName(const char *name);

    int8_t registerTask(Task *task);
    bool unregisterTask(int8_t taskId);

private:
    std::vector<Task *> tasks;
};

#endif
