#!/usr/bin/python3

import os
import re

source = '/home/bucher/sviluppo/MPLABXProjects/extDevices.X/mcc_generated_files'
dest = os.environ.get('PYSUPSICTRL') + '/CodeGen/dsPIC'

folders = ['adc', 'pwm', 'qei', 'uart']

for fold in folders:
    mainfold = source + '/' + fold

    # Include files
    files = files = os.listdir(mainfold)

    for el in files:
        if el.endswith('.h'):
            print('Modifying ' + el)
            fname = mainfold + '/' + el
            f = open(fname, 'r', encoding='latin-1')
            txt = f.readlines()
            f.close()

            fname = dest + '/include/' + el
            f = open(fname, 'w', encoding='latin-1')
            for line in txt:
                new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
                f.writelines(new_line)
            f.close()

    # Sources
    srcfold = mainfold + '/src'
    files = files = os.listdir(srcfold)

    for el in files:
        if el.endswith('.c'):
            print('Modifying ' + el)
            fname = srcfold + '/' + el
            f = open(fname, 'r', encoding='latin-1')
            txt = f.readlines()
            f.close()

            fname = dest + '/devices/' + el
            f = open(fname, 'w', encoding='latin-1')
            for line in txt:
                new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
                f.writelines(new_line)
            f.close()

    sysHfiles = ['clock.h', 'clock_types.h', 'interrupt.h', 'interrupt_types.h', 'pins.h', 'system.h', 'system_types.h']
    sysCfiles = ['clock.c', 'interrupt.c', 'pins.c']

    mainfold = source + '/system'
    for el in sysHfiles:
        if el.endswith('.h'):
            print('Modifying ' + el)
            fname = mainfold + '/' + el
            f = open(fname, 'r', encoding='latin-1')
            txt = f.readlines()
            f.close()

            fname = dest + '/include/' + el
            f = open(fname, 'w', encoding='latin-1')
            for line in txt:
                new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
                f.writelines(new_line)
            f.close()

    srcfold = mainfold + '/src'
    for el in sysCfiles:
        if el.endswith('.c'):
            print('Modifying ' + el)
            fname = srcfold + '/' + el
            f = open(fname, 'r', encoding='latin-1')
            txt = f.readlines()
            f.close()

            fname = dest + '/devices/' + el
            f = open(fname, 'w', encoding='latin-1')
            for line in txt:
                new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
                f.writelines(new_line)
            f.close()

    fname = srcfold + '/' + 'system.c'
    print('Modifying system.c')
    f = open(fname, 'r', encoding='latin-1')
    txt = f.readlines()
    f.close()
    fname = dest + '/devices/system.c'
    f = open(fname, 'w', encoding='latin-1')
    for line in txt:
        if not 'dmt' in line and not 'DMT' in line:
            new_line = re.sub(r'#include\s+["<](?:.*/)?([^">]+\.h)[">]', r'#include <\1>', line)
            f.writelines(new_line)
    f.close()







