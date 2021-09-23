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
 * 2019-02-14	  Jianjia Ma   Add layer.free() method.
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "nnom.h"

const char default_layer_names[][12] = DEFUALT_LAYER_NAMES;
const char default_activation_names[][8] = ACTIVATION_NAMES;
const char default_cell_names[][8] = DEFUALT_CELL_NAMES;
size_t nnom_memory_taken = 0;

// local static functions (when libc/dynamic memory interfaces are not avaiable. )
#ifdef NNOM_USING_STATIC_MEMORY
static uint8_t *nnom_static_buf = NULL;    //pointer to static buffer
static size_t nnom_static_buf_size = 0;    //static buf size
static size_t nnom_static_buf_curr = 0;
void nnom_set_static_buf(void* buf, size_t size)
{
    nnom_static_buf = buf;
    nnom_static_buf_size = size;
    nnom_static_buf_curr = 0;
}
void* nnom_malloc(size_t size)
{
    size = nnom_alignto(size, NNOM_ALIGN);
    if(size + nnom_static_buf_curr < nnom_static_buf_size)
    {
        uint8_t* new_block = nnom_static_buf_curr + nnom_static_buf;
        nnom_static_buf_curr += size;
        return new_block;
    }
    else
    {
        if(nnom_static_buf_size == 0)
            NNOM_LOG("Please set static memory using 'nnom_set_static_buf()' before calling model created.");
        else
            NNOM_LOG("No memory! Static buffer size(%d) not big enough, please increase buffer size!", 
                        (uint32_t)nnom_static_buf_size);
        return NULL;
    }
}
void nnom_free(void* p){;}
#endif // NNOM_USING_STATIC_MEMORY

void *nnom_mem(size_t size)
{
	size = nnom_alignto(size, NNOM_ALIGN);
	void *p = (void*)nnom_malloc(size);
	if (p)
	{
		nnom_memory_taken += size; //test
		nnom_memset(p, 0, size);
	}
	return p;
}

size_t nnom_mem_stat(void)
{
	return nnom_memory_taken;
}

// get the size of an IO module
static size_t io_mem_size(nnom_layer_io_t *io)
{
	size_t size = 0;
	if (io != NULL)
	{
		while (io)
		{
			size += tensor_size(io->tensor);
			io = io->aux;
		}
	}
	return size;
}

size_t nnom_alignto(size_t value, uint32_t alignment)
{
	if (value % alignment == 0)
		return value;
	value += alignment - value % alignment;
	return value;
}

static nnom_layer_t *find_last(nnom_layer_t *layer)
{
	if (layer == NULL)
		return NULL;
	// iterate every layer until the last one on the list, then return the last instance
	while (layer->out->hook.io != NULL)
		layer = layer->out->hook.io->owner;
	return layer;
}
// input start layer, return layer num
static uint32_t find_index(nnom_layer_t *start, nnom_layer_t *layer)
{
	uint32_t i = 1;
	if (start == NULL)
		return 0;
	// iterate every layer until the last one on the list, then return the index number
	while (start->out->hook.io != NULL)
	{
		i++;
		if (layer == start)
			return i;
		start = start->out->hook.io->owner;
	}
	return 0;
}

static nnom_status_t model_add(nnom_model_t *model, nnom_layer_t *layer)
{
	nnom_layer_t *last = NULL;
	nnom_layer_t *curr = NULL;

	if (layer == NULL)
	{
		NNOM_LOG("Error: added a NULL layer, could be no memory while creating layer.\n");
		return NN_NO_MEMORY;
	}

	last = find_last(model->head);
	curr = layer;

	// when the layer list is empty, the find_last() return model->head.
	if (last == NULL)
	{
		model->head = curr;
	}
	else
	{
		// hook the current layer with the last layer.
		last->out->hook.io = curr->in; // hook IO
		curr->in->hook.io = last->out;
	}
	return NN_SUCCESS;
}

