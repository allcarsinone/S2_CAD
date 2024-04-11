
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_JOBS 3
#define NUM_MACHS 3

#define PARALLEL_VERSION 1

typedef struct {
    int mach;
    int time;
}
task_t;

typedef struct {
    task_t tasks[NUM_MACHS];
}
job_t;

static unsigned out[NUM_JOBS][NUM_MACHS];

// Global variables for synchronization
pthread_mutex_t machine_mutex[NUM_MACHS] = { PTHREAD_MUTEX_INITIALIZER };

static void DUMPRESULT(void)
{
    unsigned acum = 0;
    for (unsigned i = 0; i < NUM_JOBS; ++i) {
        for (unsigned j = 0; j < NUM_MACHS; ++j) {
            printf(" %3d", acum);
            acum += out[i][j];
        }
        printf("\n");
    }
    printf("\n");
}

static void run_task(int job, int machine, int time) {
    printf("Job %d Machine %d running in %d\n", job, machine, time);
}

#if PARALLEL_VERSION

static void* job_worker(void* arg) {
    job_t* job = (job_t*) arg;
    for (int i = 0; i < NUM_JOBS; i++) {
        int mach = job->tasks[i].mach;
        int time = job->tasks[i].time;
        pthread_mutex_lock(&machine_mutex[mach]);

        printf("Processing thread %ld - ", pthread_self());
        run_task(i, mach, time);
        out[i][mach] = time;
        //printf("Processing thread %ld - ", pthread_self());

        pthread_mutex_unlock(&machine_mutex[mach]);
    }
    pthread_exit(NULL);
}

#else

static void job_worker(job_t* job)
{
    unsigned startTime = 0;
    for (unsigned i = 0; i < NUM_JOBS; ++i) {
        for (unsigned j = 0; j < NUM_MACHS; ++j) {
            out[i][j] = job[i].tasks[j].time;
            run_task(i, job[i].tasks[j].mach, job[i].tasks[j].time);
            startTime += job[i].tasks[j].time;
        }
        printf("\n");
    }
    printf("Duration Time %d\n\n", startTime);
}

#endif

int main() {
    // Example data
    job_t jobs[NUM_JOBS] = { {{{0, 3}, {1, 2}, {2, 2}}},
                             {{{0, 2}, {1, 2}, {2, 4}}},
                             {{{1, 4}, {2, 3}, {0, 1}}}
    };

#if PARALLEL_VERSION
    pthread_t threads[NUM_JOBS];

    for (int i = 0; i < NUM_JOBS; i++)
        pthread_create(&threads[i], NULL, job_worker, (void*)&jobs[i]);
    for (int i = 0; i < NUM_JOBS; i++)
        pthread_join(threads[i], NULL);
#else
    job_worker(jobs);
#endif

    DUMPRESULT();

    return 0;
}
