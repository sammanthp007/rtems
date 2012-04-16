
#include "../shared/pqbench.h"

/* PQ implementation */
#include "mrbtree.h"

/* test interface */
void pq_initialize( rtems_task_argument tid, int size ) { 
  rbtree_initialize(tid, size);
}

void pq_insert( rtems_task_argument tid, uint64_t p ) {
  rbtree_insert(tid, p);
}

uint64_t pq_first( rtems_task_argument tid ) {
  return rbtree_min(tid);
}

uint64_t pq_pop( rtems_task_argument tid ) {
  return rbtree_pop_min(tid);
}

uint64_t pq_search( rtems_task_argument tid, int key ) {
  return rbtree_search(tid, key);
}

