
.. _quickstart:

Quick start
================================


Download the |appName| 
------------------------------------------------

The app can be downloaded from `here <https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community/SimCenter/Software/>`_. 

Installation is generally not required. However, if a Microsoft Visual C++ Runtime Library error is encountered, the issue can be resolved by 
installing the latest version of Microsoft Visual C++. A installer, *vc_redist.x64.exe*, is included in the zip file.

User Interface 
-------------------------------------------------

As shown in the figures below, the tool's main window consists of three major groups: a) left control group, with commands for changing "Layer Properties" and "Motion Selection", b) "Layer plot" with  selectable layers, and c) tab bar that can be switched to "Define Layer" or show "Motion Plots". In the "Layer Properties" group, the lineedits and sliders can be used to change properties (shear wave velocity, density, thickness and damping ratio) of the current active layer as indicated in the "Changing Layer" as well as highlighted in the "Layer Plot" and "Layer Table" view. The three groups are interconnected and changes in one are automatically reflected in the others. The thickness and damping ratio of the bedrock layer is not changeable since the soil layers are assumed to be on top of an infinite half space. 

.. figure:: ./images/Tab1.png
    :scale: 30 %
    :align: center
    :figclass: align-center

    User interface with "Defining Layers" tab shown
	
.. figure:: ./images/Tab2.png
    :scale: 30 %
    :align: center
    :figclass: align-center

    User interface with "Motion Plots" tab shown

Add/remove soil layers 
-------------------------------------------------

Under the Layer tab, click the "Add Layer"/"Remove Layer" button on the top of the table to add/remove a soil layer. 

Modify properties of a layer
-------------------------------------------------

Density, thickness, shear wave velocity, and damping ratio can be modified for each layer. Click on the layer plot or any row in the layer table to select a soil layer. Subsequently, lineedits and sliders will be associated to the selected layer. SI units are used for all values.

Select a different input motion 
-------------------------------------------------
Several input motions are pre-defined in this tool including earthquake recordings, harmonic vibrations, as well as a frequency sweep time series. The frequency lineedit and 
slider can be used to vary the frequency of the harmonic vibration, thus will only be enabled when this motion is selected. 

Additional motions can also be imported and viewed in this tool by checking the "Load User Motion" option. The current version only supports the JSON format. Examples of input motion JSON files can be found `in the repository <https://github.com/NHERI-SimCenter/TransferFunctionTool/tree/master/resources/motions>`_.

Lock axes 
-------------------------------------------------
By default, the axis limits of each figure are replotted after any changes in properties or motion. However, these limits can be locked by checking the "Lock Axes" checkbox. Then the 
effect of each property on the transfer function can be visualized and compared. To reset axes, simply click the "Reset Figures" button.  

