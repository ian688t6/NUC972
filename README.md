# How to configure toolchain?
``` bash
# cd tools/host/toolchain
# sudo tar -xvf /host/toolchain/arm_linux_4.8.tar.gz -C /opt/nuvoton
```

``` bash
edit /etc/profile add PATH toolchain 
export PATH="/opt/hisi-linux/x86-arm/arm-hisiv300-linux/target/bin:$PATH" 
export PATH="/opt/nuvoton/arm_linux_4.8/bin:$PATH" 
```

# How to compile the source the first time?
* Get the nuvoton sdk code
``` bash
# make prepare
```

* Compile the source code
``` bash
# make
```

* Install the target, libs, etc to the rootfs dir
``` bash
# make install
```

* Pack the rootfs image
``` bash
# make rootfs
```

* Clean all the source
``` bash
# make cleanall
```

* Clean all the source and remove build dir
``` bash
# make distclean
```

# How to compile the tools?
* Build the tools
``` bash
# make build mod=tools
```
* Clean the tools
``` bash
# make clean mod=tools
```

# How to compile the kernel source?
* Build the os
``` bash
# make build mod=os
```
* Clean the os
``` bash
# make clean mod=os
```

# How to compile the boot code?
* Build the boot
``` bash
# make build mod=boot
```
* Clean the boot
``` bash
# make clean mod=boot
```

# How to compile the applications?
* Build the apps
``` bash
# make build mod=apps
```
* Clean the apps
``` bash
# make clean mod=apps
```

# ARM9 Family BSP
---
**NUC970 Link**
- [Buildroot](https://gitee.com/OpenNuvoton/NUC970_Buildroot)
- [U-boot](https://gitee.com/OpenNuvoton/NUC970_U-Boot_v2016.11)
- [Linux-3.10.x](https://gitee.com/OpenNuvoton/NUC970_Linux_Kernel)
- [Linux Applications](https://gitee.com/OpenNuvoton/NUC970_Linux_Applications)
- [Non-OS BSP](https://gitee.com/OpenNuvoton/NUC970_NonOS_BSP)
- [NuWriter](https://gitee.com/OpenNuvoton/NUC970_NuWriter)
- [NuWriter Linux CMD](https://gitee.com/OpenNuvoton/NUC970_NuWriter_CMD)

