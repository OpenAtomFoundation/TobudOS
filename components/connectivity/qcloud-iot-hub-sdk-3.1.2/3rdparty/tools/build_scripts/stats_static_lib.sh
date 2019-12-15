#! /bin/bash

TARGET=$1
TEMPD=$(mktemp -d -t STATD-XXXX)
TEMPF=$(mktemp -t STATF-XXXX)

#echo "TARGET = ${TARGET}"
#echo "STAGED = ${STAGED}"
#echo "TEMPD = ${TEMPD}"

if [ ! -f ${TARGET} ] || [ ! -d ${STAGED} ]; then
    echo "Invalid Env"
    exit 1
fi

cp ${TARGET} ${TEMPD}
cd ${TEMPD}
ar xf $(basename ${TARGET})
rm -f $(basename ${TARGET})
${STRIP} *.o > /dev/null 2>&1

NAME=$1
for obj in $(ls *.o); do
    dir=$(find ${STAGED} -name ${obj}|xargs dirname|xargs basename)
    printf "%-12s %-32s %s\n" $(basename $1) ${obj} $(du -b ${obj}|awk '{ print $1 }')
done | sort > ${TEMPF}

MODS=$(cat ${TEMPF}|awk '{ print $1 }'|sort -u)

TOTAL=$(cat ${TEMPF}|awk '{ sum += $3 } END { print sum }')
# echo "TOTAL = ${TOTAL}"

SMODS=$( \
for mod in ${MODS}; do \
    MSIZE=$(grep "^${mod}" ${TEMPF}|awk '{ sum += $3 } END { print sum }'); \
    printf "%-8s %s\n" \
        $(awk -v a=${MSIZE} -v b=${TOTAL} 'BEGIN { printf("%.2f%%\n", a/b*100); }') \
        "${mod}"; \
done | sort -nr | awk '{ print $2 }' \
)

echo ""
for mod in ${SMODS}; do
    MSIZE=$(grep "^${mod}" ${TEMPF}|awk '{ sum += $3 } END { print sum }')
    OBJS=$(grep "^${mod}" ${TEMPF}|awk '{ print $2 }')
    for obj in ${OBJS}; do
        FSIZE=$(grep "\<${obj}\>" ${TEMPF}|awk '{ print $3 }')
        printf "     %-8s %28s | %-8s %8s %-8s\n" \
            $(awk -v a=${FSIZE} -v b=${MSIZE} 'BEGIN { printf("%.2f%%\n", a/b*100); }') \
            "${obj}" "${mod}" "${FSIZE} /" "${MSIZE}"
    done | sort -nr
    echo "    -----------------------------------------------------------------"
done

echo ""
for mod in ${MODS}; do
    MSIZE=$(grep "^${mod}" ${TEMPF}|awk '{ sum += $3 } END { print sum }')
    printf "     %-8s %-12s %16s\n" \
        $(awk -v a=${MSIZE} -v b=${TOTAL} 'BEGIN { printf("%.2f%%\n", a/b*100); }') \
        "[ ${mod} ]" "${MSIZE} Bytes"
done | sort -nr

cd "${OLDPWD}"
rm -rf ${TEMPD}
rm -f ${TEMPF}
