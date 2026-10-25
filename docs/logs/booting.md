# Booting the STM32 Blue Pill

## The Linker Script
The Linker Script defines the memory regions in both RAM and FLASH areas, Where should the code and data go, Where should the vector table be stored etc is defined by the linker script.
Here are some essential part of the linker script

* ENTRY(NAME)
This keyword makes sure the code starts executing at the given address as given by NAME.
This is just metadata to be used by debugger, The actually loading of PC is handled differently by different board
In this example, when the reset is hit, The STM32 read the Reset_Handler address from the isr_vector table defined for different types of interrupts and service routine.

* MEMORY
The Memory region is defined in this section, unlike general purpose computer, small microcontroller have a special FLASH region where the execution start before it starts using the RAM at all
The linker script in this case essential track two address spaces namely, VMA(Virtual Memory Address) and LMA(Load Memory Address)
The Virtual memory address is the RAM and where new data is created and Modified

If one looks at each of the section we can see the loading here
* > RAM, This adjusts the RAM address counter by adding the space occupied by the section to the base address
* > FLASH, Similarly to the Previous one, The base of flash is added to the space occupied by the section
* '.' is called the location counter. The location counter is incremented based on the size of the sections
* >RAM AT> FLASH, This creates space for the section in both RAM and FLASH, Generally in microcontrollers, The data section is read from the FLASH and stored in the RAM before execution begins, This has to be performed by the programmer in assembly
* The first entry in the syntax that is `> RAM` defines which is the Virtual Memory Address(VMA) and to access the exact address of a point in this address space is `ADDR()` function, This function allows us to access a particular exact address of a point in RAM or VMA.
* The second entry in the syntax that is `AT> FLASH` defines which is the Load Memory Address(LMA) and to access the exact address of a point in this address space is `LOADADDR()` function, This function allows us to access a particular exact address of a point in FLASH or LMA
* FUN FACT: Doing `>FLASH AT> RAM` will make the LMA as RAM and the VMA as FLASH

* ALIGN(4) and ALIGN(8)
Align the address to the 4 bytes Edge or 8 bytes Edge before allocating space in the binary for this section

* Essential Sections
* .text: This is the code section where the executing code is stored and separated from the data, This is primarily kept in flash
* .isr_vector: This is where the ISR Vector table is kept for interrupt handling
* .rodata: This is where the read only constant data is kept, This data is primarily kept in flash
* .data: This where initialized variables go to, This is stored both in flash and RAM.
* .bss: This is a very crucial section for uninitialized sections
The .bss sections requires it's own section
### .bss section
* The .bss section stores the metadata only and actually occupies zero bytes in the executable
* This allows the program to be quickly loaded into flash or RAM.
* Example code
```c
int init_var[2] = {1,2};
int uninit_var[1024];
```
* The un initialized variable will store around 1024 * 4 bytes of data of full of zeros
* This makes the program files incredibly difficult to store and copy into RAM or FLASH
* Hence, The GNU Linker will only actually store the values of _sbss and _ebss, where _sbss is the start of the bss section and _ebss is the end of the bss section.
* When the program is loaded into the RAM, the _sbss to _ebss must be manually filled with zeros by the programmer in assembly.
* Hence the program.elf or program.bin is now extremely slim and thin for it to be loaded into RAM.

### Sections and Sub Sections
```ld
*(.text)
*(.text*)
```
In the above code, the .text section tells the linker into include all the *(.text) section from the program in the text section, and the *(.text*) tells the linker to include the subsections of .text section inside the text section itself
Similar things can be said for the other section

* Here is an example of a .text section entry in assembly and .text* section entry in assembly
```assembly
.section .text
/* code goes here */

.section .text.Reset_Handler
/*code goes here*/
```

In the above given program the .text section has some code which is placed in the *(.text) section by the linker and the .text.Reset_Handler is a subsection of the .text section which is placed in the *(.text*) by the linker script

## Further important keywords
* KEEP()
This is keyword used to prevent optimization by the linker, Modern compilers and linkers use a lot of heavy optimization by stripped out unused functions out of the binary, but there are several use cases we would want those functions there even when we don't use them, For eg, The ISR vector table, even if our code never references that, when a dynamic exception occurs the Table must be present, hence we use KEEP to prevent such crashes.

* .init and .fini
These are very essential when it comes to user space code, They setup the stack, variables and arguments, init memory segments, calling main and handling exit codes.
There are filed called crt0.S file which do this and are directly compiled into simple user space C programs.
In Baremetal and no OS C programming the main functions, bss clearing setting stack space is already done by the programmer inn assembly, hence such sections are not required.

# The Assembly Program
The assembly program for start up is given here `../../startup.S`, This file is the initial boot code required for the STM32, The STM32 has a RAM and a FLASH, During boot the initial global variables are stored in the FLASH space, we have to copy this into the RAM for easier access.
The assembly program performs the following in almost all MCU's
* Define ISR Vector Table, This section is used for Interrupt handling
This section is particularly designed to be a part of the `.isr_vector` section, It has a subsection called `gpfn_vectors`, This section is particularly important
- The `a` in this section represent allocatable, meaning just data and not executable code
- The `.type` shows the type of the section, It is current given as `%object`, This gives some information to the GDB and other debugging tools
- Generally every predefine section is given `%progbits`, which mean that this section occupies physical space in the executable unlike `%nobits`, which tells us that this section is full of zeros, We must zero it out after booting and the section does not take up space in .bin or .elf files
- Similarly the `Reset_Handler` can also be define, It is a subsection of `.text` so, `.text.Reset_Handler` is used, It is a function, hence `.type Reset_Handler, %function`
- The `ax` for the reset handler, is given because the section `Reset_Handler` within the `.text` section is also an executable section.
- The `.weak` directive, is used to make sure the symbol can be reused, if we ever define a symbol again called `Reset_Handler` is created, This symbol can be dropped.
- `.size` directive is used to define the sizes of sections which can be used by debugging tools
- The General format for the `.size` directive is
```assembly
.size function_name, . - function_name
/* (. - function_name) gives us the size of the function*/
```
```
```


* Copy the data from the FLASH to the RAM
- This is where the `_sidata = LOADADDR(.data)` comes in, This line gives us the physical address of the start of the `.data` section in the FLASH region.
- We use this section to copy the data into the RAM, using the functions `CopyDataInit` and `LoopCopyDataInit`

* We then make use of the `_sbss` and the `_ebss` to zero out the bss section using the functions `FillZerobss` and `LoopFillZerobss`.

* Finally we call the main function in the `entry.c` file




