#include "iotx_cm_internal.h"

#if defined(MQTT_COMM_ENABLED) || defined(MAL_ENABLED) 
#include "iotx_cm_mqtt.h"
#endif
#ifdef COAP_COMM_ENABLED
#include "iotx_cm_coap.h"
#endif


static void *fd_lock = NULL;
static iotx_cm_connection_t *_cm_fd[CM_MAX_FD_NUM] = {NULL};
static int _get_fd(iotx_cm_connection_t *handle);
static int _recycle_fd(int fd);
static int inline _fd_is_valid(int fd);
static int inited_conn_num = 0;

#ifdef DEVICE_MODEL_GATEWAY
    static void *_iotx_cm_yield_thread_func(void *params);
    static void *yield_thread = NULL;
    static int yield_task_leave = 1;
#endif

const char ERR_INVALID_PARAMS[] = "invalid parameter";
int iotx_cm_open(iotx_cm_init_param_t *params)
{
    int fd;
    iotx_cm_connection_t *connection = NULL;

    switch (params->protocol_type) {
        case IOTX_CM_PROTOCOL_TYPE_MQTT:
#if defined(MQTT_COMM_ENABLED) || defined(MAL_ENABLED)
            connection = iotx_cm_open_mqtt(params);
#endif
            break;
        case IOTX_CM_PROTOCOL_TYPE_COAP:
#ifdef COAP_COMM_ENABLED
            connection = iotx_cm_open_coap(params);
#endif
            break;            
        default:
            break;
    }

    if (connection == NULL) {
        cm_err("cm opon failed");
        return -1;
    }
    fd = _get_fd(connection);
    if (fd < 0) {
        cm_err("get fd failed");
        connection->close_func();
        return -1;
    }
    connection->fd = fd;
    return fd;
}

int iotx_cm_connect(int fd, uint32_t timeout)
{
    iotx_cm_connect_fp connect_func;
    int ret;

    if (_fd_is_valid(fd) == -1) {
        cm_err(ERR_INVALID_PARAMS);
        return -1;
    }
    HAL_MutexLock(fd_lock);
    connect_func = _cm_fd[fd]->connect_func;
    HAL_MutexUnlock(fd_lock);

    iotx_event_post(IOTX_CONN_CLOUD);

    ret = connect_func(timeout);

    if (ret == 0) {
        inited_conn_num++;
        if (inited_conn_num == 1) {

#ifdef DEVICE_MODEL_GATEWAY
            int stack_used;
            hal_os_thread_param_t task_parms = {0};
            task_parms.stack_size = 6144;
            task_parms.name = "cm_yield";
            ret = HAL_ThreadCreate(&yield_thread, _iotx_cm_yield_thread_func, NULL,
                                   &task_parms, &stack_used);
            if (ret < 0) {
                inited_conn_num--;
            }
#endif
        }
        iotx_event_post(IOTX_CONN_CLOUD_SUC);    
    } else {
        iotx_event_post(IOTX_CONN_CLOUD_FAIL);
    }


    return ret;
}

static int _iotx_cm_yield(int fd, unsigned int timeout)
{
    iotx_cm_yield_fp yield_func;

    if (fd_lock == NULL) {
        return NULL_VALUE_ERROR;
    }

    if (fd == -1) {
        int i;
        for (i = 0; i < CM_MAX_FD_NUM; i++) {
            yield_func = NULL;
            HAL_MutexLock(fd_lock);
            if (_cm_fd[i] != NULL) {
                yield_func =  _cm_fd[i]->yield_func;
            }
            HAL_MutexUnlock(fd_lock);
            if (yield_func != NULL) {
                yield_func(timeout);
            }
        }
        return 0;
    }

    if (_fd_is_valid(fd) == -1) {
        cm_err(ERR_INVALID_PARAMS);
        return -1;
    }

    HAL_MutexLock(fd_lock);
    yield_func =  _cm_fd[fd]->yield_func;
    HAL_MutexUnlock(fd_lock);
    return yield_func(timeout);

}
#ifdef DEVICE_MODEL_GATEWAY
static void *_iotx_cm_yield_thread_func(void *params)
{
    yield_task_leave = 0;
    while (inited_conn_num > 0) {
        _iotx_cm_yield(-1, CM_DEFAULT_YIELD_TIMEOUT);
    }
    yield_task_leave = 1;
    return NULL;
}
#endif

