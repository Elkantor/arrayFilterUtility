#include <stdio.h>
#include "arrayFilterUtility/arrayFilterUtility.h"

typedef enum {
    STATE_FREE,
    STATE_ALLOCATED,
    STATE_PROCESSING,
    STATE_FINISHED,
    NUM_STATES
} TaskState;

typedef struct {
    uint32_t id;
    char payload[252];
} BigTask;

// Capacity of 500 -> uint16_t is enough to store sizes
DEFINE_FILTERED_ARRAY(BigTask, TaskPool, uint16_t, NUM_STATES, 500)

int main(void) {
    TaskPool pool;
    TaskPool_init(&pool); // Can also be declared on the stack as "TaskPool pool = {0};"

    BigTask t1 = {.id = 1};
    BigTask t2 = {.id = 2};
    TaskPool_push(&pool, STATE_ALLOCATED, t1);
    TaskPool_push(&pool, STATE_ALLOCATED, t2);

    printf("Tasks ALLOCATED : %u\n", TaskPool_count(&pool, STATE_ALLOCATED));

    TaskPool_change_state(&pool, STATE_ALLOCATED, 0, STATE_PROCESSING);

    printf("Tasks ALLOCATED : %u\n", TaskPool_count(&pool, STATE_ALLOCATED));
    printf("Tasks PROCESSING : %u\n", TaskPool_count(&pool, STATE_PROCESSING));

    TaskPool_remove_at(&pool, STATE_PROCESSING, 0);

    printf("Total left tasks : %u\n", TaskPool_total_count(&pool));

    return 0;
}