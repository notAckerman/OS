// Shortest-job-first (SJF), при котором задачи планируются в порядке
// продолжительности использования ими ЦПУ.

#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"


struct node *head = NULL;

void add(char *name,int priority,int burst){
    Task *task = malloc(sizeof(Task));
    task->name=name;
    task->priority = priority;
    task->burst = burst;
    insert(&head,task);
}

Task *pickNextTask() {
    if (head != NULL) {
        Task *min = head->task;
        struct node *curr = head->next;

        while (curr != NULL) {
            if (curr->task->burst < min->burst)
                min = curr->task;

            curr = curr->next;
        }

        delete(&head, min);

        return min;
    } else {
        return NULL;
    }
}

void schedule(Parameters *parameters) {
    Task *task = pickNextTask();
    int currentTime = 0;
    while (task != NULL) {
        parameters->responseTime += currentTime; // время отклика

        run(task, task->burst);

        currentTime += task->burst;
        parameters->turnaroundTime += currentTime; // Оборотное время

        task = pickNextTask();
    }
}