/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "http2_api.h"
#include "http2_wrapper.h"

#ifndef ON_DAILY
#define ON_DAILY
#endif  
#if defined(ON_DAILY)
	#define HTTP2_ONLINE_SERVER_URL       "10.101.12.205"
	#define HTTP2_ONLINE_SERVER_PORT      9999
	#define HTTP2_PRODUCT_KEY             "a1L5EUOh21s"
	#define HTTP2_DEVICE_NAME             "zhangmei_test01"
	#define HTTP2_DEVICE_SECRET           "KQCftfEDCx35LChyEwZoYY6FCYidTOp0"
#elif defined(ON_PRE)
	#define HTTP2_ONLINE_SERVER_URL       "100.67.141.158"
	#define HTTP2_ONLINE_SERVER_PORT      8443
	#define HTTP2_PRODUCT_KEY             "b1XVhqfan1X"
	#define HTTP2_DEVICE_NAME             "YvhjziEQmKusCFUgRpeo"
	#define HTTP2_DEVICE_SECRET           "QjkhCrAX0SbNWgKpIamuiDdLkk23Q1r7"
#else
	#define HTTP2_ONLINE_SERVER_URL       NULL
	#define HTTP2_ONLINE_SERVER_PORT      443
	#define HTTP2_PRODUCT_KEY             "DM5b8zbTWJs"
	#define HTTP2_DEVICE_NAME             "mydevice1"
	#define HTTP2_DEVICE_SECRET           "q4tiwQuICYfr6JQ8aUFjWxocuXJ5ruEx"
#endif

#define EXAMPLE_TRACE(fmt, ...)                        \
    do {                                               \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__);                \
        HAL_Printf("%s", "\r\n");                      \
    } while (0)


void*   pid1;

static const char UPLOAD_STRING[] =
  "It was almost December, and Jonas was beginning to be frightened. No. Wrong word, Jonas thought."
  "Frightened meant that deep, sickening feeling of something terrible about to happen. Frightened was"
  " the way he had felt a year ago when an unidentified aircraft had overflown the community twice."
  " He had seen it both times. Squinting toward the sky, he had seen the sleek jet, almost a blur at"
  " its high speed, go past, and a second later heard the blast of sound that followed. Then one more time,"
  " a moment later, from the opposite direction, the same plane. At first, he had been only fascinated. "
  "He had never seen aircraft so close, for it was against the rules for Pilots to fly over the community."
  " Occasionally, when supplies were delivered by cargo planes to the landing field across the river,"
  " the children rode their bicycles to the riverbank and watched, intrigued, the unloading and then the "
  "takeoff directed to the west, always away from the community. But the aircraft a year ago had been different."
  " It was not a squat, fat-bellied cargo plane but a needle-nosed single-pilot jet. Jonas, looking around"
  " anxiously, had seen others — adults as well as children — stop what they were doing and wait, confused,"
  " for an explanation of the frightening event. Then all of the citizens had been ordered to go into the nearest"
  " building and stay there. IMMEDIATELY, the rasping voice through the speakers had said. LEAVE YOUR BICYCLES "
  "WHERE THEY ARE. Instantly, obediently, Jonas had dropped his bike on its side on the path behind his family's "
  "dwelling. He had run indoors and stayed there, alone. His parents were both at work, and his little sister,"
  " Lily, was at the Childcare Center where she spent her after-school hours. Looking through the front window,"
  " he had seen no people: none of the busy afternoon crew of Street Cleaners, Landscape Workers, and Food "
  "Delivery people who usually populated the community at that time of day. He saw only the abandoned bikes here "
  "and there on their sides; an upturned wheel on one was still revolving slowly. He had been frightened then. "
  "The sense of his own community silent, waiting, had made his stomach churn. He had trembled. But it had been "
  "nothing. Within minutes the speakers had crackled again, and the voice, reassuring now and less urgent, "
  "had explained that a Pilot-in-Training had misread his navigational instructions and made a wrong turn. "
  "Desperately the Pilot had been trying to make his way back before his error was noticed. NEEDLESS TO SAY, "
  "HE WILL BE RELEASED, the voice had said, followed by silence. There was an ironic tone to that final message, "
  "as if the Speaker found it amusing; and Jonas had smiled a little, though he knew what a grim statement it had"
  " been. For a contributing citizen to be released from the community was a final decision, a terrible punishment,"
  " an overwhelming statement of failure. Even the children were scolded if they used the term lightly at play, "
  "jeering at a teammate who missed a catch or stumbled in a race. Jonas had done it once, had shouted at his "
  "best friend, &quot;That's it, Asher! You're released!&quot; when Asher's clumsy error had lost a match for "
  "his team. He had been taken aside for a brief and serious talk by the coach, had hung his head with guilt and "
  "embarrassment, and apologized to Asher after the game. Now, thinking about the feeling of fear as he pedaled "
  "home along the river path, he remembered that moment of palpable, stomach-sinking terror when the aircraft had"
  " streaked above. It was not what he was feeling now with December approaching. He searched for the right word"
  " to describe his own feeling. Jonas was careful about language. Not like his friend, Asher, who talked too "
  "fast and mixed things up, scrambling words and phrases until they were barely recognizable and often very funny. "
  "Jonas grinned, remembering the morning that Asher had dashed into the classroom, late as usual, arriving "
  "breathlessly in the middle of the chanting of the morning anthem. When the class took their seats at the "
  "conclusion of the patriotic hymn, Asher remained standing to make his public apology as was required. "
  "&quot;I apologize for inconveniencing my learning community.&quot; Asher ran through the standard apology"
  " phrase rapidly, still catching his breath. The Instructor and class waited patiently for his explanation."
  " The students had all been grinning, because they had listened to Asher's explanations so many times before. "
  "&quot;I left home at the correct time but when I was riding along near the hatchery, the crew was separating "
  "some salmon. I guess I just got distraught, watching them. &quot;I apologize to my classmates,&quot; Asher"
  " concluded. He smoothed his rumpled tunic and sat down. &quot;We accept your apology, Asher.&quot; The class"
  " recited the standard response in unison. Many of the students were biting their lips to keep from laughing."
  " &quot;I accept your apology, Asher,&quot; the Instructor said. He was smiling. &quot;And I thank you, because "
  "once again you have provided an opportunity for a lesson in language. &quot;Distraught' is too strong an adjective"
  " to describe salmon-viewing.&quot; He turned and wrote &quot;distraught&quot; on the instructional board. "
  "Beside it he wrote &quot;distracted.&quot; Jonas, nearing his home now, smiled at the recollection. Thinking, "
  "still, as he wheeled his bike into its narrow port beside the door, he realized that frightened was the wrong "
  "word to describe his feelings, now that December was almost here. It was too strong an adjective. He had waited a"
  " long time for this special December. Now that it was almost upon him, he wasn't frightened, but he was ... eager,"
  " he decided. He was eager for it to come. And he was excited, certainly. All of the Elevens were excited about the"
  " event that would be coming so soon. But there was a little shudder of nervousness when he thought about it, about"
  " what might happen. Apprehensive, Jonas decided. That's what I am. &quot;Who wants to be the first tonight, for "
  "feelings?&quot; Jonas's father asked, at the conclusion of their evening meal. It was one of the rituals,"
  " the evening telling of feelings. Sometimes Jonas and his sister, Lily, argued over turns, over who would get to go first."
  " Their parents, of course, were part of the ritual; they, too, told their feelings each evening. But like all parents — "
  "all adults — they didn't fight and wheedle for their turn. Nor did Jonas, tonight. His feelings were too complicated this evening.";


