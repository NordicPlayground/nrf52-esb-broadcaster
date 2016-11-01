#include "radio.h"
#include "nrf_esb.h"

#include "sdk_common.h"
#include "nrf.h"
#include "nrf_esb_error_codes.h"
#include "nrf_error.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "boards.h"

static nrf_esb_config_t nrf_esb_config = NRF_ESB_DEFAULT_CONFIG;
static radio_packet_received_t rx_callback;
static nrf_esb_payload_t tx_payload, rx_payload;
static volatile bool radio_tx_busy = false, radio_started = false;

void nrf_esb_event_handler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
            radio_tx_busy = false;
            nrf_esb_disable();
            nrf_esb_config.mode = NRF_ESB_MODE_PRX;
            nrf_esb_init(&nrf_esb_config);
            nrf_esb_start_rx();
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            radio_tx_busy = false;
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            if (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS && rx_callback)
            {  
                rx_callback(rx_payload.data, rx_payload.length);
            }
            break;
    }
}

uint32_t radio_init(uint16_t network_id, radio_packet_received_t radio_received_callback)
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0xC5, 0xE7, 0xE7, 0x3A};
    uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2};
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 };

    nrf_esb_config.payload_length           = 250;
    nrf_esb_config.tx_mode                  = NRF_ESB_TXMODE_AUTO;
    nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
    nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_2MBPS;
    nrf_esb_config.mode                     = NRF_ESB_MODE_PRX;
    nrf_esb_config.event_handler            = nrf_esb_event_handler;
    nrf_esb_config.selective_auto_ack       = true;
    
    base_addr_0[1] ^= (network_id & 0xFF);
    base_addr_0[2] ^= (network_id >> 8);

    err_code = nrf_esb_init(&nrf_esb_config);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_base_address_0(base_addr_0);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_base_address_1(base_addr_1);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_prefixes(addr_prefix, 8);
    VERIFY_SUCCESS(err_code);
    
    err_code = nrf_esb_set_rf_channel(network_id % 64 + 4);
    VERIFY_SUCCESS(err_code);
    
    err_code = nrf_esb_start_rx();
    VERIFY_SUCCESS(err_code);
    
    rx_callback = radio_received_callback;
    
    radio_started = true;

    return err_code;
}

bool radio_packet_send(uint8_t *packet, uint32_t packet_length)
{
    if(radio_started)
    {
        while(radio_tx_busy);
        nrf_esb_stop_rx();
        nrf_esb_disable();
        nrf_esb_config.mode = NRF_ESB_MODE_PTX;
        nrf_esb_init(&nrf_esb_config);
        nrf_esb_start_tx();
        memcpy(tx_payload.data, packet, packet_length);
        tx_payload.length = packet_length;
        tx_payload.pipe = 0;
        tx_payload.noack = true;
        if(nrf_esb_write_payload(&tx_payload) == NRF_SUCCESS) radio_tx_busy = true;
        return true;
    }
    return false;
}
