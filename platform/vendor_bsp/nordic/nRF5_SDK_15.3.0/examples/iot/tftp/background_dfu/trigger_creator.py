#!/usr/bin/env python
"""
Copyright (c) 2017, Nordic Semiconductor ASA

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form, except as embedded into a Nordic
   Semiconductor ASA integrated circuit in a product or a software update for
   such product, must reproduce the above copyright notice, this list of
   conditions and the following disclaimer in the documentation and/or other
   materials provided with the distribution.

3. Neither the name of Nordic Semiconductor ASA nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

4. This software, with or without modification, must only be used with a
   Nordic Semiconductor ASA integrated circuit.

5. Any software provided in binary form under this license must not be reverse
   engineered, decompiled, modified and/or disassembled.

THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""

import tempfile
import argparse
import binascii
import sys
import json
import os.path

from nordicsemi.dfu.package import Package

def _get_manifest_items(manifest):
    import inspect
    result = []

    for key, value in inspect.getmembers(manifest):
        if (key.startswith('__')):
            continue
        if not value:
            continue
        if inspect.ismethod(value) or inspect.isfunction(value):
            continue

        result.append((key, value))

    return result

def _get_file_names(manifest):
    data_attrs = _get_manifest_items(manifest)
    if (len(data_attrs) > 1):
        raise RuntimeError("More than one image present in manifest")
    data_attrs = data_attrs[0]
    firmware = data_attrs[1]
    return firmware.dat_file, firmware.bin_file

def unpack_package(zip_file_path, unpack_path):
    manifest = Package.unpack_package(zip_file_path, unpack_path)
    return _get_file_names(manifest)

def _create_empty_firmware_desc():
    return {"s": 0, "id": 0, "is": 0, "iid": 0}

def _create_empty_path_desc():
    return {"app": "", "appsd": "", "bl": ""}

def get_len_and_crc(file_path):
    with open(file_path, 'rb') as f:
        file = f.read()
        
    return len(file), binascii.crc32(file) & 0xffffffff

def create_trigger_file(output_file, tftp_prefix, input_path, init_filename, firmware_filename):
    trigger = {"app": _create_empty_firmware_desc(),
               "sd":  _create_empty_firmware_desc(),
               "bl":  _create_empty_firmware_desc(),
               "p":   _create_empty_path_desc(),
               "i":   _create_empty_path_desc()}
    
    trigger["i"]["app"] = ''.join([tftp_prefix, init_filename])
    trigger["p"]["app"] = ''.join([tftp_prefix, firmware_filename])
    
    init_len, init_crc = get_len_and_crc(os.path.join(input_path, init_filename))
    firm_len, firm_crc = get_len_and_crc(os.path.join(input_path, firmware_filename))
    
    trigger["app"]["s"] = firm_len
    trigger["app"]["id"] = firm_crc
    trigger["app"]["is"] = init_len
    trigger["app"]["iid"] = init_crc
    
    json.dump(trigger, output_file)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Creates package description file for tftp bootloader')
    parser.add_argument('package_file',
                        help = 'Path to package zip file')
    parser.add_argument('output_file',
                        help = 'Output file name. Stdout is used if file name is not given',
                        nargs = '?')
    parser.add_argument('--tftp_path',
                        help = 'The path in the tftp server where firmware is going to be placed',
                        nargs = '?',
                        default = '')

    args = parser.parse_args()

    if (args.output_file):
        output_file = open(args.output_file, 'w')
    else:
        output_file = sys.stdout

    unpacked_zip_path = os.path.join(tempfile.mkdtemp(), 'unpacked_zip')

    init_filename, firm_filename = unpack_package(args.package_file, unpacked_zip_path)
    create_trigger_file(output_file,
                        args.tftp_path,
                        unpacked_zip_path,
                        init_filename, 
                        firm_filename)