// find an available hook on the io module, normally used by output io module.
// input, the output io module that wants to hook on
// output, the new hook that added to the end of the hook list on the io
static nnom_layer_hook_t *allocate_hook(nnom_layer_io_t *io)
{
	nnom_layer_hook_t *hook;
	if (io == NULL)
		return NULL;
	hook = &io->hook;

	// if the primary hook is empty, reture it directly.
	if (hook->io == NULL)
	{
		return hook;
	}
	else
	{
		// find the empty place and allocate new hook for us
		while (hook->next != NULL)
		{
			hook = hook->next;
		}
		hook->next = nnom_mem(sizeof(nnom_layer_hook_t));
		if (hook->next == NULL)
			return NULL;
		return hook->next;
	}
}

// to check if an io is hooked to other layer
// input the primary io of a layer's input or output
// return, the new io that added to the io list.
static nnom_layer_io_t *allocate_io(nnom_layer_io_t *io)
{
	if (io == NULL)
		return NULL;

	// if the io is free to used
	if (io->hook.io == NULL)
	{
		return io;
	}
	else
	{
		// find the empty place and allocate new hook for us
		while (io->aux != NULL)
		{
			io = io->aux;
		}
		io->aux = nnom_mem(sizeof(nnom_layer_io_t));
		if (io->aux == NULL)
			return NULL;
		// the owner for new io is inherited
		io->aux->owner = io->owner;
		return io->aux;
	}
}

// hook the current layer to the input layer
// this function only to connect (single output layer) to (single input layer).
static nnom_layer_t *model_hook(nnom_layer_t *curr, nnom_layer_t *last)
{
	nnom_layer_io_t *curr_in_io;
	nnom_layer_hook_t *last_io_hook;

	if (last == NULL || curr == NULL)
		return NULL;

	// add a new hook to the output io of the last layer
	last_io_hook = allocate_hook(last->out);
	// add a new input io to the current layer's input list.
	curr_in_io = allocate_io(curr->in);

	// manually hook them togeter.
	last_io_hook->io = curr_in_io;
	curr_in_io->hook.io = last->out;

	return curr;
}

// merge a few layers using specified method
// num = the number of layer that will be merged
// method = functional layer such as (concat(), mult(), add(), sub())
static nnom_layer_t *model_mergex(nnom_layer_t *method, int num, ...)
{
	nnom_layer_t *layer_in;
	va_list valist;

	if (method == NULL)
		return NULL;

	va_start(valist, num);
	for (int i = 0; i < num; i++)
	{
		// get the input layer
		layer_in = va_arg(valist, nnom_layer_t *);
		model_hook(method, layer_in);
	}
	va_end(valist);
	return method;
}

// merge 2 input
// this is an older interface
// method = functional layer such as (concat(), mult(), add(), sub())
static nnom_layer_t *model_merge(nnom_layer_t *method, nnom_layer_t *in1, nnom_layer_t *in2)
{
	return model_mergex(method, 2, in1, in2);
}

// This api will merge activation to layer's actail to avoid the extra layer for activation
static nnom_layer_t *model_active(nnom_activation_t *act, nnom_layer_t *target)
{
	// simple and easy
	target->actail = act;
	return target;
}

// when model=NULL, it create a new sequential model
nnom_model_t *new_model(nnom_model_t *model)
{
	nnom_model_t *m = model;
	if (m == NULL)
	{
		m = nnom_mem(sizeof(nnom_model_t));
		m->is_allocated = true;
	}
	else
	{
		nnom_memset(m, 0, sizeof(nnom_model_t));
		m->is_allocated = false;
	}

	// set methods
	m->add = model_add;
	m->hook = model_hook;
	m->merge = model_merge;
	m->mergex = model_mergex;
	m->active = model_active;

	return m;
}

static void io_tensor_delete(nnom_layer_io_t* io)
{
	while (io)
	{
		nnom_free(io->tensor);
		io = io->aux;
	}
}

// delete all the aux hooks
// delete aux io only, keep the primary io.
static void io_list_delete(nnom_layer_io_t *io)
{
	nnom_layer_hook_t *hook, *next_hook;
	nnom_layer_io_t *next_io;
	while (io)
	{
		// store the next io
		next_io = io->aux;

		// release hooks list first
		hook = io->hook.next;
		while (hook)
		{
			next_hook = hook->next;
			nnom_free(hook);
			hook = next_hook;
		}

		// now we can release the aux io itself
		// but if this io is the primary input/out of the layer, it will be freed with they layer's instance since they are allocated together.
		if (io != io->owner->in && io != io->owner->out)
			nnom_free(io);

		// next aux io
		io = next_io;
	}
}

