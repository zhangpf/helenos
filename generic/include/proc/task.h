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

#ifndef __TASK_H__
#define __TASK_H__

#include <typedefs.h>
#include <synch/spinlock.h>
#include <adt/list.h>
#include <ipc/ipc.h>

/** Task structure. */
struct task {
	SPINLOCK_DECLARE(lock);
	link_t th_head;		/**< List of threads contained in this task. */
	link_t tasks_link;	/**< Link to other tasks within the system. */
	as_t *as;		/**< Address space. */
	/* IPC stuff */
	answerbox_t answerbox;  /**< Communication endpoint */
	phone_t phones[IPC_MAX_PHONES];
	atomic_t active_calls;  /**< Active asynchronous messages */
};

extern spinlock_t tasks_lock;
extern link_t tasks_head;

extern void task_init(void);
extern task_t *task_create(as_t *as);
extern task_t *task_run_program(void * program_addr);

#endif
