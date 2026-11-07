#ifndef REGS_H
#define REGS_H

#define __IO volatile

#define BIT(x) (1UL << (x))
#define SET_BIT(reg,x) ((reg) |= BIT((x)))
#define CLEAR_BIT(reg,x) ((reg) &= ~BIT((x)))
#define GET_BIT(reg,x) (((reg) >> (x)) & 1UL)

#define RCC_REGS ((RCC_TypeDef*)0x40021000)
#define GPIOA ((GPIO_TypeDef*)0x40010800)
#define GPIOC ((GPIO_TypeDef*)0x40011000)
#define AFIO ((AFIO_TypeDef*)0x40010000)
#define EXTI ((EXTI_TypeDef*)0x40010400)

typedef struct
{
  __IO uint32_t IMR;
  __IO uint32_t EMR;
  __IO uint32_t RTSR;
  __IO uint32_t FTSR;
  __IO uint32_t SWIER;
  __IO uint32_t PR;
} EXTI_TypeDef;

typedef struct
{
  __IO uint32_t CRL;
  __IO uint32_t CRH;
  __IO uint32_t IDR;
  __IO uint32_t ODR;
  __IO uint32_t BSRR;
  __IO uint32_t BRR;
  __IO uint32_t LCKR;
} GPIO_TypeDef;

typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t CFGR;
  __IO uint32_t CIR;
  __IO uint32_t APB2RSTR;
  __IO uint32_t APB1RSTR;
  __IO uint32_t AHBENR;
  __IO uint32_t APB2ENR;
  __IO uint32_t APB1ENR;
  __IO uint32_t BDCR;
  __IO uint32_t CSR;


} RCC_TypeDef;

typedef struct
{
  __IO uint32_t EVCR;
  __IO uint32_t MAPR;
  __IO uint32_t EXTICR[4];
  uint32_t RESERVED0;
  __IO uint32_t MAPR2;  
} AFIO_TypeDef;

#endif // !REGS_H
