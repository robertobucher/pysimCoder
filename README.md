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

A Virtual Machine (Virtual Box) is available. Please write to me to get the address and password to download it.

It is also possible to get a docker image with pysimCoder using

```
docker pull robertobucher/pysimcoder
```
More info at the end of this file.

http://robertobucher.dti.supsi.ch/pysimcoder/

23.01.2019 roberto.bucher@supsi.ch

# Installation

Project requires Python 3.9 runtime to be present on the system. Installation instructions 
and scripts expect Debian derived distibution and may require adjustements on other systems.

Superuser privileges are necessary for installation of the compiled drivers.

In cases when environment doesn't fulfill the aforementioned conditions,
a containerized deployment with one of the provided images is preferrable.

Install system dependencies and python libraries
```

sudo apt install gcc gfortran git cmake binutils emacs python3.9 python3-numpy \
    python3-scipy python3-sympy python3-matplotlib python3-pip jupyter-qtconsole \
    python3-lxml python3-pyqtgraph libopenblas-dev liblapack-dev libxml2-dev \
    libcomedi-dev python3-pyqt5

sudo pip install -r requirements.txt
```

Alternatively the dependencies can be installed with provided scripts:
```
sudo python3 ubuntu_dependency_installer.py
sudo python3 python_libs_install.py
```

Build and install the code as a superuser with:
```
sudo make
```

Set environment variables in user `.bashrc`
```
make user
```

You may also install modules for different targets as a normal user with:
```
make <target>
```

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

pysimCoder can run in Windows under WSL.

Simply install some additional packages download pysimCoder and istall it.

```
sudo apt-get install \
	gcc \
	gfortran \
	git \
	cmake \
	binutils \
	emacs \
	python3 \
	python3-numpy \
	python3-scipy \
	python3-sympy \
	python3-matplotlib \
	python3-pip \
	jupyter-qtconsole \
	python3-lxml \
	python3-pyqtgraph \
	libopenblas-dev \
	liblapack-dev \
	libxml2-dev \
	libcomedi-dev \
	python3-pyqt5 \
	python3-qwt \
	python3-serial
```
In **Debian** the following package should be installed for pyQt6

```
	qt6-svg-dev

```
In **Ubuntu** the following packages should be installed for pyQt6
```
    qt6-base-dev
    libqt6svg6-dev
```
Then move to a directory where to install pysimCoder and do

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







