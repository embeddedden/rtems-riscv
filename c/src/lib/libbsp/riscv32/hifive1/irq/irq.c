/**
 * @file
 *
 * @ingroup riscv_interrupt
 *
 * @brief Interrupt support.
 */

/*
 * RISCV CPU Dependent Source
 *
 * Copyright (c) 2015 University of York.
 * Hesham ALMatary <hmka501@york.ac.uk>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/fe310.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

/* Almost all of the jobs that the following functions should
 * do are implemented in cpukit
 */

void bsp_interrupt_handler_default(rtems_vector_number vector)
{
    printk("spurious interrupt: %u\n", vector);
}

rtems_status_code bsp_interrupt_facility_initialize()
{
  return 0;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  return 0;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  return 0;
}

void Machine_SW_ISR ()
{
    while (1);
}

void Machine_Tmr_ISR ()
{
    while(1);
}

void Machine_External_ISR ()
{
    while(1);
}

static uint32_t cntr = 0;
static uint32_t cntr1 = 0;
static uint32_t cntr2 = 0;
static uint32_t excep = 0; 
static uint32_t cause, mie, mip, mtval;
static uint64_t times[10] = {0};
static uint64_t cmprs[10] = {0};

void handle_trap_new ()
{ 
    int time_val = 0;
    asm volatile ("csrr %0, mcause": "=r" (cause));
    asm volatile ("csrr %0, mie": "=r" (mie));
    asm volatile ("csrr %0, mip": "=r" (mip));
    asm volatile ("csrr %0, mbadaddr": "=r" (mtval));
    volatile uint64_t * mtime = (volatile uint64_t *)0x0200bff8;
    if (cause & MCAUSE_INT) { 
      /* an interrupt occurred */
      if ((cause & MCAUSE_MTIME) == MCAUSE_MTIME) {
	/* Timer interrupt */
        asm volatile ("csrci mie, 0x80");
	    asm volatile ("csrr %0, mie": "=r" (mie));
	    asm volatile ("csrr %0, mip": "=r" (mip));
        volatile uint64_t * mtimecmp = (volatile uint64_t *)0x02004000;
	    (*mtimecmp) = (*mtime) + FE310_CLOCK_PERIOD;

	    cntr++;
        asm volatile ("csrsi mie, 0x80");
        asm volatile ("csrr %0, mip": "=r" (mip));	
        bsp_interrupt_handler_table[1].handler(bsp_interrupt_handler_table[1].arg);
      } else if ((cause & MCAUSE_MEXT) == MCAUSE_MEXT) {
	      /*External interrupt */
          asm volatile ("csrci mie, 0x800");
          cntr1 += 1;
      } else if ((cause & MCAUSE_MSWI) == MCAUSE_MSWI) {
	      /* Software interrupt */
	      volatile uint32_t * msip_reg = (volatile uint32_t *) 0x02000000;
	      *msip_reg = 0;
	      cntr2 += 1;
      }
    } else {
      /* an exception occurred */
      excep += 1; /* Exception occurred */
    }





}
