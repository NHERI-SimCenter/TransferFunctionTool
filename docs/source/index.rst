.. Transfer Function Tool documentation master file, created by
   sphinx-quickstart on Thu Sep  3 14:06:36 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to the Transfer Function Tool (TFT) documentation!
==========================================================

What is the Transfer Function Tool (TFT)
----------------------------------------
The SimCenter Transfer Function Tool (TFT) focuses on the application of the transfer function principle to the propagation of waves in one dimensional layered soil profiles. Taking advantage of the FFT, and simplicity in evaluating the transfer function for a soil profile, the tool demonstrates how to calculate an output (surface) motion induced by a given input (bedrock) motion.
The tool allows users define layered soil profiles underlain by rigid/elastic rock and change the
characteristics of each layer, including thickness, shear wave velocity, density, and damping ratio of
each layer. The user can also choose from various predefined input (rock) motions or supply specific motions.
By changing the soil profile and input motion, the user can examine how each frequency in the input motion is
amplified, or de-amplified by the soil deposit. Users can then develop a clear understanding of the effect of
soil properties on wave propagation through layered media and its application to site response analysis.

.. toctree::
   :maxdepth: 2
   :caption: Contents:
   
   quickstart
   theory
   how_to_build
   license


