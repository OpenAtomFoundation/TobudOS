## 一、编写目的

该文档用作提供 SDK 开发的代码规范说明，开发者提交代码前应仔细阅读。

## 二、代码规范描述

### 2.1 头文件

#### 2.1.1 头文件保护

头文件应支持c++引用，并提供保护头，根据开源治理要求，本项目中保护头定义如下：

`IOT_HUB_DEVICE_C_SDK_<dir_path>_INC_<file_name>_`

比如 `mqtt_client.h`：
```
#ifndef IOT_HUB_DEVICE_C_SDK_SERVICES_MQTT_CLIENT_INC_MQTT_CLIENT_H_
#define IOT_HUB_DEVICE_C_SDK_SERVICES_MQTT_CLIENT_INC_MQTT_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_SERVICES_MQTT_CLIENT_INC_MQTT_CLIENT_H_
```


#### 2.1.2 内联及结构体

头文件中若声明联函数及结构体，则必须定义，即如果对外提供结构体以及内联函数，请务必放到头文件中定义。

#### 2.1.3 前置声明

尽量避免前置声明，而采用包含头文件。

#### 2.1.4 内联函数

内联函数不超过10行。

C语言使用内联函数需要注意，C99才开始支持，并且一般需要编译器开了优化选项才支持，这个跟C++不一样。

#### 2.1.5 头文件包含顺序

通常系统头文件会放到相关头文件中，以保证相关头文件的易用性。

```c
dir2/foo2.h（相关头文件）
空行
C 系统头文件
空行
其他库的 `.h`文件
本项目内 `.h`文件
```

---

### 2.2 作用域

#### 2.2.1 静态变量

在源文件中定义一个不需要被外部引用的变量时，可以将它们声明为`static`。但是不要在头文件中这么做。

#### 2.2.2 局部变量

将函数变量尽可能置于最小作用域内，并在变量声明时进行初始化。

```c
int i = 1;

while (const char* p = strchr(str, '/')) str = p + 1;
```

---

### 2.3 函数

#### 2.3.1 输出参数

1. 推荐优先使用返回值作为函数输出。函数的参数列表排序为：输入参数在前，输出参数在后。
2. 输入参数，通常是值参或者是 const 引用，纯输出参数和输入兼输出参数则是非 const 指针。
3. 参数排序时，请将所有的纯输入参数置于所有输出参数之前。

以上从易读性角度考虑，不做强制要求，但是在函数的`doxygen`注释中需要通过`@param[in]`、`@param[out]`、`@param[in,out]`列出。

#### 2.3.2 简短函数

如果一个函数超过了 40 行，则可以思考下，能否在不破坏程序结构的前提之下，对函数进行拆分。

以上从易读性角度考虑，当`review`时超过3次无法看懂，即需考虑进行拆分。

---

### 2.4 语言特性

#### 2.4.1 const

在 API 里，只要合理，就应该使用 const
* 如果函数不会修改传你入的引用或指针类型参数，该参数应声明为 const
* 按值传入的函数参数，const 对调用者没有影响，因此不建议在函数声明中使用
* 尽可能将方法声明为 const，除非它们改变了对象的逻辑状态，否则不能安全地并发调用它们。

#### 2.4.2 整形

1. 在 C 整型中, 只使用 `int`。在合适的情况下，推荐使用标准类型如 `size_t` 和 `ptrdiff_t`。大多数情况下，`int`即可覆盖。
2. `<stdint.h> `定义了 `int16_t`、`uint32_t`、`int64_t` 等整型，在需要确保整型大小时可以使用它们代替 short、unsigned long long 等。

#### 2.4.3 64位可移植性

1. 请记住，sizeof(void *) != sizeof(int)。如果需要指针大小的整数，请使用 `intptr_t`
2. 根据需要使用大括号初始化来创建 64 位常量

```c
int64_t my_value{0x123456789};
uint64_t my_mask{3ULL << 48};
```

#### 2.4.4 预处理宏

