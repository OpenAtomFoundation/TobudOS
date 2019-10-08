#
# Copyright (c) 2017 Nordic Semiconductor ASA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
#
#   3. Neither the name of Nordic Semiconductor ASA nor the names of other
#   contributors to this software may be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
#   4. This software must only be used in or with a processor manufactured by Nordic
#   Semiconductor ASA, or in or with a processor manufactured by a third party that
#   is used in combination with a processor manufactured by Nordic Semiconductor.
#
#   5. Any software provided in binary or object form under this license must not be
#   reverse engineered, decompiled, modified and/or disassembled.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
import logging
import Queue
import time
import socket
import sys
from thread import *
import argparse

HOST = ''   # Symbolic name meaning all available interfaces
PORT = 8889 # Arbitrary non-privileged port
 
parser = argparse.ArgumentParser(description='BLE serial deamon.')
parser.add_argument('--com', help='COM port name (e.g. COM110).',
                    nargs='*', required=False, default="")
parser.add_argument('--snr', help='segger id.',
                    nargs=1, required=False, default="")
parser.add_argument('--family', help='chip family.',
                    nargs=1, required=False, default="")
parser.add_argument('--name', help='Device name (advertising name).',
                    nargs='*', required=True, default=None)
args = parser.parse_args()

def ble_driver_init(conn_ic_id):
    global BLEDriver, Flasher, DfuTransportBle
    from pc_ble_driver_py import config
    config.__conn_ic_id__ = conn_ic_id

ble_driver_init(args.family[0])  

from nordicsemi.dfu.dfu_transport_ble import DFUAdapter
from pc_ble_driver_py.exceptions      import NordicSemiException, IllegalStateException
from pc_ble_driver_py.ble_driver      import config,Flasher,BLEDriver, BLEDriverObserver, BLEEnableParams, BLEUUIDBase, BLEUUID, BLEAdvData, BLEGapConnParams, NordicSemiException
from pc_ble_driver_py.ble_driver      import ATT_MTU_DEFAULT
from pc_ble_driver_py.ble_adapter     import BLEAdapter, BLEAdapterObserver, EvtSync

global nrf_sd_ble_api_ver
nrf_sd_ble_api_ver = config.sd_api_ver_get()
logger  = logging.getLogger(__name__)

