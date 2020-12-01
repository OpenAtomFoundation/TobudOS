#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys
import os
import shutil

demo_dir_prefix = 'qcloud-iot-ble-'
script_path = os.path.split(os.path.realpath(__file__))[0]
sdk_path = os.path.join(script_path, '..', '..')


def extract_nrf2832(dest_dir):
    print('extract code for nrf52832 start, dest dir %s' % dest_dir)
    if os.path.exists(dest_dir):
        shutil.rmtree(dest_dir)
    shutil.copytree(os.path.join(sdk_path, 'samples', 'nrf52832'), dest_dir)

    new_sdk_path = os.path.join(dest_dir, 'qcloud_iot_explorer_ble')
    shutil.copytree(os.path.join(sdk_path, 'inc'), os.path.join(new_sdk_path, 'inc'))
    shutil.copytree(os.path.join(sdk_path, 'src'), os.path.join(new_sdk_path, 'src'))
    print('extract code success')
    pass


def extract_esp32(dest_dir):
    print('extract code for esp32, dest dir %s' % dest_dir)
    if os.path.exists(dest_dir):
        shutil.rmtree(dest_dir)
    shutil.copytree(os.path.join(sdk_path, 'samples', 'esp32'), dest_dir)

    new_sdk_path = os.path.join(dest_dir, 'components', 'qcloud_llsync')
    shutil.copytree(os.path.join(sdk_path, 'inc'), os.path.join(new_sdk_path, 'inc'))
    shutil.copytree(os.path.join(sdk_path, 'src'), os.path.join(new_sdk_path, 'src'))

    print('extract code success')
    pass


def extract_lifesense(dest_dir):
    print('extract code for lifesense, dest dir %s' % dest_dir)
    if os.path.exists(dest_dir):
        shutil.rmtree(dest_dir)
    shutil.copytree(os.path.join(sdk_path, 'samples', 'lifesense'), dest_dir)

    new_sdk_path = os.path.join(dest_dir, 'qcloud_iot_explorer_ble')
    shutil.copytree(os.path.join(sdk_path, 'inc'), os.path.join(new_sdk_path, 'inc'))
    shutil.copytree(os.path.join(sdk_path, 'src'), os.path.join(new_sdk_path, 'src'))

    print('extract code success')
    pass


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('\nUsage: python3 %s <platform> [dest dir]' % sys.argv[0])
        print('\nDefinitions:')
        print('<platform>\t%s' % 'Device type. The following are allowed: nrf52832, esp32, lefesense')
        print('[dest dir]\t%s %s\n' % ('Where the code stored. Default path: ', script_path))
    else:
        if len(sys.argv) == 2:
            dest_dir = os.path.join(script_path, demo_dir_prefix + sys.argv[1])
        else:
            dest_dir = os.path.join(sys.argv[2], demo_dir_prefix + sys.argv[1])

        if 'nrf52832' == sys.argv[1]:
            extract_nrf2832(dest_dir)
        elif 'esp32' == sys.argv[1]:
            extract_esp32(dest_dir)
        elif 'lifesense' == sys.argv[1]:
            extract_lifesense(dest_dir)
        else:
            print('Unknow platform %s, extract failed' % sys.argv[1])
            pass
