# pysimCoder

pysimCoder is a Rapid Prototyping application, that can be used to generate realtime code for different targets.

At present it is possible to generate code for Linux (with or without preempt-rt) on PC and Raspberry PI, for the BRIKI card (partially on the SAMD21 uC, see www.meteca.org) and for the NuttX RTOS.

The behaviour is similar to other RPC applications like Simulink and XCos.

Despite the fact that the main purpose of pysimCoder is to generate RT control code, it is also possible to perfom simple simulations mixing continous time and discrete time blocks.

A Virtual Machine (Virtual Box) is available. Please write to me to get
the address and password to download it.

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

It is now possible to generate a docker container and image to run pysimCoder:

Install docker (see https://docs.docker.com/engine/install/ubuntu/).

Enter the pysimCoder folder and launch
```
docker build  -t pysimc ."

docker run -it pysimc
```

In the bash launch:
```
make -f make4docker.mak
exit
```

After this operations it is possible to generate a new image with pysimCoder installed.
Get the container ID with
```
docker container ls -a
```

Generate the new image
```
docker commit <CONTAINER ID> pysimcoder
```

Now it is possible to launch the docker simply with the command:
```
docker run -it --rm --env="DISPLAY" --net=host -v $XAUTHORITY:/tmp/.XAuthority -e XAUTHORITY=/tmp/.XAuthority pysimcoder
```

I've tested this chain under Debian bookworm by installing the bullseye version of docker.




