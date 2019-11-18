#! /bin/bash
if [ "$(uname)" = "Darwin" ]; then
    SED=gsed
else
    SED=sed
fi

function Trace()
{
    if [ "${VERBOSE_PRE_BLD}" != "" ]; then
        echo "$1" 1>&2
    fi
}

function Update_Sources()
{
    if [ -f ${BLD_DIR}/${STAMP_UNPACK} ]; then
        Trace "Skipped @ ${BLD_DIR}/${STAMP_UNPACK}"
        return 0
    fi
    if [ "${PKG_SWITCH}" = "" ]; then
        Trace "Warning @ CONFIG_${MODULE} = '${PKG_SWITCH}'"
    fi

    Trace "MODULE_NAME:     [${MODULE}]"
    Trace "SRC_DIR:         [${SRC_DIR}]"
    Trace "BLD_DIR:         [${BLD_DIR}]"
    Trace "PACKAGE_DIR:     [${PACKAGE_DIR}]"
    Trace "PKG_SOURCE:      [${PKG_SOURCE}]"
    Trace "PKG_BRANCH:      [${PKG_BRANCH}]"
    Trace "PKG_REVISION:    [${PKG_REVISION}]"
    Trace "PKG_UPSTREAM:    [${PKG_UPSTREAM}]"

    for i in $(find -L ${MODULE} -type d -not -path "*.git*"|${SED} "s,${MODULE}/*,,1"|${SED} '/^$/d'); do
        mkdir -p ${BLD_DIR}/${i}
    done

    if [ "${PKG_SOURCE}" != "" ] && [ -d ${PACKAGE_DIR}/${PKG_SOURCE} ]; then
        GIT_BASENAME=$(basename ${PKG_SOURCE})
        GIT_NAME_LEN=$(echo -n "${GIT_BASENAME}"|wc -c|awk '{ print $1 }')
        GIT_OFFSET=$(( GIT_NAME_LEN - 3 ))
        GIT_MOD_NAME=$(echo "${GIT_BASENAME}" | cut -c"1-$(( GIT_OFFSET-1 ))"|awk '{ print $1 }')
        GIT_POSTFIX=$(echo "${GIT_BASENAME}" | cut -c"${GIT_OFFSET}-${GIT_NAME_LEN}"|awk '{ print $1 }')
        Trace "GIT_POSTFIX:     [${GIT_POSTFIX}]"
        Trace "GIT_MOD_NAME:    [${GIT_MOD_NAME}]"
    fi
    set +x

    if [ "${GIT_POSTFIX}" = ".git" ]; then
        GIT_COPY=${MODULE}/${GIT_MOD_NAME}
        DEV_COPY_BASE=${TOP_DIR}.pkgs
        Trace "GIT_COPY:        [${GIT_COPY}]"
        Trace "DEV_COPY_BASE:   [${DEV_COPY_BASE}]"

        eval "${CMDSET_X}"
        mkdir -p ${DEV_COPY_BASE}
        cd ${DEV_COPY_BASE}
        if [ ! -d ${GIT_MOD_NAME} ]; then
            git clone ${OPTION_Q} ${PACKAGE_DIR}/${PKG_SOURCE} ${GIT_MOD_NAME} \
                $([ "${PKG_BRANCH}" = "" ] || echo "-b ${PKG_BRANCH}")
            if [ "${PKG_UPSTREAM}" != "" ]; then
                cd ${GIT_MOD_NAME};
                git remote add upstream ${PKG_UPSTREAM}
                cd ${OLDPWD};
            fi
        fi

        if [ "${PKG_REVISION}" != "" ]; then
            cd ${GIT_MOD_NAME} && git reset ${OPTION_Q} --hard ${PKG_REVISION} && cd ${OLDPWD}
        else
            cd ${GIT_MOD_NAME}
            git pull ${OPTION_Q} origin ${PKG_BRANCH} || exit 1
        fi

        cd ${TOP_DIR}
        rm -fr ${SRC_DIR}/${GIT_MOD_NAME}
        ln -sf ${DEV_COPY_BASE}/${GIT_MOD_NAME} ${SRC_DIR}/${GIT_MOD_NAME}
        touch ${BLD_DIR}/${STAMP_UNPACK}
        PKG_SOURCE=""
        set +x

        SRC_ELEM=$(ls ${SRC_DIR}|grep -v ${HD_MAKEFILE}|grep -v ${MAKE_SEGMENT})
        BLD_ELEM=$(ls ${BLD_DIR}|grep -v ${HD_MAKEFILE}|grep -v ${MAKE_SEGMENT})
        if [ "${BLD_ELEM}" = "" ]; then
            for iter in ${SRC_ELEM}; do
                Trace "Bulk [${SRC_DIR}/${iter} => ${BLD_DIR}"
#                cp -Lrf ${SRC_DIR}/${iter} ${BLD_DIR}
            done
            return 0
        fi
    fi

#    for FILE in \
#        $(find -L ${SRC_DIR}/ -type f -o -type l -name "*.[ch]" -o -name "*.mk" -o -name "*.cpp") \
#        $(find ${SRC_DIR}/ -maxdepth 1 -name "*.patch" -o -name "lib*.a" -o -name "lib*.so") \
#        $([ "" != "${PKG_SOURCE}" ] && [ -d ${PACKAGE_DIR}/${PKG_SOURCE} ] && find ${PACKAGE_DIR}/${PKG_SOURCE}/ -type f -o -type l) \
#    ; \
#    do
#        if  [ "" != "${PKG_SOURCE}" ] && \
#            [ -d ${PKG_SOURCE} ] && \
#            [ "$(dirname ${FILE})" != "${TOP_DIR}/${MODULE}" ]; then
#            SUBD=$(echo $(dirname ${FILE})|${SED} "s:$(dirname ${PKG_SOURCE})::")
#            SUBD=$(echo ${SUBD}|${SED} "s:${SRC_DIR}::")
#        else
#            SUBD=$(echo $(dirname ${FILE})|${SED} "s:${SRC_DIR}::")
#            SUBD=$(echo ${SUBD}|${SED} "s:${PACKAGE_DIR}/*::")
#        fi
#
#        COPY_DIR=${OUTPUT_DIR}/${MODULE}/${SUBD}
#        mkdir -p ${COPY_DIR}
#        COPY_BASE=$(basename ${FILE})
#        FILE_COPY=${COPY_DIR}/${COPY_BASE}
#        # grep -q "\.git" <<< ${FILE} || Trace "Check: ${FILE}: ${FILE_COPY}"
#
#        if [ ! -e ${FILE_COPY} -o \
#             ${FILE} -nt ${FILE_COPY} ]; then
#             mkdir -p ${BLD_DIR}/${FILE_DIR}
#             cp -f ${FILE} ${FILE_COPY}
#        fi
#    done

}

