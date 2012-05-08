/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

/* Extra workspace for spill structure in KB
 * Spill structure:
 *  pq_node:
 *    2 ptrs per node MAXLEVEL nodes: 128*MAXLEVEL bits=16*MAXLEVEL bytes
 *    2 uint32: 64b = 8B
 *    1 uint32 for height: 32b = 4B
 *  == 16*MAXLEVEL + 12 bytes
 *  Max nodes: PQ_MAX_SIZE - QUEUE_SIZE
 *
 * Upper bound on space: PQ_MAX_SIZE * sizeof(pq_node)
 */
#include "../shared/params.h"
#define MAXLEVEL (16) // make this consistent!
#define CONFIGURE_MEMORY_OVERHEAD \
  (16+(NUM_APERIODIC_TASKS*PQ_MAX_SIZE * (16*MAXLEVEL+12))/1000)

#include <rtems/confdefs.h>
