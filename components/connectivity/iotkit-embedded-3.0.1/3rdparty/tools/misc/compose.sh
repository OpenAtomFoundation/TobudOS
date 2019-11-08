#! /bin/bash

alias_map()
{
    VAR_ALIAS="$1"
    VAR_NAME="$2"
    VAR_VAL="$(eval echo '${'${VAR_NAME}'}')"

    eval "${VAR_ALIAS}=${VAR_VAL}"
    VARS_LIST="${VARS_LIST} ${VAR_ALIAS}"
}

SELF_DIR=$(cd "$(dirname "$0")";pwd)
JPARSER=${SELF_DIR}/JSON.sh
MODEL_FL="${SELF_DIR}/../../model.json"
SRC_FL="${SELF_DIR}/linkkit_example_auto.c"
DST_FL="$1"
VARS_FL="${PWD}/.temp_dm_auto_cache"
VARS_LIST=""

if [ -f ${MODEL_FL} ]; then
    echo "Processing [${MODEL_FL}] ..."
fi

${JPARSER} -c < ${MODEL_FL} > ${VARS_FL}
source ${VARS_FL}

alias_map "DEVICE_PK"        "profile__productKey"
alias_map "DM_PROP_ID"       "properties__0__identifier"
alias_map "DM_EVT_ID"        "events__1__identifier"
alias_map "DM_EVT_OU_KEY"    "events__1__outputData__0__identifier"

mkdir -p $(dirname ${DST_FL})
cp ${SRC_FL} ${DST_FL}
echo "Recognised Variables:"
echo ""
for iter in ${VARS_LIST}; do
    iter_val=$(eval echo '$'${iter})
    printf "%16s : %-32s\n" ${iter} ${iter_val}
    sed -i "s: ###${iter}### :${iter_val}:g" ${DST_FL}
done
echo ""

rm -f ${VARS_FL}
