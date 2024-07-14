# pysimCoder

![Screenshot_2021-10-31_08-15-46](https://user-images.githubusercontent.com/8348158/139572654-7fc549da-5530-410e-b092-64104a037606.png)

pysimCoder is a Rapid Prototyping application, that can be used to generate realtime code for different targets.

At present it is possible to generate code for Linux (with or without preempt-rt) on PC and Raspberry PI, for the BRIKI card (partially on the SAMD21 uC, see www.meteca.org) and for the NuttX RTOS.

The behaviour is similar to other RPC applications like Simulink and XCos.

**pysimCoder can run in any Linux environment or in Windows using WSL.** (at the bottom of this README is explained how to install pysimCoder in WSL)

Despite the fact that the main purpose of pysimCoder is to generate RT control code, it is also possible to perfom simple simulations mixing continous time and discrete time blocks.

Since 07.09.2022 pysimCoder implements subsystems (more than one block, right mouse click) and a new save format, based on json, which replaces the previous format of the files based on XML. Old files can be loaded and saved in the new format. The new format is better readable, because it can be simply opened by a python script like this:

```
import json
import pprint

fname = 'example.dgm'

f = open(fname,'r')
msg = f.read()
f.close()
fileDict = json.loads(msg)
pprint.pprint(fileDict)
```

![Screenshot_2021-10-31_07-16-53](https://user-images.githubusercontent.com/8348158/139570719-132cbe3c-190f-401b-b754-003959d08f5a.png)

More info at the end of this file.

http://robertobucher.dti.supsi.ch/pysimcoder/

23.01.2019 roberto.bucher@supsi.ch

# Installation

A complete Makefile to install pysimCoder under Ubuntu or Debian can be found at this link

```
https://github.com/robertobucher/LinuxLabo
```

Other distirbution can be installed in similar ways.

Finally you launch the application from the command line.
The `.bashrc` may have to be reloaded beforehand.

```
pysimCoder
```

# pysimCoder as docker image

It is now possible to pull a docker image to run pysimCoder in a container.

Install docker (see https://docs.docker.com/engine/install/ubuntu/).

Download a pysimCoder image directly from the Docker Container page:

```
docker pull robertobucher/pysimcoder:latest
```

The image can be launched with
```
$ docker run --rm --env="DISPLAY" --net=host -v $XAUTHORITY:/tmp/.XAuthority -e XAUTHORITY=/tmp/.XAuthority robertobucher/pysimcoder:latest
```
At the prompt launch
```
psc
```

More info and the Dockerfile can be found at https://github.com/robertobucher/pysimCoder-Docker

# pysimCoder in Windows

pysimCoder can run in Windows using the only decent MS Windows application -> WSL2.

Simply install some additional packages download pysimCoder and istall it.

```
    sudo apt-get update
    sudo apt-det dist-upgrade
```

The required packages are the same as in this link

```
https://github.com/robertobucher/LinuxLabo
```

The installation of pysimCoder is a little different under Windows because of the impossibility to install the nrt driver.

```
git clone https://github.com/robertobucher/pysimCoder
cd pysimCoder
make addfiles
make modules
make fmu
sudo make link
make user
make alias
make full_lib
```






