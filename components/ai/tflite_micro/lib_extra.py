#--coding=utf-8--
import os
import shutil
import argparse

parser = argparse.ArgumentParser(description="Tensorflow Lite Micro Lib gen")
parser.add_argument("--tensorflow_path", default="/home/hyliu/ARM/tensorflow", 
    type=str, help="tensorflow path(input)")
parser.add_argument("--tflitemicro_path", default='/home/hyliu/ARM/',
    type=str, help="tflitemicro path(output)")

opt = parser.parse_args()

tflu_lib_path = os.path.join(opt.tflitemicro_path, 'Source')
if not os.path.isdir(tflu_lib_path):
    os.mkdir(tflu_lib_path)

tflite_micro_hello_path = os.path.join(opt.tensorflow_path,
    "tensorflow/lite/micro/tools/make/gen/linux_x86_64/prj/hello_world/keil")

def copydirs(from_file, to_file):
    if not os.path.exists(to_file):  # 如不存在目标目录则创建
        os.makedirs(to_file)
    files = os.listdir(from_file)  # 获取文件夹中文件和目录列表
    for f in files:
        if os.path.isdir(from_file + '/' + f):  # 判断是否是文件夹
            copydirs(from_file + '/' + f, to_file + '/' + f)  # 递归调用本函数
        else:
            shutil.copy(from_file + '/' + f, to_file + '/' + f)  # 拷贝文件

copydirs(os.path.join(tflite_micro_hello_path, 'tensorflow'), os.path.join(tflu_lib_path, "tensorflow"))
copydirs(os.path.join(tflite_micro_hello_path, 'third_party'), os.path.join(tflu_lib_path, "third_party"))

cmsis = os.path.join(opt.tensorflow_path, "tensorflow/lite/micro/tools/make/downloads/cmsis/CMSIS")
tflu_lib_tensorflow = os.path.join(tflu_lib_path, 'tensorflow')
#拷贝缺失文件
shutil.copy(os.path.join(os.path.join(cmsis,"DSP/Include/arm_helium_utils.h")),
    os.path.join(tflu_lib_tensorflow,"lite/micro/tools/make/downloads/cmsis/CMSIS/DSP/Include/"))
shutil.copy(os.path.join(os.path.join(cmsis,"Core/Include/cmsis_armclang.h")),
    os.path.join(tflu_lib_tensorflow,"lite/micro/tools/make/downloads/cmsis/CMSIS/DSP/Include/"))
shutil.copy(os.path.join(os.path.join(cmsis,"Core/Include/cmsis_compiler.h")),
    os.path.join(tflu_lib_tensorflow,"lite/micro/tools/make/downloads/cmsis/CMSIS/DSP/Include/"))
shutil.copy("./LICENSE",tflu_lib_path)
#删除benchmarks和example
shutil.rmtree(os.path.join(tflu_lib_tensorflow, "lite/micro/benchmarks"))
shutil.rmtree(os.path.join(tflu_lib_tensorflow, "lite/micro/examples"))

print("---tensorflow lite micro source file extract successful---")