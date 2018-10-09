#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

#define PWM_FREQUENCY 55

// Constants for min & max LED brightness
#define MIN_BRIGHT 100
#define MAX_BRIGHT 900

// Constant for LED delay
#define LED_DELAY ROM_SysCtlDelay(10000)

int main(void)
{
    /* Volatile variables - cannot be eliminated by compiler */
    volatile uint32_t ui32Load;         // PWM count to be placed in Load register
    volatile uint32_t ui32PWMClock;     // PWM clock (CPU_Clock / 64)
    volatile uint16_t red, blue, green; // RGB LED brightness
    red = blue = green = MIN_BRIGHT;    // set brightness to minimum brightness


    /* Clock SETUP: CPU (40 MHz) & PWM (40 MHz / 64 = 625 KHz)  */
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);


    /* PWM SETUP */
    // enable PWM1 & Port F peripherals
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // set PF1, PF2, PF3 as a PWM output
    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    ROM_GPIOPinConfigure(GPIO_PF1_M1PWM5);
    ROM_GPIOPinConfigure(GPIO_PF2_M1PWM6);
    ROM_GPIOPinConfigure(GPIO_PF3_M1PWM7);

    // set PWMs as down-counters & load count value based on desired PWM frequency
    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

    // set pulse width based on adjust & load value AND enable PWMs as output and to run
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, red * ui32Load / 1000);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, blue * ui32Load / 1000);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, green * ui32Load / 1000);
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT|PWM_OUT_6_BIT|PWM_OUT_7_BIT, true);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3);


    // cycle through colors with varying brightness
    while(1){
        for(; red <= MAX_BRIGHT; red++){
            for(; blue <= MAX_BRIGHT; blue++){
                for(; green <= MAX_BRIGHT; green++){
                    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, green * ui32Load / 1000);
                    LED_DELAY;
                }
                ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, blue * ui32Load / 1000);
                LED_DELAY;
            }
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, red * ui32Load / 1000);
            LED_DELAY;
        }

        for(; red >= MIN_BRIGHT; red--){
            for(; blue >= MIN_BRIGHT && red == MAX_BRIGHT; blue--){
                for(; green >= MIN_BRIGHT && blue == MAX_BRIGHT &&  red == MAX_BRIGHT; green--){
                    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, green * ui32Load / 1000);
                    LED_DELAY;
                }
                ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, blue * ui32Load / 1000);
                LED_DELAY;
            }
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, red * ui32Load / 1000);
            LED_DELAY;
        }
    }
}

