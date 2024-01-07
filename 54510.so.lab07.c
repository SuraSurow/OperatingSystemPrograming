// SO IN1 21A LAB06
// Wojciech Surowiecki
// sw54510@zut.edu.pl

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

long double g_pi = 1.0;
pthread_mutex_t mutex;

typedef struct {
    double start;
    double end;
} ThreadArgs;

void* pieceOfWallis(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    long double pi = 1.0;
    printf("\nThread %ld started , size:\t%0.f\tfirst:\t%0.f", pthread_self(),
        threadArgs->end - threadArgs->start, threadArgs->start);
    for (double i = threadArgs->start; i <= threadArgs->end; i++) {
        long double multiple = (4.0 * i * i) / ((2.0 * i - 1.0) * (2.0 * i + 1.0));
        pi *= multiple;
    }
    printf("\nThread %ld finished result:\t%2.Lf", pthread_self(), 2.0 * pi);

    pthread_mutex_lock(&mutex);
    g_pi *= pi;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

long double computingPi(int n) {
    long double pi = 1.0;

    for (int i = 1; i <= n; i++) {
        long double multiple = (4.0 * i * i) / ((2.0 * i - 1.0) * (2.0 * i + 1.0));
        pi *= multiple;
    }

    return 2.0 * pi;
}

void handleError(char* notify, int exitCode) {
    fprintf(stderr, "Error: '%s'\n", notify);
    exit(exitCode);
}

int main(int argc, char* argv[]) {
    if (argc != 3)
        handleError("Amount call's argument must be 2", 1);
    double check[2] = { atof(argv[1]), atof(argv[2]) };
    int amountW, amountT;
    amountW = atoi(argv[1]);
    amountT = atoi(argv[2]);

    if (check[0] != (double)amountW || check[1] != (double)amountT) {
        handleError("Amount call's argument must be integer", 2);
    }
    if (amountW < 1 || amountW >= 1000000000) {
        handleError(
            "First call's argument out of scope (between 1 and 1 000 000 000)", 3);
    }
    if (amountT < 1 || amountT >= 100) {
        handleError("First call's argument out of scope (between 1 and 100)", 4);
    }

    pthread_t threads[amountT];
    ThreadArgs threadArgs[amountT];
    pthread_mutex_init(&mutex, NULL);

    struct timespec start, finish;
    double elapsed;

    double last = amountW % amountT;
    double piece = ((double)amountW - last) / (double)amountT;
    for (int i = 0; i < amountT - 1; i++) {
        threadArgs[i].start = i * piece + 1;
        threadArgs[i].end = (i + 1) * piece;
    }
    threadArgs[amountT - 1].start = (amountT - 1) * piece + 1;
    threadArgs[amountT - 1].end = amountW;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < amountT; i++)
        pthread_create(&threads[i], NULL, pieceOfWallis, (void*)&threadArgs[i]);

    for (int i = 0; i < amountT; i++)
        pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("\nResult(MultiThread): PI=%.17Lf t:=%.5fs\n", g_pi * 2.0, elapsed);
    pthread_mutex_destroy(&mutex);

    g_pi = 1.0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    long double piWithoutThread = computingPi(amountW);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    double timeWithoutThread = (finish.tv_sec - start.tv_sec);
    timeWithoutThread += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Result(OneThread): PI=%.17Lf t:=%.5fs\n", piWithoutThread,
        timeWithoutThread);

    return 0;
}