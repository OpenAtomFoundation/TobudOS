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
export QTOP

####################################################################


export BP=Linux_Posix

TREE_LIB_ENABLE=1
lib=
subdirs =


all::
	make -C ${QTOP}/arch BP=${BP}
	make -C ${QTOP}/kernel BP=${BP}
	make -C ${QTOP}/osal BP=${BP}
	make -C ${QTOP}/net BP=${BP}
	make -C ${QTOP}/devices BP=${BP}
exec = 
LD_A_FILES += $(LIBDIR)/libarch.a
LD_A_FILES += $(LIBDIR)/libkernel.a
LD_A_FILES += $(LIBDIR)/libhello_world.a
LD_A_FILES += $(LIBDIR)/libcmsis_os.a
LDFLAGS += -lpthread

include ${QTOP}/qmk/generic/Make.exec


