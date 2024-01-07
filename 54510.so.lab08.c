#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double g_pi;
HANDLE mutexStatic;

typedef struct {
    int threadStart;
    int threadEnd;
} WorkerArgs;

double getPiOneThread(int iterations) {
    double result = 0.0;
    for (int i = 0; i < iterations; i++) {
        double term = (i % 2 == 0) ? 1.0 : -1.0;
        double denominator = 2.0 * i + 1.0;
        result += term / denominator;
    }
    return result * 4.0;
}

DWORD WINAPI LeibnizThread(LPVOID data) {
    WorkerArgs* args = (WorkerArgs*)data;
    double partialResult = 0.0;

    printf("Thread Initiated:\t%u\tRange:\t%d\tfrom\t%d\tto\t%d\n", GetCurrentThreadId(),
        (int)(args->threadEnd - args->threadStart + 1), args->threadStart, args->threadEnd);

    for (int i = args->threadStart; i <= args->threadEnd; i++) {
        double sign = (i % 2 == 0) ? 1.0 : -1.0;
        double denominator = 2.0 * i + 1.0;
        partialResult += sign / denominator;
    }

    WaitForSingleObject(mutexStatic, INFINITE);
    g_pi += partialResult;
    ReleaseMutex(mutexStatic);

    printf("Thread %u completed with partial result:\t%.17Lf\n", GetCurrentThreadId(), partialResult);
    return 0;
}

void static HandleError(const char* message, int exitCode) {
    fprintf(stderr, "Error: '%s'\n", message);
    exit(exitCode);
}

int main(int argc, char* argv[]) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER startTime;
    LARGE_INTEGER endTime;

    if (argc != 3)
        HandleError("Exactly 2 command line arguments are required", 1);

    double check[2] = { atof(argv[1]), atof(argv[2]) };
    const int numWorkItems = atoi(argv[1]);
    const int numThreads = atoi(argv[2]);

    if (check[0] != (double)numWorkItems || check[1] != (double)numThreads) {
        HandleError("Command line arguments must be integers", 2);
    }

    if (numWorkItems < 1 || numWorkItems >= 1000000000) {
        HandleError("First command line argument out of range (between 1 and 1 000 000 000)", 3);
    }

    if (numThreads < 1 || numThreads >= 100) {
        HandleError("Second command line argument out of range (between 1 and 100)", 4);
    }

    mutexStatic = CreateMutex(NULL, FALSE, NULL);
    if (mutexStatic == NULL) {
        HandleError("Mutex Initialization Failed (for dev-> check init mutex)", 5);
    }

    HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    if (threads == NULL) {
        HandleError("Failed to allocate memory for thread handles", 6);
    }

    WorkerArgs* threadArgs = (WorkerArgs*)malloc(numThreads * sizeof(WorkerArgs));
    if (threadArgs == NULL) {
        HandleError("Failed to allocate memory for thread arguments", 7);
    }

    DWORD* threadID = (DWORD*)malloc(numThreads * sizeof(DWORD));
    if (threadID == NULL) {
        HandleError("Failed to allocate memory for thread IDs", 8);
    }

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);

    int workItemPerThread = numWorkItems / numThreads;
    int remainingWorkItems = numWorkItems % numThreads;
    int start = 0;

    for (int i = 0; i < numThreads; i++) {
        threadArgs[i].threadStart = start;
        threadArgs[i].threadEnd = start + workItemPerThread - 1 + (i < remainingWorkItems ? 1 : 0);
        threads[i] = CreateThread(NULL, 0, LeibnizThread, (LPVOID)&threadArgs[i], 0, &threadID[i]);
        if (threads[i] == NULL) HandleError("Failed to create thread", 10);
        start = threadArgs[i].threadEnd + 1;
    }

    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    QueryPerformanceCounter(&endTime);
    double elapsedTime = (double)(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;

    printf("\nResult(MultiThread): PI=%.17Lf t:=%.5fs\n", 4.0 * g_pi, elapsedTime);

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);

    double piOneThread = getPiOneThread(numWorkItems);

    QueryPerformanceCounter(&endTime);

    double elapsedTime_ = (double)(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;
    printf("\nResult(OneThread): PI=%.17Lf t:=%.5fs\n", piOneThread, elapsedTime_);

    for (int i = 0; i < numThreads; i++) CloseHandle(threads[i]);

    CloseHandle(mutexStatic);
    free(threads);
    free(threadArgs);
    free(threadID);

    return 0;
}
