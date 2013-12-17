//
//  test_appcycle.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 8.12.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
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
static uint8_t node1_2004_1_expected_value = 0;
static uint8_t node1_2004_2_expected_value = 0;
static uint8_t node1_2004_3_expected_value = 0;
static uint8_t node1_2005_1_expected_value = 0;
static uint8_t node1_2005_2_expected_value = 0;
static uint8_t node1_2005_3_expected_value = 0;
static uint8_t node1_2006_1_expected_value = 0;
static uint8_t node1_2006_2_expected_value = 0;
static uint8_t node1_2006_3_expected_value = 0;
static uint8_t node1_2007_1_expected_value = 0;
static uint8_t node1_2007_2_expected_value = 0;
static uint8_t node1_2007_3_expected_value = 0;
static uint8_t node1_2008_1_expected_value = 0;
static uint8_t node1_2008_2_expected_value = 0;
static uint8_t node1_2008_3_expected_value = 0;
static uint8_t node1_2009_1_expected_value = 0;
static uint8_t node1_2009_2_expected_value = 0;
static uint8_t node1_2009_3_expected_value = 0;
static sdo_server_command node1_expected_sdo_response = sdo_command_server_download_init;
static nmt_state node1_expected_nmt_state = nmt_state_pre_operational;
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
static void set_initial_sdo_values(void);
static void send_pdos(uint8_t update_expected_vals);
static void check_pdo_data_using_sdo_read(void);
static void change_tpdo_values(void);

