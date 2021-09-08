/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-02-05     Jianjia Ma   The first version
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"
#include "nnom_local.h"
#include "nnom_layers.h"

size_t shape_size(nnom_3d_shape_t *s)
{
	if (s == NULL)
		return 0;
	return s->h * s->w * s->c;
}

nnom_3d_shape_t shape(size_t h, size_t w, size_t c)
{
	nnom_3d_shape_t s;
	s.h = h;
	s.w = w;
	s.c = c;
	return s;
}
nnom_3d_shape_t kernel(size_t h, size_t w)
{
	return shape(h, w, 1);
}
nnom_3d_shape_t stride(size_t h, size_t w)
{
	return shape(h, w, 1);
}
nnom_3d_shape_t dilation(size_t h, size_t w)
{
	return shape(h, w, 1);
}

nnom_border_t border(size_t top, size_t bottom, size_t left, size_t right)
{
	nnom_border_t b;
	b.top = top;
	b.bottom = bottom;
	b.left = left;
	b.right = right;
	return b;
}

// this function has to be used while assign a io for a layer.
// because the io needs to know who is its owner.
nnom_layer_io_t *io_init(void *owner_layer, nnom_layer_io_t *io)
{
	io->owner = (nnom_layer_t *)owner_layer;
	return io;
}

// this function is to add a new IO to current inited IO
// input, the targeted IO that the new IO will be added to
// output , the new IO
nnom_layer_io_t *io_add_aux(nnom_layer_io_t *targeted_io)
{
	nnom_layer_io_t *new_io;
	// check if the targeted io is inited, and its aux = NULL
	if (targeted_io == NULL || targeted_io->owner == NULL || targeted_io->aux != NULL)
		return NULL;
	// create new io, init it
	new_io = nnom_mem(sizeof(nnom_layer_io_t));
	if (new_io == NULL)
		return NULL;
	// add to aux
	targeted_io->aux = new_io;
	return io_init(targeted_io->owner, new_io);
}
