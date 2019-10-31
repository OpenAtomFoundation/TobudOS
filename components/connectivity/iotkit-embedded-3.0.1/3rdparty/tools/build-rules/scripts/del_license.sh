#! /bin/bash

START_LINE=$(head -3 $1|grep -n '^ *\/\*'|cut -d':' -f1)
END_LINE=$(grep -m 1 -n '^ *\*\/' $1|cut -d':' -f1)
echo "[${START_LINE},${END_LINE}]"

if [ "${START_LINE}" != "" ] && [ "${END_LINE}" != "" ]; then
    sed -i "${START_LINE},${END_LINE}d" $1
    echo "LICENSE REMOVED: $1"
fi
