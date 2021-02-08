# 概述

`Tensorflow Lite Micro` 是 `TensorFlow Lite` 针对 AIOT 的轻量级 AI 引擎，专门用于在微控制器和其他资源受限的设备上运行机器学习模型。

# 1. 建立与转换模型

由于嵌入式设备存储空间有限，因此限制了深度学习的应用，同时考虑到平台算力以及算子支持等因素，因此在模型设计以及部署阶段需充分了解硬件平台资源以及预期性能。

本部分将介绍将 TensorFlow 模型转换为可在嵌入式设备上运行的模型的过程。

## 1.1 模型转换

将一个已训练好的 TensorFlow 模型转换为可以在嵌入式设备中运行的 Tensorflow Lite 模型可以使用 [TensorFlow Lite 转换器 Python API](https://tensorflow.google.cn/lite/microcontrollers/build_convert) 。它能够将模型转换成 [`FlatBuffer`](https://google.github.io/flatbuffers/) 格式，减小模型规模，并修改模型及算子以支持 TensorFlow Lite 运算。

### 1.1.1  量化

为了获得尽可能小的模型，某些情况下可以考虑使用 [训练后量化](https://tensorflow.google.cn/lite/performance/post_training_quantization) 。它会降低模型中数字的精度，从而减小模型规模，比如将 FP32 转化为 Int8。不过，这种操作可能会导致模型推理准确性的下降，对于小规模模型尤为如此，所以我们需要在量化前后分析模型的准确性，以确保这种损失在可接受范围内。

以下这段 Python 代码片段展示了如何使用预训练量化进行模型转换：

```python
import tensorflow as tf
converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir)
converter.optimizations = [tf.lite.Optimize.OPTIMIZE_FOR_SIZE]
tflite_quant_model = converter.convert()
open("converted_model.tflite", "wb").write(tflite_quant_model)
```

### 1.1.2  将模型文件转换为一个 C 数组

许多微控制器平台没有本地文件系统，从程序中使用一个模型最简单的方式是将其转换为 C 数组并将其编译进应用程序。

以下的 unix 命令会生成一个包含 TensorFlow Lite 模型的 C 源文件，其中模型数据以 `char` 数组形式表现：

```bash
xxd -i converted_model.tflite > model_data.cc
```

其输出类似如下：

```c
unsigned char converted_model_tflite[] = {
  0x18, 0x00, 0x00, 0x00, 0x54, 0x46, 0x4c, 0x33, 0x00, 0x00, 0x0e, 0x00,
  // <Lines omitted>
};
unsigned int converted_model_tflite_len = 18200;
```

生成此文件后，你可以将其包含到程序中。在嵌入式平台上，我们需要将该数组声明为 `const` 类型以获得更好的内存效率。

## 1.2 模型结构与训练

在设计一个面向微控制器的模型时，考虑模型的规模、工作负载以及模型所使用到的算子是非常重要的。

### 1.2.1 模型规模

一个模型必须在二进制和运行时方面都足够小，以使其编译进应用程序后满足目标设备的内存限制。

为了创建一个更小的模型，你可以在模型设计中采用少而小的层。然而，小规模的模型更易导致欠拟合问题。这意味着对于许多应用，尝试并使用符合内存限制的尽可能大的模型是有意义的。但是，使用更大规模的模型也会增加处理器工作负载。

注：在一个 Cortex M3 上，TensorFlow Lite Micro 的 core runtime 仅占约16 KB。

### 1.2.2 工作负载

工作负载受到模型规模，结构与复杂度的影响，大规模、复杂的模型可能会导致更高的功耗。在实际的应用场景中，功耗与热量的增加可能会带来其他问题。

### 1.2.3 算子支持

TensorFlow Lite Micro 目前仅支持有限的 TensorFlow 算子，因此可运行的模型也有所限制。谷歌正致力于在参考实现和针对特定结构的优化方面扩展算子支持。Arm 的 CMSIS-NN 开源加速库也为算子的支持和优化提供了另一种可能。

已支持的算子在文件 [`all_ops_resolver.cc`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/all_ops_resolver.cc) 中列出。

## 1.3 运行推断

以下部分将介绍软件包自带语音例程中的 [main_functions.cc](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/examples/person_detection_experimental/main_functions.cc) 文件，并阐述了如何使用 Tensorflow Lite Micro 来进行 AI 推理。

### 1.3.1 包含项

```C++
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
```

- [`micro_ops.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/kernels/micro_ops.h) 提供给解释器（interpreter）用于运行模型的操作。
- [`micro_error_reporter.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/micro_error_reporter.h) 输出调试信息。
- [`micro_interpreter.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/micro_interpreter.h) Tensorflow Lite Micro 解释器，用来运行模型。
- [`schema_generated.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/schema/schema_generated.h) 定义 TensorFlow Lite [`FlatBuffer`](https://google.github.io/flatbuffers/) 数据结构。
- [`version.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/version.h) 提供 Tensorflow Lite 架构的版本信息。

示例中还包括其他一些文件，比如：

```C++
#include "tensorflow/lite/micro/examples/micro_speech/micro_features/micro_model_settings.h"
#include "tensorflow/lite/micro/examples/micro_speech/micro_features/model.h"
```

- [`model.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/examples/micro_speech/micro_features/model.h) 将模型存储为 `char` 类型数组。阅读 [“构建与转换模型”](https://tensorflow.google.cn/lite/microcontrollers/build_convert)来了解如何将 Tensorflow Lite 模型转换为该格式。
- [`micro_model_settings.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/examples/micro_speech/micro_features/micro_model_settings.h) 定义与模型相关的各种常量。

### 1.3.2 设置日志记录

要记录日志，需要实例化 `tflite::MicroErrorReporter` 类：

```C++
tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;
```

该对象被传递到解释器（interpreter）中用于记录日志。由于微控制器通常具有多种日志记录机制，因此 `tflite::MicroErrorReporter` 在实现上考虑了设备的差异性。

### 1.3.3 加载模型

在以下代码中，实例化的 `char` 数组中包含了模型信息，`g_tiny_conv_micro_features_model_data` （要了解其是如何构建的，请参见 [“构建与转换模型”](https://tensorflow.google.cn/lite/microcontrollers/build_convert) 。随后我们检查模型来确保其架构版本与使用版本兼容：

```C++
const tflite::Model* model =
    ::tflite::GetModel(g_tiny_conv_micro_features_model_data);
if (model->version() != TFLITE_SCHEMA_VERSION) {
  error_reporter->Report(
      "Model provided is schema version %d not equal "
      "to supported version %d.\n",
      model->version(), TFLITE_SCHEMA_VERSION);
  return 1;
}
```

### 1.3.4 实例化 OP 解析器

解释器（interpreter）需要一个 [`micro_ops`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/kernels/micro_ops.h) 实例来访问 Tensorflow 操作。可以扩展此类来添加自定义操作：

```C++
tflite::ops::micro::micro_op_resolver resolver;
```

### 1.3.5 分配内存

我们需要预先为输入、输出以及中间变量分配一定的内存。该预分配的内存是一个大小为 `tensor_arena_size` 的 `uint8_t` 数组。它将会作为 `tflite::SimpleTensorAllocator` 实例化的参数：

```C++
const int tensor_arena_size = 10 * 1024;
uint8_t tensor_arena[tensor_arena_size];
tflite::SimpleTensorAllocator tensor_allocator(tensor_arena,
                                               tensor_arena_size);
```

注意：所需内存大小取决于使用的模型，可能需要通过实验来确定。

### 1.3.6 实例化解释器（Interpreter）

我们创建一个 `tflite::MicroInterpreter` 实例并传递相关变量：

```C++
tflite::MicroInterpreter interpreter(model, resolver, &tensor_allocator,
                                     error_reporter);
```

### 1.3.7 验证输入维度

`MicroInterpreter` 实例可以通过调用 `.input(0)` 返回模型输入张量的指针。其中 `0` 代表第一个（也是唯一的）输入张量。我们通过检查这个张量来确认它的维度与类型是否与应用匹配：

```C++
TfLiteTensor* model_input = interpreter.input(0);
if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
    (model_input->dims->data[1] != kFeatureSliceCount) ||
    (model_input->dims->data[2] != kFeatureSliceSize) ||
    (model_input->type != kTfLiteUInt8)) {
  error_reporter->Report("Bad input tensor parameters in model");
  return 1;
}
```

在上述代码中，变量 `kFeatureSliceCount` 和 `kFeatureSliceSize` 与输入相关，其定义在 [`micro_model_settings.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/examples/micro_speech/micro_features/micro_model_settings.h) 中。枚举值 `kTfLiteUInt8` 是对 Tensorflow Lite 某一数据类型的引用，其定义在 [`common.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/c/common.h) 中。

### 1.3.8 生成特征

微控制器的音频输入作为输入到模型中的数据。[`feature_provider.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/examples/micro_speech/feature_provider.h) 中定义的 `FeatureProvider` 类捕获音频并将其转换为一组特征集合。当该类被实例化时，我们将获取的 `TfLiteTensor` 以及 `FeatureProvider` 作为参数，填充输入数据用于模型运算：

```C++
FeatureProvider feature_provider(kFeatureElementCount, model_input->data.uint8);
```

`FeatureProvider` 将计算最近一秒的音频，生成一组特征后填充输入张量：

```C++
TfLiteStatus feature_status = feature_provider.PopulateFeatureData(
    error_reporter, previous_time, current_time, &how_many_new_slices);
```

在此例中，特征生成和推理发生在同一循环，因此设备能不断捕捉和处理最新的音频数据。

在编写程序时，可以通过其它方式生成特征数据，但需要注意，特征数据填充完成后才能进行推理。

### 1.3.9 运行模型

通过在 `tflite::MicroInterpreter` 实例上调用 `Invoke()` 可快速触发推理：

```C++
TfLiteStatus invoke_status = interpreter.Invoke();
if (invoke_status != kTfLiteOk) {
  error_reporter->Report("Invoke failed");
  return 1;
}
```

通过检查返回值 `TfLiteStatus` 来确定运行是否成功。在 [`common.h`](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/c/common.h) 中定义的 `TfLiteStatus` 有 `kTfLiteOk` 和 `kTfLiteError`两种状态。

### 1.3.10 获取输出

模型的输出张量可以通过在 `tflite::MicroIntepreter` 上调用 `output(0)` 获得，其中 `0` 代表第一个（也是唯一的）输出张量。

在示例中，输出是一个数组，表示输入属于不同类别（“是”（yes）、“否”（no）、“未知”（unknown）以及“静默”（silence））的概率。由于它们是按照集合顺序排列的，我们可以使用简单的逻辑来确定概率最高的类别：

```C++
TfLiteTensor* output = interpreter.output(0);
uint8_t top_category_score = 0;
int top_category_index;
for (int category_index = 0; category_index < kCategoryCount;
     ++category_index) {
    const uint8_t category_score = output->data.uint8[category_index];
    if (category_score > top_category_score) {
        top_category_score = category_score;
        top_category_index = category_index;
    }
}
```

在示例的其他部分中，使用了一个更加复杂的算法来平滑多帧的识别结果。该部分在 [recognize_commands.h](https://github.com/tensorflow/tensorflow/tree/5e0ed38eb746f3a86463f19bcf7138a959ddb2d4/tensorflow/lite/micro/examples/micro_speech/recognize_commands.h) 中有所定义。在处理任何连续的数据流时，也可以使用相同的技术来提高可靠性和准确度。

# 2. 制作 tensorflow_lite_micro.lib

## 2.1 获得 Tensorflow Lite Micro 库

构建库并从 TensorFlow master branch 中运行测试，请执行以下命令：

将 TensorFlow project 下载到本地，在终端中输入以下命令：

```bash
git clone https://github.com/tensorflow/tensorflow.git
git checkout 5e0ed38eb746f3a86463f19bcf7138a959ddb2d4
```

注：进行 checkout 的原因是由于 Tensorflow 官方仓库的更新速度较快，为了方便开发者学习 .lib 库的制作方法，作者将使用上述版本来进行演示，开发者如果在实际操作过程中出现问题的话也可以通过 [此链接](https://github.com/QingChuanWS/tensorflow) 直接获得作者所使用的Tensorflow工程。

进入 clone 好的仓库：

```bash
cd tensorflow
```

项目中的 `Makefile` 能够生成包含所有源文件的独立项目，并支持导入目前成熟的嵌入式开发环境。目前支持的环境主要有 Arduino, Keil, Make 和 Mbed。

注意：我们为其中一些环境托管预建项目。参阅 [支持的平台](https://tensorflow.google.cn/lite/microcontrollers/overview#supported_platforms)。

要在 Make 中生成项目，请使用如下指令：

```bash
make -f tensorflow/lite/micro/tools/make/Makefile generate_projects
```

这需要几分钟下载并配制依赖。结束后，新的文件夹生成并包含源文件，例如 `tensorflow/lite/micro/tools/make/gen/linux_x86_64/prj/hello_world/keil` 包含了 hello world 的 Keil uVision 工程。

以下以 hello_world 工程为例，分离出与实际应用无关的 tflite_micro 源文件，用于后续 .lib 库生成。

运行本目录下的 `lib_extra.py` 生成 tflite micro 源文件包：

| 参数               | 含义                                            |
| ------------------ | ----------------------------------------------- |
| --tensorflow_path  | 下载的tensorflow仓库的根目录（绝对路径）  |
| --tflitemicro_path | 生成的tensorflow lite micro路径（绝对路径） |

脚本成功运行后打印 `--tensorflow lite micro source file extract successful--` 信息，并在对应的 `tflitemicro_path` 路径下生成 `Source` 文件夹存放 Tensorflow Lite Micro 源文件。

## 2.2 将源文件加入 KEIL 工程并生成 .lib 库

### 2.2.1 添加文件 ###
新建目标芯片的 KEIL 工程（本次示例以 ARM Cortex M4 为例），将 `Source` 目录下的 `tensorflow` 和 `third_party` 文件夹导入到 KEIL 工程根目录下，并添加 `tensorflow` 目录中除 `lite/micro/kernels` 以及 `lite/micro/tools` 文件以外的所有源文件（包含 .c 和 .cc)。如下图所示：

<div align=center>
<img src="image/lib文件目录.png" width=80% />
</div>
**注意**

在添加 `tensorflow/lite/micro/kernel` 目录下的源文件时需要区分 `reference` 算子和 Arm `CMSIS-NN`优化加速算子。s`tensorflow/lite/micro/kernel` 文件夹内容如下图中所示：

<div align=center>
<img src="image/cmsis和reference.png" width=80% />
</div>

**注：CMSIS-NN 是 Arm 在 AI 领域针对 IOT 设备开发的神经网络加速库，其目的是为了让 AI 在算力和资源有限的设备上落地，更好的发挥 Arm 的生态优势。相关代码和文档已经开源 (https://www.keil.com/pack/doc/CMSIS/NN/html/index.html) 。在 Tensorflow Lite Micro 框架下的 CMSIS-NN 算子与 reference 算子性能对比可参考[附录](./TFlite_Micro_Component_User_Guide.md#%E9%99%84%E5%BD%95cmsis-nn-%E5%AF%B9-tensorflow-lite-micro-%E7%9A%84%E8%BF%90%E7%AE%97%E6%80%A7%E8%83%BD%E4%BC%98%E5%8C%96) 。**

#### 2.2.1.1 采用 CMSIS-NN 生成 .lib 文件 ####

需要：

1. 添加 `tensorflow/lite/micro/kernel/cmsis-nn` 源文件；
2. 添加 `tensorflow/lite/micro/kernel/` 中的算子时，请不要添加 `add.cc`，`conv.cc`，`depthwise_conv.cc`，`softmax.cc`，`fully_connected.cc`，`pooling.cc`，`mul.cc` 源文件；
3. 添加 `tensorflow/lite/micro/tools` 文件夹下的源文件。

#### 2.2.1.2 采用 reference 算子生成 .lib 文件 ####

需要:

1. 添加 `tensorflow/lite/micro/kernel/` 中的全部算子；
2. 无需添加 `tensorflow/lite/micro/tools` 文件夹下的源文件。

### 2.2.2 配制编译选项 ###

采用 compiler version 6 编译器并关闭 Microlib ：

<div align=center>
<img src="image/编译器配置.png" width=80% />
</div>

选择 Create Library 选项并修改 .lib 库名为： `tensorflow_lite_micro` 

<div align=center>
<img src="image/编译目标为.lib.png" width=80% />
</div>

配置有关的宏、包含的头文件路径并设置代码优化等级：

<div align=center>
<img src="image/配置include和优化等级等.png" width=80% />
</div>

最后点击编译链接选项，即可在工程根目录的 `Objects` 文件夹下生成 ARM Cortex M4 对应的 .lib 库。其他内核型号的 tflite_micro 库以此类推。

## 附录：CMSIS-NN 对 Tensorflow Lite Micro 的运算性能优化

- 硬件平台：Necluo STM32L496ZG
- 测试输入图片：`tensorflow\lite\micro\tools\make\downloads\person_model_int8` 目录中 `person_image_data.cc` 和 `no_person_image_data.cc` 保存的 96 * 96 pixels ( uint_8 ) 灰度图。
- 单次执行和 10 次累计执行的测试结果如下：

|                      Case                      |   Disable ARM-CMSIS-NN   |   Enable ARM-CMSIS-NN   | Improvement |
| :--------------------------------------------: | :----------------------: | :------------------: | :---------: |
|          Initialize_Benchmark_Runner           |     65 ticks (65 ms)     |   66 ticks (66 ms)   |      *      |
|              Run_Single_Iteration              |  12476 ticks (12476 ms)  |  633 ticks (633 ms)  |   19.71X    |
|   Person_Detection_Ten_Ierations_With_Person   | 124769 ticks (124769 ms) | 6324 ticks (6324 ms) |   19.73X    |
| Person_Detection_Ten_Ierations_With_out_Person | 124770 ticks (124770 ms) | 6325 ticks (6325 ms) |   19.72X    |

## TencentOS-tiny AI 开发组件

TencentOS-tiny 已将 Tensorflow Lite Micro 以及 CMSIS-NN 集成到 AI 组件中，并通过其他组件与腾讯云无缝相连，打通从云到端整条链路，助力 AI 的发展与落地。随着越来越多的厂商采用 Arm Cortex M55 和 Ethos U NPU IP，相信未来端侧AI的应用会更加广阔。

TencentOS-tiny AI组件：https://github.com/rceet/tencentos-tiny-with-tflitemicro-and-iot/tree/master/components/ai/tflite_micro

Arm Cortex M55：https://www.arm.com/products/silicon-ip-cpu/cortex-m/cortex-m55

Arm Ethos U55：https://www.arm.com/products/silicon-ip-cpu/ethos/ethos-u55

Arm Ethos U65：https://www.arm.com/products/silicon-ip-cpu/ethos/ethos-u65
