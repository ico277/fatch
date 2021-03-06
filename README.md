# fatch
A fast command line system information tool written in C.

# Compilation
## With GPU support
### Dependencies
Fatch needs libpci to search for installed GPUs.

Debian* (apt) `libpci-dev`

Arch* (pacman) `pci-utils`

*: and based distros

### Building

Now to compile it run

```bash
make build
```

and then to install (you might need root permission so it might be necessary to run with `sudo`)

```bash
make install
```
## Without GPU support
### Dependencies
Fatch has no dependencies without GPU support.

### Building

Now to compile it run

```bash
make build_nopci
```

and then to install (you might need root permission so it might be necessary to run with `sudo`)

```bash
make install
```

# Credit
This tool uses ASCII art from [neofetch](https://github.com/dylanaraps/neofetch) and [pfetch](https://github.com/dylanaraps/pfetch).
