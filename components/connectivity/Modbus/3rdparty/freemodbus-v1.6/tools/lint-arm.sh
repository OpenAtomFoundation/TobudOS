splint \
    -I. -I../../modbus/include -I../../modbus/rtu -I../../modbus/ascii \
    -Iport -Ilibrary/include \
    -Ifreertos/include -Ifreertos/portable/GCC/ARM7_STR71X/ \
    -DSTR71X_GCC=1 -nullpass +charint -booltype BOOL $@
