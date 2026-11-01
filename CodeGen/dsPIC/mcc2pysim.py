#!/usr/bin/python3

import os
import re

source = '/home/bucher/sviluppo/MPLABXProjects/pysimC.X/mcc_generated_files'
dest = os.path.join(os.environ.get('PYSUPSICTRL'), 'CodeGen', 'dsPIC')

#folders = ['adc', 'pwm_hs', 'qei', 'timer', 'uart']
folders = ['adc', 'pwm_hs', 'qei', 'uart']

for fold in folders:
    mainfold = os.path.join(source, fold)

    # Include files
    files = files = os.listdir(mainfold)

    for el in files:
        if el.endswith('.h'):
            print('Modifying ' + el)
            fname = os.path.join(mainfold, el)
            f = open(fname, 'r', encoding='latin-1')
            txt = f.readlines()
            f.close()

            fname = os.path.join(dest, 'include', el)
            f = open(fname, 'w', encoding='latin-1')
            for line in txt:
                new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
                f.writelines(new_line)
            f.close()

    # Sources
    srcfold = os.path.join(mainfold, 'src')
    files = files = os.listdir(srcfold)

    for el in files:
        if el.endswith('.c'):
            print('Modifying ' + el)
            fname = os.path.join(srcfold, el)
            f = open(fname, 'r', encoding='latin-1')
            txt = f.readlines()
            f.close()

            fname = os.path.join(dest, 'devices', el)
            f = open(fname, 'w', encoding='latin-1')
            for line in txt:
                new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
                f.writelines(new_line)
            f.close()

    sysHfiles = ['clock.h', 'clock_types.h', 'interrupt.h', 'interrupt_types.h', 'pins.h', 'system.h', 'system_types.h']
    sysCfiles = ['clock.c', 'interrupt.c', 'pins.c']

    mainfold = os.path.join(source, 'system')
    for el in sysHfiles:
        if el.endswith('.h'):
            print('Modifying ' + el)
            fname = os.path.join(mainfold, el)
            f = open(fname, 'r', encoding='latin-1')
            txt = f.readlines()
            f.close()

            fname = os.path.join(dest, 'include', el)
            f = open(fname, 'w', encoding='latin-1')
            for line in txt:
                new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
                f.writelines(new_line)
            f.close()

    srcfold = os.path.join(mainfold, 'src')
    for el in sysCfiles:
        if el.endswith('.c'):
            print('Modifying ' + el)
            fname = os.path.join(srcfold, el)
            f = open(fname, 'r', encoding='latin-1')
            txt = f.readlines()
            f.close()

            fname = os.path.join(dest, 'devices', el)
            f = open(fname, 'w', encoding='latin-1')
            for line in txt:
                new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
                f.writelines(new_line)
            f.close()

    fname = os.path.join(srcfold, 'system.c')
    print('Modifying system.c')
    f = open(fname, 'r', encoding='latin-1')
    txt = f.readlines()
    f.close()
    fname = os.path.join(dest, 'devices', 'system.c')
    f = open(fname, 'w', encoding='latin-1')
    for line in txt:
        if not 'dmt' in line and not 'DMT' in line:
            new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
            f.writelines(new_line)
    f.close()







