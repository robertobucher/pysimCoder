#!/usr/bin/env python

from glob import glob
import os

if __name__ == '__main__':
    from numpy.distutils.core import setup,Extension
 
    setup(name = '',
          version = '1.0',
          author = 'Roberto Bucher',
          author_email = 'roberto.bucher@supsi.ch',
          url = 'http://robertobucher.dti.supsi.ch',
          license = 'GPLv2',
          py_modules = ['pyqt45', 'pyqt5']
         )
