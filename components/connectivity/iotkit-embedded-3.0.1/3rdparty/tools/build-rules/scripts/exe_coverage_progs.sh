#! /bin/bash
set -e

if [ "${OUTPUT_DIR}" = "" ] || [ ! -d ${OUTPUT_DIR} ]; then exit 1; fi

cd ${OUTPUT_DIR}/usr/bin

echo ""

iter=0
while (( iter < ${#PROGS[@]} )); do
    UT_CMD=$(eval echo '${PROGS['${iter}']}')
    UT_PROG=$(echo ${UT_CMD}|cut -d' ' -f1)

    echo "[${iter}] RUNNING '${UT_CMD}' with '${UT_PROG}'"|grep --color ".*"
    echo ""

    if [ -f ${UT_PROG} ]; then
        ${UT_CMD}
    else
        echo "${UT_CMD} SPECIFIED BUT ${UT_PROG} NOT FOUND"|grep --color ".*"
    fi

    iter=$(( iter + 1 ))
done
