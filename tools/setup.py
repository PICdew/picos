#!/usr/bin/env python

from distutils.core import setup, Extension
 
module1 = Extension('picdisk', sources = ['picdisk.c'], include_dirs=['../..'], define_macros=[("USE_PYTHON","1")])
 
setup (name = 'picdisk',
        version = '0.6',
        description = 'This is a demo package',
        ext_modules = [module1])
