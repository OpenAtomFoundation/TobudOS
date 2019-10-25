Respond to *sync* and *reset* events
------------------------------------

sync
~~~~

The NimBLE stack is inoperable while the host and controller are out of
sync. In a combined host-controller app, the sync happens immediately at
startup. When the host and controller are separate, sync typically
occurs in under a second after the application starts. An application
learns when sync is achieved by configuring the host's *sync callback*:
``ble_hs_cfg.sync_cb``. The host calls the sync callback whenever sync
is acquired. The sync callback has the following form:

.. code-block:: cpp

    typedef void ble_hs_sync_fn(void);

Because the NimBLE stack begins in the unsynced state, the application
should delay all BLE operations until the sync callback has been called.

reset
~~~~~

Another event indicated by the host is a *controller reset*. The NimBLE
stack resets itself when a catastrophic error occurs, such as loss of
communication between the host and controller. Upon resetting, the host
drops all BLE connections and loses sync with the controller. After a
reset, the application should refrain from using the host until sync is
again signaled via the sync callback.

An application learns of a host reset by configuring the host's *reset
callback*: ``ble_hs_cfg.reset_cb``. This callback has the following
form:

.. code-block:: cpp

    typedef void ble_hs_reset_fn(int reason);

The ``reason`` parameter is a :doc:`NimBLE host return
code <../ble_hs/ble_hs_return_codes>`.

Example
~~~~~~~

The following example demonstrates the configuration of the sync and
reset callbacks.

.. code-block:: cpp

    #include "sysinit/sysinit.h"
    #include "console/console.h"
    #include "host/ble_hs.h"

    static void
    on_sync(void)
    {
        /* Begin advertising, scanning for peripherals, etc. */
    }

    static void
    on_reset(int reason)
    {
        console_printf("Resetting state; reason=%d\n", reason);
    }

    int
    main(void)
    {
        /* Initialize all packages. */
        sysinit();

        ble_hs_cfg.sync_cb = on_sync;
        ble_hs_cfg.reset_cb = on_reset;

        /* As the last thing, process events from default event queue. */
        while (1) {
            os_eventq_run(os_eventq_dflt_get());
        }
    }
