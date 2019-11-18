NimBLE Heart Rate Sensor Example
================================
This example demonstrates how to implement asynchronous data transfer using GATT
notifications by implementing a mock-up BLE heart rate sensor. Once flashed on
a node, you can connect to it using any phone or device capable of using such a
heart rate sensor, e.g. Nordics `nRF Toolbox` for Android
(https://play.google.com/store/apps/details?id=no.nordicsemi.android.nrftoolbox).

The heart rate sensor is simulated by simply providing an up- and down counter
for its BPM value.

On top of the heart rate service, this example implements the device information
as well as the battery service.
