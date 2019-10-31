#! /bin/bash

TARGET_FILE=${OUTPUT_DIR}/.one_makefile
rm -f ${TARGET_FILE}

CONFIG_VERNDOR=$(grep -m 1 "VENDOR *:" .config|awk '{ print $NF }')
EXT_IFLAGS=$( \
for iter in \
    $(find -L \
        tools/build-rules/misc \
        include \
        tests \
        src \
        ${IMPORT_DIR}/${CONFIG_VENDOR}/include \
            -type d -not -path "*.git*" -not -path "*.O*" 2>/dev/null); do \
                echo "    -I${iter} \\"; \
    done)
IFLAGS=$( \
for iter in \
    $(find -L \
        ${COMP_LIB_COMPONENTS} \
        ${EXPORT_DIR} \
        ${SUBDIRS} \
        ${EXTRA_INCLUDE_DIRS} \
        ${IMPORT_DIR}/${CONFIG_VENDOR}/include \
            -type d -not -path "*.git*" -not -path "*.O*" 2>/dev/null); do \
                echo "    -I${iter} \\"; \
    done)

if [ "${WITH_LCOV}" = "1" ]; then
    CFLAGS=$( \
    echo "${CFLAGS}" \
        | xargs -n 1 \
        | awk '{ printf("    %s \\\n", $0); }' \
    )
else
    CFLAGS=$( \
    echo "${CFLAGS}" \
        | xargs -n 1 \
        | grep -v '\-\-coverage' \
        | awk '{ printf("    %s \\\n", $0); }' \
    )
fi

ETC_OBJS=$(
for i in ${ALL_LIBS}; do
    j=$(grep "${i}$" ${STAMP_BLD_VAR} | cut -d' ' -f1 | sed 's|LIBA_TARGET_|LIB_OBJS_|g')
    k=$(grep "${j}" ${STAMP_BLD_VAR} | cut -d' ' -f3-)
    for l in ${k}; do
        echo "${j//LIB_OBJS_/}/${l}"
    done
done)
ALL_LIBS=$(for iter in ${ALL_LIBS}; do echo -n "${OUTPUT_DIR}/usr/lib/${iter} "; done)
ALL_BINS=$(for iter in ${ALL_PROG}; do echo -n "${OUTPUT_DIR}/usr/bin/${iter} "; done)
OUTPUT_D=$(basename ${OUTPUT_DIR})

if [ "$(echo ${CFLAGS}|grep -w '\-D_PLATFORM_IS_LINUX_')" = "" ]; then
    ALL_BINS=""
fi

cat << EOB >> ${TARGET_FILE}
include ${RULE_DIR}/funcs.mk

SHELL   := bash
Q       ?= @
VPATH   := $(for iter in ${COMP_LIB_COMPONENTS}; do echo -n "${OUTPUT_DIR}/${iter} "; done)

EXT_IFLAGS  := \\
${EXT_IFLAGS}

IFLAGS  := \\
${IFLAGS}

CFLAGS  := \\
    ${EXTRA_CFLAGS} \\
${CFLAGS}

STAMP_LCOV  := ${STAMP_LCOV}

.PHONY: all
all: ${OUTPUT_DIR}/usr/lib/${COMP_LIB} ${ALL_LIBS} ${ALL_BINS}
	\$(Q)cp -rf ${EXTRA_INSTALL_HDRS} ${OUTPUT_DIR}/usr/include 2>/dev/null || true
	@rm -f *.gcda *.gcno \$\$(find ${RULE_DIR} -name "*.o")

	@if [ "\$(WITH_LCOV)" = "1" ]; then \\
	    mkdir -p \$\$(dirname \$(STAMP_LCOV)); \\
	    touch \$(STAMP_LCOV); \\
	else \\
	    rm -f \$(STAMP_LCOV); \\
	fi

ifneq (,\$(findstring gcc,\$(CC)))
$(for iter in ${COMP_LIB_OBJS}; do
    echo "sinclude ${OUTPUT_DIR}/${iter/.o/.d}"
done
)
endif

${OUTPUT_DIR}/usr/lib/${COMP_LIB}: \\
$(for iter in ${COMP_LIB_OBJS}; do
    echo "    ${OUTPUT_DIR}/${iter} \\"
done
)

	\$(Q)mkdir -p \$\$(dirname \$@)
	\$(Q)\$(call Brief_Log,"AR",\$\$(basename \$@),"...")
	\$(Q)${AR} -rcs \$@ \$^ 2>/dev/null

