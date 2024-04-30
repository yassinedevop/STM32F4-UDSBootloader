/**
 * @file drvPins.c
 * @author Little Basdart (twitch.tv/LittleBasdart)
 * @brief Pin Driver Implementation
 * @version 0.1
 * @date 2022-04-18
 * 
 * @copyright Copyright (c) 2022
 * @license GPLv3
 */
#include "drvPins.h"
#include "stm32f407xx.h"

/**
 * @brief List of available CAN bus pin mappings.
 */
#define CAN_USE_GPIOA 1
#define CAN_USE_GPIOB 2
#define CAN_USE_GPIOD 3

/**
 * @brief Selection of CAN bus pin mapping.
 * @todo Select your can bus pins over here!
 */
#define CAN_PORT CAN_USE_GPIOA

/**
 * @brief Mapping of pin ID to Port and Pin number.
 * @note This must be in sync with ::drvPins_pinId_t
 */
static struct 
{
    GPIO_TypeDef * port;
    uint8_t pin;
}
pinMapping[pinId_count] = 
{
    // just some sample pins
    [pinId_LED1] = {GPIOA, 0},
    [pinId_LED2] = {GPIOA, 1},
    [pinId_LED3] = {GPIOA, 2},
    [pinId_LED4] = {GPIOA, 3},
    [pinId_LED5] = {GPIOA, 4},
};


void drvPins_init (void)
{
    RCC->AHB1ENR |= 0x1FF;

    // can bus pins
#if CAN_PORT==CAN_USE_GPIOA
    // GPIOA 11 + 12
    uint32_t afr = GPIOA->AFR[1];
    afr &= ~0x0000FF00; // clear bits 8-15
    afr |= 0x00009900; 
    GPIOA->AFR[1] = afr;
    
    GPIOA->OSPEEDR |= 0x03C00000;       // consider EMF!

    uint32_t mode = GPIOA->MODER;
    // pin 11 and 12 to output
        // Clear the mode for pins 11 and 12 (each pin takes 2 bits)
    // Masking: clear bits 22 and 24 (pin 11) and 24 and 26 (pin 12)
    mode &= ~(0x3 << (2 * 11)); // Clear mode for pin 11 (0b11 << 22)
    mode &= ~(0x3 << (2 * 12)); // Clear mode for pin 12 (0b11 << 24)

    // Set the mode to output for pins 11 and 12
    mode |= (0x1 << (2 * 11)); // Set output mode for pin 11 (0b01 << 22)
    mode |= (0x1 << (2 * 12)); // Set output mode for pin 12 (0b01 << 24)
    GPIOA->MODER = mode;
#elif CAN_PORT==CAN_USE_GPIOB
    // GPIOB 8 + 9
    uint32_t afr = GPIOB->AFR[1];
    afr &= ~0x000000FF;
    afr |= 0x00000099;
    GPIOB->AFR[1] = afr;

    GPIOB->OSPEEDR |= 0x000F0000;       // consider EMF!

    uint32_t mode = GPIOB->MODER;
    mode &= ~0x00F00000;
    mode |= 0x00A00000;
    GPIOB->MODER = mode;
#elif CAN_PORT==CAN_USE_GPIOD
    // GPIOD 0 + 1
    uint32_t afr = GPIOD->AFR[0];
    afr &= ~0x000000FF;
    afr |= 0x00000099;
    GPIOD->AFR[0] = afr;
   
    GPIOD->OSPEEDR |= 0x0000000F;       // consider EMF!

    uint32_t mode = GPIOD->MODER;
    mode &= ~0x0000000F;
    mode |= 0x0000000A;
    GPIOD->MODER = mode;
#else
#error CAN Bus Port not defined!
#endif

    // init user pins

    // TODO: put your pin inits here
    // some sample pin initialization:
    // uint32_t pinMode = GPIOA->MODER;
    // // pin 11 and 12 to output
    // pinMode &= ~0x000000FF;
    // pinMode |= 0x00000055;
    // GPIOA->MODER = pinMode;
}

void drvPins_setPin (drvPins_pinId_t pin, bool state)
{
    if (pin < pinId_count)
    {
        pinMapping[pin].port->BSRR = 1uL << (state ? pinMapping[pin].pin : pinMapping[pin].pin+16);
    }
}

bool drvPins_getPin (drvPins_pinId_t pin)
{
    bool result = false;
    if (pin < pinId_count)
    {
        result = (0 != (pinMapping[pin].port->IDR & 1uL << pinMapping[pin].pin));
    }
    return result;
}
