# How to compile the source at the first time?
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

