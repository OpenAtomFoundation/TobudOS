#! /bin/bash

TEXT=(
    "Copyright (c) 2014-2016 Alibaba Group. All rights reserved."
    "License-Identifier: Apache-2.0"
    ""
    "Licensed under the Apache License, Version 2.0 (the \"License\"); you may"
    "not use this file except in compliance with the License."
    "You may obtain a copy of the License at"
    ""
    "http://www.apache.org/licenses/LICENSE-2.0"
    ""
    "Unless required by applicable law or agreed to in writing, software"
    "distributed under the License is distributed on an \"AS IS\" BASIS, WITHOUT"
    "WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied."
    "See the License for the specific language governing permissions and"
    "limitations under the License."
    ""
)

TOTAL_LINE=${#TEXT[@]}
i=$(( TOTAL_LINE - 1 ))

sed -i "1i\ */" $1
while (( i >= 0 )); do
    j=$(eval echo '${TEXT['$i']}')

    if [ "$j" = "" ]; then
        sed -i "1i\ *" $1
    else
        if [ "$(echo $j|awk '{ print NF }')" = "1" ] && [ "$(echo $j | cut -c1-7)" = "http://" ]; then
            sed -i "1i\ *    $j" $1
        else
            sed -i "1i\ * $j" $1
        fi
    fi

    i=$(( i - 1 ))
done
sed -i "1i\/*" $1
