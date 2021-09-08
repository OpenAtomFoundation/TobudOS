/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "model.h"
#include "no_micro_features_data.h"
#include "yes_micro_features_data.h"
#include "micro_speech.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

namespace {
tflite::MicroErrorReporter micro_error_reporter;
tflite::MicroMutableOpResolver<4> micro_op_resolver;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;

// In order to use optimized tensorflow lite kernels, a signed int8_t quantized
// model is preferred over the legacy unsigned model format. This means that
// throughout this project, input images must be converted from unisgned to
// signed format. The easiest and quickest way to convert from unsigned to
// signed 8-bit integers is to subtract 128 from the unsigned value to get a
// signed value.

// An area of memory to use for input, output, and intermediate arrays.
// Create an area of memory to use for input, output, and intermediate arrays.
const int tensor_arena_size = 10 * 1024;
uint8_t tensor_arena[tensor_arena_size];
}  // namespace

//TF_LITE_MICRO_TESTS_BEGIN

//TF_LITE_MICRO_TEST(TestInvoke) {
extern "C" void speech_init(int8_t* input_data){
  // Set up logging.
  

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = ::tflite::GetModel(g_model);

	static tflite::MicroMutableOpResolver<4> micro_op_resolver;
	micro_op_resolver.AddDepthwiseConv2D();
	micro_op_resolver.AddFullyConnected();
	micro_op_resolver.AddReshape();
	micro_op_resolver.AddSoftmax();

// Build an interpreter to run the model with.
	static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena,
                                       tensor_arena_size,
                                       &micro_error_reporter);
	interpreter = &static_interpreter;
	interpreter->AllocateTensors();

  // Get information about the memory area to use for the model's input.
  model_input = interpreter->input(0);
}

extern "C" void speech_run(int8_t* input_data){
  // Copy a spectrogram created from a .wav audio file of someone saying "Yes",
  // into the memory area used for the input.
  const int8_t* yes_features_data = g_yes_micro_f2e59fea_nohash_1_data;
	//const int8_t* yes_features_data = input_data; //input your own speech data
  for (size_t i = 0; i < model_input->bytes; ++i) {
    model_input->data.int8[i] = yes_features_data[i];
  }

  // Run the model on this input and make sure it succeeds.
  TfLiteStatus invoke_status = interpreter->Invoke();
  TfLiteTensor* output = interpreter->output(0);
  const int kSilenceIndex = 0;
  const int kUnknownIndex = 1;
  const int kYesIndex = 2;
  const int kNoIndex = 3;

  // Make sure that the expected "Yes" score is higher than the other classes.
  uint8_t silence_score = output->data.uint8[kSilenceIndex] + 128;
  uint8_t unknown_score = output->data.uint8[kUnknownIndex] + 128;
  uint8_t yes_score = output->data.int8[kYesIndex] + 128;
  uint8_t no_score = output->data.int8[kNoIndex] + 128;

  // Run the model on this "No" input.
  invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(&micro_error_reporter, "Invoke failed\n");
  }
//  TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);

  // Get the output from the model, and make sure it's the expected size and
  // type.
  output = interpreter->output(0);
  silence_score = output->data.int8[kSilenceIndex] + 128;
  unknown_score = output->data.int8[kUnknownIndex] + 128;
  yes_score = output->data.int8[kYesIndex] + 128;
  no_score = output->data.int8[kNoIndex] + 128;
	printf("speech test-> silence: %d unknown_score: %d yes_score: %d no_score: %d\n" , silence_score,unknown_score,yes_score,no_score);

  TF_LITE_REPORT_ERROR(&micro_error_reporter, "Ran successfully\n");
}

//TF_LITE_MICRO_TESTS_END
