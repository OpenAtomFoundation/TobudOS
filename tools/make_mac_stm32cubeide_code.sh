#!/bin/bash

# 主目录
# Main directory
TOP_FOLDER=$(cd `dirname $0`/../../; pwd)
# Git源码文件夹
# Git source directory
TENCENTOS_FOLDER="TencentOS-tiny"
# 项目文件夹
# Project directory
PROJECT_FOLDER="tiny"


# 如果 TOP_FOLDER 变量为空 则会发生不可预料的风险
# 必须保证该变量有值
# Unexpected risks occur if the TOP_FOLDER variable is empty
# Must ensure that the variable has a value
if [ ! -n "$TOP_FOLDER" ]; then
    echo "TOP_FOLDER is Empty!"
    exit 0
fi

# 如果 $TOP_FOLDER"/"$TENCENTOS_FOLDER 不存在 则会发生不可预料的风险
# 必须保证拥有该文件夹
# Unexpected risk occurs if $TOP_FOLDER"/"$TENCENTOS_FOLDER does not exist
# Must ensure that you have this folder
if [ ! -d $TOP_FOLDER"/"$TENCENTOS_FOLDER ]; then
    echo "TENCENTOS_FOLDER non-existent!"
    exit 0
fi

# 选择主控型号
# Select the MCU model
declare MCU_MAP=(
    ["1"]="stm32f1xx"
    ["2"]="stm32f4xx"
    ["3"]="stm32l4xx"
    ["4"]="stm32f7xx"
    )
declare CORTEX_MAP=(
    ["1"]="cortex-m3"
    ["2"]="cortex-m4"
    ["3"]="cortex-m4"
    ["4"]="cortex-m7"
    )
# TODO:需要进一步优化 当前只适配了 arm-v7m
# TODO:Need to further optimize the current only fit arm-v7m
declare ARCH_MAP=(
    ["1"]="arm-v7m"
    ["2"]="arm-v7m"
    ["3"]="arm-v7m"
    ["4"]="arm-v7m"
    )

MCU="stm32f4xx"
CORTEX="cortex-m4"
ARCH="arm-v7m"
while :; do
    index=1
    for i in ${MCU_MAP[@]}
    do
        echo $index:${i}
        let index++
    done
    echo "Choose Your MCU Number: "
    read n
    MCU=${MCU_MAP[$n]}
    CORTEX=${CORTEX_MAP[$n]}
    ARCH=${ARCH_MAP[$n]}
    if [ ! -z $MCU ]; then
        echo "You Choose: " $MCU
        break
    fi
done

# 创建项目文件夹
# Create project directory
if [ -d "$TOP_FOLDER/$PROJECT_FOLDER" ]; then
    read -p "Has project folder:"$TOP_FOLDER"/"$PROJECT_FOLDER", delete it? [yes/no] " choose
    if [ "$choose" = "yes" ]; then
        echo "Delete "$TOP_FOLDER"/"$PROJECT_FOLDER
        rm -rf $TOP_FOLDER"/"$PROJECT_FOLDER
        mkdir $TOP_FOLDER"/"$PROJECT_FOLDER
    else
        echo "Keep "$TOP_FOLDER"/"$PROJECT_FOLDER
    fi
else
    mkdir $TOP_FOLDER"/"$PROJECT_FOLDER
fi

# 将必要的文件拷贝到项目文件夹中
# Copy the necessary files into the project folder
REQUIRE_FOLDER=( "arch" "kernel" "osal" )
for f in ${REQUIRE_FOLDER[@]}
do
    cp -R $TOP_FOLDER"/"$TENCENTOS_FOLDER"/"$f $TOP_FOLDER"/"$PROJECT_FOLDER
done

# 删除其他IDE的工程文件
# Delete project files for other IDEs
find $TOP_FOLDER"/"$PROJECT_FOLDER -type d \( -name "armcc" -o -name "iccarm" \) | xargs rm -rf

# 删除其他架构的文件
# TODO:需要进一步优化 当前只适配了 arm-v7m
# Delete files for other Arch
# TODO:Need to further optimize the current only fit arm-v7m
ls $TOP_FOLDER"/"$PROJECT_FOLDER"/arch/arm/$ARCH" | sed "s:^:$TOP_FOLDER/$PROJECT_FOLDER/arch/arm/$ARCH/:" | egrep -v "$CORTEX|common" | xargs rm -rf
ls $TOP_FOLDER"/"$PROJECT_FOLDER"/arch/arm" | sed "s:^:$TOP_FOLDER/$PROJECT_FOLDER/arch/arm/:" | egrep -v "$ARCH" | xargs rm -rf
# risc-v 架构将完全删除
# The risc-v directory will be completely removed
rm -rf $TOP_FOLDER"/"$PROJECT_FOLDER"/arch/risc-v"
