
.. _lbl-how-to-build:

How to Build
=============


To build the tool follow the instructions below:

#. Install `Qt <https://www.qt.io/>`_. Qt is free for open source developers. 

#. Download and install the qwt library following the instructions `here <https://qwt.sourceforge.io/qwtinstall.html>`__.

#. Download and install the MKL library following the instructions `here <https://software.intel.com/content/www/us/en/develop/tools/math-kernel-library.html>`__.

#. Download this repository from `Github <https://github.com/NHERI-SimCenter/TransferFunctionTool>`_.

#. Download the SimCenterCommon repository from `Github <https://github.com/NHERI-SimCenter/SimCenterCommon>`__.

#. Open the file TransferFunctionTool.pro using Qt Creator, and change the path to the appropriate qwt and MKL folders as installed in your system. 

#. Build the |appName| by clicking the Run button.


Alternative build method using Conan:

#. Install `Qt <https://www.qt.io/>`_. Qt is free for open source developers. 

#. Download and install the qwt library following the instructions `here <https://qwt.sourceforge.io/qwtinstall.html>`__.

#. Install the MKL library using the C/C++ package manager Conan
   
   * Install conan with `pip <https://docs.conan.io/en/latest/installation.html>`_ (requires python) 
   
   * Add the necessary remotes to the conan enviroment using:
   
	::
	   
	   conan remote add simcenter https://api.bintray.com/conan/nheri-simcenter/simcenter
	   conan remote add conan-community https://api.bintray.com/conan/conan-community/conan
	   conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
	   	   
#. Download this repository from `Github <https://github.com/NHERI-SimCenter/TransferFunctionTool>`__.

#. Open the file TransferFunctionTool.pro using Qt Creator, and change the path to the appropriate qwt folder as installed in your system. 

#. Run qmake. 

#. Build the |appName| by clicking the Run button.   
