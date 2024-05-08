// Первым пришел — первым обслужен» (FCFS), при котором задачи планируются
// в том порядке, в котором они запрашивают ЦПУ.

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

Task *pickNextTask(){
    if(head!=NULL){
        struct node *curr = head;
        struct node *prev = NULL;

        while (curr->next!=NULL){
            prev = curr;
            curr = curr->next;
        }
        Task * last = curr->task;

        if(prev!=NULL){
            delete(&head,last);
        } else{
            delete(&head,head->task);
        }
        return last;
    }else{
        return NULL;
    }
}

void schedule(Parameters *parameters){
    Task *task = pickNextTask();
    int currentTime = 0;
    while (task != NULL) {
        parameters->responseTime += currentTime;
        run(task, task->burst);
        currentTime += task->burst;
        parameters->turnaroundTime += currentTime;
        task = pickNextTask();
    }
}