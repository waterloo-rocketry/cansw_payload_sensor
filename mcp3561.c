#include <xc.h>
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/spi1.h"
#include "platform.h"

void init_adc() {
    LATA5 = 0;
    SPI1_Open(SPI1_ADC);
    SPI1_WriteByte(0xE8);
    SPI1_Close();
    LATA5 = 1;
}

uint32_t read_adc() {
    uint32_t data;
            
    LATA5 = 0;
    SPI1_Open(SPI1_ADC);
    SPI1_ReadBlock(&data, 3);
    SPI1_Close();
    LATA5 = 1;
    
    return data;
}