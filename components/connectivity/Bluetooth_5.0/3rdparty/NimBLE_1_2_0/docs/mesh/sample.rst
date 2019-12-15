Sample application
------------------

**blemesh** sample application implements Bluetooth Mesh node that supports On/Off and Level models.

To build application use following target. Note that since this application uses Non-resolvable Private Address there is
no need for configuring public address.

::

    newt target create blemesh
    newt target set blemesh app=@apache-mynewt-core/apps/blemesh
    newt target set blemesh bsp=@apache-mynewt-core/hw/bsp/nrf52840pdk
    newt target set blemesh build_profile=optimized
    newt target set blemesh syscfg=BLE_MESH_PB_GATT=1:BLE_MESH_DEV_UUID='(uint8_t[16]){0x22, 0x20, 0}'

Every device should have unique Device UUID so config amend and rebuild is needed for each of the devices that will be
added to a network.

::

    newt target set blemesh syscfg=BLE_MESH_PB_GATT=1:BLE_MESH_DEV_UUID='(uint8_t[16]){0x22, 0x21, 0}'
    ...
    newt target set blemesh syscfg=BLE_MESH_PB_GATT=1:BLE_MESH_DEV_UUID='(uint8_t[16]){0x22, 0x22, 0}'
    ...
    newt target set blemesh syscfg=BLE_MESH_PB_GATT=1:BLE_MESH_DEV_UUID='(uint8_t[16]){0x22, 0x23, 0}'

GATT bearer is enabled so that it is possible to provision those with Bluetooth Mesh application from Silicon Labs
(available `here <https://play.google.com/store/apps/details?id=com.siliconlabs.bluetoothmesh>`__) which doesn't
support advertising bearer.
