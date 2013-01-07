
#ifndef _WORKLOAD_H_
#define _WORKLOAD_H_
#include <rtems.h>

#include <libcpu/spillpq.h> // bad
#include "params.h"
extern spillpq_policy_t pqbench_policy[NUM_TASKS];

void initialize( rtems_task_argument tid );

void warmup( rtems_task_argument tid );

void work( rtems_task_argument tid );

// WARMUP must be defined for opal to work properly currently.
#define WARMUP
#define RESET_AFTER_WARMUP

// RESET_EACH_WORK_OP must be defined for averages to compute correctly
#define RESET_EACH_WORK_OP

// Measurement directives...
//#define MEASURE_WCET
#define MEASURE_ACET
//#define MEASURE_ENQUEUE
//#define MEASURE_DEQUEUE
//#define MEASURE_SPILL
//#define MEASURE_FILL
//#define MEASURE_CS


#endif
