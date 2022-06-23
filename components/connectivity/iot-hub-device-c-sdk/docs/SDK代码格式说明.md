<!-- TOC -->

- [一、编写目的](#%E4%B8%80%E7%BC%96%E5%86%99%E7%9B%AE%E7%9A%84)
- [二、代码格式](#%E4%BA%8C%E4%BB%A3%E7%A0%81%E6%A0%BC%E5%BC%8F)
  - [行长度](#%E8%A1%8C%E9%95%BF%E5%BA%A6)
  - [缩进](#%E7%BC%A9%E8%BF%9B)
  - [文件编码](#%E6%96%87%E4%BB%B6%E7%BC%96%E7%A0%81)
  - [分行](#%E5%88%86%E8%A1%8C)
  - [条件语句](#%E6%9D%A1%E4%BB%B6%E8%AF%AD%E5%8F%A5)
  - [循环和开关语句](#%E5%BE%AA%E7%8E%AF%E5%92%8C%E5%BC%80%E5%85%B3%E8%AF%AD%E5%8F%A5)
  - [指针](#%E6%8C%87%E9%92%88)
  - [水平留白](#%E6%B0%B4%E5%B9%B3%E7%95%99%E7%99%BD)
  - [垂直留白](#%E5%9E%82%E7%9B%B4%E7%95%99%E7%99%BD)

<!-- /TOC -->
## 一、编写目的

该文档用作提供 SDK 开发的代码格式说明，开发者提交代码前应仔细阅读。

本项目根目录提供了`.clang-format`格式文件，请优先使用`clang-format`进行代码格式统一。

如有冲突，请以`clang-format`的结果为准。

## 二、代码格式

### 行长度

最长120行

### 缩进

四个空格

### 文件编码

UTF-8 不能有BOM头。

### 分行

1.对齐式

```c
bool retval = DoSomething(averyveryveryverylongargument1,
                             argument2, argument3);
```

2.缩进式

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

### 条件语句

```c
if (condition) {
    DoSomething();  // 4 空格缩进.
}
```

### 循环和开关语句

- default不可达时，用assert
- case使用大括号非强制要求

```c
switch (var) {
    case 0: {  // 4 空格缩进
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

### 指针

```
char* ptr = NULL;
```

### 水平留白

行尾不留白

```c
void f(bool b) {  // 左大括号前总是有空格
  ...
int i = 0;  // 分号前不加空格

// 列表初始化中大括号内的空格是可选的，
// 如果加了空格，那么两边都要加上
int x[] = { 0 };
int x[] = {0};

if (b) {          // if 条件语句和循环语句关键字后均有空格
} else {          // else 前后有空格
}

while (test) {}   // 圆括号内部不紧邻空格
switch (i) {
for (int i = 0; i < 5; ++i) {

// 循环和条件语句的圆括号内可以有空格,
// 但这种情况很少，要保持一致
switch ( i ) {
if ( test ) {
for ( int i = 0; i < 5; ++i ) {

// 循环里内分号后恒有空格，分号前可以加个空格
for ( ; i < 5 ; ++i) {
switch (i) {
    case 1:         // switch case 的冒号前无空格
        ...
    case 2: break;  // 如果冒号后有代码，加个空格
  
// 赋值运算符前后总是有空格
x = 0;

// 其它二元操作符前后也恒有空格，对于表达式的子式可以不加空格
// 圆括号内部没有紧邻空格
v = w * x + y / z;
v = w*x + y/z;
v = w * (x + z);

// 在参数和一元操作符之间不加空格
x = -5;
++x;
if (x && !y)
  ...
```

### 垂直留白

看情况，根据个人习惯，增强可读性即可
