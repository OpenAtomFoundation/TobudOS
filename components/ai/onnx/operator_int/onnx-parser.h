#ifndef __ONNX_PARSER_H__
#define __ONNX_PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "onnx.pb-c.h"

Onnx__ModelProto* onnx_load_model(const char* onnx_file_name);
void onnx_model_info(Onnx__ModelProto* model);
void onnx_graph_info(Onnx__GraphProto* graph);
void onnx_graph_info_sorted(Onnx__GraphProto* graph);
void onnx_graph_input_info(Onnx__ValueInfoProto* input);
void onnx_graph_output_info(Onnx__ValueInfoProto* output);
void onnx_graph_node_info(Onnx__NodeProto* node);
void onnx_graph_initializer_info(Onnx__TensorProto* initializer);

Onnx__NodeProto* onnx_graph_get_node_by_name(Onnx__GraphProto* graph, const char* node_name);
Onnx__NodeProto* onnx_graph_get_node_by_input(Onnx__GraphProto* graph, const char* input_name);
long* onnx_graph_get_dims_by_name(Onnx__GraphProto* graph, const char* node_name);
long onnx_graph_get_dim_by_name(Onnx__GraphProto* graph, const char* node_name);
int* onnx_graph_get_weights_by_name(Onnx__GraphProto* graph, const char* node_name);

void onnx_graph_value_tensor_shape_dimension_info(Onnx__TensorShapeProto__Dimension* dim);

#endif //__ONNX_PARSER_H__
