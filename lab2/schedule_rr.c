//Циклическое планирование (RR), при котором каждая задача выполняется в течение определенного кванта времени.
// (или оставшуюся часть времени использования ЦПУ).

#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

#define QUANT 10

struct node *head = NULL;
struct node *temp = NULL;

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    insert(&head, task);
}

Task *pickNextTask() {
    if (temp != NULL) {
        Task *task = temp->task;
        temp = temp->next;
        return task;
    } else {
        return NULL;
    }
}

void schedule(Parameters *parameters) {
    temp = head;
    int fl_rt = 0;
    int fl_tr = 0;
    int temp_turnaround = 0;
    while(head != NULL) {
        Task *task = pickNextTask();
        int currentTime = 0;
        while (task != NULL) {
            if (fl_rt == 0)
                parameters->responseTime += currentTime;
            if (QUANT >= task->burst) {
                run(task, task->burst);
                delete(&head, task);

                currentTime += task->burst;
                parameters->turnaroundTime += currentTime;
                parameters->waitingTime += currentTime - task->burst;
            } else {
                run(task, QUANT);
                task->burst -= QUANT;

                currentTime += QUANT;
                parameters->turnaroundTime += currentTime;
                parameters->waitingTime += currentTime - QUANT;
            }
            task = pickNextTask();
        }
        temp = head;
        if (fl_tr == 0) {
            temp_turnaround = parameters->turnaroundTime;
            fl_tr = 1;
        }
        fl_rt = 1;
    }
    parameters->turnaroundTime += temp_turnaround;
}