#include "mcc_generated_files/mcc.h"

#include <xc.h>
#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "mcp3561.h"

#include "canlib.h"
#include "timer.h"

#define ADC_READ_PERIOD 1 // 16.7kHz
#define STATUS_CHECK_PERIOD 25000 // 0.5Hz

// memory pool for the CAN tx buffer
uint8_t tx_pool[400];

// Set up files
FATFS FatFs;
FIL Fil;
UINT bw;

// File write buffer
char file_write_buf[1500];
uint16_t file_write_buf_ptr = 0;

static char GLOBAL_FILENAME[20];

static void can_msg_handler(const can_msg_t *msg) {
    if ((get_board_type_unique_id(msg) == BOARD_TYPE_UNIQUE_ID) &&
        (get_board_inst_unique_id(msg) == BOARD_INST_UNIQUE_ID)) {
        return;
    }

    switch (get_message_type(msg)) {
        case MSG_LEDS_ON:
            // Turn on all LED
            break;
        case MSG_LEDS_OFF:
            // Turn off all LED
            break;
        case MSG_RESET_CMD:
            if (check_board_need_reset(msg)) {
                RESET();
            }
            break;
        default:
            break;
    }
}

int main(void) {
    SYSTEM_Initialize();
    
    INIT_PINS();
    
    init_adc();
    
    // Set up CAN module
    can_timing_t can_setup;
    // can_generate_timing_params(_XTAL_FREQ, &can_setup);
    can_init(&can_setup, can_msg_handler);

    // set up CAN tx buffer
    txb_init(tx_pool, sizeof(tx_pool), can_send, can_send_rdy);

    // Enable global interrupts
    INTCON0bits.GIE = 1;

    // Set up timer 0 for millis
    timer0_init();

    uint32_t last_status_micros = micros60();
    uint32_t last_adc_micros = micros60();
    
    uint16_t root_dir_files = 0;
    FFDIR dir;
    FILINFO finfo;
    
    while (f_mount(&FatFs, "", 1) != FR_OK) {
        if (micros60() - last_status_micros > STATUS_CHECK_PERIOD) {
            last_status_micros = micros60();
            can_msg_t msg;
            build_general_board_status_msg(PRIO_MEDIUM, micros60(), ACT_STATE_ILLEGAL, 0, &msg);
            txb_enqueue(&msg);
            
            TOGGLE_RED_LED();
        }

        txb_heartbeat();
    }

    // count the number of flies in the root directory of the SD card
    if (f_opendir(&dir, "/") != FR_OK) {
        // error(E_SD_FAIL_FS_INIT);
    }

    // set file info
    while (f_readdir(&dir, &finfo) == FR_OK && finfo.fname[0] != '\0') {
        root_dir_files++;
    }
    f_closedir(&dir);

    sprintf(GLOBAL_FILENAME, "PAY_%04x.txt", root_dir_files);

    if (f_open(&Fil, GLOBAL_FILENAME, FA_CREATE_NEW | FA_WRITE) == FR_OK) {
        f_write(&Fil, "BEGIN PAYLOAD LOG\n", 18, &bw);
        f_close(&Fil);
    }

    for (;;) {
        // CLRWDT();

        if ((micros60() - last_status_micros) > STATUS_CHECK_PERIOD) {
            last_status_micros = micros60();
            
            can_msg_t board_stat_msg;
            uint16_t millis = (uint16_t) micros60()/25000;
            build_general_board_status_msg(PRIO_MEDIUM, millis, 0, 0, &board_stat_msg);
            txb_enqueue(&board_stat_msg);
            
            TOGGLE_BLUE_LED();
        }
        
        if ((micros60() - last_status_micros) > ADC_READ_PERIOD) {
            last_adc_micros = micros60();
            
            // read behaviour will be to buffer data in memory until lim switch hit
            // once it hits, stop the motor, log to sd card (check file size here too), start motor, repeat
            uint32_t data;
            data = read_adc();
            
            // this is horrifically slow
            char buf[64];
            int len = snprintf(buf, 64, "%lu,%lu\n", micros60(), data);
            memcpy(file_write_buf + file_write_buf_ptr, buf, len);
            file_write_buf_ptr += len;
            
            TOGGLE_RED_LED();
        }
        
        // this too is horrifically slow
        if (file_write_buf_ptr > 1000) {
            while (f_mount(&FatFs, "", 1) != FR_OK) {
                if (micros60() - last_status_micros > STATUS_CHECK_PERIOD) {
                    last_status_micros = micros60();
                    can_msg_t msg;
                    build_general_board_status_msg(PRIO_MEDIUM, micros60(), ACT_STATE_ILLEGAL, 0, &msg);
                    txb_enqueue(&msg);
            
                    TOGGLE_RED_LED();
                }

                txb_heartbeat();
            }
            
            if (f_open(&Fil, GLOBAL_FILENAME, FA_OPEN_APPEND | FA_WRITE) == FR_OK) {
                f_write(&Fil, file_write_buf, file_write_buf_ptr, &bw);
                f_close(&Fil);
            } else {
                TOGGLE_RED_LED();
            }
            file_write_buf_ptr = 0;
            
            
            TOGGLE_GREEN_LED();
        }
        
        txb_heartbeat();
    }
}

static void __interrupt() interrupt_handler(void) {
    if (PIR5) {
        can_handle_interrupt();
    }

    if (PIE3bits.TMR0IE == 1 && PIR3bits.TMR0IF == 1) {
        timer0_handle_interrupt();
        PIR3bits.TMR0IF = 0;
    }
}