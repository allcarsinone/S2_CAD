#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define getClock() ((double)clock() / CLOCKS_PER_SEC)

#define PARALLEL_VERSION 1

#define SRC_FILE "ggOR03.txt"

#define MAX_MACHINES 10
bool machine_available[MAX_MACHINES] = { true };

typedef struct {
    int mach;
    int time;
    bool completed;
} task_t;

typedef struct {
    task_t *tasks;
    unsigned nTasks;
    pthread_mutex_t mutex;
    unsigned *out;
} job_t;

typedef struct {
    job_t **jobs;
    unsigned nJobs;
    unsigned nMaqs;
} ctx_t;

static void DUMPMATRIX(ctx_t *ctx)
{
    for (unsigned i = 0; i < ctx->nJobs; ++i) {
        for (unsigned j = 0; j < ctx->nMaqs; ++j)
            printf(" %2d %2d", ctx->jobs[i]->tasks[j].mach, ctx->jobs[i]->tasks[j].time);
        printf("\n");
    }
    printf("\n");
}

static void DUMPRESULT(ctx_t *ctx)
{
    unsigned acum = 0;
    for (unsigned i = 0; i < ctx->nJobs; ++i) {
        for (unsigned j = 0; j < ctx->nMaqs; ++j) {
            printf(" %3d", acum);
            acum += ctx->jobs[i]->out[j];
        }
        printf("\n");
    }
    printf("\n");
}

static bool loadMatrix(ctx_t *ctx)
{
    const char *filename = SRC_FILE;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }

    char line[256];
    if (fgets(line, sizeof(line), file) != NULL) {
        char *token = strtok(line, " ");
        ctx->nJobs = atoi(token);
        token = strtok(NULL, " ");
        ctx->nMaqs = atoi(token);
    }

    if (ctx->nMaqs && ctx->nJobs) {
        ctx->jobs = (job_t **)malloc(ctx->nJobs * sizeof(job_t *));
        for (unsigned i = 0; i < ctx->nJobs; ++i) {
            (ctx->jobs)[i] = (job_t *)malloc(sizeof(job_t));
            (ctx->jobs)[i]->tasks = (task_t *)malloc(ctx->nMaqs * sizeof(task_t));
            if (fgets(line, sizeof(line), file) != NULL) {
                const char *token = strtok(line, " ");
                for (unsigned j = 0; j < ctx->nMaqs; ++j) {
                    (ctx->jobs)[i]->tasks[j].mach = atoi(token);
                    token = strtok(NULL, " ");
                    (ctx->jobs)[i]->tasks[j].time = atoi(token);
                    (ctx->jobs)[i]->tasks[j].completed = false;
                    (ctx->jobs)[i]->out = malloc(ctx->nMaqs * sizeof(unsigned));
                    token = strtok(NULL, " ");
                }
            }
            ctx->jobs[i]->nTasks = ctx->nMaqs;
            pthread_mutex_init( &ctx->jobs[i]->mutex, NULL);
        }
    }
    fclose(file);
    return true;
}

static void freeMatrix(ctx_t *ctx)
{
    for (unsigned i = 0; i < ctx->nJobs; ++i) {
        if(ctx->jobs && ctx->jobs[i]) {
            free(ctx->jobs[i]->tasks);
            free(ctx->jobs[i]);
        }
    }
    free(ctx->jobs);
}

static void execute_task(int mach, int time) {
    printf("Executing operation on machine %d for %d seconds\n", mach, time);
    //sleep(time);
}

/***************************************************************************
 * Algorithm code starts here
 ***************************************************************************/

#if PARALLEL_VERSION

static void* job_worker(void* arg) {

    job_t* job = *(job_t**) arg;
    if(!job)
        return NULL;

    for (unsigned i = 0; i < job->nTasks; i++) {

        const unsigned mach = job->tasks[i].mach;
        const unsigned time = job->tasks[i].time;
        pthread_mutex_lock(&job->mutex);

        // Conditions to prevent the right sequence and no machine jobs overlapping
        while(job->out[mach] > 0 && machine_available[mach] == true && job->tasks[i-1].completed == true) {
            pthread_mutex_unlock(&job->mutex);
            usleep(100);
            pthread_mutex_lock(&job->mutex);
        }

        job->out[mach] = time;
        machine_available[mach] = false;

        pthread_mutex_unlock(&job->mutex);

        execute_task(mach, time);

        pthread_mutex_lock(&job->mutex);
        
        job->tasks[i].completed = true;
        machine_available[mach] = true;

        printf("Processing thread %ld - ", pthread_self());

        pthread_mutex_unlock(&job->mutex);
    }
    pthread_exit(NULL);
}

#else
static void job_worker(ctx_t *ctx)
{
    unsigned startTime = 0;
    for (unsigned i = 0; i < ctx->nJobs; ++i) {
        for (unsigned j = 0; j < ctx->nMaqs; ++j) {
            ctx->jobs[i]->out[j] = ctx->jobs[i]->tasks[j].time;
            execute_task(ctx->jobs[i]->tasks[j].mach, ctx->jobs[i]->tasks[j].time);
            startTime += ctx->jobs[i]->tasks[j].time;
        }
        printf("\n");
    }
    printf("Duration Time %d\n\n", startTime);
}
#endif

int main()
{
    ctx_t ctx = {};

    loadMatrix(&ctx);
    DUMPMATRIX(&ctx);

    double tempoinicio = getClock();

#if PARALLEL_VERSION
    pthread_t *threads = malloc (ctx.nJobs * sizeof(pthread_t));
    for (unsigned i = 0; i < ctx.nJobs; i++) {
        pthread_create(&threads[i], NULL, job_worker, &ctx.jobs[i]);
    }
    for (unsigned i = 0; i < ctx.nJobs; i++)
        pthread_join(threads[i], NULL);

    for (unsigned i = 0; i < ctx.nJobs; i++)
        for (unsigned j = 0; j < ctx.nMaqs; j++)
            printf(">>>>>>>>>>>>>>>>>>>>>> Count: %u\n", ctx.jobs[i]->out[j]);

    free(threads);
#else
    job_worker(&ctx);
#endif

    double tempofim = getClock();
    printf("Tempo de execução (s): %.6f\n", tempofim - tempoinicio);

    DUMPRESULT(&ctx);
    freeMatrix(&ctx);

    return 0;
}
