# QMK for TencentOS
# @author Jimmin.huang jimmin@163.com
# Makefile template
########################################################################

export QTOP
ALOWED_MAKE_VERSIONS :=3.82
ifeq ($(MAKELEVEL),0)
ifeq ($(filter $(ALOWED_MAKE_VERSIONS),$(MAKE_VERSION)),)
#$(info Make $(MAKE_VERSION) not be tested, use one of $(ALOWED_MAKE_VERSIONS))
$(info QMK for TOS)
endif
endif
ifeq ($(MAKELEVEL),0)
ifeq (help,$(findstring help,$(MAKECMDGOALS)))
NO_QMK_DEPS=1
endif
endif
# the top Makefile node that include this file
CUR_MK_NODE_DIR = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

########################################################################
#
# Set a default BP if one is not set.


SUPPORT-BP := $(sort $(subst $(QTOP)/qmk/board-pack/bp.,, $(wildcard $(QTOP)/qmk/board-pack/bp.*)))


ifndef BP
 uname := $(shell uname -s)
 ifeq ($(filter-out Linux Darwin,$(uname)),)
   BP=Linux_Posix
 endif
 ifndef	BP
  BP=native
  $(info Can't determine BP, `make help` see usage )
 endif
  $(info assume default BP=${BP} )
endif

BP := ${BP}
export BP


#
# By default, turn off the "changing directory" message.
#
MAKEFLAGS += --no-print-directory

#
# Use gmake by default
#
include ${QTOP}/qmk/generic/Make.tools

include	${QTOP}/qmk/board-pack/bp.${BP}



########################################################################

INCFLAGS = -I${QTOP}/arch/header -I${CUR_MK_NODE_DIR} -I${CUR_MK_NODE_DIR}/include  -I${CUR_MK_NODE_DIR}/inc
INCFLAGS += -DBP=\"$(BP)\" -DFILE_NAME=\"$(subst $(dir $<),,$<)\"

CFGFLAGS += ${INCFLAGS}
# STD_{C|CXX}FLAGS - Standard flags used by ALL compilations
# CFGFLAGS - Configuration flags used by ALL compilations

CFLAGS   += ${STD_CFLAGS} ${CFGFLAGS} $(ADD_CFLAGS) $(GLOBAL_CFLAGS)
CXXFLAGS += ${STD_CXXFLAGS} ${CFGFLAGS} $(ADD_CFLAGS) $(GLOBAL_CFLAGS)
LDFLAGS  += ${STD_LDFLAGS}  ${CFGLDFLAGS} $(ADD_LDFLAGS) $(GLOBAL_LDFLAGS)




V ?= 0
# use  to control printing of compilation lines
ifeq (0,$(V))
#no VERBOSE
Q:=@
else
# VERBOSE
Q:=
endif

#
# Default location to place binaries and make depend files for building
# purposes.
#

ifndef QTOPBUILD
QTOPBUILD :=build
endif
ifeq (,$(BLD_SUFFIX))
BLD_SUFFIX := .tos
endif

BLDROOT = ${QTOP}/${QTOPBUILD}/$(if ${BLDCONFIG},${BLDCONFIG}/)${BP}.$(notdir  $(firstword $(CC)))${BLD_SUFFIX}


ifdef LOCALDIR
BLDDIR = ${BLDROOT}/${LOCALDIR}
else
BLDDIR = ${BLDROOT}
endif

LIBDIR = ${BLDROOT}



#
# In each directory, build a list of local sources, objects, and headers
#
ifeq (,$(strip ${LSRCS})) # LSRCS
ifeq (,$(filter n no NO 0,$(TREE_LIB_ENABLE))) #  TREE_LIB_ENABLE yes

ifeq (,$(strip ${LSRCS_DIRS})) # LSRCS_DIRS
LSRCS_ALL = $(patsubst ./%,%, $(sort $(shell find . -name "*.c" -o -name "*.cpp" -o -name "*.s" -o -name "*.S" -o  -name "*.cc")))
else
LSRCS_ALL = $(patsubst ./%,%, $(sort $(shell find ${LSRCS_DIRS} -name "*.c" -o -name "*.cpp" -o  -name "*.s" -o -name "*.S" -o  -name "*.cc")))
endif # LSRCS_DIRS

else  #  TREE_LIB_ENABLE
LSRCS_ALL	= $(patsubst ./%,%,$(sort $(wildcard *.c *.cpp *.s *.S *.cc *.C))) # only include src in this dir.
endif  #  TREE_LIB_ENABLE

endif # LSRCS

ifneq (,$(strip ${LSRCS_EXCLUDE_FILES}))
$(info "ignore LSRCS_EXCLUDE_FILES source files: ${LSRCS_EXCLUDE_FILES}")
endif

ifndef LSRCS
LSRCS   = $(filter-out $(patsubst ./%,%,${LSRCS_EXCLUDE_FILES}),${LSRCS_ALL})
else

$(info  [$(BP)] INFO    LSRCS defined by user, use `make V=1` verbose print )

ifneq (0,$(V))
$(info  LSRCS=$(LSRCS)")
endif

endif

LOBJS	=	$(patsubst $(QTOP)/%,%,$(addsuffix .o, $(basename ${LSRCS})))
BOBJS	= 	$(addprefix ${BLDDIR}/,${LOBJS})

#
# Rule to create object file (build) directory
#

.PHONY: all install clean

.PRECIOUS: ${BLDDIR}/.tree $(EXECDIR)

%/.tree:
	@$(ECHO) "[$(BP)] MKDIR   $(dir $@)"
	$Q($(MKDIR) $(dir $@); $(TOUCH) $@)

# Rule allowing build through CPP only, creates .i file from .c file.

%.i: %.c
	@$Q$(ECHO) "[$(BP)] EEEE    ${LOCALDIR}/$<"
	$Q$(CC) -E ${CFLAGS} $< | $(SED) -e '/^ *$$/d' -e p -e d > $@

# Rule allowing build through source only, creates .s file from .c file.

%.s: %.c
	$Q$(CC) -S ${CFLAGS} $<


NO_QMK_DEPS = 1

# allow disabling of dependency file generation
# enable partial recompilation through use of included
# dependency makefiles
#
ifdef NO_QMK_DEPS
zDEPS_OPT = -MMD -MP -MF'$(@:%.o=%.d)'  -Wa,-a,-ad,-alms='$(@:%.o=%.lst)'
endif # ifndef NO_QMK_DEPS


# No dependency files, faster compile times
# no partial compile support
zDEPS_SED =
zDEPS_CPY =
zDEPS_CMD = /bin/true

define zdo
$(eval C := $1)
$Q$C
endef


#
# Default Build rules for .c --> .o, leaving the binary in BLDDIR/X.o,
# even if file not built from directory of source.
#
${BLDDIR}/%.o: $(QTOP)/%.c
	$Q$(MKDIR) $(dir $@)
	$Q$(RM) -f $@
ifdef E
	@$Q$(ECHO) '[$(BP)] EEEE    ${LOCALDIR}/$<'
	$Q$(CC) -E ${CFLAGS} $(EXTRA_CFLAGS) $< | $(SED) -e '/^ *$$/d' -e p -e d > $(@:.o=.i)
endif
ifdef S
	@$Q$(ECHO) "[$(BP)] SSSS    ${LOCALDIR}/$<"
	$Q$(CC) -S ${CFLAGS} $<
endif
	@$Q$(ECHO) "[$(BP)] CCCC    $<"
	$Q$(call zdo,$$(CC) $$(zDEPS_OPT) $$(CFLAGS) $$(EXTRA_CFLAGS) -o $$@ -c $$(realpath $$<))  && ($(zDEPS_CMD))

${BLDDIR}/%.o: %.c
	$Q$(MKDIR) $(dir $@)
	$Q$(RM) -f $@
ifdef E
	@$Q$(ECHO) "[$(BP)] EEEE    ${LOCALDIR}/$<"
	$Q$(CC) -E ${CFLAGS} $(EXTRA_CFLAGS) $< | $(SED) -e '/^ *$$/d' -e p -e d > $(@:.o=.i)
endif
	@$Q$(ECHO) "[$(BP)] CCCC    ${LOCALDIR}/$<"
	$Q$(call zdo,$$(CC) $$(zDEPS_OPT) $$(CFLAGS) $$(EXTRA_CFLAGS) -o $$@ -c $$(realpath $$<))  && ($(zDEPS_CMD))

${BLDDIR}/%.o: %.s
	$Q$(MKDIR) $(dir $@)
	$Q$(ECHO) "[$(BP)] ASAS    ${LOCALDIR}/$<"
#	$Q$(MKDIR) $(BLDDIR)/
#	$Q$(MKDIR) $(dir $@)
	$Q$(RM) -f $@
	$Q$(CC) ${CFLAGS} ${EXTRA_CFLAGS} -o $@ -c $(realpath $<)

${BLDDIR}/%.o: %.S
	$Q$(MKDIR) $(dir $@)
	$Q$(ECHO) "[$(BP)] ASAS    ${LOCALDIR}/$<"
	$Q$(RM) -f $@
	$Q$(CC) ${CFLAGS} ${EXTRA_CFLAGS} -o $@ -c $(realpath $<)

${BLDDIR}/%.o: %.cpp
	$Q$(MKDIR) $(dir $@)
ifeq (@,$(Q))
	@$(ECHO) "[$(BP)] CCCC    ${LOCALDIR}/$<"
endif
	$Q$(CXX) ${CXXFLAGS}  -o $@ -c $(realpath $<)

${BLDDIR}/%.o: %.cc
	$Q$(MKDIR) $(dir $@)
ifeq (@,$(Q))
	@$(ECHO) "[$(BP)] CCCC    ${LOCALDIR}/$<"
endif
	$Q$(CXX) ${CXXFLAGS} -o $@ -c $(realpath $<)

#
# Cause "make foo.o" in any subdirectory to put the object in the build
# directory instead of the local directory.
#
%.o: ${BLDDIR}/%.o
	@

#
# Targets for build
#
ifeq ($(MAKELEVEL),0)
ifeq (help,$(findstring help,$(MAKECMDGOALS)))
.PHONY: help
help:
	$Q$(ECHO) "Usage:"
	$Q$(ECHO) -e "\t make help"
	$Q$(ECHO) -e "make for specified board-pack"
	$Q$(foreach v, $(SUPPORT-BP), $(call zdo, $$(ECHO) -e "\t make BP=$$(v)") )
	$Q$(ECHO) -e "\t BP dir are: $(QTOP)/qmk/board-pack/"
	$Q$(ECHO) -e "Clean build"
	$Q$(ECHO) -e "\t make clean all"
	$Q$(ECHO) -e "Debug build, verbose print, generate preprocess .i for .c"
	$Q$(ECHO) -e "\t make V=1 "
	$Q$(ECHO) -e "\t make E=1 "
	$Q$(ECHO) -e "Debug Makefile"
	$Q$(ECHO) -e "\t make dm"
endif
endif



include ${QTOP}/qmk/generic/Make.lib
include ${QTOP}/qmk/generic/Make.subdirs
include ${QTOP}/qmk/generic/Make.depend


include ${QTOP}/qmk/generic/Make.debug


