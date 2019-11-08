LIBA_TARGET     := libiot_http2.a

HDR_REFS        += src/infra

DEPENDS         += wrappers external_libs/nghttp2
LDFLAGS         += -liot_sdk -liot_hal -liot_tls -liot_nghttp2

LIB_SRCS_EXCLUDE        := examples/http2_example_stream.c examples/http2_example_uploadfile.c examples/http2_example_break_resume.c

SRCS_http2-example-stream   		+= examples/http2_example_stream.c
SRCS_http2-example-uploadfile   	+= examples/http2_example_uploadfile.c
SRCS_http2-example-break-resume  	+= examples/http2_example_break_resume.c

$(call Append_Conditional, TARGET, http2-example-stream, HTTP2_COMM_ENABLED)
$(call Append_Conditional, TARGET, http2-example-uploadfile http2-example-break-resume, HTTP2_COMM_ENABLED FS_ENABLED)

