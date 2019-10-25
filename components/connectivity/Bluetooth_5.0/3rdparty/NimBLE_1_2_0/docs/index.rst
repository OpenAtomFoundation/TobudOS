BLE User Guide
----------------

Apache Mynewt offers the world's first fully open-source Bluetooth Low
Energy (BLE) or Bluetooth Smart stack fully compliant with Bluetooth 5
specifications with support for Bluetooth Mesh. It is called NimBLE.

BLE technology operates in the unlicensed industrial, scientific and
medical (ISM) band at 2.4 to 2.485 GHz which is available in most
countries. It uses a spread spectrum, frequency hopping, full-duplex
signal. BLE FHSS employs 40 2-MHz-wide channels to ensure greater
reliability over longer distances. It also features 0-dBm (1 mW) power
output and a typical maximum range of 50 meters. With Bluetooth 5
specification range can be increased 4 times and speed 2 time.

.. toctree::
   :hidden:
   :titlesonly:

   ble_sec
   ble_setup/ble_setup_intro
   ble_hs/ble_hs
   btshell Usage API <btshell/btshell_api>
   mesh/index

.. contents::
    :local:
    :depth: 2

Note that BLE is not compatible with standard Bluetooth.

Features
~~~~~~~~

NimBLE complies with Bluetooth Core Specification 5.0 which makes it an
ideal wireless technology for the Internet of Things (IoT).

-  LE Advertising Extensions
-  2Msym/s PHY for higher throughput
-  Coded PHY for LE Long Range
-  High Duty Cycle Non-Connectable Advertising
-  Channel Selection Algorithm #2 to utilize channels in more efficient
   way.
-  LE Privacy 1.2 for frequent changes to the device address to make it
   difficult to track for outsiders
-  LE Secure Connections featuring FIPS-compliant algorithms.
-  LE Data Length Extension for higher throughput
-  **Coming Soon**: Assigning an Internet Protocol (IP) address
   (complaint with the IPv6 or 6LoWPAN standard) to a Bluetooth device
   through Internet Protocol Support Profile (IPSP)

The Bluetooth 5 is backward compatible with previous Bluetooth version
4.2 which is also supported by Apache Mynewt.

Bluetooth Mesh features
~~~~~~~~~~~~~~~~~~~~~~~

Bluetooth Mesh is a great addition to and opens a wide range of new
possibilities for the IoT connectivity space. NimBLE fully supports the
following Bluetooth Mesh features:

-  Advertising and GATT bearers
-  PB-GATT and PB-ADV provisioning
-  Foundation Models (server role)
-  Relay support
-  GATT Proxy

Components
~~~~~~~~~~

A Bluetooth low energy stack (NimBLE included) consists of two
components with several subcomponents:

-  **Controller**

   -  **Physical Layer**: adaptive frequency-hopping Gaussian Frequency
      Shift Keying (GFSK) radio using 40 RF channels (0-39), with 2 MHz
      spacing.
   -  **Link Layer**: with one of five states (Standby, Advertising,
      Scanning, Initiating, Connection states) active at any time

-  **Host**

   -  **Logical Link Control and Adaptation Protocol (L2CAP)**: provides
      logical channels, named L2CAP channels, which are multiplexed over
      one or more logical links to provide packet segmentation and
      reassembly, flow control, error control, streaming, QoS etc.
   -  **Security Manager (SM)**: uses Security Manager Protocol (SMP)
      for pairing and transport specific key distribution for securing
      radio communication
   -  **Attribute protocol (ATT)**: allows a device (*Server*) to expose
      certain pieces of data, known as *Attributes*, to another device
      (*Client*)
   -  **Generic Attribute Profile (GATT)**: a framework for using the
      ATT protocol to exchange attributes encapsulated as
      *Characteristics* or *Services*
   -  **Generic Access Profile (GAP)**: a base profile which all
      Bluetooth devices implement, which in the case of LE, defines the
      Physical Layer, Link Layer, L2CAP, Security Manager, Attribute
      Protocol and Generic Attribute Profile.
   -  **Host Controller Interface (HCI)**: the interface between the
      host and controller either through software API or by a hardware
      interface such as SPI, UART or USB.

Subsequent chapters in this manual will go into the details of the
implementation of each component, APIs available, and things to consider
while designing a NimBLE app.

Example NimBLE projects
~~~~~~~~~~~~~~~~~~~~~~~

Mynewt comes with two built-in projects that allow users to play with
NimBLE, try the tutorials out with, and see how to use available
services.

1. **btshell** : A simple shell application which provides a basic
   interface to the host-side of the BLE stack.
2. **bleprph**: A basic peripheral device with no user interface. It
   advertises automatically on startup, and resumes advertising whenever
   a connection is terminated. It supports a maximum of one connection.
3. **blemesh**: A sample application for Bluetooth Mesh Node using
   on/off model.