使用宏时要非常谨慎，尽量以内联函数，枚举和常量代替。往用宏展开性能关键的代码，现在可以用内联函数替代。用宏表示常量可被 const 或constexpr 变量代替。用宏 "缩写" 长变量名可被引用代替。
下面给出的用法模式可以避免使用宏带来的问题；如果你要用宏，尽可能遵守：
* 不要在 .h 文件中定义宏，常见的通用配置需要在头文件中定义，宏命名应清晰且易区分。
* 在马上要使用时才进行 #define，使用后要立即 #undef。
* 不要只是对已经存在的宏使用 #undef，选择一个不会冲突的名称。
* 不要用 ## 处理函数，类和变量的名字。

#### 2.4.5 0和NULL

整数用 0，浮点数用 0.0，指针用 NULL，字符用 '\0'。

#### 2.4.6 sizeof

尽可能用 sizeof(varname) 代替 sizeof(type)。

#### 2.4.7 别名

不要把仅用于实现的别名放到公共 API 里；公共 API 里只放用户使用的别名。

在定义公共别名时，把新名字的意图写入文档，包括：是否保证始终和当前指向的类型相同，或是否打算使用更有限的兼容性。这让用户知道：是否可以将这些类型视为可替换类型，或者是否必须遵循更特定的规则，并且帮助实现部分保留别名更改的自由度。

#### 2.4.8 动态分配

如果必须使用动态分配，那么更倾向于将所有权保持在分配者手中。如果其他地方要使用这个对象，最好传递它的拷贝，或者传递一个不用改变所有权的指针或引用。

### 2.4 命名约定

#### 2.4.1 通用命名规则

函数命名、变量命名、文件命名要有描述性，少用缩写。

#### 2.4.2 文件命名

文件名要全部小写，可以包含下划线 "`_`" 或连字符 "`-`"，依照项目的约定.如果没有约定，那么 "`_`" 更好。
不要使用已经存在于 /usr/include 下的文件名，如 db.h。

#### 2.4.3 类型命名

结构体、类型定义 (typedef)、枚举——均使用相同约定，即以大写字母开始，每个单词首字母均大写，不包含下划线。

```c
struct UrlTableProperties {
  string name;
  int num_entries;
};
// 枚举
enum UrlTableErrors { ...
```

#### 2.4.4 变量命名

变量 (包括函数参数) 和数据成员名一律小写，单词之间用下划线连接。其中全局变量中`static`修饰的以`sg`开头，非`static`修饰的以`g`开头

```c
static int sg_count = 0;
int g_count = 0;
```

#### 2.4.5 常量命名、枚举型变量以及宏

声明为const 的变量、枚举型变量或者宏，或在程序运行期间其值始终保持不变的，全部使用大写，单词用下划线`_`隔开。

```c
const MAX_COUNT = 10;
#define MAX_SIZE_OF_NUM 10
enum PinStateType {
	PIN_OFF,
	PIN_ON
};
```

#### 2.4.6 函数命名

内部函数命名均已小写形式，加下划线隔开

```c
const char *iot_ca_get(void);
```

对外API以及跨平台移植接口使用大小写混合，用下划线隔开，并以`IOT`或者`HAL`开头

```c
void *IOT_OTA_Init(const char *product_id, const char *device_name, void *ch_signal);
int HAL_AT_Uart_Send(void *data, uint32_t size)
```

---
### 2.5 注释

#### 2.5.1 注释风格

统一使用`doxegen`注释风格。

#### 2.5.2 注释示例

- 文件头：所有文件的开头应该包含以下内容：

```c
/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file mqtt_packet_deserialize.c
 * @brief implements mqtt packet deserialize. Reference paho.mqtt.embedded-c &
 * http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/mqtt-v3.1.1.pdf
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-05-21
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-05-21 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */
```

- 函数注释：参数的输入/输出必须指明

