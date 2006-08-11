/*
 * Copyright (C) 2001-2004 Jakub Jermar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup genarch	
 * @{
 */
/**
 * @file
 * @brief	Zilog 8530 serial port / keyboard driver.
 */

#include <genarch/kbd/z8530.h>
#include <genarch/kbd/key.h>
#include <genarch/kbd/scanc.h>
#include <genarch/kbd/scanc_sun.h>
#include <arch/drivers/z8530.h>
#include <arch/drivers/kbd.h>
#include <arch/interrupt.h>
#include <cpu.h>
#include <arch/asm.h>
#include <arch.h>
#include <typedefs.h>
#include <console/chardev.h>
#include <console/console.h>
#include <interrupt.h>

/*
 * These codes read from z8530 data register are silently ignored.
 */
#define IGNORE_CODE	0x7f		/* all keys up */

static void z8530_suspend(chardev_t *);
static void z8530_resume(chardev_t *);

chardev_t kbrd;
static chardev_operations_t ops = {
	.suspend = z8530_suspend,
	.resume = z8530_resume,
	.read = key_read
};

void z8530_interrupt(int n, istate_t *istate);
void z8530_wait(void);

/** Initialize keyboard and service interrupts using kernel routine */
void z8530_grab(void)
{
}
/** Resume the former interrupt vector */
void z8530_release(void)
{
}

#include <print.h>

/** Initialize z8530. */
void z8530_init(void)
{
	chardev_initialize("z8530_kbd", &kbrd, &ops);
	stdin = &kbrd;

	z8530_write_a(WR1, WR1_IARCSC);	/* interrupt on all characters */
	z8530_write_a(WR2, 12);		/* FIXME: IRQ12 ??? */

	/* 8 bits per character and enable receiver */
	z8530_write_a(WR3, WR3_RX8BITSCH | WR3_RX_ENABLE);
	
	z8530_write_a(WR9, WR9_MIE);	/* Master Interrupt Enable. */
}

/** Process z8530 interrupt.
 *
 * @param n Interrupt vector.
 * @param istate Interrupted state.
 */
void z8530_interrupt(int n, istate_t *istate)
{
}

/** Wait until the controller reads its data. */
void z8530_wait(void) {
}

/* Called from getc(). */
void z8530_resume(chardev_t *d)
{
}

/* Called from getc(). */
void z8530_suspend(chardev_t *d)
{
}

char key_read(chardev_t *d)
{
	char ch;	

	while(!(ch = active_read_buff_read())) {
		uint8_t x;
		while (!(z8530_read_a(RR0) & RR0_RCA))
			;
		x = z8530_read_a(RR8);
		if (x != IGNORE_CODE) {
			if (x & KEY_RELEASE)
				key_released(x ^ KEY_RELEASE);
			else
				active_read_key_pressed(x);
		}
	}
	return ch;
}

/** Poll for key press and release events.
 *
 * This function can be used to implement keyboard polling.
 */
void z8530_poll(void)
{
	uint8_t x;

	while (z8530_read_a(RR0) & RR0_RCA) {
		x = z8530_read_a(RR8);
		if (x != IGNORE_CODE) {
			if (x & KEY_RELEASE)
				key_released(x ^ KEY_RELEASE);
			else
				key_pressed(x);
		}
	}
}

/** @}
 */
