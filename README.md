# Memory mapping driver

An external Linux kernel module for creating mappings in certain virtual addresses bypassing standard **mmap** API. 

## Driver building

Navigate to the kernel directory and run from command line:

```bash
make
```

Also, you can immediately load driver into kernel with following command:

```bash
make load
```

To unload driver use:

```bash
make unload
```