#!/usr/bin/env python

from glob import glob
import os

if __name__ == '__main__':
    from numpy.distutils.core import setup,Extension
 
    setup(name = 'myEnv',
          version = '0.1',
          description = 'Python SUPSI Simulation Library',
          author = 'Roberto Bucher',
          author_email = 'roberto.bucher@supsi.ch',
          url = 'http://robertobucher.dti.supsi.ch',
          license = 'GPLv2',
          requires = ['numpy'],
          package_dir = {'myEnv' : 'src'},
          packages = ['myEnv'],
    	  ext_package = 'myEnv'
         )
