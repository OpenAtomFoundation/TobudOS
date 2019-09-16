#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

hdirs = [
    "tiny/osal/cmsis_os",
    "tiny/arch/arm/arm-v7m/common/include",
    "tiny/arch/arm/arm-v7m/cortex-m0+/gcc",
    "tiny/arch/arm/arm-v7m/cortex-m3/gcc",
    "tiny/arch/arm/arm-v7m/cortex-m4/gcc",
    "tiny/arch/arm/arm-v7m/cortex-m7/gcc",
    "tiny/kernel/core/include",
    "tiny/kernel/hal/include",
    "tiny/kernel/pm/include",
    "tiny/kernel/hal/include",
    "tiny/devices/bc26",
    "tiny/devices/bc35_28_95",
    "tiny/devices/esp8266",
    "tiny/devices/esp8266_tencent_firmware",
    "tiny/devices/rhf76_lora",
    "tiny/devices/sim800a",
    "tiny/devices/m6312",
    "tiny/devices/m5310a",
    "tiny/net/at/include",
    "tiny/net/lora_module_wrapper",
    "tiny/net/sal_module_wrapper",
    "tiny/net/tencent_firmware_module_wrapper",
    "tiny/components/utils/JSON/include",
    "tiny/components/security/mbedtls/3rdparty/include",
    "tiny/components/security/mbedtls/3rdparty/include/mbedtls",
    "tiny/components/security/mbedtls/wrapper/include",
    "tiny/components/connectivity/TencentCloud_SDK/ports/TencentOS_tiny/include",
    "tiny/components/fs/vfs/include",
    "tiny/components/fs/fatfs/wrapper/include",
    "tiny/components/fs/vfs/include",
    "tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/sdk-impl/exports",
    "tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/sdk-impl",
    "tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/device/include",
    "tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/mqtt/include",
    "tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/utils/farra",
    "tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/system/include",
    "tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/utils/lite",
    "tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/sdk-impl",
    "tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/utils/digest",
    "tiny/components/connectivity/Eclipse-Paho-MQTT/wrapper/include",
    "tiny/components/connectivity/Eclipse-Paho-MQTT/3rdparty/include",
    "tiny/test/include",
    #"tiny/components/connectivity/LoraWAN/boards_common",
    #"tiny/components/connectivity/LoraWAN/system",
    #"tiny/components/connectivity/LoraWAN/radio",
    #"tiny/components/connectivity/LoraWAN/mac",
    #"tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/shadow/include",
    #"tiny/components/connectivity/TencentCloud_SDK/qcloud-iot-sdk-embedded-c/src/ota/include",
]

def Usage() :
    print("Usage:\n")
    print("\t{0} <.cproject file path> <new ,cproject file path>".format(sys.argv[0]))


def MakeOptions(subprefix) :
    s = ""
    for d in hdirs :
        s = s + subprefix + '<listOptionValue builtIn="false" value="../' + d + '"/>\n'

    return s

def main() :
    if len(sys.argv) != 3 :
        Usage()
        return

    cprj     = sys.argv[1]
    new_cprj = sys.argv[2]

    fd = open(cprj)
    lines = fd.readlines()
    fd.close()

    outlines = []
    for line in lines :
        tabcnt = line.count('\t')
        prefix = '\t'*tabcnt
        subprefix = '\t'*(tabcnt+1)

        if 'tiny' in line :
            continue

        if 'MQTT_COMM_ENABLED' in line or 'MBEDTLS_CONFIG_FILE' in line :
            continue

        if "</sourceEntries>" in line :
            outlines.append(subprefix+'<entry flags="VALUE_WORKSPACE_PATH" kind="sourcePath" name="tiny"/>'+'\n')
            outlines.append(line)
            continue

        if '"definedSymbols">' in line :
            outlines.append(line)
            outlines.append(subprefix+'<listOptionValue builtIn="false" value="MQTT_COMM_ENABLED=1"/>'+'\n')
            outlines.append(subprefix+'<listOptionValue builtIn="false" value="MBEDTLS_CONFIG_FILE=&lt;iothub_tls_config.h&gt;"/>'+'\n')
            continue


        if "includePath" not in line :
            outlines.append(line)
            continue

        if '"includePath">' in line :
            outlines.append(line)
            outlines.append(MakeOptions(subprefix))
        else :
            outlines.append(line.replace('"includePath"/>', '"includePath">').replace('IS_VALUE_EMPTY="true"', 'IS_VALUE_EMPTY="false"'))
            outlines.append(MakeOptions(subprefix))
            outlines.append(prefix + '</option>\n')

            

    fd = open(new_cprj, "w+")
    for line in outlines :
        fd.write(line)
    fd.close()



if __name__ == "__main__" :
    main()

