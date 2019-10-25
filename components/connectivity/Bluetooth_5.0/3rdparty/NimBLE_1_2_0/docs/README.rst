NimBLE Bluetooth Stack Documentation
#################################

This folder holds the documentation for the NimBLE Bluetooth stack from the
`Apache Mynewt`_ project. It is  built using `Sphinx`_.
The source code also contains inline comments in `Doxygen`_
format to document the APIs.

The complete project documentation can be found at `mynewt documentation`_

.. contents::

Writing Documentation
=======================

See: https://github.com/apache/mynewt-documentation#writing-documentation

Previewing Changes
==========================

In order to preview any changes you make you must first install a Sphinx
toolchain as described at https://github.com/apache/mynewt-documentation#id3.
 Then:

.. code-block:: bash

  $ cd docs
  $ make clean && make preview && (cd _build/html && python -m SimpleHTTPServer 8080)

.. _Apache Mynewt: https://mynewt.apache.org/
.. _mynewt documentation: https://github.com/apache/mynewt-documentation
.. _Sphinx: http://www.sphinx-doc.org/
.. _Doxygen: http://www.doxygen.org/
