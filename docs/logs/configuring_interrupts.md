# Configuring Interrupts
The external interrupts are sent to the NVIC (Nested Vectored Interrupt Controller) via the EXTI Lines, We have to Configure the EXTIx in order to get the interrupt from button and signals outside the Blue Pill.
Essentially in the ISR Vector table we have the interrupt handlers for EXTI's already when the interrupts are triggered the Program Control goes to the EXTI Interrupt Service Routines. 
Hence we have two Configuration steps
1. Configure the GPIO Properly(see ./configuring_gpio.md)
2. Configure and Unmask the Interrupts in the Registers Related to the EXTI Lines
3. Configure the NVIC (This required another manual)

## Essential Registers to Configure the EXTI
- There are essentially 15 EXTI lines for external interrupts from GPIO pins
- Each of these lines can be connected to ports from any of the pin sets
- Example, EXTI0 Line can sample interrupts from PA0, PB0 ... PG0, Similarly for EXTI1 and so on and so forth
![Diagram for the EXTI Lines](../images/EXTI_LINES.png)

- The Essential Registers are
1. RCC_APB2ENR: This register has already been discussed in configuring_gpio's, In this context we must turn on the AFIOEN bit for turning on the clock for the alternate functions of the GPIO pins

2. EXTI_IMR: External Interrupts Mask Register
3. EXTI_RTSR: External Interrupts Rising trigger selection register
4. EXTI_FTSR: External Interrupts Falling Trigger selection register
5. AFIO_EXTICRx: Alternate Function I/O External Interrupt Configuration Register

### RCC_APB2ENR
- This is just for turning on the AFIO clock, we will just have to set the bit 0 of this register

### EXTI_IMR
- This is the register which either allows or masks the given EXTI line
- If a bit is set, then the EXTI interrupt line is on
- else the Interrupts to those lines are ignored.
![EXTI_IMR Register](../images/EXTI_IMR.png)

### EXTI_RTSR
- This register Configures if they EXTI line needs to generate an interrupt if there is an Rising Edge
- If this is 1 then EXTIx line triggers interrupts at Rising Edge
- If this is 0, then no interrupts are generated at rising edge
![EXTI_RTSR Register](../images/EXTI_RTSR.png)

### EXTI_FTSR
- This register configures if the EXTI line need to generate an interrupts if there is an Falling Edge.
- If this is 1 then EXTI Line interrupts are generated at falling edge of the port
- If this is 0, then EXTI line interrupts are not generated at falling edges
![EXTI_FTSR Register](../images/EXTI_FTSR.png)


> [!NOTE]
> If both RTSR and FTSR are turned on, then the interrupts are sampled and generated for both the rising edge and the falling edge for the port.

### AFIO_EXTICRx
- This is the register which chooses which Port Set is used for a particular interrupt line,
- For eg, 0000 on the EXTI1 line chooses the PA1 as the point from whether changes are acknowledged and interrupts are generated.
- Similarly if EXTI2 line is present and we write 0010 then the PC2 is set as the point from which changes are acknowledged and interrupts are generated.

![AFIO_EXTICRx Register](../images/AFIO_EXTICRx.png)

## Steps Involved (Till Now before configuring the NVIC Directly)
- Turn on the AFIOEN Clock in RCC_APB2ENR Register
- Use the AFIO_EXTICRx to choose the correct Port for the particular EXTI lines
- Use the FTSR or RTSR Registers to Configure the Type of Interrupts (Rising Edge Int, Falling Edge Int or both)
- Use the EXTI_IMR Register to turn on the interrupt, Unmask the interrupts

> [!NOTE]
> Before doing this, one must configure the port which is about to be used as input push/pull type.

- This itself will not trigger the interrupt we have some more registers for NVIC to Unmask and Set the Priority for.

## Essential Registers to Configure NVIC(Nested Vectored Interrupt Controller)
The configuration for the NVIC is not explicitly mentioned in RM008 for the STM32Fxxxx Series for MCU, Instead we require a different manual called PM0056 for accessing configuration registers.

The NVIC has the following essential registers,

### NVIC_ISERx(Nested Vectored Interrupt Controller Interrupt Set Enable Register)
- This is the register used to unmask an interrupt line to generate interrupt for the CPU
- For eg, The bit 6 of the NVIC_ISER1 is the register used to configure the EXTI0 and bit 7 of NVIC_ISER1 the EXTI1 line.

- The register map of the NVIC looks like this, There are upto 81 configurable interrupts lines which can be used and priorities can be set.

![NVIC_ISERx_MAP](../images/NVIC_ISER_MAP.png)

- There are three usable ISERx register here, for each register, each bit represents a interrupt line. Each interrupt line can be given priority using 8 bit values in the NVIC_IPRx Register, More about this register will be discussed in the next section

### NVIC_IPRx(Nested Vectored Interrupt Controller Interrupts Priority Register)

- In a single NVIC_ISERx Register there are 32 bit, Which mean, each NVIC_IPRx register can control the Interrupt priority for only 32/8 = 4 Interrupts as configured in the NVIC_ISERx.

- If there are 81 interrupt lines, There must be atleast 81*8 = 648 bits used, Dividing this by 32 we get, 648/32 = 20.25, Atleast there must be 21 NVIC_IPRx Registers(numbered from NVIC_IPR0 to NVIC_IPR20).

- This can be observed in the below image from the manual, as given by STMMicroelectronics in PM0056.

![NVIC_IPRx_MAP](../images/NVIC_IPRx_MAP.png)

- Using this we have to configure the NVIC to get our interrupts

> [!NOTE]
> We read the RM008 Manual to understand which bit to manipulate to the properly EXTI lines to the proper NVIC_ISERx and NVIC_IPRx
> Given below shows the respective bits to be modified in the NVIC_ISERx for each EXTI Lines

![EXTI1 to NVIC_ISERx Map](../images/EXTI_TO_NVIC_MAP.png)

## Steps to Configure the Interrupts
- See the Position of each of the EXTI lines in the table, and that exact bit must be set in the NVIC_ISERx Table

- After setting the corresponding bit in the NVIC_ISERx table we must look into setting the priorities of these interrupts so that the actual 'NESTED' action can happen, The more priority interrupt will be nested and handled first before the or while doing the lesser priority interrupt.

- To configure the NVIC_IPRx we must first calculate where the 8 bit value which we are aiming for is there, We use a `volatile uint8_t*` and use indexes to access the correct 8 bit register.

- Only the second MSR 4 bit are used to set the interrupts priority.
- Lower the Priority value more the Priority.

- Here is some sample code
```c
volatile uint8_t* nvic_iprx = (volatile uint8_t*)0xE000E400;
// We can access the correct 8 bit for say the seventh interrupt line
nvic_iprx[7] &= ~(0xF << 4);
// In the above code we clear the MSR 4 bits of the bit field for the priority value to be the lowest hence making the priority more.
```


![IPR Bit Assignments](../images/IPR_BIT_ASSIGNMENT.png)


