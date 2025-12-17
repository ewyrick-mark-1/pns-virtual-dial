#include <stdio.h>
#include <string.h>
#include <math.h>
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    
    while(1){
        printf("test\n");
        sleep_ms(1000);
    }
    
    return 0;
}