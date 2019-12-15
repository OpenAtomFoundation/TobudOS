#! /bin/bash

# LOCK_PATTERN=bash_lock
# LOCK_FILE=${LOCK_PATTERN}.$$

# cleanup ()
# {
#     rm -f ${LOCK_FILE}
# }

# trap cleanup EXIT

# if [ "$(ls ${LOCK_PATTERN}.* 2>/dev/null)" = "" ];then
#     echo "LOCK" > ${LOCK_FILE}
# else
#     echo "Another Extract Script Is Running, Exit..."
#     exit
# fi

env_check()
{
    CHECK_OS=$(uname -r | grep 'ali')
    if [ "${OS}" = "Linux" ] && [ "${CHECK_OS}" = "" ]; then
        export PATH=tools/prebuilt/ubuntu/bin:${PATH}
    fi
    gawk --help > /dev/null 2>&1
    if [ "$?" != "0" ];then
        echo "Please install gawk, using sudo apt-get install gawk for ubuntu 16.04"
        exit
    fi
}

extract_from_cloud()
{
    OS=$(uname)
    EXTRACT_ID=$(curl --connect-timeout 5 -sF "file=@make.settings" --url https://linkkit.aliyuncs.com/upload/config?pk=a1AuWIoEr4Z\&os=${OS})
    # echo ${EXTRACT_ID}
    RETRY_COUNT=0
    if [ "${EXTRACT_ID}" != "" ];then
        echo ". Download request sent, waiting respond ..."
        sleep 2

        while :
        do
            DOWNLOAD_FILE=$(curl -s --connect-timeout 5 https://linkkit.aliyuncs.com/get/linkkit?extractId=${EXTRACT_ID})
            # echo ${DOWNLOAD_FILE}

            if [ "${DOWNLOAD_FILE}" = "404" ] || [ "${DOWNLOAD_FILE}" = "" ];then
                break

            elif [ "${DOWNLOAD_FILE}" = "406" ];then
                echo ". Respond generating, wait longer"

                if [ "${RETRY_COUNT}" = "20" ];then
                    break
                fi

                RETRY_COUNT=$[RETRY_COUNT+1]

                echo ". Retried ${RETRY_COUNT}/20"
                sleep 2

            else

                echo ""
                curl ${DOWNLOAD_FILE} > output.zip
                rm -rf output
                unzip -q output.zip
                rm -rf output.zip
                echo ""
                echo "Please pick up extracted source files in [${PWD}/${OUTPUT_DIR}]"
                echo ""
                # rm -rf ${LOCK_FILE}
                exit
            fi
        done
    fi
}

OS="$(uname)"

if [ "${OS}" = "Linux" ]; then
    FIND="find -L"
else
    FIND="find"
fi

OUTPUT_DIR=output
INFRA_DIR=${OUTPUT_DIR}/eng/infra
WRAPPERS_DIR=${OUTPUT_DIR}/eng/wrappers

XTRC_FILE_RULS=./tools/misc/xtrc_file_rules
TEMP_FILE_RULS="${PWD}/.temp_file_rule_filter"

XTRC_WRAPPER_RULS=./tools/misc/xtrc_wrapper_rules
TEMP_WRAPPER_RULS="${PWD}/.temp_wrapper_rule_filter"
WRAPPER_DOC=./tools/misc/wrapper

# Try Extract Linkkit From Cloud
#
if [ "$1" = "" ] || ( [ "$1" != "test" ] && [ "$1" != "local" ] ) then
        extract_from_cloud
fi

# environment check
env_check

# Prepare Config Macro In make.settings
MACRO_LIST=$(sed -n '/#/!{/=y/p}' make.settings | sed -n 's/=y//gp' | sed -n 's/FEATURE_//gp')

mkdir -p ${OUTPUT_DIR} ${OUTPUT_DIR}/examples
rm -rf $(${FIND} ${OUTPUT_DIR} -mindepth 1 -maxdepth 1|grep -v release)

# Generate infra_config.h and extract necessary infra files
mkdir -p ${INFRA_DIR}
echo "#ifndef _INFRA_CONFIG_H_" > ${INFRA_DIR}/infra_config.h
echo -e "#define _INFRA_CONFIG_H_\n" >> ${INFRA_DIR}/infra_config.h
echo "${MACRO_LIST}" | sed -n 's/^/#define /p' >> ${INFRA_DIR}/infra_config.h
echo -e "\n#endif" >> ${INFRA_DIR}/infra_config.h

${FIND} ./src -name "infra_types.h" | xargs -i cp -f {} ${INFRA_DIR}
${FIND} ./src -name "infra_defs.[ch]" | xargs -i cp -f {} ${INFRA_DIR}
${FIND} ./src -name "infra_list.h" | xargs -i cp -f {} ${INFRA_DIR}
${FIND} ./src -name "infra_compat.[ch]" | xargs -i cp -f {} ${INFRA_DIR}

# echo -e "${MACRO_LIST}"

cond_and_check()
{
    COND_AND_VAR=$(echo $1 | gawk -F '&' '{for(i=1;i<=NF;i++){print $i;}}')
    # echo "${COND_AND_VAR}"
    for item in ${COND_AND_VAR}
    do
        echo ${MACRO_LIST} | grep -wo ${item} > /dev/null
        if [ $? -ne 0 ];then
            return 1
        fi
    done
    return 0
}

cond_not_check()
{
    COND_NOT_VAR=$(echo $1 | gawk -F '&' '{for(i=1;i<=NF;i++){print $i;}}')
    # echo "${COND_AND_VAR}"
    for item in ${COND_NOT_VAR}
    do
        echo ${MACRO_LIST} | grep -wo ${item} > /dev/null
        if [ $? -eq 0 ];then
            return 0
        fi
    done
    return 1
}

DOTS_LINE=".................................................................."

extract_file_by()
{
    local rule="$*"

    COND_AND=$(echo $rule | gawk -F'|' '{print $1}')
    COND_NOT=$(echo $rule | gawk -F'|' '{print $2}')
    SRC_DIR=$(echo $rule | gawk -F'|' '{print $3}')
    DEST_DIR=$(echo $rule | gawk -F'|' '{print $4}')

    # echo "${COND_AND}"
    cond_and_check "${COND_AND}"
    if [ $? -ne 0 ]; then
        return 1
    fi

    # echo "${COND_NOT}"
    cond_not_check "${COND_NOT}"
    if [ $? -eq 0 ]; then
        return 1
    fi

    # echo "${SRC_DIR}"
    # echo "${DEST_DIR}"

    if [ "${DEST_DIR}" != "" ];then
        mkdir -p ${DEST_DIR} && ${FIND} ${SRC_DIR} -maxdepth 1 -name *.[ch] | xargs -i cp -rf {} ${DEST_DIR}
    fi
}

echo ""
echo "Analysing extract rules for sources and wrappers ..."
echo ""

SWITCHES=$(cat make.settings | grep -v '^#' | sed '/^$/d;s:FEATURE_::g;s:=.*::g')
SWCH_PAT="$(echo ${SWITCHES}|sed 's: :\\\|:g')"
SPEC_PAT="$(echo ${SWITCHES}|sed 's:\([_A-Z]*\) :^\1||\\\|:g')"
SPEC_PAT="${SPEC_PAT}||"

grep ${SWCH_PAT} ${XTRC_FILE_RULS} > ${TEMP_FILE_RULS}
grep ${SWCH_PAT} ${XTRC_WRAPPER_RULS} > ${TEMP_WRAPPER_RULS}

FUNC_NAME_LIST=""
HEADER_FILE_LIST=""

FUNC_NAME_LIST=$(grep "${SPEC_PAT}" ${TEMP_WRAPPER_RULS}|gawk -F '|' '{ print $3 }'|sort -u)
HEADER_FILE_LIST=$(grep "${SPEC_PAT}" ${TEMP_WRAPPER_RULS}|gawk -F '|' '{ print $4 }'|sort -u)

FUNC_PAT="$(echo ${FUNC_NAME_LIST}|sed 's: :\\\|:g')"
HDER_PAT="$(echo ${HEADER_FILE_LIST}|sed 's: :\\\|:g')"

# [ "${FUNC_PAT}" != "" ] && sed -i "/${FUNC_PAT}/d" ${TEMP_WRAPPER_RULS}
# [ "${HDER_PAT}" != "" ] && sed -i "/${HDER_PAT}/d" ${TEMP_WRAPPER_RULS}

#if [ "${FUNC_PAT}" != "" ] && [ "${HDER_PAT}" != "" ]; then
#    sed -i "/${FUNC_PAT}/{/${HDER_PAT}/d}" ${TEMP_WRAPPER_RULS}
#fi

printf "Interpret [%03d] sources rules" $(cat ${TEMP_FILE_RULS}|wc -l|sed 's/[[:space:]]//g')
printf " from [%03d] base\n" $(cat ${XTRC_FILE_RULS}|wc -l|sed 's/[[:space:]]//g')

printf "Interpret [%03d] wrapper rules" $(cat ${TEMP_WRAPPER_RULS}|wc -l|sed 's/[[:space:]]//g')
printf " from [%03d] base" $(cat ${XTRC_WRAPPER_RULS}|wc -l|sed 's/[[:space:]]//g')

echo ""

FUNC_NAME_LIST="$(echo ${FUNC_NAME_LIST}|tr ' ' '\n')\n"
HEADER_FILE_LIST="$(echo ${HEADER_FILE_LIST}|tr ' ' '\n')\n"

FUNC_NAME_LIST=""
HEADER_FILE_LIST=""

echo ""
# Read xtrc_file_rules
TOTAL_ITERATION=$(wc -l ${TEMP_FILE_RULS}|gawk '{ print $1 }')
ITER=0

while read rule
do
    ITER=$(( ${ITER} + 1 ))
    printf "\r%.40s %.2f%%" "Extract Files ${DOTS_LINE}" $(echo 100*${ITER}/${TOTAL_ITERATION}|bc -l)
    TEST=$(echo $rule | gawk -F'|' '{print NF}')
    if [ ${TEST} -ne 4 ];then
        continue
    fi

    if [ "${OS}" = "Linux" ]; then
        extract_file_by ${rule} &
    else
        extract_file_by ${rule}
    fi

done < ${TEMP_FILE_RULS}

[ "${OS}" = "Linux" ] && wait

rm -f ${TEMP_FILE_RULS}
echo -e ""

# Generate wrapper.c
mkdir -p ${WRAPPERS_DIR}
cp -f wrappers/wrappers_defs.h ${WRAPPERS_DIR}/

# Read xtrc_wrapper_rules
TOTAL_ITERATION=$(wc -l ${TEMP_WRAPPER_RULS}|gawk '{ print $1 }')
ITER=0

while read rule
do
    ITER=$(( ${ITER} + 1 ))
    printf "\r%.40s %.2f%%" "Extract HAL/Wrapper Functions ${DOTS_LINE}" $(echo 100*${ITER}/${TOTAL_ITERATION}|bc -l)
    TEST=$(echo $rule | gawk -F'|' '{print NF}')
    if [ ${TEST} -ne 4 ];then
        continue
    fi

    COND_AND=$(echo $rule | gawk -F'|' '{print $1}')
    COND_NOT=$(echo $rule | gawk -F'|' '{print $2}')
    FUNC_NAME=$(echo $rule | gawk -F'|' '{print $3}')
    HEADER_FILE=$(echo $rule | gawk -F'|' '{print $4}')

    # echo "${COND_AND}"
    cond_and_check "${COND_AND}"
    if [ $? -ne 0 ];then
        continue
    fi

    # echo "${COND_NOT}"
    cond_not_check "${COND_NOT}"
    if [ $? -eq 0 ];then
        continue
    fi

    if [ "${FUNC_NAME}" != "" ];then
        FUNC_NAME_LIST="${FUNC_NAME_LIST}""${FUNC_NAME}\n"
    fi
    if [ "${HEADER_FILE}" != "" ];then
        HEADER_FILE_LIST="${HEADER_FILE_LIST}""${HEADER_FILE}\n"
    fi
done < ${TEMP_WRAPPER_RULS}
rm -f ${TEMP_WRAPPER_RULS}

echo -e ""

FUNC_NAME_LIST=$(echo -e "${FUNC_NAME_LIST}" | sed -n '/^$/!{p}' | sort -u)
HEADER_FILE_LIST=$(echo -e "${HEADER_FILE_LIST}" | sed -n '/^$/!{p}' | sort -u)

# For Debug
if [ "${FUNC_NAME_LIST}" != "" ];then
    echo -e "\nHAL/Wrapper Function List:" && echo -e "${FUNC_NAME_LIST}" |gawk '{ printf("%03d %s\n", NR, $0); }'
fi

if [ "${HEADER_FILE_LIST}" != "" ];then
    echo -e "\nHAL/Wrapper Header File List:" && echo -e "${HEADER_FILE_LIST}" |gawk '{ printf("%03d %s\n", NR, $0); }'
fi

# Annotation For wrapper.c
sed -n  '/WRAPPER_NOTE:/{:a;N;/*\//!ba;p}' ${WRAPPER_DOC} | sed -n '1d;p' >> ${WRAPPERS_DIR}/wrapper.c

# Output Header File Into wrapper.c
echo -e "#include \"infra_types.h\"" >> ${WRAPPERS_DIR}/wrapper.c
echo -e "#include \"infra_defs.h\"" >> ${WRAPPERS_DIR}/wrapper.c
echo -e "#include \"infra_compat.h\"" >> ${WRAPPERS_DIR}/wrapper.c
echo -e "#include \"wrappers_defs.h\"" >> ${WRAPPERS_DIR}/wrapper.c
echo -e "${HEADER_FILE_LIST}" | sed -n '/.h/{s/^/#include "/p}' | sed -n 's/$/"/p' >> ${WRAPPERS_DIR}/wrapper.c
echo -e "" >> ${WRAPPERS_DIR}/wrapper.c

# Generate Default Implenmentation For HAL/Wrapper Function
echo ""
TOTAL_ITERATION=$(echo "${FUNC_NAME_LIST}"|wc -w|sed 's/[[:space:]]//g')
ITER=0

for func in $(echo "${FUNC_NAME_LIST}")
do
    ITER=$(( ${ITER} + 1 ))
    printf "\r%.40s %.2f%%" "Generate wrapper.c ${DOTS_LINE}" $(echo 100*${ITER}/${TOTAL_ITERATION}|bc -l)

    # echo ${func}
    if [ "${func}" = "" ];then
        continue
    fi

    FUNC_DEC=$(${FIND} ./src -name *wrapper.h | xargs -i cat {})
    FUNC_DEC=$(echo "${FUNC_DEC}" | sed -n '/.*'${func}'(.*/{/.*);/ba;{:c;N;/.*);/!bc};:a;p;q}')
    
    DATA_TYPE=$(echo "${FUNC_DEC}" | head -1 | gawk -F' ' '{if ($1~/^DLL/ || $1~/extern/) {if ($3~/*/) {print $2"*";} else {print $2;}} else {if ($2~/*/) {print $1"*";} else {print $1;}}}'# | sed s/[[:space:]]//g)
    # echo -e "\n${DATA_TYPE}"
    # echo -e "\n${FUNC_DEC}"

    FUNC_FILE=$(grep ${func} ./wrappers/os/ubuntu/* | gawk -F':' '{print $1}' | sed -n 's/.\/wrappers\///g;s/\//\\\//g;p' | sed -n '1,1p')
    if [ "${FUNC_FILE}" = "" ];then
        FUNC_FILE=$(grep ${func} ./wrappers/tls/* | gawk -F':' '{print $1}' | sed -n 's/.\/wrappers\///g;s/\//\\\//g;p' | sed -n '1,1p')
    fi
    # echo -e "\n${FUNC_FILE}"

    sed -n '/WRAPPER_FUNC_REFERENCE:/{:a;N;/*\//!ba;p}' ${WRAPPER_DOC} | sed -n '1d;s/FUNC_NAME/'${func}'/g;s/FUNC_FILE/'${FUNC_FILE}'/g;p' >> ${WRAPPERS_DIR}/wrapper.c
    sed -n '/'${func}':/{:a;N;/*\//!ba;p}' ${WRAPPER_DOC} | sed -n '1d;p' >> ${WRAPPERS_DIR}/wrapper.c

    if [ "${DATA_TYPE}" = "void" ];then
        echo "${FUNC_DEC}" | sed -n 's/^  *//1;/;/{s/;/\n{\n\treturn;\n}\n\n/g};p' >> ${WRAPPERS_DIR}/wrapper.c
    else
        echo "${FUNC_DEC}" | sed -n 's/^  *//1;/;/{s/;/\n{\n\treturn ('${DATA_TYPE}')1;\n}\n\n/g};p' >> ${WRAPPERS_DIR}/wrapper.c
    fi
done

if [ "${TOTAL_ITERATION}" = "0" ]; then
    echo "Only [dev_sign] enabled, so NO function requires being implemented in [${WRAPPERS_DIR}/wrapper.c]"
else
    echo ""
fi

echo -e "#ifndef _SDK_INCLUDE_H_" > ${OUTPUT_DIR}/eng/sdk_include.h
echo -e "#define _SDK_INCLUDE_H_\n" >> ${OUTPUT_DIR}/eng/sdk_include.h
echo -e "#include \"infra_types.h\"" >> ${OUTPUT_DIR}/eng/sdk_include.h
echo -e "#include \"infra_defs.h\"" >> ${OUTPUT_DIR}/eng/sdk_include.h
echo -e "#include \"infra_compat.h\"" >> ${OUTPUT_DIR}/eng/sdk_include.h
echo -e "#include \"wrappers_defs.h\"" >> ${OUTPUT_DIR}/eng/sdk_include.h
find ${OUTPUT_DIR}/eng -name "*wrapper.h" | gawk -F'/' '{print $NF}' | sed -n 's/^/#include "/g;s/$/"/gp' >> ${OUTPUT_DIR}/eng/sdk_include.h
find ${OUTPUT_DIR}/eng -name "*api.h" | gawk -F'/' '{print $NF}' | sed -n 's/^/#include "/g;s/$/"/gp' >> ${OUTPUT_DIR}/eng/sdk_include.h
echo -e "\n#endif" >> ${OUTPUT_DIR}/eng/sdk_include.h

# if echo "${SWITCHES}"|grep -qw "DEVICE_MODEL_ENABLED"; then
#     echo ""
#     bash tools/misc/compose.sh ${PWD}/output/examples/linkkit_example_auto.c
# fi

echo ""
echo "Please pick up extracted source files in [${PWD}/${OUTPUT_DIR}]"
echo ""

cp tools/misc/makefile.output output/Makefile
if [ "${1}" = "test" ];then
    ENV_TEST=$(cat .config 2>/dev/null| sed -n '/VENDOR/{s/[[:space:]]//gp}'| gawk -F ':' '{print $2}')
    if [ "${ENV_TEST}" = "ubuntu" ];then
        rm -f ${WRAPPERS_DIR}/wrapper.c
        cp -rf wrappers/os/ubuntu ${WRAPPERS_DIR}/
        cp -rf wrappers/tls ${WRAPPERS_DIR}/
        cp -rfl external_libs ${WRAPPERS_DIR}/
        rm -f ${WRAPPERS_DIR}/ubuntu/HAL_UART_linux.c
    fi
fi

# rm -rf ${LOCK_FILE}
