#include "Arduino.h"
#include "gd32vf103.h"

unsigned long millis(void){
     return (uint64_t)(get_timer_value() * (4000.F / SystemCoreClock));
}

unsigned long micros(void){
    return (unsigned long)(get_timer_value() * (4000000.0 / SystemCoreClock));
}

void delay(unsigned long dwMs){
    return delayMicroseconds(dwMs * 1000);
}

void delayMicroseconds(unsigned int dwUs){
    uint64_t start_mtime, delta_mtime;

    // Don't start measuruing until we see an mtime tick
    uint64_t tmp = get_timer_value();
    do {
    start_mtime = get_timer_value();
    } while (start_mtime == tmp);

    do {
    delta_mtime = get_timer_value() - start_mtime;
    }while(delta_mtime <(SystemCoreClock/4000000.0 *dwUs ));

    return;
}