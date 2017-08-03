#include <bsp/irq.h>
#include <bsp/fatal.h>
#include <bsp/fe310.h>

static int dummy;

static void FE310_clock_driver_support_install_isr(
  rtems_isr_entry Clock_isr
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
    
  sc = rtems_interrupt_handler_install(
    1,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) Clock_isr,
    NULL
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static void FE310_clock_driver_support_at_tick ( void )
{
  dummy++;
}

static void FE310_clock_init ( void )
{
  volatile uint64_t * mtime = (uint64_t *)0x0200bff8;
  volatile uint64_t * mtimecmp = (uint64_t *)0x02004000;
  (*mtimecmp) = (*mtime) + FE310_CLOCK_PERIOD + 0x3000;
  asm volatile ("csrsi mie, 0x80");
  asm volatile ("csrsi mstatus, 0x8");
  
}

static void FE310_clock_driver_support_shutdown_hardware( void )
{
   dummy++ ;
}

#define Clock_driver_support_initialize_hardware() \
  FE310_clock_init()

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#define Clock_driver_support_install_isr(isr,old) \
  FE310_clock_driver_support_install_isr ( isr )

#define Clock_driver_support_at_tick() \
  FE310_clock_driver_support_at_tick()

#define Clock_driver_support_shutdown_hardware() \
  FE310_clock_driver_support_shutdown_hardware()

#include "../../../shared/clockdrv_shell.h"

/* static void Clock_driver_timecounter_tick ( void )
{
  rtems_clock_tick ();
}
*/


