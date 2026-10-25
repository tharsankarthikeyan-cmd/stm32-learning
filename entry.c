#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void delay(uint32_t delay_val){
  volatile uint32_t i = 0;
  while(i <= delay_val){
    i++;
  }
}
void main(){
    // 1. Enable the clock for GPIO Port C (Bit 4 of APB2ENR)
    *(volatile uint32_t *)0x40021018 |= (1 << 4);

    volatile uint32_t* led_conf = (volatile uint32_t*)0x4001100C;
    volatile uint32_t* rcc_cr = (volatile uint32_t*)0x40021000;
    volatile uint32_t* rcc_cfg = (volatile uint32_t*)0x40021004;


    // 2. Configure PC13 as an output pin (Bits 20-23 of GPIOC_CRH)
    *(volatile uint32_t *)0x40011004 &= ~(0xF << 20); // Clear bits
    *(volatile uint32_t *)0x40011004 |= (0x2 << 20);  // Output mode
    *led_conf |= (1 << 13);
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
    while(1){
      if((*led_conf >> 13) & 0x1){
        *led_conf &= ~(1 << 13);
      }
      else{
        *led_conf |= (1 << 13);
      }
      delay(1000000);
    }
}

