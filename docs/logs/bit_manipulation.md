# Bit Manipulation Methods, Tricks and Tips
Bit Manipulation is over common in low level systems programming and register Manipulation,
Due to it's importance we can actually define some common methods and references to make our jobs easier

## How to set a certain bit
Setting a certain bit is very simple, The main ideas is the OR operation,
The OR operation is generally used to set some bits at the same time

Say If we wanna set nth of a register then we perform
```c
reg |= (1 << n);
```

The right hand side creates a bitmask which looks like
BITMASK: 10000...0000, Here there are n zero which when OR with the entire register allows the values of the register to pass through the 0's while only setting the nth bit.
This is because N | 0 = N;

## How to clear a certain bit
Clearing a certain bit is very simple, The main ideas is the AND-NOT Clearing
Say If we wanna clear nth of a register then we perform

```c
reg &= ~(1 << n);
```

The right hand side evaluates to,
BITMASK: 1111110111....11, Here there n ones after the given nth bit, which contains the zero, By using the property of AND, all the bits of the register pass through the ones in the bit mask when performing AND operation, This is because N & 1 = N
While the nth bit is always cleared to zero since N & 0 = 0

## How to extract a few bits from the Register

For extracting the values of a few bits from the register bits we again use the AND operation.
We perform the following bitmask

```c
value = (reg >> n) & 0x7;
```

The above code will extract a particular three bits from the register
We bit shift the register till the nth bit then, we extract the required no of bits using the AND bitmask
The generated bit mask is
BITMASK: 111, Here there are three 1's which extracts the values from the register which is already bit shifted towards the right by n.

## How to set a few bits with a few random values
Say we wanna insert 1011 to the [n+4:n] range of the register then,
1. We must first clear all the bits there by using
```c
reg &= ~(0xF << n);
```

We now have cleared the 4 bits to the left of nth bit in the register now the range [n+4:n] is filled with zero

2. We use the setting method as discussed above,
```c
reg |= (0xB << n);
```


## To Toggle a bit use the XOR operator

```c
reg ^= (1 << n);
```
