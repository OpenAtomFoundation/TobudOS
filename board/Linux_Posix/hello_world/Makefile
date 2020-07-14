###################################################################
#automatic detection QTOP and LOCALDIR
CUR_DIR := $(patsubst %/,%,$(dir $(realpath $(firstword $(MAKEFILE_LIST)))))
TRYQTOP := $(shell if [ -n "$$QTOP" ] ; then\
        echo $$QTOP;\
        else\
        cd $(CUR_DIR); while /usr/bin/test ! -e qmk ; do \
        dir=`cd ../;pwd`;                       \
        if [ "$$dir" = "/" ] ; then             \
           echo Cannot find QTOP in $(firstword $(MAKEFILE_LIST)) 1>&2; \
           exit 1;                              \
        fi ;                                    \
        cd $$dir;                               \
        done ;                                  \
        pwd;                                    \
        fi)
QTOP ?= $(realpath ${TRYQTOP})

ifeq ($(QTOP),)
$(error Please run this in a tree)
endif
LOCALDIR = $(patsubst %/,%,$(subst $(realpath $(QTOP))/,,$(CUR_DIR)))

####################################################################


export BP=Linux_Posix

TREE_LIB_ENABLE=1
lib=
subdirs =

include ${QTOP}/qmk/generic/Make.tpl

all::
	make -C ${QTOP}/arch
	make -C ${QTOP}/kernel
	make -C ${QTOP}/osal
	make -C ${QTOP}/net
	make -C ${QTOP}/devices
exec = 
LD_A_FILES += $(LIBDIR)/libarch.a
LD_A_FILES += $(LIBDIR)/libkernel.a
LD_A_FILES += $(LIBDIR)/libhello_world.a
LD_A_FILES += $(LIBDIR)/libcmsis_os.a
LDFLAGS += -lpthread
include ${QTOP}/qmk/generic/Make.exec