static void on_header(uint32_t stream_id, char *channel_id,int cat,const uint8_t *name,uint32_t namelen, 
                              const uint8_t *value,uint32_t valuelen, uint8_t flags ,void *user_data)
{
    EXAMPLE_TRACE("~~~~~stream_id = %d, channel_id=%s, name = %s, value = %s, flag = %d user_data =%p\n", stream_id,channel_id,name,value,flags ,user_data);
}

static void on_chunk_recv(uint32_t stream_id, char *channel_id,const uint8_t *data, uint32_t len,uint8_t flags,void *user_data)
{
     EXAMPLE_TRACE("~~~~~stream_id = %d, channel_id=%s, data = %.*s, len = %d flag = %d\n", stream_id, channel_id, len, data, len, flags);
}
static void on_stream_close(uint32_t stream_id, char *channel_id,uint32_t error_code,void *user_data)
{
     EXAMPLE_TRACE("~~~~~stream_id = %d channel_id=%s, error_code = %d\n", stream_id,channel_id,error_code);
}
static void on_stream_frame_send(uint32_t stream_id, char *channel_id, int type, uint8_t flags,void *user_data){
    EXAMPLE_TRACE("~~~~~stream_id = %d user_data =%p, type = %d\n", stream_id,user_data,type);
}

static http2_stream_cb_t my_cb = {
    .on_stream_header_cb = on_header,
    .on_stream_chunk_recv_cb = on_chunk_recv,
    .on_stream_close_cb = on_stream_close,
    .on_stream_frame_send_cb = on_stream_frame_send,
};

static int test_user_data;