int iotx_cm_yield(int fd, unsigned int timeout)
{
#ifdef DEVICE_MODEL_GATEWAY
    return 0;
#else
    return _iotx_cm_yield(fd, timeout);
#endif
}


int iotx_cm_sub(int fd, iotx_cm_ext_params_t *ext, const char *topic,
                iotx_cm_data_handle_cb topic_handle_func, void *pcontext)
{
    iotx_cm_sub_fp sub_func;

    if (_fd_is_valid(fd) == -1) {
        cm_err(ERR_INVALID_PARAMS);
        return -1;
    }

    HAL_MutexLock(fd_lock);
    sub_func =  _cm_fd[fd]->sub_func;
    HAL_MutexUnlock(fd_lock);
    return sub_func(ext, topic, topic_handle_func, pcontext);
}

int iotx_cm_unsub(int fd, const char *topic)
{
    iotx_cm_unsub_fp unsub_func;

    if (_fd_is_valid(fd) == -1) {
        cm_err(ERR_INVALID_PARAMS);
        return -1;
    }

    HAL_MutexLock(fd_lock);
    unsub_func =  _cm_fd[fd]->unsub_func;
    HAL_MutexUnlock(fd_lock);
    return unsub_func(topic);
}



int iotx_cm_pub(int fd, iotx_cm_ext_params_t *ext, const char *topic, const char *payload, unsigned int payload_len)
{
    iotx_cm_pub_fp pub_func;
    
    if (_fd_is_valid(fd) == -1) {
        cm_err(ERR_INVALID_PARAMS);
        return -1;
    }

    HAL_MutexLock(fd_lock);
    pub_func =  _cm_fd[fd]->pub_func;
    HAL_MutexUnlock(fd_lock);
    return pub_func(ext, topic, payload, payload_len);
}

int iotx_cm_close(int fd)
{
    iotx_cm_close_fp close_func;

    if (_fd_is_valid(fd) != 0) {
        cm_err(ERR_INVALID_PARAMS);
        return -1;
    }

    if (inited_conn_num > 0) {
        inited_conn_num--;
    }

    if (inited_conn_num == 0) {
#ifdef DEVICE_MODEL_GATEWAY
        while (!yield_task_leave) {
            HAL_SleepMs(10);
        }
        if (yield_thread != NULL) {
            HAL_ThreadDelete(yield_thread);
        }
#endif
    }

    HAL_MutexLock(fd_lock);
    close_func = _cm_fd[fd]->close_func;
    HAL_MutexUnlock(fd_lock);
    if (close_func() != 0) {
        return -1;
    }
    if (_recycle_fd(fd) != 0) {
        return -1;
    }

    if (inited_conn_num == 0) {
        if (fd_lock != NULL) {
            HAL_MutexDestroy(fd_lock);
            fd_lock = NULL;
        }
    }

    return 0;
}


static int inline _fd_is_valid(int fd)
{
    int ret;

    if (fd_lock == NULL) {
        return NULL_VALUE_ERROR;
    }

    HAL_MutexLock(fd_lock);
    ret = (fd >= 0 && fd < CM_MAX_FD_NUM && _cm_fd[fd] != NULL) ? 0 : -1;
    HAL_MutexUnlock(fd_lock);
    return ret;
}

static int _recycle_fd(int fd)
{
    if (fd_lock == NULL) {
        fd_lock = HAL_MutexCreate();
        if (fd_lock == NULL) {
            return -1;
        }
    }

    if (fd < 0 || fd > CM_MAX_FD_NUM - 1) {
        return -1;
    }

    HAL_MutexLock(fd_lock);
    _cm_fd[fd] = NULL;
    HAL_MutexUnlock(fd_lock);

    return 0;
}

static int _get_fd(iotx_cm_connection_t *handle)
{
    int i;
    if (handle == NULL) {
        return NULL_VALUE_ERROR;
    }

    if (fd_lock == NULL) {
        fd_lock = HAL_MutexCreate();
        if (fd_lock == NULL) {
            return -1;
        }
    }

    HAL_MutexLock(fd_lock);
    for (i = 0; i < CM_MAX_FD_NUM; i++) {
        if (_cm_fd[i] == NULL) {
            _cm_fd[i] = handle;
            HAL_MutexUnlock(fd_lock);
            return i;
        }
    }
    HAL_MutexUnlock(fd_lock);
    cm_err("cm fd reached the limit");
    return -1;
}
