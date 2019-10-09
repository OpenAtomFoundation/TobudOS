<!--
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
-->

# Apache Mynewt NimBLE

## Overview

See (https://mynewt.apache.org/network/ble/ble_intro/).

## Building

NimBLE is usually built as a part of Apache Mynewt OS, but ports for
other RTOS-es are also available.

### Linux

1. Build the sample application

```no-highlight
   cd porting/examples/linux
   make
```

2. Run the sample application

First insert a USB Bluetooth dongle.  These are typically BLE 4.0 capable.

Verify the dongle is connected with hciconfig:

```no-highlight
   $ hciconfig
hci0:	Type: BR/EDR  Bus: USB
	BD Address: 00:1B:DC:06:62:5E  ACL MTU: 310:10  SCO MTU: 64:8
	DOWN
	RX bytes:5470 acl:0 sco:0 events:40 errors:0
	TX bytes:5537 acl:176 sco:0 commands:139 errors:1
```

Then run the application built in step one.  The application is configured
in sysconfig.h to use hci0.

```no-highlight
   cd porting/examples/linux
   sudo ./_build/nimble_linux.out
```

3. Build and run the unit tests

The Operating System Abstraction Layer (OSAL) used to port Nimble to Linux
has a suite of unit tests.

```no-highlight
   cd tests/unit/porting/npl
   make test
```
