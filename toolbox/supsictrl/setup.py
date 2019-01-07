#!/usr/bin/env python

from glob import glob
import os

if __name__ == '__main__':
    from numpy.distutils.core import setup,Extension
 
    setup(name = 'supsictrl',
          version = '0.3',
          description = 'Python SUPSI Control Systems Library',
          author = 'Roberto Bucher',
          author_email = 'roberto.bucher@supsi.ch',
          url = 'http://robertobucher.dti.supsi.ch',
          license = 'GPLv2',
          requires = ['scipy', 'matplotlib', 'control'],
          package_dir = {'supsictrl' : 'src'},
          packages = ['supsictrl'],
    	  ext_package = 'supsictrl'
         )
