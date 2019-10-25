NimBLE Host ATT Client Reference
--------------------------------

Introduction
~~~~~~~~~~~~

The Attribute Protocol (ATT) is a mid-level protocol that all BLE devices use to exchange data. Data is exchanged when
an ATT client reads or writes an attribute belonging to an ATT server. Any device that needs to send or receive data
must support both the client and server functionality of the ATT protocol. The only devices which do not support ATT
are the most basic ones: broadcasters and observers (i.e., beaconing devices and listening devices).

Most ATT functionality is not interesting to an application. Rather than use ATT directly, an application uses the
higher level GATT profile, which sits directly above ATT in the host. NimBLE exposes the few bits of ATT functionality
which are not encompassed by higher level GATT functions. This section documents the ATT functionality that the NimBLE
host exposes to the application.

API
~~~~~~

.. doxygengroup:: bt_host
    :content-only:
    :members:
