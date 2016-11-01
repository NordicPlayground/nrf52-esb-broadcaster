#ifndef __RADIO_H
#define __RADIO_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*radio_packet_received_t)(uint8_t *packet, uint32_t packet_length);

uint32_t radio_init(uint16_t network_id, radio_packet_received_t packet_received_callback);

bool radio_packet_send(uint8_t *packet, uint32_t packet_length);
    
#endif
