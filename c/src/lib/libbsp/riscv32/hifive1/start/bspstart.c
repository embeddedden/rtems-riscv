/*
 *  Copyright (c) 2017
 *  Denis Obrezkov <denisobrezkov@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/prci.h>
#include <bsp/fe310.h>

/*
 * This routine make initialization of HiFive1 (FE310) counters.
 */

void bsp_start( void )
{
  volatile uint32_t * pll_reg = (volatile uint32_t *) PRCI_PLLCFG;
  volatile uint32_t * high_freq_reg = (volatile uint32_t *) PRCI_HFROSCCFG;
  volatile uint32_t * spi0 = (volatile uint32_t *) 0x10014000;

#ifdef USE_HFROSC
  /* Setting up osc frequency */
  uint32_t tmp_reg = 0;
  /* Install divider in high frequency oscillator */
  tmp_reg |= (HFROSC_DIV_VAL & 0x2f) << HFROSC_DIV_OFFSET;
  tmp_reg |= (HFROSC_TRIM_VAL & 0x1F) << HFROSC_TRIM_OFFSET;
  tmp_reg |= (HFROSC_EN_VAL & 0x1) << HFROSC_EN_OFFSET;
  (*high_freq_reg) = tmp_reg;
  while (( (*high_freq_reg) & ((HFROSC_RDY_VAL & 0x1) \
                  << HFROSC_RDY_OFFSET)) == 0 ) {
    ;
  }
#endif /* USE_HFROSC */

#ifdef USE_HFXOSC
  volatile uint32_t * ext_freq_reg = (volatile uint32_t *) PRCI_HFXOSCCFG;
  (*ext_freq_reg) |= ((HFXOSC_EN_VAL & 0x1) << HFXOSC_EN_OFFSET);
  while (( (*ext_freq_reg) & ((HFXOSC_RDY_VAL & 0x1) \
                  << HFXOSC_RDY_OFFSET)) == 0 ) {
    ;
  }
  (*pll_reg) |= (0x1 << 18);
  (*pll_reg) |= (0x1 << 17);
  (*pll_reg) |= (0x1 << PLL_SEL_OFFSET);
  (*high_freq_reg) &= ~(0x1 << HFROSC_EN_OFFSET);
  
#endif /* USE_HFXOSC */
#ifndef USE_PLL
  /* Disable PLL */
  (*pll_reg) &= ~(0x1 << PLL_SEL_OFFSET);
#else 

#endif
}
     
