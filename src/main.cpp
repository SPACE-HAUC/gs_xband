/**
 * @file main.cpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief 
 * @version See Git tags for version information.
 * @date 2021.08.04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <pthread.h>
#include <unistd.h>
#include "txmodem.h"
#include "gs_xband.hpp"
#include "meb_debug.hpp"
#include "network.hpp"

int main(int argc, char **argv)
{
    // Set up global data.
    global_data_t global_data[1] = {0};
    network_data_init(global_data->network_data, SERVER_PORT);
    global_data->network_data->rx_active = true;

    // 1 = All good, 0 = recoverable failure, -1 = fatal failure (close program)
    global_data->network_data->thread_status = 1;

    // Create Ground Station Network thread IDs.
    pthread_t net_polling_tid, net_rx_tid;

    // Start the RX threads, and restart them should it be necessary.
    // Only gets-out if a thread declares an unrecoverable emergency and sets its status to -1.
    while (global_data->network_data->thread_status > -1)
    {
        // Initialize and begin socket communication to the server.
        if (!global_data->network_data->connection_ready)
        {
            // TODO: Check if this is the desired functionality.
            // Currently, the program will not proceed past this point if it cannot connect to the server.
            while (gs_connect_to_server(global_data->network_data) != 1)
            {
                dbprintlf(RED_FG "Failed to establish connection to the server.");
                usleep(5 SEC);
            }
        }

        // Start the threads.
        pthread_create(&net_polling_tid, NULL, gs_polling_thread, global_data->network_data);
        pthread_create(&net_rx_tid, NULL, gs_network_rx_thread, global_data);

        void *thread_return;
        pthread_join(net_polling_tid, &thread_return);
        pthread_join(net_rx_tid, &thread_return);

        // Loop will begin, restarting the threads.
    }

    // Shutdown the X-Band radio.
    // txmodem_stop(global_data->tx_modem);
    txmodem_destroy(global_data->tx_modem);
    adf4355_pw_down(global_data->ADF);
    adf4355_destroy(global_data->ADF);
    adradio_destroy(global_data->radio);
    

    return 1;
}