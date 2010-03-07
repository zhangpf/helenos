/*
 * Copyright (c) 2010 Jiri Svoboda
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

#ifndef RUN_H_
#define RUN_H_

#include "mytypes.h"

void run_init(run_t *run);
void run_program(run_t *run, stree_program_t *prog);
void run_fun(run_t *run, run_fun_ar_t *fun_ar, rdata_item_t **res);

void run_print_fun_bt(run_t *run);

rdata_var_t *run_local_vars_lookup(run_t *run, sid_t name);
run_fun_ar_t *run_get_current_fun_ar(run_t *run);
run_block_ar_t *run_get_current_block_ar(run_t *run);
stree_csi_t *run_get_current_csi(run_t *run);

void run_value_item_to_var(rdata_item_t *item, rdata_var_t **var);
void run_fun_ar_set_args(run_t *run, run_fun_ar_t *fun_ar, list_t *args);
void run_fun_ar_create(run_t *run, rdata_var_t *obj, stree_fun_t *fun,
    run_fun_ar_t **rfun_ar);

run_thread_ar_t *run_thread_ar_new(void);
run_fun_ar_t *run_fun_ar_new(void);
run_block_ar_t *run_block_ar_new(void);

#endif