class BLE_Serial(BLEDriverObserver, BLEAdapterObserver):
    BASE_UUID   = BLEUUIDBase([0x6E, 0x40, 0x00, 0x00, 0xB5, 0xA3, 0xF3, 0x93,
                               0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E])

    RX_UUID     = BLEUUID(0x0003, BASE_UUID)
    TX_UUID     = BLEUUID(0x0002, BASE_UUID)
    LOCAL_ATT_MTU     = 23
    def __init__(self, comport, periph_name, s_conn):
        driver           = BLEDriver(serial_port    = comport,
                                     baud_rate      = 115200)
        adapter          = BLEAdapter(driver)
        self.evt_sync           = EvtSync(['connected', 'disconnected'])
        self.target_device_name = periph_name
        self.target_device_addr = 0
        self.conn_handle        = None
        self.adapter            = adapter
        self.notifications_q    = Queue.Queue()
        self.adapter.observer_register(self)
        self.adapter.driver.observer_register(self)
        self.s_conn = s_conn
        self.adapter.driver.open()
        ble_enable_params = BLEEnableParams(vs_uuid_count      = 10,
                                            service_changed    = False,
                                            periph_conn_count  = 0,
                                            central_conn_count = 1,
                                            central_sec_count  = 1)
        if nrf_sd_ble_api_ver >= 3:
            logger.info("\nBLE: ble_enable with local ATT MTU: {}".format(DFUAdapter.LOCAL_ATT_MTU))
            ble_enable_params.att_mtu = BLE_Serial.LOCAL_ATT_MTU

        self.adapter.driver.ble_enable(ble_enable_params)
        self.adapter.driver.ble_vs_uuid_add(BLE_Serial.BASE_UUID)

        self.connect()

    def connect(self):
        # self.target_device_name = target_device_name
        # self.target_device_addr = target_device_addr
        logger.debug('BLE: connect: target address: 0x{}'.format(self.target_device_addr))
        logger.info('BLE: Scanning...')
        self.adapter.driver.ble_gap_scan_start()
        self.conn_handle = self.evt_sync.wait('connected')
        if self.conn_handle is None:
            raise NordicSemiException('Timeout. Target device not found.')
        logger.info('BLE: Connected to target')
        logger.debug('BLE: Service Discovery')

        if nrf_sd_ble_api_ver >= 3:
            if DFUAdapter.LOCAL_ATT_MTU > ATT_MTU_DEFAULT:
                logger.info('BLE: Enabling longer ATT MTUs')
                self.att_mtu = self.adapter.att_mtu_exchange(self.conn_handle)
                logger.info('BLE: ATT MTU: {}'.format(self.att_mtu))
            else:
                logger.info('BLE: Using default ATT MTU')

        self.adapter.service_discovery(conn_handle=self.conn_handle)
        logger.debug('BLE: Enabling Notifications')
        self.adapter.enable_notification(conn_handle=self.conn_handle, uuid=BLE_Serial.RX_UUID)
        return self.target_device_name, self.target_device_addr

    def on_gap_evt_connected(self, ble_driver, conn_handle, peer_addr, role, conn_params):
        self.evt_sync.notify(evt = 'connected', data = conn_handle)

    def on_gap_evt_disconnected(self, ble_driver, conn_handle, reason):
        self.evt_sync.notify(evt = 'disconnected', data = conn_handle)
        logger.info('BLE: Disconnected...')
        self.connect()

    def on_gap_evt_adv_report(self, ble_driver, conn_handle, peer_addr, rssi, adv_type, adv_data):
        dev_name_list = []
        if BLEAdvData.Types.complete_local_name in adv_data.records:
            dev_name_list = adv_data.records[BLEAdvData.Types.complete_local_name]

        elif BLEAdvData.Types.short_local_name in adv_data.records:
            dev_name_list = adv_data.records[BLEAdvData.Types.short_local_name]

        dev_name        = "".join(chr(e) for e in dev_name_list)
        address_string  = "".join("{0:02X}".format(b) for b in peer_addr.addr)
        logger.debug('Received advertisement report, address: 0x{}, device_name: {}'.format(address_string, dev_name))

        if (dev_name == self.target_device_name):
            conn_params = BLEGapConnParams(min_conn_interval_ms = 15,
                                           max_conn_interval_ms = 30,
                                           conn_sup_timeout_ms  = 4000,
                                           slave_latency        = 0)
            logger.info('BLE: Found target advertiser, address: 0x{}, name: {}'.format(address_string, dev_name))
            logger.info('BLE: Connecting to 0x{}'.format(address_string))
            self.adapter.connect(address = peer_addr, conn_params = conn_params)
            # store the name and address for subsequent connections

    def on_notification(self, ble_adapter, conn_handle, uuid, data):
        if self.conn_handle         != conn_handle: return
        if BLE_Serial.RX_UUID.value != uuid.value:  return
        
        logger.debug("Received notification {}".format(len(data)))

        #send to the socket
        buf = [chr(x) for x in data]
        buf = ''.join(buf)
        self.s_conn.sendall(buf)

    def write_data(self, data):
        self.adapter.write_req(self.conn_handle, BLE_Serial.TX_UUID, data)

#Function for handling connections. This will be used to create threads
def clientthread(conn):
    #Sending message to connected client
    conn.send('Welcome to the server. Type something and hit enter\n') #send only takes string
     
    #infinite loop so that function do not terminate and thread do not end.
    buf = []
    while True:
         
        #Receiving from client
        data = conn.recv(1024)
        outbuf = [ord(x) for x in data]
        
        b.write_data(outbuf)  
        
     
    #came out of loop
    conn.close()


args.name = ' '.join(args.name)
comport = args.com[0]
jlink_snr =  args.snr[0]
flash_connectivity = True
logging.basicConfig(format='%(message)s', level=logging.INFO)

if flash_connectivity:
        flasher = Flasher(serial_port=comport, snr = jlink_snr) 
        if flasher.fw_check():
            logger.info("Board already flashed with connectivity firmware.")
        else:
            logger.info("Flashing connectivity firmware...")
            flasher.fw_flash()
            logger.info("Connectivity firmware flashed.")
        flasher.reset()
        time.sleep(1)


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print 'Socket created'


 
#Bind socket to local host and port
try:
    s.bind((HOST, PORT))
except socket.error as msg:
    print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()
     
print 'Socket bind complete'
 
#Start listening on socket
s.listen(10)
print 'Socket now listening'
 
#now keep talking with the client
while 1:
    #wait to accept a connection - blocking call
    conn, addr = s.accept()
    print 'Connected with ' + addr[0] + ':' + str(addr[1])
    
    b = BLE_Serial(comport, args.name, conn)
    print 'BLE serial started'

    #start new thread takes 1st argument as a function name to be run, second is the tuple of arguments to the function.
    start_new_thread(clientthread ,(conn,))
 
s.close()