static int http2_stream_test()
{
    int ret;
    device_conn_info_t conn_info;
    void *handle;
    memset(&conn_info, 0, sizeof( device_conn_info_t));
    conn_info.product_key = HTTP2_PRODUCT_KEY;
    conn_info.device_name = HTTP2_DEVICE_NAME;
    conn_info.device_secret = HTTP2_DEVICE_SECRET;
    conn_info.url = HTTP2_ONLINE_SERVER_URL;
    conn_info.port = HTTP2_ONLINE_SERVER_PORT;

    handle = IOT_HTTP2_Connect(&conn_info,&my_cb);
    if(handle == NULL) {
        return -1;
    }

    http2_header header[] = {
        MAKE_HEADER("test_name", "test_http2_header"),
        MAKE_HEADER("x-for-test", "hello world"),
    };
    
    header_ext_info_t my_header_info = {
        header,
        2
    };
    stream_data_info_t info_upload, info_download;
    memset(&info_upload,0,sizeof(stream_data_info_t));

    //info_upload.identify = "com/aliyun/iotx/vision/picture/device/upstream";
    info_upload.identify = "iotx/vision/voice/intercom/live";

    memset(&info_download, 0, sizeof(stream_data_info_t));
    info_download.identify = "iotx/vision/voice/intercom/live";
    info_download.user_data = (void *)&test_user_data;
    
    EXAMPLE_TRACE("----------usr_data =%p\n",(void *)&test_user_data);
    ret = IOT_HTTP2_Stream_Open(handle, &info_download, &my_header_info);
    if (ret < 0) {
        EXAMPLE_TRACE("=========iotx_http2_downstream_open failed %d!!!!!\n", ret);
        IOT_HTTP2_Disconnect(handle);
        return -1;
    }

    ret = IOT_HTTP2_Stream_Query(handle, &info_download, &my_header_info);
    if (ret < 0) {
        EXAMPLE_TRACE("=========iotx_http2_downstream_query failed %d!!!!!\n", ret);
        IOT_HTTP2_Stream_Close(handle, &info_download);
        IOT_HTTP2_Disconnect(handle);
        return -1; 
    }
    info_upload.user_data = (void *)&test_user_data;
    ret = IOT_HTTP2_Stream_Open(handle, &info_upload, &my_header_info);
    if(ret < 0) {
        EXAMPLE_TRACE("=========iotx_http2_upstream_open failed %d!!!!!\n", ret);
        IOT_HTTP2_Stream_Close(handle, &info_download);
        IOT_HTTP2_Disconnect(handle);
        return -1;
    }
    //send request 1
    info_upload.stream = (char *)UPLOAD_STRING;
    info_upload.stream_len= sizeof(UPLOAD_STRING);
    info_upload.send_len = 0;
    info_upload.packet_len=2048;

    while(info_upload.send_len<info_upload.stream_len) {
        info_upload.stream = (char *)UPLOAD_STRING + info_upload.send_len;
        if(info_upload.stream_len-info_upload.send_len<info_upload.packet_len) {
            info_upload.packet_len = info_upload.stream_len-info_upload.send_len;
        }
        ret = IOT_HTTP2_Stream_Send(handle, &info_upload, &my_header_info);
        if(ret <0 ) {
            EXAMPLE_TRACE("send err, ret = %d\n",ret);
            break;
        }
        EXAMPLE_TRACE("iotx_http2_stream_send info_upload.send_len =%d ret = %d\n", info_upload.send_len,ret);
    }

    //send request 2
    info_upload.stream = (char *)UPLOAD_STRING;
    info_upload.stream_len= sizeof(UPLOAD_STRING);
    info_upload.send_len = 0;
    info_upload.packet_len=1024;

    while(info_upload.send_len<info_upload.stream_len) {
        info_upload.stream = (char *)UPLOAD_STRING + info_upload.send_len;
        if(info_upload.stream_len-info_upload.send_len<info_upload.packet_len) {
            info_upload.packet_len = info_upload.stream_len-info_upload.send_len;
        }
        ret = IOT_HTTP2_Stream_Send(handle, &info_upload, &my_header_info);
        if(ret <0 ) {
            EXAMPLE_TRACE("send err, ret = %d\n",ret);
            break;
        }
        EXAMPLE_TRACE("iotx_http2_stream_send info_upload.send_len =%d ret = %d\n", info_upload.send_len,ret);
    }

    EXAMPLE_TRACE("=========Wait 20 seconds to receive downstream data\n");
    HAL_SleepMs(20000);

    IOT_HTTP2_Stream_Close(handle, &info_upload);
    IOT_HTTP2_Stream_Close(handle, &info_download);
    HAL_SleepMs(2000);

    ret = IOT_HTTP2_Disconnect(handle);
    EXAMPLE_TRACE("close connect %d\n",ret);
    return 0;
}

int main(int argc, char **argv)
{
    int ret;

    ret = http2_stream_test();

    return ret;
}
