#!/usr/bin/python3

import glob
import os

mylist = [f for f in glob.glob('*-eps-converted-to.pdf')]

for el in mylist:
    el1 = el.split('-')
    el1 = el1[0]

    el2 = el1+'.eps'
    el3 = el1+'.pdf'
    cmd1 = 'mv ' + el + ' ' + el3
    print(cmd1)
    os.system(cmd1)
    cmd2 = 'rm ' + el + ' ' + el2
    print(cmd2)
    os.system(cmd2)

    

    