```c
/**
 * @brief Deserialize the supplied (wire) buffer into publish data. See 3.3.
 *
 * @param[in] buf the raw buffer data, of the correct length determined by the remaining length field
 * @param[in] buf_len the length in bytes of the data in the supplied buffer
 * @param[out] flags the MQTT dup, qos, retained flag
 * @param[out] packet_id returned integer - the MQTT packet identifier
 * @param[out] topic_name returned string - the MQTT topic in the publish
 * @param[out] topic_len returned integer - the length of the MQTT topic
 * @param[out] payload returned byte buffer - the MQTT publish payload
 * @param[out] payload_len returned integer - the length of the MQTT payload
 * @return @see MQTTPacketErrCode
 */
```

- 结构体注释：结构体成员必须采用`/**< xxx */`才能显示在`doxygen`中

```c
/**
 * @brief Defines the MQTT "Last Will and Testament" (LWT) settings for the connect packet.
 *
 */
typedef struct {
    const char* topic_name; /**< The LWT topic to which the LWT message will be published */
    const char* message;    /**< The LWT payload */
    uint8_t     retained;   /**< The retained flag for the LWT message */
    uint8_t     qos;        /**< The quality of service setting for the LWT message */
} MQTTPacketWillOptions;
```

- 宏注释：对于需要注释的宏，需要在之前使用以下格式才能显示在`doxygen`中

```
/**
 * @brief header 1 byte + remaining length 1~4 byte(s).
 *
 */
#define MAX_NO_OF_REMAINING_LENGTH_BYTES 4
```

---

### 2.6 格式

除了命名，格式请以`clang-format`格式化结果为准，可参考[SDK代码格式说明](./SDK代码格式说明.md)

#### 2.6.1 行长度

每一行代码最大字符数为 120。

#### 2.6.2 非ASCII字符

尽量不使用非 ASCII 字符，使用时必须使用 UTF-8 编码。
非 ASCII 字符（中文等）只能用于注释当中，含有中文注释的代码文件必须使用 UTF-8 编码保存，不能有BOM头。

#### 2.6.3 空格

只使用空格，每次缩进 4 个空格。
不要在代码中使用制表符。你应该设置编辑器将制表符转为空格。

#### 2.6.4 函数声明与定义

返回类型和函数名在同一行，参数也尽量放在同一行，如果放不下就对形参分行，分行方式与函数调用一致。
* 使用好的参数名。
* 只有在参数未被使用或者其用途非常明显时，才能省略参数名。
* 如果返回类型和函数名在一行放不下，分行。
* 如果返回类型与函数声明或定义分行了，不要缩进。
* 左圆括号总是和函数名在同一行。
* 函数名和左圆括号间永远没有空格。
* 圆括号与参数间没有空格。
* 左大括号总在最后一个参数同一行的末尾处，不另起新行。
* 右大括号总是单独位于函数最后一行，或者与左大括号同一行。
* 右圆括号和左大括号间总是有一个空格。
* 所有形参应尽可能对齐。
* 缺省缩进为 4 个空格。
* 换行后的参数保持 4 个空格的缩进。

#### 2.6.5 浮点字面量

浮点字面常量都要带小数点，小数点两边都要有数字，即使它们使用指数表示法。如果所有浮点数都采用这种熟悉的形式，可以提高可读性，不会被误认为是整数，指数符号的E/e不会被误认为十六进制数字。可以使用整数字面常量初始化浮点变量(假设变量类型可以精确地表示该整数)，但是请注意，指数表示法中的数字绝不能用整数。
```
float f = 1.0f;
float f2 = 1;   // 也可以用整数字面常量初始化浮点数
long double ld = -0.5L;
double d = 1248.0e6;
```

#### 2.6.6 函数调用

允许3种格式，在一行内写完函数调用，在圆括号里对参数分行，或者参数另起一行且缩进四格。
原则上，尽可能精简行数，比如把多个参数适当地放在同一行里。

函数调用遵循如下形式：

```c
bool retval = DoSomething(argument1, argument2, argument3);
```

