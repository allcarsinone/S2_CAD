#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SRC_FILE "ggOR03.txt"

typedef struct task {
    int mach;
    int time;
} task_t;

typedef struct job {
    int jobId;
    task_t *task;
} job_t;

static void DUMPMATRIX(job_t ***jobs, int nJobs, int nTasks)
{
    for (int i = 0; i < nJobs; ++i) {
        for (int j = 0; j < nTasks; ++j)
            printf(" %2d %2d", (*jobs)[i]->task[j].mach, (*jobs)[i]->task[j].time);
        printf("\n");
    }
}

static void DUMPRESULT(int out[128][128], int nJobs, int nTasks)
{
    for (int i = 0; i < nJobs; ++i) {
        for (int j = 0; j < nTasks; ++j)
            printf(" %2d", out[i][j]);
        printf("\n");
    }
    printf("\n");
}

static void freeJobs(job_t **jobs, int nJobs) {
    if(!jobs)
        return;
    for (int i = 0; i < nJobs; ++i) {
        free(jobs[i]->task);
        free(jobs[i]);
    }
    free(jobs);
}

static bool loadMatrix(job_t ***jobs, int *nJobs, int *nTasks) {
    const char *filename = SRC_FILE;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }
    char line[256];
    if (fgets(line, sizeof(line), file) != NULL) {
        char *token = strtok(line, " ");
        *nJobs = atoi(token);
        token = strtok(NULL, " ");
        *nTasks = atoi(token);
    }
    const bool ok = (*nTasks && *nJobs);
    if (ok) {
        *jobs = (job_t **)calloc(*nJobs, sizeof(job_t *));
        for (int i = 0; i < *nJobs; ++i) {
            (*jobs)[i] = (job_t *)malloc(sizeof(job_t));
            (*jobs)[i]->jobId = i;
            (*jobs)[i]->task = (task_t *)malloc(*nTasks * sizeof(task_t));
            //printf("Job %d -", (*jobs)[i]->jobId);
            if (fgets(line, sizeof(line), file) != NULL) {
                const char *token = strtok(line, " ");
                for (int j = 0; j < *nTasks; ++j) {
                    (*jobs)[i]->task[j].mach = atoi(token);
                    token = strtok(NULL, " ");
                    (*jobs)[i]->task[j].time = atoi(token);
                    token = strtok(NULL, " ");
                }
            }
        }
    }
    fclose(file);
    return ok;
}

static void runTask(int *startTime, int job, task_t *task)
{
    printf("Running Job %d on Machine %d\n", job, task->mach);
    *startTime += task->time;
}

static void jobShopAlgorithm(job_t ***jobs, int out[128][128], int nJobs, int nTasks)
{
    int startTime = 0;
    for (int i = 0; i < nJobs; ++i) {
        for (int j = 0; j < nTasks; ++j) {
            out[i][j] = startTime;
            runTask(&startTime, i, &(*jobs)[i]->task[j]);
        }
        printf("\n");
    }
    printf("Duration Time %d\n\n", startTime);
}

int main() {
    job_t **jobs = NULL;
    int nJobs = 0, nTasks = 0;
    if (!loadMatrix(&jobs, &nJobs, &nTasks)) {
        printf("Failed to load matrix.\n");
        return 1;
    }

    DUMPMATRIX(&jobs, nJobs, nTasks);

    /* JobShop algorythm */
    int out[128][128];
    jobShopAlgorithm(&jobs, out, nJobs, nTasks);

    DUMPRESULT(out, nJobs, nTasks);

    freeJobs(jobs, nJobs);
    return 0;
}
