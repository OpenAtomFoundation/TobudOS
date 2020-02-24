# ---------------------------------------------------------------------------
# server.sh - starts a local PPP server to connect with the target.
#
# Autor: Christian Walter
# ---------------------------------------------------------------------------
SER_DEV="/dev/ttyUSB0"
SER_SPEED="115200"
stty raw -parenb cs8  -cstopb -F $SER_DEV
/usr/sbin/pppd debug nodetach $SER_DEV $SER_SPEED
