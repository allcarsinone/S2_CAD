#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SRC_FILE "ggOR06.txt"

enum {
    IDX_JOB,
    IDX_MAQ,
    IDX_TIM
};

static void DUMPMATRIX(int jobs[32][32][32], int nJobs, int nTasks)
{
    for (int i = 0; i < nJobs; ++i) {
        for (int j = 0; j < nTasks; ++j)
            printf(" %2d %2d", jobs[i][j][IDX_MAQ], jobs[i][j][IDX_TIM]);
        printf("\n");
    }
    printf("\n");
}

static void DUMPRESULT(int out[32][32], int nJobs, int nTasks)
{
    for (int i = 0; i < nJobs; ++i) {
        for (int j = 0; j < nTasks; ++j)
            printf(" %3d", out[i][j]);
        printf("\n");
    }
    printf("\n");
}

static bool loadMatrix(int jobs[32][32][32], int *nJobs, int *nTasks) {
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
        for (int i = 0; i < *nJobs; ++i) {
            //printf("Job %d -", (*jobs)[i]->jobId);
            if (fgets(line, sizeof(line), file) != NULL) {
                const char *token = strtok(line, " ");
                for (int j = 0; j < *nTasks; ++j) {
                    jobs[i][j][IDX_JOB] = i; // Job
                    jobs[i][j][IDX_MAQ] = atoi(token);
                    token = strtok(NULL, " ");
                    jobs[i][j][IDX_TIM] = atoi(token); // Time
                    token = strtok(NULL, " ");
                }
            }
        }
    }
    fclose(file);
    return ok;
}

static void runTask(int *startTime, int job, int maq, int time)
{
    printf("Running Job %d on Machine %d during %d\n", job, maq, time);
    *startTime += time;
}

static void jobShopAlgorithm(int jobs[32][32][32], int out[32][32], int nJobs, int nTasks)
{
    int startTime = 0;
    for (int i = 0; i < nJobs; ++i) {
        for (int j = 0; j < nTasks; ++j) {
            out[i][j] = startTime;
            runTask(&startTime, i, jobs[i][j][IDX_MAQ], jobs[i][j][IDX_TIM]);
        }
        printf("\n");
    }
    printf("Duration Time %d\n\n", startTime);
}

int main() {
    int jobs[32][32][32];
    int nJobs = 0, nTasks = 0;
    if (!loadMatrix(jobs, &nJobs, &nTasks)) {
        printf("Failed to load matrix.\n");
        return 1;
    }

    DUMPMATRIX(jobs, nJobs, nTasks);

    /* JobShop algorythm */
    int out[32][32];
    jobShopAlgorithm(jobs, out, nJobs, nTasks);

    DUMPRESULT(out, nJobs, nTasks);

    return 0;
}
