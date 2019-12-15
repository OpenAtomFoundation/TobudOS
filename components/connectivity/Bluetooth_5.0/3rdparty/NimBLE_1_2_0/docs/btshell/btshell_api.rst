API for btshell app
-------------------

"btshell" is one of the sample applications that come with Mynewt. It is a shell application which provides a basic
interface to the host-side of the BLE stack. "btshell" includes all the possible roles (Central/Peripheral) and they may
be run simultaneously. You can run btshell on a board and issue commands that make it behave as a central or a peripheral
with different peers.

**btshell** is a new application that uses shell subsystem introduced in Mynewt 1.1 and has updated commands and
parameters names. Thanks to support for tab completion commands names are more descriptive and self-explanatory
without requiring extensive typing.

Highlighted below are some of the ways you can use the API to establish connections and discover services and
characteristics from peer devices. For descriptions of the full API, go to the next sections on
:doc:`btshell_GAP` and :doc:`btshell_GATT`.

.. contents::
   :local:
   :depth: 2

.. toctree::
   :hidden:
   :titlesonly:

   GAP <btshell_GAP>
   GATT <btshell_GATT>
   btshell_advdata

Set device address.
~~~~~~~~~~~~~~~~~~~

On startup, btshell has the following identity address configuration:

-  Public address: None
-  Random address: None

The below ``set`` commands can be used to change the address configuration:

::

    set addr_type=public addr=<device-address>
    set addr_type=random addr=<device-address>

For example:

::

    set addr_type=public addr=01:02:03:04:05:06
    set addr_type=random addr=c1:aa:bb:cc:dd:ee

The address configuration can be viewed with the ``gatt-show-addr`` command, as follows:

::

    gatt-show-addr
    public_id_addr=01:02:03:04:05:06 random_id_addr=c1:aa:bb:cc:dd:ee

Initiate a direct connection to a device
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this case, your board is acting as a central and initiating a connection with another BLE device. The example
assumes you know the address of the peer, either by scanning for available peers or because you have set up the peer
yourself.

.. code-block:: none
    :emphasize-lines: 1

    connect peer_addr=d4:f5:13:53:d2:43
    connection established; handle=1 our_ota_addr_type=0 our_ota_addr=0a:0b:0c:0d:0e:0f out_id_addr_type=0 our_id_addr=0a:0b:0c:0d:0e:0f peer_addr_type=0 peer_addr=43:d2:53:13:f5:d4 conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

The ``handle=1`` in the output indicates that it is connection-1.

Configure advertisements to include device name
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this case, your board is acting as a peripheral.

With Extended Advertising enabled (should be executed after advertise-configure):

::

    advertise-set-adv-data name=<your-device-name>

With Extended Advertising disabled:

::

    set-adv-data name=<your-device-name>

Begin sending undirected general advertisements
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this case, your board is acting as a peripheral.

With Extended Advertising enabled:

::

    advertise-configure connectable=1 legacy=1 scannable=1
    advertise-start

With Extended Advertising disabled:

::

    advertise conn=und discov=gen

Show established connections.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    gatt-show-conn

Discover and display peer's services, characteristics, and descriptors.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is how you discover and then display the services of the peer you established earlier across connection-1.

.. code-block:: none
    :emphasize-lines: 1,2

    gatt-discover-full conn=1
    gatt-show
    [ts=132425ssb, mod=64 level=2] CONNECTION: handle=1 addr=d4:f5:13:53:d2:43
    [ts=132428ssb, mod=64 level=2]     start=1 end=5 uuid=0x1800
    [ts=132433ssb, mod=64 level=2]     start=6 end=16 uuid=0x1808
    [ts=132437ssb, mod=64 level=2]     start=17 end=31 uuid=0x180a
    [ts=132441ssb, mod=64 level=2]     start=32 end=65535 uuid=00000000-0000-1000-1000000000000000


Read an attribute belonging to the peer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    gatt-read conn=1 attr=21

Write to an attribute belonging to the peer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    gatt-write conn=1 attr=3 value=0x01:0x02:0x03

Perform a passive scan
~~~~~~~~~~~~~~~~~~~~~~

This is how you tell your board to listen to all advertisements around it. The duration is specified in ms.

::

    scan duration=1000 passive=1 filter=no_wl
