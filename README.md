# pysimCoder
Simulink like editor for Python

A Virtual Machine (Virtual Box) is available. Please write to me to get
the address and password to download it.

See "README" for more info

http://robertobucher.dti.supsi.ch/pysimcoder/

23.01.2019 roberto.bucher@supsi.ch

# Installation

Install dependencies
```

sudo apt install python3-venv python3-pip libxml2-dev libxmlsec1-dev libcomedi-dev python-pyqt5 python-scipy

sudo python3 ubuntu_dependency_installer.py
pip install -U numpy
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