/****************************** Global Functions *****************************/
extern int test_appcycle_run(void)
{
    uint32_t interval_ms = 10;
    int test_duration_sec = 3;
    test_running = 1;
    srand((unsigned int)time(NULL));
    
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
        uint8_t test_stage = 0;
        
        for (tick_count_ms = 0; tick_count_ms < 1000 * test_duration_sec; tick_count_ms += interval_ms)
        {
            if (error)
            {
                break;
            }
            
            // Initialize heartbeat and OD entries that contain data that will be manipulated by PDO's
            if (test_stage == 0 && tick_count_ms > 100)
            {
                log_write_ln("test_appcycle: setting up node1 heartbeat");
                sdo_message_expedited_download_request(&message, 0x1017, 0, node1_heartbeat_interval_ms);
                message.id = 0x600 + node1.node_id;
                message.data_len = 8;
                can_bus_send_message(&message);
                set_initial_sdo_values();
                test_stage++;
            }
            
            // Set NMT state to Operational
            if (test_stage == 1 && tick_count_ms > 200)
            {
                log_write_ln("test_appcycle: Sending NMT operational command");
                node1_expected_nmt_state = nmt_state_operational;
                nmt_master_send_command(node1.node_id, nmt_command_operational);
                test_stage++;
            }
            
            if (test_stage == 2 && tick_count_ms > 300)
            {
                // Send some PDO's and update expected values
                send_pdos(1);
                check_pdo_data_using_sdo_read();
                test_stage++;
            }
            
            // Set NMT state to Stopped
            if (test_stage == 3 && tick_count_ms > 500)
            {
                log_write_ln("test_appcycle: Sending NMT stopped command");
                node1_expected_nmt_state = nmt_state_stopped;
                nmt_master_send_command(node1.node_id, nmt_command_stopped);
                test_stage++;
            }
            
            // Test functionality in stopped mode
            if (test_stage == 4 && tick_count_ms > 600)
            {
                // Should not reply to SDO
                log_write_ln("test_appcycle: Sending SDO requests while in stopped mode");
                message.id = 0x600 + node1.node_id;
                message.data_len = 8;
                sdo_message_expedited_download_request(&message, 0x1017, 0, node1_heartbeat_interval_ms);
                can_bus_send_message(&message);
                sdo_message_expedited_download_request(&message, 0x1000, 0, 123);
                can_bus_send_message(&message);
                sdo_message_expedited_download_request(&message, 0x1001, 0, 4356);
                can_bus_send_message(&message);
                // Send some PDO's as well, but don't update expected values
                // The node should not parse these
                send_pdos(0);
                test_stage++;
            }
            
            // Set NMT state to pre-operational
            if (test_stage == 5 && tick_count_ms > 800)
            {
                log_write_ln("test_appcycle: Sending NMT pre-operational command");
                node1_expected_nmt_state = nmt_state_pre_operational;
                nmt_master_send_command(node1.node_id, nmt_command_pre_operational);
                test_stage++;
            }
            
            // Test functionality in pre-operational mode
            if (test_stage == 6 && tick_count_ms > 900)
            {
                // The node should reply to the SDO requests
                log_write_ln("test_appcycle: Sending SDO requests while in pre-operational mode");
                message.id = 0x600 + node1.node_id;
                message.data_len = 8;
                node1_expected_sdo_response = sdo_command_server_download_init;
                sdo_message_expedited_download_request(&message, 0x1017, 0, node1_heartbeat_interval_ms);
                can_bus_send_message(&message);
                node1_expected_sdo_response = sdo_command_server_abort_transfer;
                sdo_message_expedited_download_request(&message, 0x1000, 0, 123);
                can_bus_send_message(&message);
                node1_expected_sdo_response = sdo_command_server_abort_transfer;
                sdo_message_expedited_download_request(&message, 0x1001, 0, 4356);
                can_bus_send_message(&message);
                // Send some PDO's as well, but don't update expected values
                // The node should not parse these
                send_pdos(0);
                check_pdo_data_using_sdo_read();
                test_stage++;
            }
            
            // Set NMT state to operational
            if (test_stage == 7 && tick_count_ms > 1100)
            {
                log_write_ln("test_appcycle: Sending NMT operational command");
                node1_expected_nmt_state = nmt_state_operational;
                nmt_master_send_command(node1.node_id, nmt_command_operational);
                test_stage++;
            }
            
            // Test functionality in operational mode
            if (test_stage == 8 && tick_count_ms > 1200)
            {
                // The node should reply to SDO requests
                log_write_ln("test_appcycle: Sending SDO requests while in operational mode");
                message.id = 0x600 + node1.node_id;
                message.data_len = 8;
                node1_expected_sdo_response = sdo_command_server_download_init;
                sdo_message_expedited_download_request(&message, 0x1017, 0, node1_heartbeat_interval_ms);
                can_bus_send_message(&message);
                node1_expected_sdo_response = sdo_command_server_abort_transfer;
                sdo_message_expedited_download_request(&message, 0x1000, 0, 123);
                can_bus_send_message(&message);
                node1_expected_sdo_response = sdo_command_server_abort_transfer;
                sdo_message_expedited_download_request(&message, 0x1001, 0, 4356);
                can_bus_send_message(&message);
                // Send some PDO's as well and update expected values
                // The node should parse these
                send_pdos(1);
                check_pdo_data_using_sdo_read();
                test_stage++;
            }
            
            // Check TPDO's by updating their values via SDO requests
            if (test_stage == 9 && tick_count_ms > 2000)
            {
                change_tpdo_values();
                test_stage++;
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
            uint8_t value20071 = msg->data[0];
            uint8_t value20072 = msg->data[1];
            uint8_t value20073 = msg->data[2];
            if (value20071 == node1_2007_1_expected_value &&
                value20072 == node1_2007_2_expected_value &&
                value20073 == node1_2007_3_expected_value)
            {
                log_write_ln("[%_4d] test_appcycle: node1 TPDO1 received. COB-ID: %Xh. Values OK", tick_count_ms, msg->id);
            }
            else
            {
                error = 1;
                log_write_ln("[%_4d] test_appcycle: node1 TPDO1 received. COB-ID: %Xh. ERROR values.", tick_count_ms, msg->id);
            }
            if (node1_expected_nmt_state != nmt_state_operational)
            {
                error = 1;
                log_write_ln("test_appcycle: ERROR: Not supposed to receive PDO's when state is not operational.");
            }
        }
        else if (msg->id == node1_tpdo2_cob_id)
        {
            uint8_t value20081 = msg->data[0];
            uint8_t value20082 = msg->data[1];
            uint8_t value20083 = msg->data[2];
            uint8_t value20091 = msg->data[3];
            uint8_t value20092 = msg->data[4];
            uint8_t value20093 = msg->data[5];
            if (value20081 == node1_2008_1_expected_value &&
                value20082 == node1_2008_2_expected_value &&
                value20083 == node1_2008_3_expected_value &&
                value20091 == node1_2009_1_expected_value &&
                value20092 == node1_2009_2_expected_value &&
                value20093 == node1_2009_3_expected_value)
            {
                log_write_ln("[%_4d] test_appcycle: node1 TPDO2 received. COB-ID: %Xh. Values OK", tick_count_ms, msg->id);
            }
            else
            {
                error = 1;
                log_write_ln("[%_4d] test_appcycle: node1 TPDO2 received. COB-ID: %Xh. ERROR values.", tick_count_ms, msg->id);
            }
            if (node1_expected_nmt_state != nmt_state_operational)
            {
                error = 1;
                log_write_ln("test_appcycle: ERROR: Not supposed to receive PDO's when state is not operational.");
            }
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
            if (node_state != node1_expected_nmt_state)
            {
                error = 1;
                log_write_ln("test_appcycle: ERROR: NMT state not expected.");
            }
        }
        else if (msg->id == 0x580 + node1.node_id)
        {
            sdo_server_command cmd = sdo_get_server_command(msg);
            log_write("[%_4d] test_appcycle: node1 SDO response received, response: ", tick_count_ms);
            switch (cmd)
            {
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
            if (cmd != node1_expected_sdo_response)
            {
                error = 1;
                log_write_ln("test_appcycle: ERROR: SDO response not expected.");
            }
            if (node1_expected_nmt_state != nmt_state_operational && node1_expected_nmt_state != nmt_state_pre_operational)
            {
                error = 1;
                log_write_ln("test_appcycle: ERROR: Not supposed to receive SDO's when state is not operational or pre-operational.");
            }
            if (!error)
            {
                // See if this is response to an upload request
                if (cmd == sdo_command_server_upload_init)
                {
                    // This is a response to an upload request.
                    // If these are values that are related to PDO data, then check their values
                    uint16_t index = sdo_get_index(msg);
                    uint8_t sub_index = sdo_get_sub_index(msg);
                    uint32_t data = sdo_get_expedited_data(msg);
                    if (index == 0x2004)
                    {
                        if (sub_index == 1)
                        {
                            if (data != node1_2004_1_expected_value)
                            {
                                error = 1;
                            }
                        }
                        else if (sub_index == 2)
                        {
                            if (data != node1_2004_2_expected_value)
                            {
                                error = 1;
                            }
                        }
                        else if (sub_index == 3)
                        {
                            if (data != node1_2004_3_expected_value)
                            {
                                error = 1;
                            }
                        }
                    }
                    else if (index == 0x2005)
                    {
                        if (sub_index == 1)
                        {
                            if (data != node1_2005_1_expected_value)
                            {
                                error = 1;
                            }
                        }
                        else if (sub_index == 2)
                        {
                            if (data != node1_2005_2_expected_value)
                            {
                                error = 1;
                            }
                        }
                        else if (sub_index == 3)
                        {
                            if (data != node1_2005_3_expected_value)
                            {
                                error = 1;
                            }
                        }
                    }
                    else if (index == 0x2006)
                    {
                        if (sub_index == 1)
                        {
                            if (data != node1_2006_1_expected_value)
                            {
                                error = 1;
                            }
                        }
                        else if (sub_index == 2)
                        {
                            if (data != node1_2006_2_expected_value)
                            {
                                error = 1;
                            }
                        }
                        else if (sub_index == 3)
                        {
                            if (data != node1_2006_3_expected_value)
                            {
                                error = 1;
                            }
                        }
                    }
                    if (error)
                    {
                        log_write_ln("test_appcycle: RPDO check: 0x%X:%d value ERROR.", index, sub_index);
                    }
                    else
                    {
                        log_write_ln("test_appcycle: RPDO check: 0x%X:%d value OK.", index, sub_index);
                    }
                }
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

static void set_initial_sdo_values(void)
{
    node1_expected_sdo_response = sdo_command_server_download_init;
    node1_2004_1_expected_value = rand() % 255;
    node1_2004_2_expected_value = rand() % 255;
    node1_2004_3_expected_value = rand() % 255;
    node1_2005_1_expected_value = rand() % 255;
    node1_2005_2_expected_value = rand() % 255;
    node1_2005_3_expected_value = rand() % 255;
    node1_2006_1_expected_value = rand() % 255;
    node1_2006_2_expected_value = rand() % 255;
    node1_2006_3_expected_value = rand() % 255;
    
    message.id = 0x600 + node1.node_id;
    message.data_len = 8;
    log_write_ln("test_appcycle: setting initial values for OD entries related to PDO's");
    sdo_message_expedited_download_request(&message, 0x2004, 1, node1_2004_1_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2004, 2, node1_2004_2_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2004, 3, node1_2004_3_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2005, 1, node1_2005_1_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2005, 2, node1_2005_2_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2005, 3, node1_2005_3_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2006, 1, node1_2006_1_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2006, 2, node1_2006_2_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2006, 3, node1_2006_3_expected_value);
    can_bus_send_message(&message);
    
    change_tpdo_values();
}

static void change_tpdo_values(void)
{
    node1_expected_sdo_response = sdo_command_server_download_init;
    node1_2007_1_expected_value = rand() % 255;
    node1_2007_2_expected_value = rand() % 255;
    node1_2007_3_expected_value = rand() % 255;
    node1_2008_1_expected_value = rand() % 255;
    node1_2008_2_expected_value = rand() % 255;
    node1_2008_3_expected_value = rand() % 255;
    node1_2009_1_expected_value = rand() % 255;
    node1_2009_2_expected_value = rand() % 255;
    node1_2009_3_expected_value = rand() % 255;
    
    message.id = 0x600 + node1.node_id;
    message.data_len = 8;
    log_write_ln("test_appcycle: changing TPDO values");
    sdo_message_expedited_download_request(&message, 0x2007, 1, node1_2007_1_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2007, 2, node1_2007_2_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2007, 3, node1_2007_3_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2008, 1, node1_2008_1_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2008, 2, node1_2008_2_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2008, 3, node1_2008_3_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2009, 1, node1_2009_1_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2009, 2, node1_2009_2_expected_value);
    can_bus_send_message(&message);
    sdo_message_expedited_download_request(&message, 0x2009, 3, node1_2009_3_expected_value);
    can_bus_send_message(&message);
}

static void send_pdos(uint8_t update_expected_vals)
{
    message.id = node1_rpdo1_cob_id;
    message.data_len = 3;
    uint8_t new2004_1 = rand() % 255;
    uint8_t new2004_2 = rand() % 255;
    uint8_t new2004_3 = rand() % 255;
    if (update_expected_vals)
    {
        node1_2004_1_expected_value = new2004_1;
        node1_2004_2_expected_value = new2004_2;
        node1_2004_3_expected_value = new2004_3;
    }
    message.data[0] = new2004_1;
    message.data[1] = new2004_2;
    message.data[2] = new2004_3;
    log_write_ln("test_appcycle: Sending RPDO1");
    can_bus_send_message(&message);
    
    message.id = node1_rpdo2_cob_id;
    message.data_len = 6;
    uint8_t new2005_1 = rand() % 255;
    uint8_t new2005_2 = rand() % 255;
    uint8_t new2005_3 = rand() % 255;
    uint8_t new2006_1 = rand() % 255;
    uint8_t new2006_2 = rand() % 255;
    uint8_t new2006_3 = rand() % 255;
    if (update_expected_vals)
    {
        node1_2005_1_expected_value = new2005_1;
        node1_2005_2_expected_value = new2005_2;
        node1_2005_3_expected_value = new2005_3;
        node1_2006_1_expected_value = new2006_1;
        node1_2006_2_expected_value = new2006_2;
        node1_2006_3_expected_value = new2006_3;
    }
    message.data[0] = new2005_1;
    message.data[1] = new2005_2;
    message.data[2] = new2005_3;
    message.data[3] = new2006_1;
    message.data[4] = new2006_2;
    message.data[5] = new2006_3;
    log_write_ln("test_appcycle: Sending RPDO2");
    can_bus_send_message(&message);
}

static void check_pdo_data_using_sdo_read(void)
{
    node1_expected_sdo_response = sdo_command_server_upload_init;
    message.id = 0x600 + node1.node_id;
    message.data_len = 8;
    log_write_ln("test_appcycle: reading contents of OD that was updated using PDO's");
    sdo_message_upload_request(&message, 0x2004, 1);
    can_bus_send_message(&message);
    sdo_message_upload_request(&message, 0x2004, 2);
    can_bus_send_message(&message);
    sdo_message_upload_request(&message, 0x2004, 3);
    can_bus_send_message(&message);
    sdo_message_upload_request(&message, 0x2005, 1);
    can_bus_send_message(&message);
    sdo_message_upload_request(&message, 0x2005, 2);
    can_bus_send_message(&message);
    sdo_message_upload_request(&message, 0x2005, 3);
    can_bus_send_message(&message);
    sdo_message_upload_request(&message, 0x2006, 1);
    can_bus_send_message(&message);
    sdo_message_upload_request(&message, 0x2006, 2);
    can_bus_send_message(&message);
    sdo_message_upload_request(&message, 0x2006, 3);
    can_bus_send_message(&message);
}