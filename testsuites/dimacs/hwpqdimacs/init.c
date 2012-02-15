/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"

rtems_task Init(
  rtems_task_argument ignored
)
{
  
  printf("Unpacking tar filesystem\nThis may take awhile...\n");
  if(Untar_FromMemory(0x100000000UL, 4136960) != 0) {
    printf("Can't unpack tar filesystem\n");
    exit(1);
  }
  	
  printf( "\n\n*** dimacs9 shortest path benchmark ***\n" );

  char *argv[] = {
    "sq.exe",
    "/USA-road-d.BAY.gr.gz",
    "/USA-road-d.BAY.ss.gz",
    "ignored.txt"
  };
  main(4,argv);

  printf( "*** end of dimacs9 shortest path benchmark ***\n" );
  exit( 0 );
}

