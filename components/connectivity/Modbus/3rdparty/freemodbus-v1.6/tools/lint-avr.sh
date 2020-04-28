splint \
    -I. -I../../modbus/include -I../../modbus/rtu -I../../modbus/ascii \
    -Iport -I/cygdrive/c/WinAVR/avr/include \
    -nullpass +charint -booltype BOOL -D__AVR_ATmega168__=1 $@
