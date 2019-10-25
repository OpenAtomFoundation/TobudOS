Configure device address
------------------------

A BLE device needs an address to do just about anything. For information
on the various types of Bluetooth addresses, see the `NimBLE Host
Identity Reference :doc:`<../ble_hs/ble_hs_id/ble_hs_id>`.

There are several methods for assigning an address to a NimBLE device.
The available options are documented below:

Method 1: Configure nRF hardware with a public address
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When Mynewt is running on a Nordic nRF platform, the NimBLE controller
will attempt to read a public address out of the board's FICR or UICR
registers. The controller uses the following logic while trying to read
an address from hardware:

1. If the *DEVICEADDRTYPE* FICR register is written, read the address
   programmed in the *DEVICEADDR[0]* and *DEVICEADDR[1]* FICR registers.
2. Else if the upper 16 bits of the *CUSTOMER[1]* UICR register are 0,
   read the address programmed in the *CUSTOMER[0]* and *CUSTOMER[1]*
   UCI registers.
3. Else, no address available.

Method 2: Hardcode a public address in the Mynewt target
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The NimBLE controller package exports a
:doc:`syscfg <../../../os/modules/sysinitconfig/sysinitconfig>` setting
called ``BLE_PUBLIC_DEV_ADDR``. This setting can be overridden at the
application or target level to configure a public Bluetooth address. For
example, a target can assign the public address *11:22:33:44:55:66* as
follows:

::

    syscfg.vals:
        BLE_PUBLIC_DEV_ADDR: '(uint8_t[6]){0x66, 0x55, 0x44, 0x33, 0x22, 0x11}'

This setting takes the form of a C expression. Specifically, the value
is a designated initializer expressing a six-byte array. Also note that
the bytes are reversed, as an array is inherently little-endian, while
addresses are generally expressed in big-endian.

Note: this method takes precedence over method 1. Whatever is written to
the ``BLE_PUBLIC_DEV_ADDR`` setting is the address that gets used.

Method 3: Configure a random address at runtime
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Random addresses get configured through the NimBLE host. The following
two functions are used in random address configuration:

-  :doc:`ble_hs_id_gen_rnd <../ble_hs/ble_hs_id/functions/ble_hs_id_gen_rnd>`:
   Generates a new random address.
-  :doc:`ble_hs_id_set_rnd <../ble_hs/ble_hs_id/functions/ble_hs_id_set_rnd>`:
   Sets the device's random address.

For an example of how this is done, see the :doc:`<../../../os/tutorials/ibeacon>`.

*Note:* A NimBLE device can be configured with multiple addresses; at
most one of each address type.
