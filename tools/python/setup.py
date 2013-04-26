#!/usr/bin/env python

from distutils.core import setup, Extension
 
module1 = Extension('core', sources = ['picdisk.c','picos.py.c'], include_dirs=['../..',"."], define_macros=[("USE_PYTHON","1"),("HAVE_CONFIG_H","1")])
 
setup (name ='picos',
       version = '0.6',
       url = 'http://wiki.davecoss.com/Picos',
       license = 'GPL v3',
       description = 'Utility functions for working with PICOS',
       author = 'David Coss, Ph.D.',
       author_email='david@davecoss.com',
       packages = ['picos'],
       ext_package = 'picos',
       ext_modules = [module1])
