/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_esb.h"
#include "sdk_common.h"
#include "nrf.h"
#include "nrf_esb_error_codes.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
#include "boards.h"
#include "radio.h"
#include "pwm.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// To create a new network, use a unique network ID (in the range 0 - 65536)
#define RF_NETWORK_ID    12

enum {COMMAND_SET_DUTY_CYCLE = 1};

#define SERVO_PWM_MAX 2350
#define SERVO_PWM_MIN 650

nrf_esb_payload_t rx_payload;
    
void clocks_start( void )
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

void gpio_init( void )
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
}

void set_servo_value(uint32_t percentage)
{
    NRF_LOG_INFO(" Send duty cycle: %i\r\n", percentage);
    pwm0_set_duty_cycle(percentage * (SERVO_PWM_MAX - SERVO_PWM_MIN) / 100 + SERVO_PWM_MIN);
}

void radio_packet_received(uint8_t *data, uint32_t data_length)
{
    uint32_t percentage;
    
    switch(data[0])
    {
        case COMMAND_SET_DUTY_CYCLE:
            // Set the duty cycle of the PWM according to the incoming packet
            if(data_length >= 3)
            {
                percentage = (data[1] - '0') * 10 + (data[2] - '0');
                set_servo_value(percentage);
            }
            break;
    }
}

void send_servo_state(uint32_t percentage)
{
    uint8_t rf_command[] = {COMMAND_SET_DUTY_CYCLE, (percentage / 10) + '0', (percentage % 10) + '0'};
    radio_packet_send(rf_command, 3);
    NRF_LOG_INFO(" Sending servo update: %i\r\n", percentage);
}

int main(void)
{
    uint32_t err_code;

    gpio_init();

    NRF_LOG_INIT(NULL);
      
    clocks_start();
    
    timer_init();

    timer_start();
    
    pwm0_init(4);
    
    pwm0_set_duty_cycle(SERVO_PWM_MIN);

    // Initialize the radio library with a network ID set by the RF_NETWORK_ID define
    err_code = radio_init(RF_NETWORK_ID, radio_packet_received);
    APP_ERROR_CHECK(err_code);
    
    NRF_LOG_INFO(" Enhanced ShockBurst Broadcaster Example\r\n");

    while (true)
    {              
        // Check for console input
        uint8_t new_command = NRF_LOG_GETCHAR();
        if(new_command >= '0' && new_command <= '9')
        {
            send_servo_state((new_command - '0') * 10);
        }
    }
}
