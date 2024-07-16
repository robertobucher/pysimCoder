# NuttX MCU OS PySimCoder Target

## Setup Target

Configure and build NuttX for your platform and test
that it correctly runs in NSH configuration.

If you plan to load ELF executables at runtime
by NuttX then look into section
Enable NuttX to Run Standalone ELF Executables

When the NuttX build for given target is ready run

    make export

in the NuttX compilation/source directory.
ZIP archive with NuttX OS link kit is built.

Extract ZIP archive under pysimCoder/CodeGen/nuttx
directory and remove version suffix from
the `nuttx-export` directory name. The directory
structure should look like

    pysimCoder/CodeGen/nuttx/nuttx-export
    pysimCoder/CodeGen/nuttx/nuttx-export/include
    ...

## Create and Build Model for NuttX Target

Start pysimCoder.py and draw model. It can be test
with `sim.tmf` the first. Then select `nuttx.tmf` in
the `Block Settings` dialog entry `Template Makefile`
and run `Generate C-code`. Check command line output
if the model is build correctly.

There are more options how to load and start model.

1) load model_name.elf into generic NuttX system with exported
   symbols and start it as regular program

2) modify application entry point in NuttX configuration by change
       ``CONFIG_USER_ENTRYPOINT="nsh_main"``
   to
       ``CONFIG_USER_ENTRYPOINT="main"``
   Then compiled and linked system image starts directly linked in model.

3) left entry-point intact and register model as built-in
   command into NuttX shell (NSH). The model main()
   function is registered as built-in executable `main`,
   which can be started from NuttX shell by simple typing `main`.

## Tested Boards and Configurations

### imxRT Teensy-4.1

ARM Cortex-M7 @600 MHz, double capable FPU

Board by PJRC https://www.pjrc.com/store/teensy41.html

Tested on open-design Base Board by PiKRON
https://gitlab.com/pikron/projects/imxrt-devel/-/wikis/teensy_bb

Design files and NuttX system configuration to build system
capable to load generated model ELF files over TFTP there
https://gitlab.com/pikron/projects/imxrt-devel

The configuration setup provided in mainline NuttX development tree

    tools/configure.sh teensy-4.x:pikron-bb

### LX_CPU/LX_RoCoN LPC4088 Based Robotic Controller

ARM Cortex-M4 @120 MHz, single capable FPU only

Xilinx XC6SLX9 for motion control coprocessor

Board documentation
http://www.pikron.com/pages/products/cpu_boards/lx_cpu.html

The configuration setup provided in mainline NuttX development tree

    tools/configure.sh lx_cpu:nsh

The ETHERNET, TFTP, CAN, uLAN RS-485 and more provided by setup.
All sources and FPGA design available
https://gitlab.com/pikron/projects/lx_cpu/lx-rocon

Motion control porting to NuttX in progress.

## Enable NuttX to Run Standalone ELF Executables

If you plan to load compiled model ELF standalone executables into running system
then you need to enable `CONFIG_BOARDCTL_APP_SYMTAB` and `CONFIG_BOARDCTL_APP_SYMTAB`
to export symbols which resolve ELF file references to NuttX libC and system-calls.

There is list of required and suggested options to enable runtime ELF executables
load. Options to load executables over ETHERNET and TFTP are included as well.

    CONFIG_ARCH_RAMVECTORS=y   for runtime drivers and kernel modules registration
    CONFIG_BOARDCTL_APP_SYMTAB=y   with system calls API/symbols
    CONFIG_BOARDCTL_OS_SYMTAB=y    with "shared" libC API/symbols
    CONFIG_BUILTIN=y
    CONFIG_CLOCK_MONOTONIC=y
    CONFIG_ELF=y
    CONFIG_FS_BINFS=y
    CONFIG_FS_PROCFS=y            for ps
    CONFIG_FS_PROCFS_REGISTER=y   for drivers modules
    CONFIG_FS_ROMFS=y             for integrate /etc
    CONFIG_FS_TMPFS=y             for place to store loaded ELF programs
    CONFIG_IDLETHREAD_STACKSIZE=2048  may be needs adjust to fit the tasks stack
    CONFIG_LIBC_EXECFUNCS=y       critical to start program
    CONFIG_LIBC_STRERROR=y        may be
    CONFIG_MAX_TASKS=16           ?
    ...
    CONFIG_NET=y                 worth to load programs over ETHERNET
    CONFIG_NETDB_DNSCLIENT=y
    CONFIG_NETDEV_LATEINIT=y
    CONFIG_NETDEV_STATISTICS=y
    CONFIG_NETINIT_DHCPC=y
    CONFIG_NETINIT_NOMAC=y
    CONFIG_NETUTILS_FTPC=y
    CONFIG_NETUTILS_TELNETD=y
    CONFIG_NETUTILS_TFTPC=y     to fetch program from the server
    CONFIG_NET_ARP_SEND=y
    CONFIG_NET_BROADCAST=y
    ...
    CONFIG_NET_IPv6=y   most worth to be set for full system
    CONFIG_NET_LOOPBACK=y
    CONFIG_NET_PKT=y
    CONFIG_NET_ROUTE=y
    CONFIG_NET_SOLINGER=y
    CONFIG_NET_STATISTICS=y
    CONFIG_NET_TCP=y
    CONFIG_NET_TCPBACKLOG=y
    CONFIG_NET_TCP_KEEPALIVE=y
    CONFIG_NET_TCP_WRITE_BUFFERS=y
    CONFIG_NET_UDP=y
    ...
    CONFIG_NSH_BUILTIN_APPS=y  for registered apps in NSH binary
    CONFIG_NSH_FILEIOSIZE=512
    CONFIG_NSH_FILE_APPS=y     I am not sure now but robably to load application from files or to export entries in APP Fs
    CONFIG_NSH_LINELEN=64
    CONFIG_NSH_READLINE=y
    CONFIG_NSH_ROMFSETC=y
    ...
    CONFIG_PSEUDOTERM=y
    CONFIG_PTHREAD_CLEANUP=y
    CONFIG_PTHREAD_MUTEX_TYPES=y
    CONFIG_PTHREAD_STACK_MIN=1024  guess
    ...
    CONFIG_RR_INTERVAL=10
    ...
    CONFIG_SCHED_WAITPID=y
    CONFIG_SERIAL_TERMIOS=y
    ...
    CONFIG_SYMTAB_ORDEREDBYNAME=y needs ordered tables but faster
    CONFIG_SYSTEM_DHCPC_RENEW=y
    CONFIG_SYSTEM_NSH=y
    CONFIG_SYSTEM_NSH_STACKSIZE=4096
    CONFIG_SYSTEM_NTPC=y
    CONFIG_SYSTEM_PING6=y
    CONFIG_SYSTEM_PING=y
    CONFIG_SYSTEM_TEE=y
    CONFIG_USER_ENTRYPOINT="nsh_main"

## TimerHook

The TimerHook folder contains examples of modifications required in order to use the **nuttx_timerhook.tmf** template in NuttX.
