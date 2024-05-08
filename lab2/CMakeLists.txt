cmake_minimum_required(VERSION 3.28)
project(lab2 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab2
        schedule_fcfs.c
        schedule_sjf.c
        schedule_priority.c
        schedule_rr.c
        schedule_priority_rr.c
)
