#! /bin/bash
TARGET_FILE=$1
SRC_LIST=$(for i in ${LIB_SRCS}; do
    echo ${i}|${SED} "s:${TOP_DIR}:    \${PROJECT_SOURCE_DIR}:g"
done)
DIR_LIST=$(for i in ${LIB_SRCS}; do
    dirname ${i}|sed "s:${TOP_DIR}/*::g"
done|sort -u)
MOD_NAME=$(echo ${MODULE_NAME}|sed 's:/:_:g')

rm -f ${TARGET_FILE}


cat << EOB >> ${TARGET_FILE}
$(for i in ${INTERNAL_INCLUDES} ${EXTERNAL_INCLUDES}; do
    if grep -q "$(basename ${OUTPUT_DIR})/usr/include" <<< "${i}"; then
        continue;
    fi
    echo $i \
        | ${SED} "s:-I${TOP_DIR}\(.*\):INCLUDE_DIRECTORIES (\${PROJECT_SOURCE_DIR}\1):g"
done)

EOB

if echo ${CMAKE_EXPORT_LIBS} | grep -qw ${MODULE_NAME}; then
    TYPE="SHARED"
else
    TYPE="OBJECT"
fi

if [ "${LIBA_TARGET}" != "" ]; then
    LNAME=${LIBA_TARGET#lib}
    LNAME=${LNAME%.a}

    cat << EOB >> ${TARGET_FILE}
FILE (GLOB ${MOD_NAME}_SRCS
$(for i in ${DIR_LIST}; do echo "    \${PROJECT_SOURCE_DIR}/${i}/*.c"; done)
)

ADD_LIBRARY (${LNAME} ${TYPE}
    \${${MOD_NAME}_SRCS}
    \${EXTRA_C_SOURCES}
)

EOB
fi

#
# We do NOT compose ADD_TARGET and TARGET_LINK_LIBRARY segment for those
#
if grep -qw "${MODULE_NAME}" <<< "${NOEXEC_CMAKE_DIRS}"; then
    exit 0
fi

TARGET_COUNT=$(echo "${TARGET}" | awk '{ print NF }')

if (( TARGET_COUNT == 1 )); then
    if grep -qw ${TARGET} <<< ${WIN32_CMAKE_SKIP}; then
        echo "IF (NOT WIN32)" >> ${TARGET_FILE}
    fi
    cat << EOB >> ${TARGET_FILE}
ADD_EXECUTABLE (${TARGET}
$(for i in ${SRCS}; do
    echo ${i} | ${SED} "s:${TOP_DIR}:    \${PROJECT_SOURCE_DIR}:g"
done)
)

$(for i in \
    $(echo ${LDFLAGS} | grep -o '\-l[^ ]*' | sort -u | sed 's:^-l::g'); do
        if [ "${i}" = "pthread" -o "${i}" = "rt" ]; then echo "IF (NOT MSVC)"; fi
        echo "TARGET_LINK_LIBRARIES (${TARGET} ${i})"
        if [ "${i}" = "pthread" -o "${i}" = "rt" ]; then echo "ENDIF (NOT MSVC)"; fi
done)
EOB
    if grep -qw ${TARGET} <<< ${WIN32_CMAKE_SKIP}; then
        echo "ENDIF (NOT WIN32)" >> ${TARGET_FILE}
    fi
    echo "" >> ${TARGET_FILE}
fi

if (( TARGET_COUNT > 1 )); then
    cat << EOB >> ${TARGET_FILE}
$(for i in ${TARGET}; do
    if echo ${WIN32_CMAKE_SKIP} | grep -qw ${i}; then
        echo "IF (NOT WIN32)"
    fi
    echo "ADD_EXECUTABLE (${i}"

    j=${i//-/_}
    k=$(eval echo '${''SRCS_'"${j}"'}')

    for v in ${k}; do
        echo "    ${v}"
    done
    echo ")"
    if echo ${WIN32_CMAKE_SKIP} | grep -qw ${i}; then
        echo "ENDIF (NOT WIN32)"
    fi
done)

$(for i in ${TARGET}; do
    if echo ${WIN32_CMAKE_SKIP} | grep -qw ${i}; then
        echo "IF (NOT WIN32)"
    fi
    echo "TARGET_LINK_LIBRARIES (${i} ${COMP_LIB_NAME})"
    for j in $(echo ${LDFLAGS} | grep -o '\-l[^ ]*' | sort -u | sed 's:^-l::g' | grep -vw ${COMP_LIB_NAME}); do
        if [ "${j}" = "pthread" -o "${j}" = "rt" ]; then echo "IF (NOT MSVC)"; fi
        echo "TARGET_LINK_LIBRARIES (${i} ${j})"
        if [ "${j}" = "pthread" -o "${j}" = "rt" ]; then echo "ENDIF (NOT MSVC)"; fi
    done
    if echo ${WIN32_CMAKE_SKIP} | grep -qw ${i}; then
        echo "ENDIF (NOT WIN32)"
    fi
    echo ""
done)

SET (EXECUTABLE_OUTPUT_PATH ../out)
EOB

fi
