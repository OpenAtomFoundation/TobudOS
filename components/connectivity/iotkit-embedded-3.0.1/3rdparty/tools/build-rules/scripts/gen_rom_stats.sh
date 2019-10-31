#! /bin/bash

#echo "COMP_LIB = ${COMP_LIB}"
#echo "STRIP = ${STRIP}"

bash ${TOP_DIR}/extract.sh test

cd ${DIST_DIR}/eng

cp -rf wrappers/wrappers_defs.h infra/*.h ${FINAL_DIR}/include

echo "Selected functions:"
echo ""
for iter in $(ls|grep -v 'wrappers\|infra'); do
    echo " . [${iter}]"
    cp -f ${iter}/*_api.h ${FINAL_DIR}/include 2>/dev/null
    cp -f ${iter}/*_wrapper.h ${FINAL_DIR}/include 2>/dev/null
done

cd ${OLDPWD}

echo ""
printf "    | %-5s | %-35s | %14s | %26s |\n" "RATE" "OBJ NAME" "BYTES/TOTAL" "MODULE NAME"

for iter in ${ALL_SUB_DIRS}; do
    [ ! -d ${LIBOBJ_TMPDIR}/${iter} ] && continue

    cd ${LIBOBJ_TMPDIR}/${iter}

    ITER_OBJS=$(find . -name "*.o")
    [ "${ITER_OBJS}" != "" ] || continue
    ${STRIP} ${ITER_OBJS} 2>/dev/null

    ITER_SIZE=$(size ${ITER_OBJS} | ${SED} '1d' | awk '{ sum += $1 } END { print sum }')

    [ "${ITER_SIZE}" != "0" ] && \
    printf "    |-%-.5s-|-%-.35s-|-%.14s-|-%.26s-|\n" \
        "-----------------------------------------" \
        "-----------------------------------------" \
        "-----------------------------------------" \
        "-----------------------------------------"


    for j in ${ITER_OBJS}; do
        size $j | ${SED} '1d' \
            | awk -v sum="${ITER_SIZE}" \
                  -v mod="${iter}" \
                  -v obj="$(basename ${j})" \
                  '{ if ($1 != 0) printf("%d %.4s%% %s %-d/%d %s\n", $1, ($1/sum)*100, obj, $1, sum, mod); }'
    done \
         | sort -nr \
         | cut -d' ' -f2- \
         | awk '{ printf("    | %-5s | %-35s | %14s | %26s |\n", $1, $2, $3, $4); }'

    cd ${OLDPWD}
done

echo ""
echo ""
echo ""

printf "    | %-5s | %-35s | %-9s | %-9s | %-10s | %-6s |\n" \
    "RATE" "MODULE NAME" "ROM" "RAM" "BSS" "DATA"
printf "    |-%-.5s-|-%-.35s-|-%-.9s-|-%-.9s-|-%-.10s-|-%-.6s-|\n" \
    "-------------" \
    "--------------------------------------------" \
    "-------------" \
    "-------------" \
    "-------------" \
    "-------------"

cd ${LIBOBJ_TMPDIR}
TOTAL_ROM=$(find $(for i in ${COMP_LIB_COMPONENTS}; do echo "${OUTPUT_DIR}/${i}"; done) -name "*.o" 2>/dev/null \
    | xargs size \
    | awk '{ sum += $1 } END { print sum }')
cd ${OLDPWD}

for iter in ${COMP_LIB_COMPONENTS}; do
    [ ! -d ${LIBOBJ_TMPDIR}/${iter} ] && continue

    cd ${LIBOBJ_TMPDIR}/${iter}

    ITER_OBJS=$(find . -name "*.o")
    [ "${ITER_OBJS}" != "" ] || continue
    ${STRIP} ${ITER_OBJS} 2>/dev/null

    size ${ITER_OBJS} | ${SED} '1d' \
        | awk -v name=${iter} -v total_rom=${TOTAL_ROM} '
            BEGIN { sum_rom = sum_ram = sum_bss = sum_data = 0 }
            {
                sum_rom += $1 + $2;
                sum_ram += $2 + $3;
                sum_bss += $3;
                sum_data += $2;
            }
            END {
                rate = sum_rom / total_rom * 100;
                printf("%d | %.4s%% | %-35s | %-9s | %-9s | %-10s | %-6s |\n",
                    sum_rom,
                    rate,
                    name,
                    sum_rom,
                    sum_ram,
                    sum_bss,
                    sum_data);
            }
        '

    cd ${OLDPWD}
done \
    | sort -nr \
    | cut -d' ' -f2- \
    | ${SED} 's!.*!    &!g' \
    | awk -v total_rom=${TOTAL_ROM} '
        BEGIN { sum_rom = sum_ram = sum_bss = sum_data = 0 }
        {
            sum_rom += $6;
            sum_ram += $8;
            sum_bss += $10;
            sum_data += $12;
            print;
        }
        END {
            rate = sum_rom / total_rom * 100;
            printf("    |-------|-------------------------------------|-----------|-----------|------------|--------|\n");
            if (rate < 100)
                printf("    | %.4s%% | %-35s | %-9s | %-9s | %-10s | %-6s |\n", rate, "- IN TOTAL -", sum_rom, sum_ram, sum_bss, sum_data);
            else
                printf("    |  %.3s%% | %-35s | %-9s | %-9s | %-10s | %-6s |\n", rate, "- IN TOTAL -", sum_rom, sum_ram, sum_bss, sum_data);
        }
        '

echo ""
