/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/lite/micro/examples/person_detection_experimental/image_provider.h"
#include "tensorflow/lite/micro/examples/person_detection_experimental/model_settings.h"
#include "tensorflow/lite/micro/examples/person_detection_experimental/person_image_data.h"
#include "tensorflow/lite/micro/examples/person_detection_experimental/no_person_image_data.h"
#include "stdint.h"
#include "sys.h"
#define OV2640_PIXEL_WIDTH  ((uint16_t)96)
#define OV2640_PIXEL_HEIGHT ((uint16_t)96)

extern uint16_t camBuffer[OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT];

uint8_t RGB565toGRAY(uint16_t bg_color)
{
    uint8_t bg_r = 0;
    uint8_t bg_g = 0;
    uint8_t bg_b = 0;
    bg_r = ((bg_color>>11)&0xff)<<3;
    bg_g = ((bg_color>>5)&0x3f)<<2;
    bg_b = (bg_color&0x1f)<<2;
    uint8_t gray = (bg_r*299 + bg_g*587 + bg_b*114 + 500) / 1000;
    return gray;
}

void Input_Convert(uint16_t* camera_buffer , uint8_t* model_buffer)
{
	for(int i=0 ; i<OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT ; i++)
	{
		model_buffer[i] = RGB565toGRAY(camera_buffer[i]);
	}
}

extern uint8_t modelBuffer[OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT];

TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, int8_t* image_data) {
  //for (int i = 0; i < image_width * image_height * channels; ++i) {
	//for (int i = 0; i < OV2640_PIXEL_WIDTH * OV2640_PIXEL_HEIGHT * 1; ++i) {
    //image_data[i] = g_no_person_data[i]; // ÎÞÈËÊý¾Ý -- Óë²âÊÔÓÃÀý²»Í¬
	//image_data[i] = g_person_data[i]; // ÓÐÈËÊý¾Ý -- Óë²âÊÔÓÃÀý½á¹ûÏàÍ¬
	//image_data[i] = modelBuffer[i]; //ÊµÏÖÉãÏñÍ·Í¼Ïñ»ñÈ
	for(int i=0 ; i<OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT ; i++)
	{
		image_data[i] = RGB565toGRAY(camBuffer[i]);
	}
  //}
  return kTfLiteOk;
}
