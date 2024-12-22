# pysimCoder ![pysimCoder](https://github.com/user-attachments/assets/11b48b40-5c70-4f6d-af4e-facab24c5563)

![image](https://github.com/user-attachments/assets/f72914c9-0979-4b80-89d5-070eb1e56c60)

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
![image](https://github.com/user-attachments/assets/e1c292ff-5377-45ca-8cb5-c65f5b3968ba)


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

    sudo usermod -aG sudo <your_username>
    touch /home/<your_username>/.bash_aliases

    sudo apt-get update && sudo apt-get upgrade -y
```

## Otherway pip inside Makefile will not work
```
    export PIP_BREAK_SYSTEM_PACKAGES=1

```
## python and pip
```
    sudo apt install make python3 python3-venv pip python3-pyqt5 python3-pyqt6
```

## required to run uic
```
    sudo apt install pyqt6-dev pyqt6-dev-tools 
    sudo apt-get install python3-pyqt5.qtsvg
```

## headers
```
    sudo apt install linux-headers-rt-amd64 linux-image-rt-amd64
```

## compilers
```
    sudo apt install build-essential gdb gcc gfortran cmake
```

## required to build `fmu`
```
    sudo apt install libxml2-dev libxslt1-dev
```

## required to build `slycot`
```
    sudo apt install -y libopenblas-dev
```

## required to build `full_lib`
```
    sudo apt install libcomedi-dev 
```

## double check if everything installed
```
    pip install -r requirements.txt
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






