#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void delay(uint32_t delay_val){
  volatile uint32_t i = 0;
  while(i <= delay_val){
    i++;
  }
}

void setup_gpio(){
  // Define the required registers
  volatile uint32_t* rcc_apb2enr = (volatile uint32_t*)0x40021018;
  volatile uint32_t* gpioa_crl = (volatile uint32_t*)0x40010800;
  volatile uint32_t* gpioa_odr = (volatile uint32_t*)0x4001080C;
  volatile uint32_t* afio_exticr1 = (volatile uint32_t*)0x40010008;
  volatile uint32_t* exti_imr = (volatile uint32_t*)0x40010400;
  volatile uint32_t* exti_rtsr = (volatile uint32_t*)0x40010408;
  volatile uint32_t* exti_ftsr = (volatile uint32_t*)0x4001040C;
  volatile uint32_t* nvic_iser = (volatile uint32_t*)0xE000E100;
  volatile uint8_t* nvic_ipr1 = (volatile uint8_t*)0xE000E400;

  // Step 1: Turn on the Clock for the Port A
  *rcc_apb2enr |= (1 << 2); // Set the second bit of the register to turn on the clock for Port A

  // Step 2: Clear the Configuration for the GPIOA Port 0
  *gpioa_crl &= ~(0xF); // Clear the Configuration for the GPIOA Port 0
  *gpioa_crl &= ~(0xF << 4);

  // Step 3: Set the Configuration for the GPIOA Port 0 to 0001: Output Mode, 10MHz Max and Push-Pull Mode
  *gpioa_crl |= (0x1);
  *gpioa_crl |= (0x8 << 4);

  // Step 4: Write to the GPIOA ODR Register
  *gpioa_odr |= (0x1);
  *gpioa_odr |= (0x1 << 1);

  // Configuring the AFIO for Interrupts
  // Enabling the AFIO Clock
  *rcc_apb2enr |= (0x1);

  // Turn on the EXT1 Line for PA1
  *afio_exticr1 &= ~(0xF << 4);

  // Unmask the Interrupt for the EXT1
  *exti_imr |= (0x1 << 1);

  // Set the Type of the Interrupts to FTSR (Falling Edge Trigger)
  *exti_ftsr |= (0x1 << 1);

  // Configure the NVIC
  // Set the 7th bit of ISER to 1;
  *nvic_iser |= (1 << 7);

  // Clear the Interrupts for bit 7 via the IPR.
  nvic_ipr1[7] &= ~(0xF << 4);
}

void main(){
    // 1. Enable the clock for GPIO Port C (Bit 4 of APB2ENR)
    *(volatile uint32_t *)0x40021018 |= (1 << 4);

    volatile uint32_t* led_conf = (volatile uint32_t*)0x4001100C;
    volatile uint32_t* rcc_cr = (volatile uint32_t*)0x40021000;
    volatile uint32_t* rcc_cfg = (volatile uint32_t*)0x40021004;
    volatile uint32_t* gpioa_odr = (volatile uint32_t*)0x4001080C;
    volatile uint32_t* gpioa_idr = (volatile uint32_t*)0x40010808;

    // 2. Configure PC13 as an output pin (Bits 20-23 of GPIOC_CRH)
    *(volatile uint32_t *)0x40011004 &= ~(0xF << 20); // Clear bits
    *(volatile uint32_t *)0x40011004 |= (0x2 << 20);  // Output mode
    // 3. Turn the LED ON by pulling PC13 LOW (Bit 13 of GPIOC_ODR)
    *rcc_cr |= 1; // Turn on the HSION, The High Speed Internal Oscillator
    while(!((*rcc_cr >> 1) & 1));
    *led_conf &= ~(1 << 13);
    *rcc_cfg &= ~(1 << 16); // Set the PLL Source as the Internal High Speed Oscillator
    *rcc_cfg &= ~(0xF << 18); // Set the PLL Multiplication as 2
    *rcc_cfg |= (0x2 << 18);
    *rcc_cr |= (1 << 24); // Turn on the PLL
    while(1){
      if((*rcc_cr >> 25) & 1){
        //*led_conf &= ~(1 << 13);
        break;
      }
    } 
    *rcc_cfg &= ~0x3;
    *rcc_cfg |= 0x2; // Switch System Clock
    while (((*rcc_cfg >> 2) & 0x3) != 0x2);


    setup_gpio();

    while(1){
      *gpioa_odr ^= (0x1);
      if((*gpioa_idr >> 1) & 0x1){
        *led_conf &= ~(1 << 13);
      }
      else{
        *led_conf |= (1 << 13);
      }
      delay(1000000);
    }
}

