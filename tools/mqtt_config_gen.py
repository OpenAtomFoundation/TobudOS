#!/usr/bin/python
# -*- coding: UTF-8 -*-

import base64
import hashlib
import hmac
import random
import string
import time
from string import Template

# bind raw_input to input in Python 2
try:
    input = raw_input
except NameError:
    pass

product_id = input("product id:")
dev_name = input("device name:")
passwd = input("password:")
sub = input("subscribe topic:[default:control]")
if sub == "":
    sub = "control"
pub = input("publish topic:[default:event]")
if pub == "":
    pub = "event"

client_id = product_id + dev_name
# expire time: 2^32 - 1 = 4294967295
username = client_id+";21010406;12365;{}".format(4294967295)

def hmacsha1(content, passwd):
        passwd_byte = base64.b64decode(passwd)
        return hmac.new(passwd_byte, content, digestmod=hashlib.sha1).hexdigest()

username = username.encode("utf-8")
passwd = passwd.encode("utf-8")
sign = hmacsha1(username, passwd)

template_header = ('#ifndef  TOS_MQTT_CONFIG_H\n'
            '#define  TOS_MQTT_CONFIG_H\n'
            '\n'
            '#define MQTT_SERVER_IP          "111.230.189.156"\n'
            '#define MQTT_SERVER_PORT        "1883"\n'
            '#define MQTT_PRODUCT_ID         "$product"\n'
            '#define MQTT_DEV_NAME           "$dev"\n'
            '#define MQTT_CLIENT_ID          "$product$dev"\n'
            '#define MQTT_USR_NAME           "$product$dev;21010406;12365;4294967295"\n'
            '#define MQTT_PASSWORD           "$sign;hmacsha1"\n'
            '#define MQTT_SUBSCRIBE_TOPIC    "$product/$dev/$sub"\n'
            '#define MQTT_PUBLISH_TOPIC      "$product/$dev/$pub"\n'
            '\n'
            '#define MQTT_SERVER_DOMAIN      "$product.iotcloud.tencentdevices.com"\n'
            '#define MQTT_TOPIC              "$product/$dev"\n'
            '#define MQTT_SUBSCRIBE_TOPIC_DOWN    "$thing/down/property/$product/$dev"\n'
            '#define MQTT_PUBLISH_TOPIC_UP      "$thing/up/property/$product/$dev"\n'
            '\n'
            '#endif\n'
            '\n')

template_c = ('#ifndef  TOS_MQTT_CONFIG_H\n'
            'tos_sal_module_parse_domain(MQTT_SERVER_DOMAIN,host_ip,sizeof(host_ip));\n'
            '\n'
            'mqtt_set_port(client, MQTT_SERVER_PORT);\n'
            'mqtt_set_host(client, host_ip);\n'
            'mqtt_set_client_id(client, MQTT_CLIENT_ID);\n'
            'mqtt_set_user_name(client, MQTT_USR_NAME);\n'
            'mqtt_set_password(client, MQTT_PASSWORD);\n'
            'mqtt_set_clean_session(client, 1);\n'
            '\n'
            'error = mqtt_subscribe(client, MQTT_SUBSCRIBE_TOPIC_DOWN, QOS0, tos_topic_handler);\n'
            '\n'
            'error = mqtt_publish(client, MQTT_PUBLISH_TOPIC_UP, &msg);\n'
            '\n'
            '#endif\n'
            '\n')

src_header = Template(template_header)
src_c = Template(template_c)

d = {
        'product':product_id,
        'dev':dev_name,
        'sign':sign,
        'sub':sub,
        'pub':pub,
        'thing':'$thing'
    }

#do the substitution
dst_header = src_header.substitute(d)
dst_c = src_c.substitute(d)
print("===============Generate mqtt_config.h==================")
print(dst_header)
with open('mqtt_config.h', 'w') as f:
    f.write(dst_header)

print("===============Generate mqtt_connect_demo.c==================")
print(dst_c)
with open('mqtt_connect_demo.c', 'w') as f:
    f.write(dst_c)
