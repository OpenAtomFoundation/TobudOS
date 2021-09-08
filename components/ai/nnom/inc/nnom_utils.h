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

#ifndef __NNOM_UTILS_H__
#define __NNOM_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"

typedef struct _nnom_predict_t
{
	uint16_t *confusion_mat; // confusiong matrix
	uint32_t *top_k;		 // which stored the num of prediction in rank_k, example: Top-2 = top_k[0]+top_k[1]
	nnom_model_t *model;	 // the model to run
	int8_t *buf_prediction;  // the pointer to the output of softmax layer(normally the end of classifier).

	// setting
	uint32_t label_num;  // number of types in classification
	uint32_t top_k_size; // number of k that wants to know.

	// running
	uint32_t predict_count; // how many prediction is done

	//timing
	uint32_t t_run_total;	// total running time
	uint32_t t_predict_start; // when it is initial
	uint32_t t_predict_total; // total time of the whole test
} nnom_predict_t;

// create a prediction
// input model, the buf pointer to the softwmax output (Temporary, this can be extract from model)
// the size of softmax output (the num of lable)
// the top k that wants to record.
nnom_predict_t *prediction_create(nnom_model_t *m, int8_t *buf_prediction, size_t label_num, size_t top_k_size); // currently int8_t

// after a new data is set in input
// feed data to prediction
// input the current label, (range from 0 to total number of label -1)
// (the current input data should be set by user manully to the input buffer of the model.)
// return NN_ARGUMENT_ERROR if parameter error
nnom_status_t prediction_run(nnom_predict_t *pre, uint32_t true_label, uint32_t* predict_label, float* prob);

// to mark prediction finished
void prediction_end(nnom_predict_t *pre);

// free all resources
void prediction_delete(nnom_predict_t *pre);

// print matrix
void prediction_matrix(nnom_predict_t *pre);

// print top-k
void prediction_top_k(nnom_predict_t *pre);

// this function is to print sumarry
void prediction_summary(nnom_predict_t *pre);

// -------------------------------

// stand alone prediction API
// this api test one set of data, return the prediction
// return the predicted label
// return NN_ARGUMENT_ERROR if parameter error
nnom_status_t nnom_predict(nnom_model_t *m, uint32_t *label, float *prob);

void model_stat(nnom_model_t *m);

void model_io_format(nnom_model_t *m);

#ifdef __cplusplus
}
#endif

#endif /*__NNOM_UTILS_H__ */