// there are 2 type of memory in a layer
// *primary memory* is allocated when a layer instance is created, they are created by layer API (Conv2D()...).
// 		it includes the layer instance, primary input, primary output, and an optional computational memory buffer instance
//		each io module also has one primary hook.
// *secondary memory* are axiliary io modules, axiliary hooks and activations which created by model.xx() APIs (model.hook(), model.active()...)
//		it includes the list of aux io modules, the list of aux hooks.
//
// Additionaly, layer's private free method must be called to free layer's private resources
// Such as activation instance passed to Activation() layer, and private memory allcated within Lambda layer.
//
// A layer is consist of a few io modules. primary io are allocated with layers instance.
// each of the io has a few hooks. primary hooks are included in the io module.
// so only "aux" hooks and ios need to be freed separately.
static void layer_delete(nnom_layer_t *layer)
{
	if (layer == NULL)
		return;

	// call private free of the layer
	if (layer->free)
		layer->free(layer);

	// delete the tensors first. only input layer should delete input 
	if (layer->type == NNOM_INPUT)
		io_tensor_delete(layer->in);
	io_tensor_delete(layer->out);

	// release secondary memory on the layers.
	// they are io lists and hooks list
	io_list_delete(layer->in);
	io_list_delete(layer->out);

	// release activations (it takes null too)
	nnom_free(layer->actail);

	// release primary memory
	nnom_free(layer);
	return;
}

void model_delete(nnom_model_t *m)
{
	nnom_layer_t *layer;
	nnom_layer_t *next;
	if (m == NULL)
		return;

	// uses shortcut list to iterate the model,
	// start from head
	layer = m->head;
	while (layer)
	{
		// get the next before releasing current
		next = layer->shortcut;
		// your term
		layer_delete(layer);
		// who's next!
		layer = next;
	}

	// free the memory blocks for the network's buffer
	nnom_free(m->blocks->blk);

	// free model instance itself
	if (m->is_allocated)
		nnom_free(m);
	else
		nnom_memset(m, 0, sizeof(nnom_model_t));
	
	nnom_memory_taken = 0;
	return;
}

// find an available memory block.
static nnom_mem_block_t *allocate_block(nnom_mem_block_t *list)
{
	nnom_mem_block_t *free = NULL;
	uint32_t idx;

	for (idx = 0; idx < NNOM_BLOCK_NUM; idx++)
	{
		if (list[idx].owners == 0)
			break;
	}
    if(idx == NNOM_BLOCK_NUM)
    {
        NNOM_LOG("\nERROR! No enough memory block for parallel buffers, please increase the 'NNOM_BLOCK_NUM' in 'nnom_port.h'\n");
        return NULL;
    }

	free = &list[idx];
	return free;
}

static void release_block(nnom_mem_block_t *block)
{
	if (block->owners > 0)
		block->owners -= 1;
	if (block->owners == 0)
		block->state = NNOM_BUF_EMPTY;
}

static void release_input_mem(nnom_layer_t *layer)
{
	nnom_layer_io_t *in;
	// release all input of buf
	in = layer->in;
	while (in != NULL)
	{
		release_block(in->mem);
		in = in->aux;
	}
}
static void release_comp_mem(nnom_layer_t *layer)
{
	// release computational buf if exist
	if (layer->comp != NULL)
	{
		release_block(layer->comp->mem);
	}
}

// return the length of the io lists
size_t nnom_io_length(nnom_layer_io_t *io)
{
	size_t num = 0;
	if (io == NULL)
		return 0;
	while (io != NULL)
	{
		num++;
		io = io->aux;
	}
	return num;
}

// return the length of the hook lists
size_t nnom_hook_length(nnom_layer_hook_t *hook)
{
	size_t num = 0;
	if (hook == NULL)
		return 0;
	while (hook != NULL)
	{
		num++;
		hook = hook->next;
	}
	return num;
}