%.o:
	\$(Q)\$(call Brief_Log,"CC",\$\$(basename \$@),"...")
	\$(Q)mkdir -p \$\$(dirname \$@)
	\$(Q)S=\$\$(echo \$@|sed 's,${OUTPUT_DIR},${TOP_DIR},1'); \\
    if echo \$\${S//.o/.c} | grep -q 'mbedtls\|HAL_\|hal-impl'; then \\
        ${CC} -c \\
            -o \$@ \\
            \$(filter-out -ansi,\$(CFLAGS)) \\
            \$(IFLAGS) \\
            \$\${S//.o/.c}; \\
    else \\
        ${CC} -c \\
            -o \$@ \\
            \$(CFLAGS) \\
            \$(IFLAGS) \\
            \$\${S//.o/.c}; \\
    fi

ifneq (,\$(findstring gcc,\$(CC)))
%.d:
	@\\
( \\
	D=\$\$(dirname \$@|sed 's,${OUTPUT_DIR},${TOP_DIR},1'); \\
	F=\$\$(basename \$@); \\
	F=\$\${F/.d/.c}; \\
	mkdir -p \$\$(dirname \$@); \\
	${CC} -MM -I\$(CURDIR) \\
	    \$(IFLAGS) \\
	    \$(filter-out -ansi,\$(CFLAGS)) \\
	\$\${D}/\$\${F} > \$@.\$\$\$\$; \\
	sed -i 's!\$(shell basename \$*)\.o[ :]!\$*.o:!1' \$@.\$\$\$\$; \\
	mv \$@.\$\$\$\$ \$@; \\
)
endif

EOB

for i in ${ALL_LIBS}; do
    n=$(basename ${i})
    j=$(grep "${n}$" ${STAMP_BLD_VAR}|cut -d' ' -f1|sed 's:LIBA_TARGET_::1')
    k=$(echo 'LIB_OBJS_'"${j}")
    k=$(grep -m 1 "^${k}" ${STAMP_BLD_VAR}|cut -d' ' -f3-)
    k=$(for l in ${k}; do echo -n "${OUTPUT_DIR}/${j}/${l} "; done)

    for m in ${k}; do
        echo "sinclude ${m/.o/.d}" >> ${TARGET_FILE}
    done
    echo "" >> ${TARGET_FILE}

    cat << EOB >> ${TARGET_FILE}
${OUTPUT_DIR}/usr/lib/${n}: \\
$(for m in ${k}; do
    echo "    ${m} \\";
done)

	\$(Q)mkdir -p \$\$(dirname \$@)
	\$(Q)\$(call Brief_Log,"AR",\$\$(basename \$@),"...")
	\$(Q)${AR} -rcs \$@ \$^ 2>/dev/null

EOB
done

for i in ${ALL_PROG}; do
    j=$(grep -w -m 1 "^SRCS_${i}" ${STAMP_BLD_VAR}|cut -d' ' -f3-)
    k=$(grep -w -m 1 "TARGET_.* = .*${i}" ${STAMP_BLD_VAR}|cut -d' ' -f1|sed 's:TARGET_::1')
    q=${k}
    if [ "$(grep -w -m 1 "^TARGET_${k}" ${STAMP_BLD_VAR}|cut -d' ' -f3-|awk '{ print NF }')" = "1" ]; then
        k=""
    fi
    LFLAGS=$(grep -w -m 1 "^LDFLAGS_${q}" ${STAMP_BLD_VAR}|cut -d' ' -f3-)
    if [ "${CC}" = "gcc" ]; then
        if [ "$(uname)" != "Darwin" ]; then
            LFLAGS="${LFLAGS} -lgcov"
        fi
    fi
    j=$(for n in ${j}; do p=$(echo ${n}|cut -c1); [ "${p}" = "/" ] && echo -n "${n}" || echo -n "${TOP_DIR}/${q}/${n} "; done)

    EXTRA_SRCS=$(grep -w -m 1 "^EXTRA_SRCS_${q}" ${STAMP_BLD_VAR}|cut -d' ' -f3-)

    cat << EOB >> ${TARGET_FILE}
${OUTPUT_DIR}/usr/bin/${i}: \\
    ${EXTRA_SRCS} \\
$(for m in ${j} ${OUTPUT_DIR}/usr/lib/${COMP_LIB} ${ALL_LIBS}; do
    echo "    ${m} \\"|sed 's!//*!/!g';
done)

	\$(Q)\$(call Brief_Log,"LD",\$\$(basename \$@),"...")
	\$(Q)${CC} \\
        -o \$@ \\
        $([ "$i" != "sdk-testsuites" ] && echo "\$(IFLAGS)" || echo "\$(EXT_IFLAGS)") \\
        \$(filter-out -Wdeclaration-after-statement -ansi,\$(CFLAGS)) \\
        \$(filter-out %.a,\$^) \\
        $( if [ "${i}" = "sdk-testsuites" ] && uname -a|grep -qw Ubuntu; then echo "${TOP_DIR}/${IMPORT_VDRDIR}/${PREBUILT_LIBDIR}/libcurl.a"; fi ) \\
        -L${OUTPUT_DIR}/usr/lib \\
        ${LFLAGS} $( if [ "${i}" = "sdk-testsuites" ] && ! uname -a|grep -qw Ubuntu; then echo "-lcurl"; fi )

EOB
done

