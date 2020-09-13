
.. _lbl-how-to-build:

How to Build
=============


To build the tool, follow the below instructions:

#. Install `Qt <https://www.qt.io/>`_. Qt is free for open source developers. 

#. Download and install the qwt library following instructions `here <https://qwt.sourceforge.io/qwtinstall.html>`__.

#. Download and install the MKL library following instructions `here <https://software.intel.com/content/www/us/en/develop/tools/math-kernel-library.html>`__.

#. Download this repository from `Github <https://github.com/NHERI-SimCenter/TransferFunctionTool>`_.

#. Download SimCenterCommon repository from `Github <https://github.com/NHERI-SimCenter/SimCenterCommon>`__.

#. Open the file TransferFunctionTool.pro using Qt Creator, change path to the qwt and MKL libraies. 

#. Build |appName| by clicking the Run button.


Alternative Build Method Using Conan:

#. Install `Qt <https://www.qt.io/>`_. Qt is free for open source developers. 

#. Download and install the qwt library following instructions `here <https://qwt.sourceforge.io/qwtinstall.html>`__.

#. Install MKL library using the C/C++ package manager Conan
   
   * Install conan with `pip <https://docs.conan.io/en/latest/installation.html>`_ (require python) 
   
   * Add necessary remotes to conan enviroment using:
   
	::
	   
	   conan remote add simcenter https://api.bintray.com/conan/nheri-simcenter/simcenter
	   conan remote add conan-community https://api.bintray.com/conan/conan-community/conan
	   conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
	   	   
#. Download this repository from `Github <https://github.com/NHERI-SimCenter/TransferFunctionTool>`__.

#. Open the file TransferFunctionTool.pro using Qt Creator, change path to the qwt library. 

#. Run qmake. 

#. Build |appName| by clicking the Run button.   