// The shortcut version of find_last() method. 
// must be used after compiling. 
static nnom_layer_t *layer_shortcut_find_last(nnom_layer_t *start)
{
	nnom_layer_t *layer = start;
	if (start == NULL)
		return NULL;
	while (layer->shortcut != NULL)
		layer = layer->shortcut;
	return layer;
}

// call while compiling.
// the shorcut is for fast running and fast iliterating.
// simply link every layer as a list. ordered by its runing order
static nnom_status_t layer_shortcut_add(nnom_layer_t *start, nnom_layer_t *curr)
{
	nnom_layer_t *layer = start;
	// first one, return
	if (start == curr)
	{
		return NN_SUCCESS;
	}
	// find the end of the list, and add curr layer to the end of it.
	while (layer->shortcut != NULL)
	{
		// if the layer is already in shortcut list, tell upper.
		if (curr == layer)
			return NN_ARGUMENT_ERROR;
		layer = layer->shortcut;
	}
	layer->shortcut = curr;

	return NN_SUCCESS;
}

// input the layer number,
static void print_layer_info(nnom_layer_t *layer, uint32_t layer_count)
{
	size_t in_size = io_mem_size(layer->in);
	size_t out_size = io_mem_size(layer->out);
	size_t compsize;
	size_t mac = layer->stat.macc;
	if (layer->comp != NULL)
		compsize = layer->comp->size;
	else
		compsize = 0;
	// names
	if(layer->type != NNOM_RNN)
		NNOM_LOG("#%-3d %-10s - ", layer_count, default_layer_names[layer->type]);
	else
	{
		NNOM_LOG("#%-3d %-3s/", layer_count, default_layer_names[layer->type]);
		NNOM_LOG("%-6s - ", default_cell_names[((nnom_rnn_layer_t*)layer)->cell->type]);
	}
		
	// activations
	if (layer->actail != NULL)
		NNOM_LOG("%-8s - ", default_activation_names[layer->actail->type]);
	else
		NNOM_LOG("         - ");

	NNOM_LOG("(");
	for (int i = 0; i < 3; i++)
	{
		if (layer->out->tensor->num_dim > i)
			NNOM_LOG("%4d,", layer->out->tensor->dim[i]);
		else 
			NNOM_LOG("     ");
	}
	NNOM_LOG(")  ");
	
	// MAC operation
	if(mac == 0)
		NNOM_LOG("        ");
	else if (mac < 10000)
		NNOM_LOG("%7d ", (uint32_t)mac);
	else if (mac < 1000*1000)
		NNOM_LOG("%6dk ", (uint32_t)(mac/1000));
	else if (mac < 1000*1000*1000)
		NNOM_LOG("%3d.%02dM ", (uint32_t)(mac/(1000*1000)), (uint32_t)(mac%(1000*1000)/(10*1000))); // xxx.xx M
	else
		NNOM_LOG("%3d.%02dG ", (uint32_t)(mac/(1000*1000*1000)), (uint32_t)(mac%(1000*1000*1000)/(10*1000*1000))); // xxx.xx G
	
	// memory 
	NNOM_LOG("(%6d,%6d,%6d)", (uint32_t)in_size, (uint32_t)out_size,(uint32_t) compsize);
}

static void print_memory_block_info(nnom_mem_block_t *block_pool)
{
	// show the memory blocks's lifetime (number of owners)
	NNOM_LOG("   ");
	for (int i = 0; i < NNOM_BLOCK_NUM; i++)
	{
		if (i % 4 == 0)
			NNOM_LOG(" ");
		if (block_pool[i].owners)
			NNOM_LOG("%d ", block_pool[i].owners);
		else
			NNOM_LOG("- ");
	}
	NNOM_LOG("\n");
}

