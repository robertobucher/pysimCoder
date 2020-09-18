import os

path = os.environ.get('STM32_HOME') + '/objects/obj/'

fnin = open('list1','r')
fnout = open('objects.list1','w')
for el in fnin:
    fnout.write(path+el)
fnin.close()
fnout.close()

fnin = open('list2','r')
fnout = open('objects.list2','w')
for el in fnin:
    fnout.write(path+el)
fnin.close()
fnout.close()



