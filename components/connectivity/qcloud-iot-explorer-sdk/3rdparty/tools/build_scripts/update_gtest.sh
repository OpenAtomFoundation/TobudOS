#!/bin/bash

GOOGLE_TEST="external_libs/googletest"

if [ ! -d ${GOOGLE_TEST} ]; then
    git clone -q packages/gtest_module.git ${GOOGLE_TEST}
else
    cd ${GOOGLE_TEST}
    git pull -q
    cd -
fi