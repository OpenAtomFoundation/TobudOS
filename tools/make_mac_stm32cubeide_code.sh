#!/bin/bash

declare mcu_map=(["1"]="stm32f1xx" ["2"]="stm32f4xx" ["3"]="stm32l4xx" ["4"]="stm32f7xx")
declare cortex_map=(["1"]="cortex-m3" ["2"]="cortex-m4" ["3"]="cortex-m4" ["4"]="cortex-m7")
mcu="stm32f1xx"
cortex="cortex-m3"
while :; do
    read -p "Choose Your MCU: [1:stm32f1xx, 2:stm32f4xx, 3:stm32l4xx, 4:stm32f7xx] " n 
    mcu=${mcu_map[$n]}
    cortex=${cortex_map[$n]}
    if [ ! -z $mcu ]; then
        echo "You choose: " $mcu
        break
    fi
done


rm -rf tiny
cp -rf TencentOS_tiny tiny
cd tiny

rm -rf .git
ls | egrep -v "arch|kernel|osal|components|devices|net|platform|test" | xargs rm -rf

find . -type d \( -name "armcc" -o -name "iccarm" \) | xargs rm -rf
cd arch/arm/arm-v7m && ls | egrep -v "$cortex|common" | xargs rm -rf && cd -

rm -rf components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/samples
rm -rf components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/sdk-tests/
rm -rf components/connectivity/TencentCloud_SDK/samples
cd components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/ && ls | egrep -v "device|mqtt|sdk-impl|system|utils" | xargs rm -rf && cd -
# radio目录下的文件夹视情况而删除
#rm -rf components/connectivity/LoraWAN/radio/sx1272/
#rm -rf components/connectivity/LoraWAN/radio/sx1276/
rm -rf components/connectivity/LoraWAN
rm -rf platform/vendor_bsp/
rm -rf platform/hal/nationz/

cd platform/hal/st/ && ls | egrep -v "$mcu" | xargs rm -rf && cd -

cd ..
