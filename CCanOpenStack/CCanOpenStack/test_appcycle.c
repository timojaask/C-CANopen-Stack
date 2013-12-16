//
//  test_appcycle.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 8.12.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include <stdint.h>
#include "test_appcycle.h"
#include "can_bus.h"
#include "co_node.h"
#include "co_stack.h"
#include "delay.h"
#include "log.h"
#include "nmt.h"
#include "nmt_master.h"
#include "nmt_slave.h"
#include "od.h"
#include "pdo.h"
#include "sdo.h"
#include "sdo_client.h"
#include "sdo_server.h"
#include "utils.h"

/*****************************************************************************
 * In this test a basic CANopen node will be created, run and it's functinality
 * will be tested.
 * The node will have the following features:
 *  - SDO server
 *  - Transmits PDO's with some mapped data
 *  - Receives PDO's
 *  - Doesn't autostart, waits for NMT master start command
 *
 * The test will do the following things to test the node's functionality:
 *  - Initialize node
 *  - Make sure node sends NMT boot and NMT pre-operational states
 *  - Test NMT commands to start/stop the node
 *  - Make sure node does only the things that it's supposed to do in each NMT state
 *  - Make sure node responds to SDO requests correctly
 *  - Make sure node sends PDO's on time and with correct information
 *  - Make sure node receives PDO's (read data back using SDO)
 *
 * TPDO mapping of the node (tpdo_params1):
 *  - TPDO1: 2007h: 1, 2, 3 (180h + NodeId)
 *  - TPDO2: 2008h: 1, 2, 3 + 2009h: 1, 2, 3 (280h + NodeId)
 *
 * RPDO mapping of the node (rpdo_params1):
 *  - RPDO1: 2004h: 1, 2, 3 (200h + NodeId)
 *  - RPDO2: 2005h: 1, 2, 3 + 2006h: 1, 2, 3 (300h + NodeId)
 *
 * TPDO transmission frequency:
 *  - TPDO1: 200 ms
 *  - TPDO2: 300 ms
 */


/***************************** Local Definitions *****************************/
/****************************** Local Variables ******************************/
static int error = 0;
static uint32_t tick_count_ms = 0;
static uint8_t test_running = 0;
static co_node node1;
static can_message message;
static uint8_t message_data[8];
static uint16_t node1_heartbeat_interval_ms = 400;
static object_dictionary od1;
static const uint8_t node1_node_id = 1;
static uint16_t node1_rpdo1_cob_id = 0x200 + node1_node_id;
static pdo_mapping_param node1_rpdo1_params[] = {
    {0x2004, 1, 8},
    {0x2004, 2, 8},
    {0x2004, 3, 8}
};
static uint16_t node1_rpdo2_cob_id = 0x300 + node1_node_id;
static pdo_mapping_param node1_rpdo2_params[] = {
    {0x2005, 1, 8},
    {0x2005, 2, 8},
    {0x2005, 3, 8},
    {0x2006, 1, 8},
    {0x2006, 2, 8},
    {0x2006, 3, 8}
};
static uint16_t node1_tpdo1_cob_id = 0x180 + node1_node_id;
static uint32_t node1_tpdo1_interval_ms = 100;
static pdo_mapping_param node1_tpdo1_params[] = {
    {0x2007, 1, 8},
    {0x2007, 2, 8},
    {0x2007, 3, 8}
};
static uint16_t node1_tpdo2_cob_id = 0x280 + node1_node_id;
static uint32_t node1_tpdo2_interval_ms = 200;
static pdo_mapping_param node1_tpdo2_params[] = {
    {0x2008, 1, 8},
    {0x2008, 2, 8},
    {0x2008, 3, 8},
    {0x2009, 1, 8},
    {0x2009, 2, 8},
    {0x2009, 3, 8}
};
/****************************** Local Prototypes *****************************/
static int initialize_nodes(uint32_t tick_count);
static void master_can_message_received(can_message *msg);
static void node1_can_message_received(can_message *msg);

