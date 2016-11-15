#ifndef __RADIO_H
#define __RADIO_H

#include <stdint.h>
#include <stdbool.h>

// Definition of the data received callback, which is used to relay packets back to the application
typedef void (*radio_packet_received_t)(uint8_t *packet, uint32_t packet_length);

// Used to initialize the library, providing a network ID and a packet received callback
uint32_t radio_init(uint16_t network_id, radio_packet_received_t packet_received_callback);

// Function for sending packets to any receiver on the same network ID
bool radio_packet_send(uint8_t *packet, uint32_t packet_length);
    
#endif
