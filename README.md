# Guncon3 Kernel Driver
Linux kernel driver for the Guncon3 light gun from Namco.

I have only tested this module on Ubuntu 14.04 and there are couple of bugs, some work is still required to make it as stable as I want. 

### Build and Installation

Build and install using `make`

``` shell
$ make
$ sudo make modules_install 
$ sudo depmod -a # you may also need to run depmod
```

and enable it with `modprobe`

``` shell
$ sudo modprobe guncon3
```


### Usage

Once the module is installed the Guncon3 should appear as a regular joystick that you should be able to see with `jstest-gtk` or similar. 
I have not setup MAME to use this light gun yet, when I have then I will put some instructions ... or if someone else does it before me PRs are always welcome.
