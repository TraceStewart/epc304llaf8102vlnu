#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"

// header files that enable UART interrupts
#include "inc/hw_ints.h"
#include "driverlib/uart.h"

// header file to manipulate characters
#include <ctype.h>


void UARTIntHandler(void)
{
    uint32_t ui32Status;
    uint8_t letter; // received letter


    uint32_t ui32ADC0Value[4];
        volatile uint32_t ui32TempAvg;
        volatile uint32_t ui32TempValueC;
        volatile uint32_t ui32TempValueF;
        uint32_t i = 0;
        uint8_t temp_str[10];

    ui32Status = UARTIntStatus(UART0_BASE, true); // get interrupt status

    UARTIntClear(UART0_BASE, ui32Status); // clear the asserted interrupts



    while(UARTCharsAvail(UART0_BASE)) // loop while there are chars
    {
        // case inversion for letters (lower->upper, upper-> lower)
        letter = UARTCharGetNonBlocking(UART0_BASE);
        if(letter == 'G')
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        if(letter == 'B')
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        if(letter == 'R')
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        if(letter == 'g')
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, ~GPIO_PIN_3);
        if(letter == 'b')
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, ~GPIO_PIN_2);
        if(letter == 'r')
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
       if(letter == 'T')
        {
           UARTCharPut(UART0_BASE, ' ');
           ROM_ADCIntClear(ADC0_BASE, 1);
                   ROM_ADCProcessorTrigger(ADC0_BASE, 1);
                   while(!ROM_ADCIntStatus(ADC0_BASE, 1, false));
                        ROM_ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
                        ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
                        ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096) / 10;
                        ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
            // Output temperature via UART: "<temp> "
                        i = 0;
                        while(ui32TempValueF != 0){
                            temp_str[i++] = (ui32TempValueF%10) + '0';
                            ui32TempValueF /= 10;
                        }

                        for(; i > 0; i--)
                            UARTCharPut(UART0_BASE, temp_str[i-1]);

                        UARTCharPut(UART0_BASE, ' ');

        }
        UARTCharPutNonBlocking(UART0_BASE, letter); // echo character

    }
}

T   int main(void) {

    /* CPU Clock SETUP () */
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    /* UART SETUP */
    // enable UART0 & Port A peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // configure Tx and Rx pins as UART pins
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // LED SETUP as part of UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // enable GPIO port for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // enable pin for LED PF2

    // initialize UART parameters: 115200 Baud Rate, 8 data bits, 1 stop bit
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // enable UART interrupt
    IntMasterEnable(); // enable processor interrupts
    IntEnable(INT_UART0); // enable the UART interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts

    // output "Enter Text: "
    UARTCharPut(UART0_BASE, 'E');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 't');
    UARTCharPut(UART0_BASE, 'e');
    UARTCharPut(UART0_BASE, 'r');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, 'C');
    UARTCharPut(UART0_BASE, 'm');
    UARTCharPut(UART0_BASE, 'd');
    UARTCharPut(UART0_BASE, ':');
    UARTCharPut(UART0_BASE, ' ');

    /* ADC SETUP */
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
        ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 32);

        ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
        ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
        ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
        ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
        ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
        ROM_ADCSequenceEnable(ADC0_BASE, 1);



    while (1) // let interrupt handler do the UART echo function
    {
        // if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
    }

}
