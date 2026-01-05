#include "TaskManager.h"
#include <stdexcept>
#include <iostream>

TaskManager::TaskManager() : currentTaskId(0), personCount(0) {
}

int TaskManager::findPersonIndex(const std::string &personName) const {
    for (int i = 0; i < personCount; ++i) {
        if (employees[i].getName() == personName) {
            return i;
        }
    }
    return -1;
}

void TaskManager::assignTask(const std::string &personName, const Task &task) {
    int index = findPersonIndex(personName);
    if (index == -1) {
        if (personCount >= MAX_PERSONS) {
            throw std::runtime_error("Maximum number of persons reached.");
        }
        employees[personCount] = Person(personName); // Construct Person directly
        index = personCount;
        personCount++;
    }

    Task new_task(task.getPriority(), task.getType(), task.getDescription());
    new_task.setId(getcurrentTaskID());
    setcurrentTaskID();
    employees[index].assignTask(new_task);
}

void TaskManager::completeTask(const std::string &personName) {
    int index = findPersonIndex(personName);
    if (index == -1) {
        return;
    }
    try {
        employees[index].completeTask();
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
}

void TaskManager::printAllEmployees() const {
    for (int i = 0; i < personCount; ++i) {
        std::cout << employees[i] << std::endl;
    }
}

void TaskManager::printTasksByType(TaskType type) const {
    SortedList<Task> result;
    for (int employee_index = 0; employee_index < personCount; ++employee_index) {
        const SortedList<Task> &current_tasks = employees[employee_index].getTasks(); // Access directly

        for (typename SortedList<Task>::ConstIterator it = current_tasks.begin(); it != current_tasks.end(); ++it) {
            if (it.operator*().getType() == type) {
                result.insert(*it);
            }
        }
    }
    for (typename SortedList<Task>::ConstIterator it = result.begin(); it != result.end(); ++it) {
        std::cout << *it << std::endl;
    }
}

void TaskManager::printAllTasks() const {
    SortedList<Task> result;
    for (int priority = 100; priority >= 0; priority--) {
        for (int employee_index = 0; employee_index < personCount; ++employee_index) {
            const SortedList<Task> &current_tasks = employees[employee_index].getTasks();
            for (typename SortedList<Task>::ConstIterator it = current_tasks.begin(); it != current_tasks.end(); ++it) {
                if (it.operator*().getPriority() == priority) {
                    result.insert(*it);
                }
            }
        }
    }
    for (typename SortedList<Task>::ConstIterator it = result.begin(); it != result.end(); ++it) {
        std::cout << *it << std::endl;
    }
}

void TaskManager::bumpPriorityByType(TaskType type, int amount) {
    if (amount < 0)
        return;

    for (int i = 0; i < personCount; ++i) {
        const SortedList<Task>& tasks = employees[i].getTasks();
        SortedList<Task> filteredTasks = tasks.filter([type](const Task& task) {
            return task.getType() == type;
        });
        SortedList<Task> updatedTasks = filteredTasks.apply([amount](const Task& task) {
            int newPriority = task.getPriority() + amount;
            if (newPriority > 100) {
                newPriority = 100;
            }
            if (newPriority < 0) {
                newPriority = 0;
            }
            Task updatedTask(newPriority, task.getType(), task.getDescription());
            updatedTask.setId(task.getId());
            return updatedTask;
        });
        SortedList<Task> unaffectedTasks = tasks.filter([type](const Task& task) {
            return task.getType() != type;
        });
        SortedList<Task> allUpdatedTasks;
        for (typename SortedList<Task>::ConstIterator it = updatedTasks.begin(); it != updatedTasks.end(); ++it) {
            allUpdatedTasks.insert(*it);
        }
        for (typename SortedList<Task>::ConstIterator it = unaffectedTasks.begin(); it != unaffectedTasks.end(); ++it) {
            allUpdatedTasks.insert(*it);
        }
        employees[i].setTasks(allUpdatedTasks);
    }
}
