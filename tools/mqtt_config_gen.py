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

template = ('#ifndef  TOS_MQTT_CONFIG_H\n'
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
            '#endif\n'
            '\n')

src = Template(template)

d = {
        'product':product_id,
        'dev':dev_name,
        'sign':sign,
        'sub':sub,
        'pub':pub
    }

#do the substitution
dst = src.substitute(d)
print("===============Generate mqtt_config.h==================")
print(dst)
with open('mqtt_config.h', 'w') as f:
    f.write(dst)