// This is a nested called functions.
// to analyse the topology of the model, calculate the output_shape of each layer and create shortcut lists.
// Nest will happend when a layer have multiple output module or mutiple output hooks.
// This function will return when
// 	1) if the layer has multiple input but not all of them are filled by last layers. returns NN_MORE_TODO
//	2) if all the output hooked are nested called. return NN_SUCCESS
//	3) if the layer is output layer. return NN_SUCCESS
nnom_status_t compile_layers(nnom_layer_t* first, nnom_layer_t *curr, nnom_mem_block_t *block_pool, uint32_t *layer_count)
{
	size_t mem_size = 0;
	nnom_layer_t *layer = curr;
	nnom_layer_io_t *in;
	nnom_layer_io_t *out;
	nnom_layer_hook_t *hook;

	nnom_mem_block_t *in_blk;
	nnom_mem_block_t *out_blk;
	
	uint32_t local_layer_count = 1;
	
	if(layer_count == NULL)
		layer_count = &local_layer_count;

	in = layer->in;
	out = layer->out;

	while (layer)
	{
		// check input
		in = layer->in;

		// check if this layer is the input layer
		// the first layer has no input hooked, and the io is not initialized
		if (in->hook.io == NULL)
		{
			// if the input is not initalized
			if (in->mem == NULL)
			{
				in_blk = allocate_block(block_pool);
				in_blk->owners += 1; // add 1
				mem_size = nnom_alignto(tensor_size(in->tensor), NNOM_ALIGN);
				in_blk->size = mem_size > in_blk->size ? mem_size : in_blk->size;
				// set the blk to the layer IO
				in->mem = in_blk;
				in->mem->state = NNOM_BUF_FILLED; //mark input buff filled
			}
		}
		else
		{
			// get the mem for every input from its hooked output. 
			while (in != NULL)
			{
				in->mem = in->hook.io->mem;
				in = in->aux;
			}
		}

		// if there are mutiple inputs, wait utill all blocks filled
		in = layer->in;
		if (in != NULL && in->aux != NULL)
		{
			while (in != NULL)
			{
				// if the mem (of its hooked output) is not allocated or is not filled. 
				// It not the time to run the layer yet, return and waits for next nested called. 
				if (in->mem == NULL || in->mem->state != NNOM_BUF_FILLED)
					return NN_MORE_TODO;
				in = in->aux;
			}
		}

		// if run to this point, then it is the right time to compile(run) this layer. 
		// compiling are seperated into the steps below. 
		// 1. to calculate the output shape. 
		// 2. to put the current layer to the end of shortcut list.
		// 3. allocate computational buffer.
		// 4. allocate output buffer for each output module. 
		// 5.1 if there is only one layer hooked to the output. we dont use nested call, but continue in this big while loop. 
		// 5.2 nested call the hooked output layers (if there are > 1 hooked to the output of this layer)

		// 1. calculate output shape while all inputs are filled
		layer->build(layer);

		// 2. add to shortcut list. 
		layer_shortcut_add(first, layer);

		// 3. assign for computational buf
		if (layer->comp != NULL)
		{
			layer->comp->mem = allocate_block(block_pool);
			layer->comp->mem->owners += 1; // add us to buffer users
			layer->comp->mem->state = NNOM_BUF_FILLED;
			// record maximum mem size in this block
			mem_size = nnom_alignto(layer->comp->size, NNOM_ALIGN);
			layer->comp->mem->size =
				mem_size > layer->comp->mem->size ? mem_size : layer->comp->mem->size;
		}

		// print current layer's info. 
		// show names, activations, mem block size
		print_layer_info(layer, (*layer_count)++);

		// 4. allocate output buffer for each output module. 
		// check output
		if (layer->out == NULL)
			return NN_SUCCESS;

		// 5.1 if there is only one layer hooked to the output. we dont use nested call, but continue in this big while loop. 
		// if the layer is Single Output, continue the loop directly. To reduce nested level
		if (layer->out->aux == NULL && layer->out->hook.next == NULL)
		{
			// single buf layer.
			if (layer->in->type == NNOM_TENSOR_BUF_NULL || layer->out->type == NNOM_TENSOR_BUF_NULL)
			{
				// pass to next layer directly, like we never touch the buffer(dont change life-time)
				layer->out->mem = layer->in->mem;
				
				// print memory before release
				print_memory_block_info(block_pool);
				// computational buf
				release_comp_mem(layer);
			}
			// not a single buf layer
			else
			{
				// allocate mem block for the output
				out_blk = allocate_block(block_pool);
				if (out_blk == NULL)
					return NN_NO_MEMORY;
				// set the life time, only one hooked layer, so the life time is 1
				out_blk->owners = 1;
				out_blk->state = NNOM_BUF_FILLED; // marked filled
				// record maximum mem size in this block
				mem_size = nnom_alignto(tensor_size(layer->out->tensor), NNOM_ALIGN);
				out_blk->size = mem_size > out_blk->size ? mem_size : out_blk->size;
				// set the blk to the layer IO
				layer->out->mem = out_blk;

				// once we allocate for output, we can now release input and comput.
				// print memory before release
				print_memory_block_info(block_pool);
				// release input mem and comp mem
				release_input_mem(layer);
				release_comp_mem(layer);
			}
		}
		// Multiple output and/or mutiple hooks
		else
		{
			// single buf layer will use the input buf for the first output
			if (layer->in->type == NNOM_TENSOR_BUF_NULL || layer->out->type == NNOM_TENSOR_BUF_NULL)
			{
				// we dont allocate new buf, but use the input
				// the ownership will be set to next layer later
				layer->out->mem = layer->in->mem;
				layer->out->mem->owners += nnom_hook_length(&layer->out->hook); // set the mem lifetime.// test
				layer->out->mem->state = NNOM_BUF_FILLED;
				
				// print memory before release
				print_memory_block_info(block_pool);
				// release computational buff and input buffer 
				release_input_mem(layer);
				release_comp_mem(layer);
			}
			// mutiple buf layer. (I/O use different memory blocks)
			else
			{
				// allocate for every output
				out = layer->out;
				while (out != NULL && out->hook.io != NULL) // the output layer have no output IO
				{
					// assign new block
					out->mem = allocate_block(block_pool);
					if (out->mem == NULL)
						return NN_NO_MEMORY;
					// record maximum mem size in this block
					mem_size = nnom_alignto(tensor_size(out->tensor), NNOM_ALIGN);
					out->mem->size = mem_size > out->mem->size ? mem_size : out->mem->size;
					// keep the block untill the last hooked layer is called.
					out->mem->owners = nnom_hook_length(&out->hook); // set lifetime of the buffer = the num of hooked layers
					out->mem->state = NNOM_BUF_FILLED;

					out = out->aux;
				}
				// once we allocate for output, we can now release input and comput (or reduce the lifetime).
				// print memory before release
				print_memory_block_info(block_pool);
				// release input mem and comp mem
				release_input_mem(layer);
				release_comp_mem(layer);
			}

			// 5.12 nested call the hooked output layers (if there are > 1 hooked to the output of this layer)
			// while all the out module(s) receive a memory block, it is ready to be sent to other layers.
			// iterate all hooked layers in each out module.
			out = layer->out;
			while (out != NULL)
			{
				// nested call hooked layer one by one.
				hook = &out->hook;
				while (hook != NULL && hook->io != NULL)
				{
					compile_layers(first, hook->io->owner, block_pool, layer_count);
					hook = hook->next;
				}

				// next io
				out = out->aux;
			}

			// when all the out is called. this should stop here.
			// once enter mutiple output iterating, the function will always return.
			// because at least one of the nested called by this function will run till the end of the model.
			return NN_SUCCESS;
		}
		// Multiple output ended.

		// return if this is output layer.
		// the output layer's output io is hooked to nothing.
		if (layer->out->hook.io == NULL)
			return NN_SUCCESS;

		// single output layer, this function continue to analyse next layer.
		// switch to next layer.
		layer = layer->out->hook.io->owner;
	}

	// seems to be redundants
	return NN_SUCCESS;
}

size_t mem_analysis_result(nnom_model_t *m)
{
	uint32_t index;
	uint32_t total_mem = 0;
	NNOM_LOG("Memory cost by each block:\n ");
	// print size of memory blocks
	for (index = 0; index < NNOM_BLOCK_NUM; index++)
	{
		total_mem += m->blocks[index].size;
		NNOM_LOG("blk_%d:%d  ", index, (uint32_t)(m->blocks[index].size));
	}
	// size of total memory cost by networks buffer
	NNOM_LOG("\n Memory cost by network buffers: %d bytes\n", total_mem);
	return total_mem;
}

// allocate memory, and set them to each block according to the mem analysis results.
nnom_status_t block_mem_set(nnom_model_t *m, void *buf)
{
	uint32_t index;
	uint32_t mem_offset = 0;

	for (index = 0; index < NNOM_BLOCK_NUM; index++)
	{
		if (m->blocks[index].size == 0)
			break;
		m->blocks[index].blk = (void *)((uint8_t*)buf + mem_offset);
		mem_offset += m->blocks[index].size;
	}
	return NN_SUCCESS;
}

// experimental: this function is temporary use to 
// assign memory blk which has assigned to input and output to the corresponding tensor
nnom_status_t tensor_mem_set(nnom_model_t *m)
{
	nnom_layer_t *layer = m->head;
	nnom_layer_io_t *io;
	while (layer)
	{
		io = layer->in;
		while (io)
		{
			io->tensor->p_data = io->mem->blk;
			io = io->aux;
		}

		io = layer->out;
		while (io)
		{
			io->tensor->p_data = io->mem->blk;
			io = io->aux;
		}

		layer = layer->shortcut;
	}
	
	return NN_SUCCESS;
}

