

Theory and Examples
==========================
This page describes basic concepts of one-dimensional ground response analysis and the usage transfer function. For more details, the user is encounged to read :cite:`Kramer1996`.


One-dimensional ground response analysis
--------------------------------------------
Evaluation of ground response is one of the most common tasks in geotechnical earthquake engineering. Ground response analyses are used to predict ground surface 
motions to develop design response spectra, to evaluate potential for liquefaction, and to determine the earthquake induced load to super structures. As seismic 
body waves travel through different geologic units, they are reflected and refracted when reaching boundaries. One-dimensional ground response analyses are based
on the assumption that all boundaries are horizontal and that the response of a soil deposit is predominantly caused by SH-waves propagating vertically from the 
underlying bedrock. 


Transfer function
-------------------

An important method for ground response analysis is based on the use of transfer function, that determines how each frequency in the bedrock (input) motion is 
amplified, or deamplified, by soil deposit. Thus, the transfer function may be viewed as a filter that acts upon some input signal to produce an output signal. 
For one dimensional site response analysis, the transfer function can be used to calcualte various response parameters, such as acceleration, velocity, 
displacement, shear stress, and shear strain, under an input motion parameter such as bedrock acceleration. Because this method relies on the principle of 
superposition, this analysis is limited to the analysis of linear system.

The basic steps are as following:

#. Obtain Fourier series for the applied motion using Fast Fourier Transform (FFT).

#. Multiply the Fourier series by the appropriate value of the transfer function at each frequency.

#. Express the output motion in time domain by obtaining inverse Fourier transform of the output motion using inverse FFT.

The greatest amplification will occur approximately at the lowest natural frequency, also know as the **fundamental frequency**:

.. math::
	\omega_0 = \frac{\pi v_s}{2H}
	
The period of vibration corresponding to the fundamental frequency is called the **characteristic site period**:

.. math::
	T_s = \frac{2\pi}{\omega_0} = \frac{4H}{v_s}

Evaluation of Transfer function
-------------------------------------------------------------
:cite:`Kramer1996` presents details about evaluation of transfer functions for four different cases by varing the assumptions for soil and 
bedrock, respectively. These cases can be replicated using this tool: 

Unifom, undamped soil on rigid rock
*************************************

.. figure:: ./images/case1.png
    :scale: 20 %
    :align: center
    :figclass: align-center

    Transfer function for an uniform, undamped soil on rigid rock

Uniform, damped soil on rigid rock
*************************************

.. figure:: ./images/case2.png
    :scale: 20 %
    :align: center
    :figclass: align-center

    Transfer function for an uniform, damped soil on rigid rock

Unifom, damped soil on elastic rock
*************************************

.. figure:: ./images/case3.png
    :scale: 20 %
    :align: center
    :figclass: align-center

    Transfer function for an uniform, damped soil on elastic rock with impedance ratio = 0.5

Layered, damped soil on elastic rock
*************************************

.. figure:: ./images/case4.png
    :scale: 20 %
    :align: center
    :figclass: align-center

    Transfer function for layered, damped soil on elastic rock

.. figure:: ./images/schematic.png
    :scale: 20 %
    :align: center
    :figclass: align-center

    Schematic for a layered soil deposit on elastic rock


Rigid bedrock vs. elastic bedrock
----------------------------------------------------
Bedrock can be modeled as rigid or elastic half space and it depends on the input motion. Rigid bedrock is used to apply within motion, that is recorded
in an downhole. While elastic bedrock is used to apply rock outcrop motion, that is recorded at rock outcrop. In this tool, rigid bedrock is mimicked by
increasing stiffness (shear wave velocity and/or density) of the bedrock.


.. bibliography:: references.bib


