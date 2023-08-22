# UPBADOOM

**U**ncomplicated **P**re-**B**oot **A**uthentication and **DOOM** image

Just another SED PBA with the option to play fbDOOM.

> [!IMPORTANT]
> SYSLINUX has trouble booting the bzImage (kernel) built by buildroot. \
> The immediate fix is to replace it with a working one from e.g. ChubbyAnt's repo. \
> Not sure what causes this problem, might want to use a [Unified Kernel Image](https://wiki.archlinux.org/title/Unified_kernel_image) in the future instead.

The project is made using [buildroot](https://buildroot.org/) and a few basic components:

- The base Linux system
- [fbDOOM](https://github.com/maximevince/fbDOOM)
  - shareware WAD
- [sedutil-sha512 (ChubbyAnt's fork)](https://github.com/ChubbyAnt/sedutil)\*
- [Syslinux](https://wiki.syslinux.org/wiki/index.php?title=The_Syslinux_Project)
- UPBADOOM: the actual Pre-Boot Authentication program

> \* To enable the original sedutil replace `BR2_PACKAGE_SEDUTIL_SHA512=y` with `BR2_PACKAGE_SEDUTIL=y` in `buildroot-external/board/upbadoom/x86_64/configs/x86_64_defconfig`

## Credits and inspiration

- [opal-kexec-pba](https://github.com/jnohlgard/opal-kexec-pba)
- [DoomLinux](https://github.com/shadlyd15/DoomLinux)
- [sedutil](https://github.com/Drive-Trust-Alliance/sedutil)
- [sedutil (ChubbyAnt's fork)](https://github.com/ChubbyAnt/sedutil)

## Building

### Clone the repository

```sh
git clone https://github.com/turtureanu/UPBADOOM.git
cd UPBADOOM
```

### Run `build.sh`

```sh
chmod +x build.sh
./build.sh
```
