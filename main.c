#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SRC_FILE "ggOR06.txt"

typedef struct task {
    int mach;
    int time;
} task_t;

typedef struct job {
    int jobId;
    task_t *task;
} job_t;

static void freeJobs(job_t **jobs, int nJobs) {
    if(!jobs)
        return;
    for (int i = 0; i < nJobs; ++i) {
        free(jobs[i]->task);
        free(jobs[i]);
    }
    free(jobs);
}

bool loadMatrix(job_t ***jobs, int *nJobs) {
    const char *filename = SRC_FILE;
    int cols = 0;
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
        cols = atoi(token);
    }
    const bool ok = (cols && nJobs);
    if (ok) {
        *jobs = (job_t **)calloc(*nJobs, sizeof(job_t *));
        for (int i = 0; i < *nJobs; ++i) {
            (*jobs)[i] = (job_t *)malloc(sizeof(job_t));
            (*jobs)[i]->jobId = i;
            (*jobs)[i]->task = (task_t *)malloc(cols * sizeof(task_t));
            printf("Job %d -", (*jobs)[i]->jobId);
            if (fgets(line, sizeof(line), file) != NULL) {
                const char *token = strtok(line, " ");
                for (int j = 0; j < cols; ++j) {
                    (*jobs)[i]->task[j].mach = atoi(token);
                    token = strtok(NULL, " ");
                    (*jobs)[i]->task[j].time = atoi(token);
                    token = strtok(NULL, " ");
                    printf(" %d %d", (*jobs)[i]->task[j].mach, (*jobs)[i]->task[j].time);
                }
                printf("\n");
            }
        }
    }
    fclose(file);
    return ok;
}

int main() {
    job_t **jobs = NULL;
    int nJobs = 0;
    if (!loadMatrix(&jobs, &nJobs)) {
        printf("Failed to load matrix.\n");
        return 1;
    }

    // TODO: JobShop algorythm here

    freeJobs(jobs, nJobs);
    return 0;
}
