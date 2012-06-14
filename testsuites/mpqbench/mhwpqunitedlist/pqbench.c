
#include "../shared/pqbench.h"

/* PQ implementation */

#include <libcpu/spillpq.h>     /* bad */
#include <libcpu/unitedlistpq.h> /* bad */

/* test interface */
void pq_initialize( rtems_task_argument tid, int size ) {
  sparc64_spillpq_initialize(
      tid,
      &pqbench_policy,
      &sparc64_unitedlistpq_ops,
      size
  );
}

