//Циклическое планирование с приоритетом: задачи планируются в порядке приоритета и используется
// циклическое планирование для задач с одинаковым приоритетом.

#include "stdio.h"
#include "stdlib.h"
#include "task.h"
#include "list.h"
#include "cpu.h"

#define QUANT 10

struct node *head = NULL;
struct node *temp = NULL;

struct node *pr_unsort = NULL;
struct node *pr_sort = NULL;

int priority_count[10] = {0};

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    priority_count[priority]++; //  массив для подсчета количества задач с определенным приоритетом

    insert(&head, task);
}

Task *pickPriorityTask() { //  выбирает задачу с наивысшим приоритетом из списка pr_unsort
    if (pr_unsort != NULL) {
        Task *max = pr_unsort->task;
        struct node *curr = pr_unsort->next;

        while (curr != NULL) {
            if (curr->task->priority < max->priority)
                max = curr->task;

            curr = curr->next;
        }

        delete(&pr_unsort, max);

        return max;
    } else {
        return NULL;
    }
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

void schedule_rr(Parameters *parameters, int *currTime) {
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

                currentTime += task->burst; currTime += task->burst;
                parameters->turnaroundTime += currentTime;
                parameters->waitingTime += currentTime - task->burst;
            } else {
                run(task, QUANT);
                task->burst -= QUANT;

                currentTime += QUANT; currTime += QUANT;
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

void schedule(Parameters *parameters) {
    pr_unsort = head;
    pr_sort = NULL;

    Task *task_pr = pickPriorityTask();
    while (task_pr != NULL) {
        insert(&pr_sort, task_pr);
        task_pr = pickPriorityTask();
    }

    struct node *curr = pr_sort;
    int currentTime = 0;
    while (curr != NULL) {
        Task *task = curr->task;
        if (priority_count[task->priority] > 1) {
            struct node *pr_temp = NULL;
            while (priority_count[task->priority] != 0) {
                Task *task_pr = curr->task;
                insert(&pr_temp, task_pr);
                priority_count[task->priority]--;
                curr = curr->next;
            }
            head = pr_temp;
            schedule_rr(parameters, &currentTime);
        } else {
            parameters->responseTime += currentTime;

            run(task, task->burst);
            curr = curr->next;

            currentTime += task->burst;
            parameters->turnaroundTime += currentTime;
            parameters->waitingTime += currentTime - task->burst;
        }
    }
}