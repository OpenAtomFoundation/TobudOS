NimBLE Host
-----------

Introduction
~~~~~~~~~~~~

At a high level, the NimBLE stack is divided into two components:

-  Host
-  Controller

This document is an API reference for the host component. If you are
interested in the general structure of the NimBLE stack and its non-host
components, you might want to read the :doc:`../index`.

The host sits directly below the application, and it serves as the
interface to the application for all BLE operations.

.. toctree::
   :titlesonly:

   Return Codes <ble_hs_return_codes>
   GAP <ble_gap>
   GATT Client <ble_gattc>
   GATT Server <ble_gatts>
   Identity <ble_hs_id>
   ATT <ble_att>
