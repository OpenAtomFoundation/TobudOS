
/*
 * Copyright (c) 2018-2020
 * Jianjia Ma
 * majianjia@live.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-07-23     Jianjia Ma   The first version
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"
#include "nnom_local.h"
#include "nnom_layers.h"
#include "layers/nnom_output.h"

nnom_layer_t *output_s(const nnom_io_config_t* config)
{
	nnom_layer_t *layer = input_s(config);
	if(layer)
	{
		layer->config = (void*) config;
		layer->type = NNOM_OUTPUT;
		layer->run = output_run;
		layer->build = default_build;
	}
	return layer;
}

nnom_layer_t *Output(nnom_3d_shape_t output_shape, void *p_buf)
{
	// they are acturally the same.. expect the type defined
	nnom_layer_t *layer = Input(output_shape, p_buf);
	if (layer != NULL)
	{
		layer->type = NNOM_OUTPUT;
		layer->run = output_run;
		layer->build = default_build;
	}
	return layer;
}

nnom_status_t output_run(nnom_layer_t *layer)
{
	nnom_io_layer_t *cl = (nnom_io_layer_t *)layer;
	nnom_memcpy(cl->buf, layer->in->tensor->p_data, tensor_size(layer->out->tensor)); // in->memory -> user memory
	return NN_SUCCESS;
}
