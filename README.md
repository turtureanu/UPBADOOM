# UPBADOOM

**U**ncomplicated **P**re-**B**oot **A**uthentication and **DOOM** image for Self-Encrypting Drives.

Just another LinuxPBA with the option to play DOOM.

The project is made out of a few basic components:

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

As a one-liner:

```sh
curl -L https://buildroot.net/downloads/buildroot-$(git ls-remote --tags https://git.buildroot.net/buildroot.git | awk '!/{}/ {print $2}' | awk -F/ '!/rc/ {print $NF}' | grep -E '^[0-9]{4}' | sort -V | tail -n 1).tar.gz | tar xvzf - && git clone https://github.com/turtureanu/upbadoom.git && cd buildroot-* && make BR2_EXTERNAL=../upbadoom/buildroot-external/ x86_64_defconfig && make
```

### Configure `buildroot`

#### Download `buildroot`

Latest stable:

```sh
curl -L https://buildroot.net/downloads/buildroot-$(git ls-remote --tags https://git.buildroot.net/buildroot.git | awk '!/{}/ {print $2}' | awk -F/ '!/rc/ {print $NF}' | grep -E '^[0-9]{4}' | sort -V | tail -n 1).tar.gz | tar xvzf -
```

#### Clone this repository

```sh
git clone https://github.com/turtureanu/upbadoom.git
```

There are a few things that you **NEED TO CHANGE**:

- `kexec` options
- Syslinux display resolution

To change the `kexec` commands, edit the `upbadoom/upbadoom.cpp` file:

```cpp
// kexec
const std::string KEXEC_IMAGE =
    "/boot/vmlinuz-linux-zen"; // path to kernel image

const std::string KEXEC_CMDLINE =
    "rw root=UUID=d5425ce5-fece-441f-92c0-440388cc9490 "
    "rootflags=subvol=@ libata.allow_tpm=1 "
    "resume=UUID=80489fcc-a413-4f7e-88f2-3c650633ab57"; // command line
                                                        // options to pass
                                                        // to kernel
                                                        // (append)
const std::string KEXEC_INITRD =
    "/boot/initramfs-linux-zen.img"; // path to initrd (initramfs)

```

And to change the Syslinux display resolution, edit `buildroot-external/board/upbadoom/x86_64/syslinux.cfg`:

```sh
    append loglevel=0 libata.allow_tpm=1 video=efifb:width:1920,height:1080 quiet ro
```

#### Use the external tree

```sh
cd buildroot-*
make BR2_EXTERNAL=../upbadoom/buildroot-external/ x86_64_defconfig
```

### Build

```sh
make
```