function Update_Makefile()
{
    BLD_MFILE=${BLD_DIR}/${HD_MAKEFILE}

    if  [ ${BLD_MFILE} -nt ${SRC_DIR}/${MAKE_SEGMENT} ] && \
        [ ${BLD_MFILE} -nt ${STAMP_BLD_ENV} ]; then
        return 0;
    fi

    rm -f ${BLD_MFILE}

    echo "MODULE_NAME := ${MODULE}" >> ${BLD_MFILE}
    cat ${STAMP_BLD_ENV} >> ${BLD_MFILE}

    cat << EOB >> ${BLD_MFILE}

include \$(RULE_DIR)/settings.mk
include \$(CONFIG_TPL)

all:

EOB

    cp -f ${SRC_DIR}/${MAKE_SEGMENT} ${BLD_DIR}/${MAKE_SEGMENT}

    cat ${BLD_DIR}/${MAKE_SEGMENT} >> ${BLD_MFILE}
    ${SED} -i 's/clean:/clean : clean-prepare/g' ${BLD_MFILE}

    cat << EOB >> ${BLD_MFILE}

env:
	@echo ""
	@printf -- "-----------------------------------------------------------------\n"
	@\$(foreach var,\$(SHOW_ENV_VARS),\$(call Dump_Var,\$(var)))
	@printf -- "-----------------------------------------------------------------\n"
	@echo ""

include \$(RULE_DIR)/rules.mk
EOB

    Trace "Updated: ${BLD_MFILE}"
}

if [ "$#" != "1" -a "$#" != "2" ]; then exit 12; fi

MODULE=${1}
BLD_DIR=${OUTPUT_DIR}/${MODULE}
SRC_DIR=${TOP_DIR}/${MODULE}

if [ ! -d ${SRC_DIR} ]; then
    exit 0
fi

if [ "${VERBOSE_PRE_BLD}" != "" ]; then
    OPTION_V="-v"
    CMDSET_X="set -x"
else
    OPTION_Q="-q"
fi

mkdir -p ${BLD_DIR}

MSG=$(printf "%-28s%s" "${MODULE}" "[..]")
echo -ne "\r                                                    "
echo -ne "\033[0;37;0;44m""\r[..] o ${MSG}""\033[0;m"
Trace ""

if [ "$#" = "1" ]; then
    Update_Sources
fi
Update_Makefile
