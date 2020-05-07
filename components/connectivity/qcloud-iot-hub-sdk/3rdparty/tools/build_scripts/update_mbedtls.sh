#!/bin/bash

MBEDTLS_DIR="external_libs/mbedtls"

if [ ! -d ${MBEDTLS_DIR} ]; then
    git clone -q packages/mbedtls_module.git ${MBEDTLS_DIR}
else
    cd ${MBEDTLS_DIR}
    git pull -q
    cd -
fi
