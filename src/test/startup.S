.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global g_pfnVectors
.global Reset_Handler

/* Linker symbols */
.word _estack
.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
  /* 1. Initialize stack pointer (handled automatically by hardware via vector table, but good practice) */
  ldr   r0, =_estack
  mov   sp, r0

  /* 2. Copy data segment initializers from Flash to SRAM */
  ldr   r0, =_sdata
  ldr   r1, =_edata
  ldr   r2, =_sidata
  movs  r3, #0
  b     LoopCopyDataInit

CopyDataInit:
  ldr   r4, [r2, r3]
  str   r4, [r0, r3]
  adds  r3, r3, #4

LoopCopyDataInit:
  adds  r4, r0, r3
  cmp   r4, r1
  bcc   CopyDataInit

  /* 3. Zero fill the BSS segment */
  ldr   r2, =_sbss
  ldr   r4, =_ebss
  movs  r3, #0
  b     LoopFillZerobss

FillZerobss:
  str   r3, [r2]
  adds  r2, r2, #4

LoopFillZerobss:
  cmp   r2, r4
  bcc   FillZerobss

  /* 4. Call system initialization (optional, e.g., clock setup) */
  /*bl    SystemInit*/

  /* 5. Call application's main function */
  bl    main

  /* 6. Infinite loop if main returns */
  b     .
  .size Reset_Handler, .-Reset_Handler

/* Default Interrupt Handler */
.section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b     Infinite_Loop
  .size Default_Handler, .-Default_Handler

/* Vector Table */
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word   _estack               /* Top of Stack */
  .word   Reset_Handler         /* Reset Handler */
  .word   Default_Handler       /* NMI Handler */
  .word   Default_Handler       /* Hard Fault Handler */
  .word   Default_Handler       /* MPU Fault Handler */
  .word   Default_Handler       /* Bus Fault Handler */
  .word   Default_Handler       /* Usage Fault Handler */
  .word   0                     /* Reserved */
  .word   0                     /* Reserved */
  .word   0                     /* Reserved */
  .word   0                     /* Reserved */
  .word   Default_Handler       /* SVCall Handler */
  .word   Default_Handler       /* Debug Monitor Handler */
  .word   0                     /* Reserved */
  .word   Default_Handler       /* PendSV Handler */
  .word   Default_Handler       /* SysTick Handler */
  
  /* External Interrupts (STM32F103 specific lines) */
  .word   Default_Handler       /* Window Watchdog */
  .word   Default_Handler       /* PVD through EXTI Line detect */
  .word   Default_Handler       /* Tamper */
  .word   Default_Handler       /* RTC */
  .word   Default_Handler       /* Flash */
  .word   Default_Handler       /* RCC */
  .word   Default_Handler       /* EXTI Line 0 */
  .word   Default_Handler       /* EXTI Line 1 */
  .word   Default_Handler       /* EXTI Line 2 */
  .word   Default_Handler       /* EXTI Line 3 */
  .word   Default_Handler       /* EXTI Line 4 */
  .word   Default_Handler       /* DMA1 Channel 1 */
  .word   Default_Handler       /* DMA1 Channel 2 */
  .word   Default_Handler       /* DMA1 Channel 3 */
  .word   Default_Handler       /* DMA1 Channel 4 */
  .word   Default_Handler       /* DMA1 Channel 5 */
  .word   Default_Handler       /* DMA1 Channel 6 */
  .word   Default_Handler       /* DMA1 Channel 7 */
  .word   Default_Handler       /* ADC1 and ADC2 */
  .word   Default_Handler       /* USB High Priority / CAN TX */
  .word   Default_Handler       /* USB Low Priority / CAN RX0 */
  .word   Default_Handler       /* CAN RX1 */
  .word   Default_Handler       /* CAN SCE */
  .word   Default_Handler       /* EXTI Line 9..5 */
  .word   Default_Handler       /* TIM1 Break */
  .word   Default_Handler       /* TIM1 Update */
  .word   Default_Handler       /* TIM1 Trigger and Commutation */
  .word   Default_Handler       /* TIM1 Capture Compare */
  .word   Default_Handler       /* TIM2 */
  .word   Default_Handler       /* TIM3 */
  .word   Default_Handler       /* TIM4 */
  .word   Default_Handler       /* I2C1 Event */
  .word   Default_Handler       /* I2C1 Error */
  .word   Default_Handler       /* I2C2 Event */
  .word   Default_Handler       /* I2C2 Error */
  .word   Default_Handler       /* SPI1 */
  .word   Default_Handler       /* SPI2 */
  .word   Default_Handler       /* USART1 */
  .word   Default_Handler       /* USART2 */
  .word   Default_Handler       /* USART3 */
  .word   Default_Handler       /* EXTI Line 15..10 */
  .word   Default_Handler       /* RTC Alarm through EXTI Line */
  .word   Default_Handler       /* USB Wakeup from suspend */
