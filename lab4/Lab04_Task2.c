#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"

int main(void)
{
    uint32_t ui32Period, led_delay; // timer setup variables
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    /* Switch and LED Interrupt SETUP */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Unlock GPIO F pin 0 (PF0)
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;

    // buttons and led GPIO setup
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    // sets button interrupt to rising edge and enable interrupt
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_0, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
    IntEnable(INT_GPIOF);


    /* Timer 0 Interrupt SETUP */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = (SysCtlClockGet() / 6);


    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);


    /* Timer 1 Interrupt SETUP */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    led_delay = SysCtlClockGet() * (3/2);
    // multiply clock by 3/2 to get 1.5 second delay

    TimerLoadSet(TIMER1_BASE, TIMER_A, led_delay-1);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    /* Enable master interrupt and start timer 0 */
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
    }
}

void Timer0IntHandler(void)
{

    // Clear the timer 0 interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
        SysCtlDelay(5000000);
    }

}


/* Added two interrupts/ISRs to handle LED turn on for 1.5 seconds: */

void PortFPin0IntHandler(void)
{
    // Turn on LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4);

    // Clear the GPIO interrupt
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);

    // Disable Timer 0 interrupt
    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);


    // Enable Timer 1 for LED's 1.5 second delay
    TimerEnable(TIMER1_BASE, TIMER_A);


}

void Timer1IntHandler(void)
{


    // Clear the timer 1 interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Disable Timer 1
    TimerDisable(TIMER1_BASE, TIMER_A);

    // Re-enable Timer 0 Interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

}
