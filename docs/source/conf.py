# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'TransferFunctionTool'
copyright = '2020, NHERI SimCenter'
author = 'Long Chen'

# The full version, including alpha/beta/rc tags
release = '1.0.0'

rst_prolog = """
.. |appName| replace:: *Tranfer Function Tool*
.. |full tool name| replace:: Tranfer Function Tool 
.. |short tool name| replace:: TFT
.. |short tool id| replace:: TFT
.. |tool github link| replace:: `TFT Github page`_
.. _TFT Github page: https://github.com/NHERI-SimCenter/TransferFunctionTool
.. |tool version| replace:: 1.0.0
"""

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_logo = './_static/images/SimCenter_TFT_logo.png'

html_css_files = [
	'css/custom.css'
]