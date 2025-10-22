#pragma once
#include <windows.h>

struct ThreadData {
    int thread_id;
    int* arr;
    int arr_size;

    HANDLE start_event;
    HANDLE cannot_continue_event;
    HANDLE continue_event;
    HANDLE stop_event;

    CRITICAL_SECTION* cs;

    int marked_count;
    bool finish_thread;
};