/****************************** Global Functions *****************************/
extern int test_appcycle_run(void)
{
    uint32_t interval_ms = 10;
    int test_duration_sec = 3;
    test_running = 1;
    
    // Initialize CAN message that will be used throughout the test
    message.data = message_data;
    message.data_len = 8;
    
    // Initialize CANopen nodes
    error = initialize_nodes(tick_count_ms);
    
    can_bus_register_message_received_handler(master_can_message_received);
    can_bus_register_message_received_handler(node1_can_message_received);
    
    if (!error)
    {
        log_write_ln("test_appcycle: starting test cycle, lasts %d seconds...", test_duration_sec);
        uint8_t nmt_operational1_sent = 0;
        uint8_t sdo_set_heartbeat1_sent = 0;
        
        for (tick_count_ms = 0; tick_count_ms < 1000 * test_duration_sec; tick_count_ms += interval_ms)
        {
            // Transmit some PDO's
            
            // TODO
            
            // Send some SDO commands
            if (!sdo_set_heartbeat1_sent && tick_count_ms > 100)
            {
                log_write_ln("test_appcycle: setting up node1 heartbeat");
                sdo_message_expedited_download_request(&message, 0x1017, 0, node1_heartbeat_interval_ms);
                message.id = 0x600 + node1.node_id;
                can_bus_send_message(&message);
                sdo_set_heartbeat1_sent = 1;
            }
            
            // Send some NMT commands and test if SDO and PDO communication is affected
            if (!nmt_operational1_sent && tick_count_ms > 200)
            {
                log_write_ln("test_appcycle: Sending NMT operational command");
                nmt_master_send_command(node1.node_id, nmt_command_operational);
                nmt_operational1_sent = 1;
            }
            
            delay_ms(interval_ms);
            // Run nodes' CANopen stack on each cycle
            co_stack_do_tasks(&node1, tick_count_ms);
        }
    }
    
    if (error)
    {
        log_write_ln("test_appcycle: ERROR: some parts of the test have failed");
    }
    else
    {
        log_write_ln("test_appcycle: OK");
    }
    
    test_running = 0;
    return error;
}
/****************************** Local Functions ******************************/
static int initialize_nodes(uint32_t tick_count)
{
    int error = 0;
    
    error |= co_stack_initialize(&node1, node1_node_id, &od1, tick_count);
    
    // Add some TPDO and RPDO mapping
    error |= pdo_add_tpdo(&node1, node1_tpdo1_cob_id, 10, node1_tpdo1_interval_ms, node1_tpdo1_params, UTILS_ARRAY_SIZE(node1_tpdo1_params));
    error |= pdo_add_tpdo(&node1, node1_tpdo2_cob_id, 10, node1_tpdo2_interval_ms, node1_tpdo2_params, UTILS_ARRAY_SIZE(node1_tpdo2_params));
    error |= pdo_add_rpdo(&node1, node1_rpdo1_cob_id, node1_rpdo1_params, UTILS_ARRAY_SIZE(node1_rpdo1_params));
    error |= pdo_add_rpdo(&node1, node1_rpdo2_cob_id, node1_rpdo2_params, UTILS_ARRAY_SIZE(node1_rpdo2_params));
    
    if (error)
    {
        log_write_ln("test_appcycle: ERROR: failed to initialize PDO");
    }
    
    return error;
}

static void master_can_message_received(can_message *msg)
{
    if (test_running)
    {
        // Use this for catching TPDOs from the slave
        if (msg->id == node1_tpdo1_cob_id)
        {
            log_write_ln("[%_4d] test_appcycle: node1 TPDO1 received. COB-ID: %Xh", tick_count_ms, msg->id);
        }
        else if (msg->id == node1_tpdo2_cob_id)
        {
            log_write_ln("[%_4d] test_appcycle: node1 TPDO2 received. COB-ID: %Xh", tick_count_ms, msg->id);
        }
        else if (msg->id == 0x700 + node1.node_id)
        {
            // This is heard beat from the slave node
            nmt_state node_state = nmt_master_process_heartbeat(msg);
            log_write("[%_4d] test_appcycle: node1 heartbeat, state: ", tick_count_ms);
            switch (node_state)
            {
                case nmt_state_boot_up:
                    log_write_ln("boot-up");
                    break;
                case nmt_state_operational:
                    log_write_ln("operational");
                    break;
                case nmt_state_pre_operational:
                    log_write_ln("pre-operational");
                    break;
                case nmt_state_stopped:
                    log_write_ln("stopped");
                    break;
                default:
                    log_write_ln("ERROR: STATE UNKNOWN");
                    error = 1;
                    break;
            }
        }
        else if (msg->id == 0x580 + node1.node_id)
        {
            sdo_server_command cmd = sdo_get_server_command(msg);
            log_write("[%_4d] test_appcycle: node1 SDO response received, response: ", tick_count_ms);
            switch (cmd) {
                case sdo_command_server_abort_transfer:
                    log_write_ln("ABORT");
                    break;
                case sdo_command_server_download_init:
                    log_write_ln("download init OK");
                    break;
                case sdo_command_server_download_segment:
                    log_write_ln("download segment OK");
                    break;
                case sdo_command_server_upload_init:
                    log_write_ln("upload init OK");
                    break;
                case sdo_command_server_upload_segment:
                    log_write_ln("upload segment OK");
                    break;
                default:
                    log_write_ln("ERROR: SDO RESPONSE UNKNOWN");
                    error = 1;
                    break;
            }
        }
    }
}
static void node1_can_message_received(can_message *msg)
{
    if (test_running)
    {
        co_stack_can_message_received_handler(&node1, msg);
    }
}