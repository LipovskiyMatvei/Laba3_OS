using namespace std;
#include <iostream>
#include <vector>
#include "struct.h"
#include <windows.h>
#include <cstdlib>




DWORD WINAPI marker_thread(LPVOID strct) {
    ThreadData* data = (ThreadData*)strct;

    WaitForSingleObject(data->start_event, INFINITE);

    srand(data->thread_id);

    while (true) {
        int random = rand();
        int index = random % data->arr_size;

        EnterCriticalSection(data->cs);

        if (data->arr[index] == 0) {
            Sleep(5);
            data->arr[index] = data->thread_id;
            data->marked_count++;
            Sleep(5);
            LeaveCriticalSection(data->cs);
        }
        else {
            cout << "Thread " << data->thread_id << " cannot mark element " << index
                << ". Marked: " << data->marked_count << " elements." << endl;
            LeaveCriticalSection(data->cs);

            SetEvent(data->cannot_continue_event);

            HANDLE events[2] = { data->continue_event, data->stop_event };
            DWORD wait_result = WaitForMultipleObjects(2, events, FALSE, INFINITE);

            if (wait_result == WAIT_OBJECT_0 + 1) {
                break;
            }
        }

        if (data->finish_thread) {
            break;
        }
    }

    EnterCriticalSection(data->cs);
    for (int i = 0; i < data->arr_size; i++) {
        if (data->arr[i] == data->thread_id) {
            data->arr[i] = 0;
        }
    }
    LeaveCriticalSection(data->cs);

    return 0;
}

int main() {
    int n;
    cout << "Enter array size: ";
    cin >> n;

    int thread_count;
    cout << "Enter number of marker threads: ";
    cin >> thread_count;

    int* arr = new int[n]();

    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);

    vector<ThreadData> threads_data(thread_count);
    vector<HANDLE> thread_handles(thread_count);
    vector<HANDLE> cannot_continue_events(thread_count);

    for (int i = 0; i < thread_count; i++) {
        threads_data[i].thread_id = i + 1;
        threads_data[i].arr = arr;
        threads_data[i].arr_size = n;
        threads_data[i].cs = &cs;
        threads_data[i].marked_count = 0;
        threads_data[i].finish_thread = false;

        threads_data[i].start_event = CreateEvent(NULL, TRUE, FALSE, NULL);
        threads_data[i].cannot_continue_event = CreateEvent(NULL, FALSE, FALSE, NULL);
        threads_data[i].continue_event = CreateEvent(NULL, FALSE, FALSE, NULL);
        threads_data[i].stop_event = CreateEvent(NULL, FALSE, FALSE, NULL);

        cannot_continue_events[i] = threads_data[i].cannot_continue_event;

        thread_handles[i] = CreateThread(NULL, 0, marker_thread, &threads_data[i], 0, NULL);
    }

    for (int i = 0; i < thread_count; i++) {
        SetEvent(threads_data[i].start_event);
    }

    vector<bool> active_threads(thread_count, true);
    int active_count = thread_count;

    while (active_count > 0) {
        WaitForMultipleObjects(thread_count, cannot_continue_events.data(), TRUE, INFINITE);

        cout << "Array contents: ";
        for (int i = 0; i < n; i++) {
            cout << arr[i] << " ";
        }
        cout << endl;

        int thread_to_terminate;
        cout << "Enter thread ID to terminate: ";
        cin >> thread_to_terminate;

        int thread_index = thread_to_terminate - 1;

        if (thread_index >= 0 && thread_index < thread_count && active_threads[thread_index]) {
            threads_data[thread_index].finish_thread = true;
            SetEvent(threads_data[thread_index].stop_event);

            WaitForSingleObject(thread_handles[thread_index], INFINITE);

            active_threads[thread_index] = false;
            active_count--;

            cout << "Array contents after termination: ";
            for (int i = 0; i < n; i++) {
                cout << arr[i] << " ";
            }
            cout << endl;

            for (int i = 0; i < thread_count; i++) {
                if (active_threads[i]) {
                    SetEvent(threads_data[i].continue_event);
                }
            }
        }
        else {
            cout << "Invalid thread ID!" << endl;
            for (int i = 0; i < thread_count; i++) {
                if (active_threads[i]) {
                    SetEvent(threads_data[i].continue_event);
                }
            }
        }
    }

    for (int i = 0; i < thread_count; i++) {
        CloseHandle(threads_data[i].start_event);
        CloseHandle(threads_data[i].cannot_continue_event);
        CloseHandle(threads_data[i].continue_event);
        CloseHandle(threads_data[i].stop_event);
        CloseHandle(thread_handles[i]);
    }

    DeleteCriticalSection(&cs);
    delete[] arr;

    return 0;
}