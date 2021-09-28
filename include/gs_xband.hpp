/**
 * @file gs_xband.hpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief 
 * @version See Git tags for version information.
 * @date 2021.08.04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef GS_XBAND_HPP
#define GS_XBAND_HPP

#include <stdint.h>
#include "txmodem.h"
#include "adf4355.h"
#include "network.hpp"
#include "libiio.h"

// Based on graph given by Sanj.
#define mV_TO_RFP(n) (((420 * (n/1000)) / 11) - 42)
#define RFP_LEEWAY 0.5 // dBm
#define RFP_INCREMENT 0.25 // dBm, minimum possible increment
#define RFP_CMD_MAX 0.0
#define RFP_CMD_MIN -85.0
#define SEC *1000000
#define SERVER_PORT 54220

typedef struct
{
    txmodem tx_modem[1];
    adf4355 PLL[1];
    adradio_t radio[1];

    double desired_power;       // Power requested via gs_client, target.
    double commanded_power;     // Power commanded by gs_xband, adjustable.
    double actual_power;        // Power read from coupler, real.
    ensm_mode desired_mode;
    bool adrs_pid_active;

    bool tx_modem_ready;
    bool PLL_ready;
    bool radio_ready;

    bool transmitting;

    NetDataClient *network_data;
    uint8_t netstat;
} global_data_t;

/**
 * @brief X-Band data structure.
 * 
 * From line 113 of https://github.com/SPACE-HAUC/shflight/blob/flight_test/src/cmd_parser.c
 * Used for:
 *  XBAND_SET_TX
 *  XBAND_SET_RX
 * 
 * FOR SPACE-HAUC USE ONLY
 * 
 */
typedef struct __attribute__((packed))
{
    float LO;
    float bw;
    uint16_t samp;
    uint8_t phy_gain;
    uint8_t adar_gain;
    uint8_t ftr;
    short phase[16];
} xband_set_data_t;

enum XBAND_COMMAND
{
    XBC_INIT_PLL = 0,
    XBC_DISABLE_PLL = 1,
    XBC_ARM_RX = 2,
    XBC_DISARM_RX = 3,
};

/**
 * @brief Initializes radio.
 * 
 * Called from transmit function if necessary.
 * 
 * @param global_data 
 * @return int 
 */
int gs_xband_init(global_data_t *global);

/**
 * @brief Transmits over X-Band to SPACE-HAUC.
 * 
 * @param global_data 
 * @param dev 
 * @param buf 
 * @param size 
 * @return int 
 */
int gs_xband_transmit(global_data_t *global, txmodem *dev, uint8_t *buf, ssize_t size);

/**
 * @brief Listens for NetworkFrames from the Ground Station Network.
 * 
 * @param args 
 * @return void* 
 */
void *gs_network_rx_thread(void *args);

/**
 * @brief Periodically sends X-Band status updates.
 * 
 */
void *xband_status_thread(void *args);

void *xband_power_pid_thread(void *args);

float ads_to_power(int16_t *ads_data);

// double read_rf_power();

#endif // GS_XBAND_HPP