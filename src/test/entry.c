#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "stm32f103xb.h"


void delay(uint32_t delay_val){
  volatile uint32_t i = 0;
  while(i <= delay_val){
    i++;
  }
}

void setup_gpio(){
  // Define the required registers
  volatile uint32_t* nvic_iser = (volatile uint32_t*)0xE000E100;
  volatile uint8_t* nvic_ipr1 = (volatile uint8_t*)0xE000E400;

  // Turn on the Clock for the Port A and Port C
  SET_BIT(RCC_REGS->APB2ENR, 2);
  SET_BIT(RCC_REGS->APB2ENR, 4);

  // Clear the Configuration for the GPIOA Port 0 and Port 1 amd GPIOC Port 13
  GPIOA->CRL &= ~(0xF);
  GPIOA->CRL &= ~(0xF << 4);
  GPIOC->CRH &= ~(0xF << 20);

  // Set the Configuration for the GPIOA Port 0 to 0001: Output Mode, 10MHz Max and Push-Pull Mode
  // Set the Configuration for the GPIOA Port 1 to 1000: Input Mode, Push-Pull 
  // Set the Configuration for the GPIOC Port 13 to 0001: Output Mode, 10MHz Max and Push-Pull Mode
  SET_BIT(GPIOA->CRL, 0);
  SET_BIT(GPIOA->CRL, 7);
  SET_BIT(GPIOC->CRH, 20);

  // Write to the GPIOA ODR Register to set the default state of the setup of input or output. High for Now.
  SET_BIT(GPIOA->ODR, 0);
  SET_BIT(GPIOA->ODR, 1);
  SET_BIT(GPIOC->ODR, 13);

  // Configuring the AFIO for Interrupts
  // Enabling the AFIO Clock
  SET_BIT(RCC_REGS->APB2ENR, 1);

  // Turn on the EXT1 Line for PA1
  AFIO->EXTICR[1] &= ~(0xF << 4);

  // Unmask the Interrupt for the EXT1
  SET_BIT(EXTI->IMR, 1);

  // Set the Type of the Interrupts to FTSR (Falling Edge Trigger)
  SET_BIT(EXTI->FTSR, 1);

  // Configure the NVIC
  // Set the 7th bit of ISER to 1;
  //*nvic_iser |= (1 << 7);
  SET_BIT(*nvic_iser,7);

  // Clear the Interrupts for bit 7 via the IPR.
  nvic_ipr1[7] &= ~(0xF << 4);
}

void main(){
  // Turn on the HSION, The High Speed Internal Oscillator
  SET_BIT(RCC_REGS->CR,0); 

  // Check if the HSIRDY bit is SET
  while(!(GET_BIT(RCC_REGS->CR,1)));

  // Set the PLL Source as the Internal High Speed Oscillator
  CLEAR_BIT(RCC_REGS->CFGR,16);

  // Set the PLL Multiplication as 2
  RCC_REGS->CFGR &= (0xF << 18); // Clear the bitfield
  //RCC_REGS->CFGR |= (0x2 << 18); // Set the Multiplication factor as 2
  SET_BIT(RCC_REGS->CFGR, 20);

  // Turn on the PLL
  SET_BIT(RCC_REGS->CR, 24);

  // Blocking loop to check the PLLRDY
  while(!(GET_BIT(RCC_REGS->CR,25)));

  // Clear the SW Clocking two bits to choose the HSI by default
  RCC_REGS->CFGR &= ~(0x3);

  // Set the bit two to form the 10 to get the PLL as the clock source.
  SET_BIT(RCC_REGS->CFGR,1);

  setup_gpio();

  while(1){
    GPIOA->ODR ^= (0x1);
    delay(1000000);
  }
}

