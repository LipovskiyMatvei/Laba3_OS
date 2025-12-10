#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "struct.h" 
#include <vector>


TEST_CASE("Testing ThreadData structure initialization") {
    int size = 10;
    std::vector<int> arr(size, 0);
    CRITICAL_SECTION cs; 
    InitializeCriticalSection(&cs);

    ThreadData data;
    data.thread_id = 5;
    data.arr = arr.data();
    data.arr_size = size;
    data.cs = &cs;
    data.marked_count = 0;

    CHECK(data.thread_id == 5);
    CHECK(data.arr_size == 10);
    CHECK(data.marked_count == 0);

    DeleteCriticalSection(&cs);
}

TEST_CASE("Testing Marking Logic Simulation") {
    int size = 5;
    std::vector<int> arr(size, 0); 
    int thread_id = 99;

    
    int index = 2;

    
    if (arr[index] == 0) {
        arr[index] = thread_id;
    }

    CHECK(arr[index] == 99);  
    CHECK(arr[0] == 0);        
}


TEST_CASE("Testing Cleanup Logic") {
    
    std::vector<int> arr = { 5, 2, 5, 3, 0 };
    int thread_to_remove = 5;

    
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i] == thread_to_remove) {
            arr[i] = 0;
        }
    }

    CHECK(arr[0] == 0); 
    CHECK(arr[1] == 2); 
    CHECK(arr[2] == 0); 
    CHECK(arr[3] == 3); 
}