// this function has to be used after memory is assigned to the layers.
// it means it has to be call after compile_model() as well.
// it simply get the output buffer and set the buffer to tailed activation of each layer..
nnom_status_t set_tailed_activation(nnom_model_t *m)
{
	NNOM_NULL_CHECK(m);
	NNOM_NULL_CHECK(m->head);
	nnom_layer_t *layer = m->head;

	// if tailed activation is exist, set it to the output.
	while (layer)
	{
		if (layer->actail != NULL)
		{
			layer->actail->tensor = layer->out->tensor;
		}
		if (layer->shortcut == NULL)
			break;
		layer = layer->shortcut;
	}

	return NN_SUCCESS;
}

// get total ops
static uint64_t model_set_ops(nnom_model_t *m)
{
	nnom_layer_t *layer;
	uint64_t total_ops = 0;
	layer = m->head;
	while (layer)
	{
		total_ops += layer->stat.macc;
		if (layer->shortcut == NULL)
			break;
		layer = layer->shortcut;
	}
	m->total_ops = total_ops;
	return total_ops;
}

// a compiler can be use for both sequencial / functional model.
// the output layer is optional only when the model is single output model
// in this case, if output = NULL, the compile can find it by its own. 
nnom_status_t model_compile(nnom_model_t *m, nnom_layer_t *input, nnom_layer_t *output)
{
	size_t buf_size;
	uint8_t *buf;
	uint32_t layer_num = 1;
	uint32_t time = nnom_ms_get();
	
	NNOM_NULL_CHECK(m);
	NNOM_NULL_CHECK(input);

	m->head = input;
	m->tail = output;
	if (output == NULL)
		m->tail = find_last(input);

	NNOM_LOG("NNoM version %d.%d.%d\n", NNOM_MAJORVERSION, NNOM_SUBVERSION, NNOM_REVISION);
	NNOM_LOG("To disable logs, please void the marco 'NNOM_LOG(...)' in 'nnom_port.h'.\n");
	#ifdef NNOM_USING_CHW
	    NNOM_LOG("Data format: Channel first (CHW)\n");
	#else
	    NNOM_LOG("Data format: Channel last (HWC)\n");
	#endif
	#ifdef NNOM_USING_CMSIS_NN
	    NNOM_LOG("Backend optimization: CMSIS-NN\n");
	#endif
    #ifdef NNOM_USING_STATIC_MEMORY
	    NNOM_LOG("Static memory size set to: %d\n", (uint32_t)nnom_static_buf_size);
	#endif
	NNOM_LOG("Start compiling model...\n");
	NNOM_LOG("Layer(#)         Activation    output shape    ops(MAC)   mem(in, out, buf)      mem blk lifetime\n");
	NNOM_LOG("-------------------------------------------------------------------------------------------------\n");

	// compile layers, started from list head, nested run till the end of models
	compile_layers(m->head, m->head, m->blocks, &layer_num);

	NNOM_LOG("-------------------------------------------------------------------------------------------------\n");

	// if model's tail is not the last layer which built by user.
	if (output->type != NNOM_OUTPUT)
		NNOM_LOG("WARNING: the last layer '%s' is not the Output Layer, please check carefully.\n",
			default_layer_names[output->type]);

	// get the total (aligned) memory requirement
	buf_size = mem_analysis_result(m);

	// allocate one big memory block
	buf = nnom_mem(buf_size);
	if (buf == NULL)
	{
		NNOM_LOG("ERROR: No enough memory for network buffer, required %d bytes\n", (uint32_t)buf_size);
		return NN_NO_MEMORY;
	}
    // all memory cost
	NNOM_LOG(" Total memory occupied: %d bytes\n", (uint32_t)nnom_memory_taken);

	// split the memory for every memory block
	block_mem_set(m, buf);

	// experimental: set memory from io to the io tensor 
	tensor_mem_set(m);

	// finally set the output buff to tailed activation on each layer
	set_tailed_activation(m);

	// calculate the total operations and set it to the model
	model_set_ops(m);
	
	// print the time. 
	if(nnom_ms_get())
		NNOM_LOG("Compling done in %d ms\n", nnom_ms_get() - time);

	return NN_SUCCESS;
}

