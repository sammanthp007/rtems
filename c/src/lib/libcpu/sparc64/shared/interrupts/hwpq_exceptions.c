
#include "hwpq_exceptions.h"
#include "spillpq.h"
#include <rtems/bspIo.h>

void sparc64_hwpq_initialize()
{
  proc_ptr old;

  _CPU_ISR_install_vector(
    SPARC_ASYNCHRONOUS_TRAP(0x41),
    sparc64_hwpq_exception_handler,
    &old
   );

  _CPU_ISR_install_vector(
    SPARC_ASYNCHRONOUS_TRAP(0x42),
    sparc64_hwpq_exception_handler,
    &old
   );

  _CPU_ISR_install_vector(
    SPARC_ASYNCHRONOUS_TRAP(0x43),
    sparc64_hwpq_exception_handler,
    &old
   );

  _CPU_ISR_install_vector(
    SPARC_ASYNCHRONOUS_TRAP(0x44),
    sparc64_hwpq_exception_handler,
    &old
   );
}

void sparc64_hwpq_drain_queue( int qid ) {
  sparc64_spillpq_drain(qid);
}

void sparc64_hwpq_exception_handler(
    uint64_t vector,
    CPU_Interrupt_frame *istate
)
{
  uint32_t level;
  uint64_t context;
  uint32_t queue_idx;
  uint32_t trap_context;
  uint32_t trap_operation;
  uint32_t trap_idx;
  int softint_bit = (vector - 0x40);
  int mask = (1<<softint_bit);

  level = sparc_disable_interrupts(); // necessary?

  // acknowledge the interrupt (allows isr to schedule another at this level)
  sparc64_clear_interrupt_bits_reg(mask);

  // get the interrupted state
  HWDS_GET_CONTEXT(context); // get context first!

  HWDS_GET_CURRENT_ID(queue_idx); // what is loaded in hw?

  trap_context = (uint32_t)context;
  trap_idx = ((trap_context)&(~0))>>20; // what is trying to be used?
//  trap_operation = (trap_context)&~(~0 << (3 + 1)); // what is the op?

  /* handle the context switch first */

  switch (softint_bit) {
    case 1: // need_spill
      sparc64_spillpq_handle_spill(
          queue_idx,
          spillpq_queue_max_size[queue_idx]/2
      );
      break;
    case 2: // need_fill
      sparc64_spillpq_handle_fill(
          queue_idx,
          spillpq_queue_max_size[queue_idx]/2
      );
      break;
    case 3: { // soft_extract
      uint64_t kv;
      HWDS_GET_PAYLOAD(kv);
      sparc64_spillpq_handle_extract(queue_idx, kv);
      break;
    }
    case 4: // context_switch
      sparc64_spillpq_context_switch(queue_idx);
      HWDS_SET_CURRENT_ID(trap_idx);
      sparc64_spillpq_handle_fill(
          trap_idx,
          spillpq_queue_max_size[trap_idx]/2
      );
      break;

    default:
      printk("Invalid softint hwpq exception\n");
      break;
  }

  sparc_enable_interrupts(level);
}
