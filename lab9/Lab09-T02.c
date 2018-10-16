#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"

#ifndef M_PI
#define M_PI                    3.14159265358979323846
#endif

#define SERIES_LENGTH 1000

float gSeriesData[SERIES_LENGTH];

int32_t i32DataCount = 0;

int main(void)
{
    float fRadians; // angle in a circle used to calculate sine


        /* FPU Initialization */
        ROM_FPULazyStackingEnable(); // turn on lazy stacking
        ROM_FPUEnable(); // turn on FPU


        /* CPU Clock SETUP - 50 MHz */
        ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);


        // go through 1000 angles in the circle
        while(i32DataCount < SERIES_LENGTH)
        {
            // store sine values onto array
            gSeriesData[i32DataCount] = 1.0 * sinf(2*M_PI*50 * i32DataCount) + 0.5 * cosf(2.0 * M_PI*200 * i32DataCount);
                // change from sin() to 1.0*sin(2pi50t) + 0.5*cos(2pi200t)

            // increment counter
            i32DataCount++;
        }

        while(1)
        {
        }
}

