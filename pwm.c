#include "pwm.h"
#include "nrf.h"

// Peripheral channel assignments
#define PWM0_GPIOTE_CH      0
#define PWM0_PPI_CH_A       0
#define PWM0_PPI_CH_B       1
#define PWM0_TIMER_CC_NUM   0

// TIMER3 reload value. The PWM frequency equals '16000000 / TIMER_RELOAD'
#define TIMER_RELOAD        20000
// The timer CC register used to reset the timer. Be aware that not all timers in the nRF52 have 6 CC registers.
#define TIMER_RELOAD_CC_NUM 5

// This function initializes timer 3 with the following configuration:
// 24-bit, base frequency 16 MHz, auto clear on COMPARE5 match (CC5 = TIMER_RELOAD)
void timer_init()
{
    NRF_TIMER3->BITMODE                 = TIMER_BITMODE_BITMODE_24Bit << TIMER_BITMODE_BITMODE_Pos;
    NRF_TIMER3->PRESCALER               = 4;
    NRF_TIMER3->SHORTS                  = TIMER_SHORTS_COMPARE0_CLEAR_Msk << TIMER_RELOAD_CC_NUM;
    NRF_TIMER3->MODE                    = TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos;
    NRF_TIMER3->CC[TIMER_RELOAD_CC_NUM] = TIMER_RELOAD;    
}

// Starts TIMER3
void timer_start()
{
    NRF_TIMER3->TASKS_START = 1;
}

// This function sets up TIMER3, the PPI and the GPIOTE modules to configure a single PWM channel
// Timer CC num, PPI channel nums and GPIOTE channel num is defined at the top of this file
void pwm0_init(uint32_t pinselect)
{  
    NRF_GPIOTE->CONFIG[PWM0_GPIOTE_CH] = GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos | 
                                         GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos | 
                                         pinselect << GPIOTE_CONFIG_PSEL_Pos | 
                                         GPIOTE_CONFIG_OUTINIT_High << GPIOTE_CONFIG_OUTINIT_Pos;

    NRF_PPI->CH[PWM0_PPI_CH_A].EEP = (uint32_t)&NRF_TIMER3->EVENTS_COMPARE[PWM0_TIMER_CC_NUM];
    NRF_PPI->CH[PWM0_PPI_CH_A].TEP = (uint32_t)&NRF_GPIOTE->TASKS_CLR[PWM0_GPIOTE_CH];
    NRF_PPI->CH[PWM0_PPI_CH_B].EEP = (uint32_t)&NRF_TIMER3->EVENTS_COMPARE[TIMER_RELOAD_CC_NUM];
    NRF_PPI->CH[PWM0_PPI_CH_B].TEP = (uint32_t)&NRF_GPIOTE->TASKS_SET[PWM0_GPIOTE_CH];
    
    NRF_PPI->CHENSET               = (1 << PWM0_PPI_CH_A) | (1 << PWM0_PPI_CH_B);
}

void pwm0_set_duty_cycle(uint32_t value)
{
    if(value == 0)
    {
        value = 1;
    }
    else if(value >= TIMER_RELOAD)
    {
        value = TIMER_RELOAD - 1;
    }
    NRF_TIMER3->CC[PWM0_TIMER_CC_NUM] = value;
}