如果同一行放不下，可断为多行，后面每一行都和第一个实参对齐，左圆括号后和右圆括号前不要留空格：

```c
bool retval = DoSomething(averyveryveryverylongargument1,
                          argument2, argument3);
```

参数也可以放在次行，缩进四格：

```c
if (...) {
  ...
  ...
  if (...) {
    DoSomething(
        argument1, argument2,  // 4 空格缩进
        argument3, argument4);
  }
```

除非影响到可读性，尽量把多个参数放在同一行，以减少函数调用所需的行数。有人认为把每个参数都独立成行，不仅更好读，而且方便编辑参数

#### 2.6.7 列表初始化

列表初始化格式，函数调用如何格式化，就如何格式化列表初始化。

#### 2.6.8 条件语句

建议不在圆括号内使用空格。关键字 if 和 else 另起一行。
在所有情况下，if 和左圆括号间都要有个空格。右圆括号和左大括号之间也要有个空格

```c
if (condition) {  // 好 - IF 和 { 都与空格紧邻
```

如果能增强可读性，简短的条件语句允许写在同一行。只有在语句简单并且没有使用 else 子句时允许使用：

```c
if (x == kFoo) return new Foo();
if (x == kBar) return new Bar();
```

通常，单行语句不需要使用大括号，如果用也没问题；复杂的条件或循环语句用大括号可读性会更好。也有一些项目要求 if必须总是使用大括号

```c
if (condition) {
    DoSomething();  // 4 空格缩进.
}
```

#### 2.6.9 循环和开关选择语句

switch 语句中的 case 块可以使用大括号也可以不用，取决于个人选择。如果有不满足 case 条件的枚举值，switch 应该总是包含一个 default匹配 (如果有输入值没有 case 去处理, 编译器将给出 warning)。如果default 永远执行不到，简单的使用 assert语句。

```c
switch (var) {
  case 0: {  // 2 空格缩进
    ...      // 4 空格缩进
    break;
  }
  case 1: {
    ...
    break;
  }
  default: {
    assert(false);
  }
}
```

空循环体应使用 {} 或 continue，而不是一个简单的分号。

#### 2.6.10 指针和引用表达式

句点或箭头前后不要有空格。 `(*, &)` 作为指针/地址操作符时之后不能有空格。`(*, &)`在用于声明指针变量或参数时，空格前置后置都可以。

#### 2.6.11 布尔表达式

如果一个布尔表达式超过标准行宽 `<line-length>`，断行方式要统一。比如，逻辑操作符要么都在行尾，要么都在行首。

#### 2.6.12 返回值

不要在 return 表达式里加上非必须的圆括号。

#### 2.6.13 变量及数组初始化

变量及数组初始化用 =，() 和 {} 均可。

#### 2.6.14 预处理指令

预处理指令不要缩进，从行首开始。即使预处理指令位于缩进代码块中，指令也应从行首开始。

```c
// 好 - 指令从行首开始
  if (lopsided_score) {
#if DISASTER_PENDING      // 正确 - 从行首开始
    DropEverything();
# if NOTIFY               // 非必要 - # 后跟空格
    NotifyClient();
# endif
#endif
    BackToNormal();
  }
```

#### 2.6.15 水平留白

水平留白的使用取决于代码的位置。永远不要在行尾添加没意义的留白。

#### 2.6.16 垂直留白

这不仅仅是规则而是原则问题: 不在万不得已，不要使用空行，尤其是:两个函数定义之间的空行不要超过 2 行，函数体首尾不要留空行,函数体中也不要随意添加空行。
基本原则是: 一屏显示的代码越多，程序的控制流越容易理解。当然,过于密集的代码块和过于疏松的代码块同样难看，这取决于你的判断，但通常是垂直留白越少越好。
下面的规则可以让加入的空行更有效:
* 函数体内开头或结尾的空行可读性微乎其微。
* 在多重 if-else 块里加空行或许有点可读性。
