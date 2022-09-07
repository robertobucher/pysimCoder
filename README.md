# pysimCoder

![Screenshot_2021-10-31_08-15-46](https://user-images.githubusercontent.com/8348158/139572654-7fc549da-5530-410e-b092-64104a037606.png)

pysimCoder is a Rapid Prototyping application, that can be used to generate realtime code for different targets.

At present it is possible to generate code for Linux (with or without preempt-rt) on PC and Raspberry PI, for the BRIKI card (partially on the SAMD21 uC, see www.meteca.org) and for the NuttX RTOS.

The behaviour is similar to other RPC applications like Simulink and XCos.

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

Install dependencies
```

sudo apt install python3-venv python3-pip libxml2-dev libxmlsec1-dev libcomedi-dev python-pyqt5 python-scipy

sudo python3 ubuntu_dependency_installer.py
pip install -U numpy
pip install tornado
```

Install python libs
```
sudo python3 python_libs_install.py
```

Switch to superuser
```
su
```

Install additional python libs
```
pip install scikit-build
pip install cmake
```

Make
```
make
```

Exit superuser
```
exit
```

Make as user
```
make user
```

Launch application
```
pysimCoder
```

# pysimCoder as docker image

It is now possible to use a docker container and image to run pysimCoder:

Install docker (see https://docs.docker.com/engine/install/ubuntu/).

Download a pysimCoder image directly from the Docker Container page:

```
docker pull robertobucher/pysimcoder:latest
```
or
```
docker pull robertobucher/pysimcoder:novnc
```

The first image can be launched with
```
$ docker run --rm --env="DISPLAY" --net=host -v $XAUTHORITY:/tmp/.XAuthority -e XAUTHORITY=/tmp/.XAuthority robertobucher/pysimcoder:latest
```
At the prompt launch
```
psc
```

The second image (pysimcoder:novnc) runs from a web browser:
```
$ docker run --rm -it -p 8080:8080 robertobucher/pysimcoder:novnc
```
Open a browser and go to  `http://<server>:8080/vnc.html`

In the terminal launch
```
psc
```

More info and the 2 Dockerfiles can be found at https://github.com/robertobucher/pysimCoder-Docker