// This is a simplified API for compile models with sequencial model only
// this does not require specified Input / Output layers
nnom_status_t sequencial_compile(nnom_model_t *m)
{
	nnom_layer_t *input, *output;
	input = m->head;
	output = find_last(input);
	return model_compile(m, input, output);
}

// run that layer
nnom_status_t layer_run(nnom_layer_t *layer)
{
	nnom_status_t result;
	uint32_t start;
	NNOM_NULL_CHECK(layer);

	// start
	start = nnom_us_get();
	// run main layer first
	result = layer->run(layer);
	// run tailed-activation if it is presented
	if (layer->actail != NULL)
	{
		layer->actail->run(layer->actail);
	}
	// done
	layer->stat.time = nnom_us_get() - start;
	return result;
}

// run the model, until the end_layer. If end_layer == NULL, run all layers.
nnom_status_t model_run_to(nnom_model_t *m, nnom_layer_t *end_layer)
{
	uint32_t layer_num = 1;
	nnom_status_t result;
	nnom_layer_t *layer;
	NNOM_NULL_CHECK(m);
	NNOM_NULL_CHECK(m->head);

	layer = m->head;
	
	// using shortcut run
	while (layer)
	{
		// run layer
		result = layer_run(layer);
		if (result != NN_SUCCESS)
		{
			NNOM_LOG("Error: #%d %s layer return error code:%d\n", layer_num, default_layer_names[layer->type], result);
			return result;
		}
		// run callback
		if(m->layer_callback != NULL)
		{
			result = m->layer_callback(m, layer);
			if (result != NN_SUCCESS)
			{
				NNOM_LOG("Error: Callback return error code %d at #%d %s layer\n", result, layer_num, default_layer_names[layer->type]);
				return result;
			}
		}		
		// check if finished
		if (layer == end_layer || layer->shortcut == NULL)
			break;
		layer = layer->shortcut;
		layer_num++;
	}

	return NN_SUCCESS;
}

// run all layers.
nnom_status_t model_run(nnom_model_t *m)
{
	return model_run_to(m, NULL);
}

// callback, called after each layer has finished the calculation. 
nnom_status_t model_set_callback(nnom_model_t *m, nnom_status_t (*layer_callback)(nnom_model_t *m, nnom_layer_t *layer))
{
	if(m->layer_callback != NULL && m->layer_callback != layer_callback)
		return NN_LENGTH_ERROR;
	
	m->layer_callback = layer_callback;
	return NN_SUCCESS;
}

// delete callback. 
void model_delete_callback(nnom_model_t *m)
{
	m->layer_callback = NULL;
}

nnom_status_t check_model_version(unsigned long model_version)
{
	nnom_status_t result = NN_SUCCESS;
	int32_t major, sub, rev;
	major = model_version/10000;
	sub = (model_version/100)%100;
	rev = model_version % 100;
	if(model_version != NNOM_VERSION)
	{
		NNOM_LOG("WARNING: model version %d.%d.%d dosen't match nnom version!\n", major, sub, rev);
		result = -NN_ARGUMENT_ERROR;
	}
	else
	{
		NNOM_LOG("Model version: %d.%d.%d\n", major, sub, rev);
	}
	return result;
}


