# Driver nrt.ko

The driver nrt.ko allows the normal user to lauch some
speciali commands for changing scheduling policy and priority and
other operations usually only permitted to superusers.

```
make
sudo make install
sudo make dev
insmod /lib/modules/`uname -r`/misc/nrt.ko
```

In programs simply put this lines or similar in your source before using
the threads

```
  uid = geteuid();
  if (uid!=0){
    fd=open("/dev/nrt",O_RDWR);
    if (fd<0){
      printf("This SW needs superuser privilegies to run!\n");
      exit(1);
    }
    close(fd);
  }
```

to set the new permissions.
