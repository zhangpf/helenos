/*
 * Copyright (c) 2010 Lenka Trochtova
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

/**
 * @defgroup root_pc PC platform driver.
 * @brief HelenOS PC platform driver.
 * @{
 */

/** @file
 */

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <fibril_synch.h>
#include <stdlib.h>
#include <str.h>
#include <ctype.h>
#include <macros.h>

#include <ddf/driver.h>
#include <ddf/log.h>
#include <ipc/dev_iface.h>
#include <ops/hw_res.h>
#include <device/hw_res.h>
#include <ops/pio_window.h>
#include <device/pio_window.h>

#define NAME "rootpc"

typedef struct rootpc_fun {
	hw_resource_list_t hw_resources;
	pio_window_t pio_window;
} rootpc_fun_t;

static int rootpc_dev_add(ddf_dev_t *dev);
static void root_pc_init(void);

/** The root device driver's standard operations. */
static driver_ops_t rootpc_ops = {
	.dev_add = &rootpc_dev_add
};

/** The root device driver structure. */
static driver_t rootpc_driver = {
	.name = NAME,
	.driver_ops = &rootpc_ops
};

static hw_resource_t pci_conf_regs[] = {
	{
		.type = IO_RANGE,
		.res.io_range = {
			.address = 0xCF8,
			.size = 4,
			.endianness = LITTLE_ENDIAN
		}
	},
	{
		.type = IO_RANGE,
		.res.io_range = {
			.address = 0xCFC,
			.size = 4,
			.endianness = LITTLE_ENDIAN
		}
	}
};

static rootpc_fun_t pci_data = {
	.hw_resources = {
		sizeof(pci_conf_regs) / sizeof(pci_conf_regs[0]),
		pci_conf_regs
	},
	.pio_window = {
		.mem = {
			.base = UINT32_C(0),
			.size = UINT32_C(0xffffffff) /* practical maximum */
		},
		.io = {
			.base = UINT32_C(0),
			.size = UINT32_C(0x10000)
		}
	}
};

/** Obtain function soft-state from DDF function node */
static rootpc_fun_t *rootpc_fun(ddf_fun_t *fnode)
{
	return ddf_fun_data_get(fnode);
}

static hw_resource_list_t *rootpc_get_resources(ddf_fun_t *fnode)
{
	rootpc_fun_t *fun = rootpc_fun(fnode);
	
	assert(fun != NULL);
	return &fun->hw_resources;
}

static bool rootpc_enable_interrupt(ddf_fun_t *fun)
{
	/* TODO */
	
	return false;
}

static pio_window_t *rootpc_get_pio_window(ddf_fun_t *fnode)
{
	rootpc_fun_t *fun = rootpc_fun(fnode);
	
	assert(fun != NULL);
	return &fun->pio_window;
}

static hw_res_ops_t fun_hw_res_ops = {
	.get_resource_list = &rootpc_get_resources,
	.enable_interrupt = &rootpc_enable_interrupt,
};

static pio_window_ops_t fun_pio_window_ops = {
	.get_pio_window = &rootpc_get_pio_window
};

/* Initialized in root_pc_init() function. */
static ddf_dev_ops_t rootpc_fun_ops;

static bool
rootpc_add_fun(ddf_dev_t *dev, const char *name, const char *str_match_id,
    rootpc_fun_t *fun_proto)
{
	ddf_msg(LVL_DEBUG, "Adding new function '%s'.", name);
	
	ddf_fun_t *fnode = NULL;
	int rc;
	
	/* Create new device. */
	fnode = ddf_fun_create(dev, fun_inner, name);
	if (fnode == NULL)
		goto failure;
	
	rootpc_fun_t *fun = ddf_fun_data_alloc(fnode, sizeof(rootpc_fun_t));
	*fun = *fun_proto;
	
	/* Add match ID */
	rc = ddf_fun_add_match_id(fnode, str_match_id, 100);
	if (rc != EOK)
		goto failure;
	
	/* Set provided operations to the device. */
	ddf_fun_set_ops(fnode, &rootpc_fun_ops);
	
	/* Register function. */
	if (ddf_fun_bind(fnode) != EOK) {
		ddf_msg(LVL_ERROR, "Failed binding function %s.", name);
		goto failure;
	}
	
	return true;
	
failure:
	if (fnode != NULL)
		ddf_fun_destroy(fnode);
	
	ddf_msg(LVL_ERROR, "Failed adding function '%s'.", name);
	
	return false;
}

static bool rootpc_add_functions(ddf_dev_t *dev)
{
	return rootpc_add_fun(dev, "pci0", "intel_pci", &pci_data);
}

/** Get the root device.
 *
 * @param dev		The device which is root of the whole device tree (both
 *			of HW and pseudo devices).
 * @return		Zero on success, negative error number otherwise.
 */
static int rootpc_dev_add(ddf_dev_t *dev)
{
	ddf_msg(LVL_DEBUG, "rootpc_dev_add, device handle = %d",
	    (int)ddf_dev_get_handle(dev));
	
	/* Register functions. */
	if (!rootpc_add_functions(dev)) {
		ddf_msg(LVL_ERROR, "Failed to add functions for PC platform.");
	}
	
	return EOK;
}

static void root_pc_init(void)
{
	ddf_log_init(NAME);
	rootpc_fun_ops.interfaces[HW_RES_DEV_IFACE] = &fun_hw_res_ops;
	rootpc_fun_ops.interfaces[PIO_WINDOW_DEV_IFACE] = &fun_pio_window_ops;
}

int main(int argc, char *argv[])
{
	printf(NAME ": HelenOS PC platform driver\n");
	root_pc_init();
	return ddf_driver_main(&rootpc_driver);
}

/**
 * @}
 */
