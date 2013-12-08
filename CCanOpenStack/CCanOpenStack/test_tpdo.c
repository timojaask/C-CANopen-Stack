//
//  test_tpdo.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 28.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "test_tpdo.h"
#include "co_node.h"
#include "can_bus.h"
#include "log.h"
#include "pdo.h"
#include "utils.h"

/***************************** Local Definitions *****************************/

/****************************** Local Variables ******************************/
uint8_t test_running = 0;
static pdo_mapping_param tpdo_params1[] = {
    {0x2007, 1, 8},
    {0x2007, 2, 8},
    {0x2007, 3, 8},
};
static uint32_t tpdo_params1_expect[] = {
    0x47, 0x48, 0x49
};
/*
 {0x2007, 0, od_data_type_uint8, od_access_type_ro, 3},
 {0x2007, 1, od_data_type_uint8, od_access_type_rw, 71},
 {0x2007, 2, od_data_type_uint8, od_access_type_rw, 72},
 {0x2007, 3, od_data_type_uint8, od_access_type_rw, 73},
 
 {0x2008, 0, od_data_type_uint8, od_access_type_ro, 3},
 {0x2008, 1, od_data_type_uint16, od_access_type_rw, 81},
 {0x2008, 2, od_data_type_uint16, od_access_type_rw, 82},
 {0x2008, 3, od_data_type_uint16, od_access_type_rw, 83},
 
 {0x2009, 0, od_data_type_uint8, od_access_type_ro, 3},
 {0x2009, 1, od_data_type_uint32, od_access_type_rw, 91},
 {0x2009, 2, od_data_type_uint32, od_access_type_rw, 92},
 {0x2009, 3, od_data_type_uint32, od_access_type_rw, 93},*/
uint16_t tpdo_cob_id;
uint8_t tpdo_received = 0;
/****************************** Local Prototypes *****************************/
static int wait_for_tpdo(co_node *node, uint32_t *tick_count, uint16_t event_time_ms);
static void can_message_received(can_message *msg);

/****************************** Global Functions *****************************/
extern int test_tpdo_run(void) {
    int error = 0;
    uint32_t tick_count = 0;
    uint16_t event_time_ms = 90;
    test_running = 1;
    
    object_dictionary od;
    od_initialize(&od);
    co_node node;
    node.node_id = 15;
    node.state = nmt_state_operational;
    node.od = &od;
    
    can_bus_register_message_received_handler(can_message_received);
    
    int num_params = UTILS_ARRAY_SIZE(tpdo_params1);
    tpdo_cob_id = 0x201;
    error |= pdo_add_tpdo(&node, tpdo_cob_id, 10, event_time_ms, tpdo_params1, num_params);
    
    // Wait for the first TPDO (should be received when tick count is 90 ms)
    wait_for_tpdo(&node, &tick_count, event_time_ms);
    // Wait for the second TPDO (should be received when tick count is 180 ms)
    wait_for_tpdo(&node, &tick_count, event_time_ms);
    // Wait for the third TPDO (should be received when tick count is 270 ms)
    wait_for_tpdo(&node, &tick_count, event_time_ms);
    
    test_running = 0;
    return error;
}
/****************************** Local Functions ******************************/
static int wait_for_tpdo(co_node *node, uint32_t *tick_count, uint16_t event_time_ms) {
    int error = 0;
    int i;
    uint32_t event_expected_tick_count = *tick_count + event_time_ms;
    for (i = 0; i < 30; i++) {
        *tick_count += 10;
        error |= pdo_send_tpdo(node, *tick_count);
        if (!tpdo_received && *tick_count < event_expected_tick_count) {
            // Waiting for timed TPDO
            log_write_ln("test_tpdo: Waiting for TPDO %Xh, tick count: %lu ms", tpdo_cob_id, *tick_count);
        } else if (tpdo_received && *tick_count < event_expected_tick_count) {
            // TPDO received too early
            error = 1;
            log_write_ln("test_tpdo: ERROR: TPDO %Xh was received too early, tick count: %lu", tpdo_cob_id, *tick_count);
        } else if (!tpdo_received && *tick_count == event_expected_tick_count) {
            // Time has passed, but TPDO not yet received
            error = 1;
            log_write_ln("test_tpdo: ERROR: TPDO %Xh was not received, tick count: %lu", tpdo_cob_id, *tick_count);
        } else if (tpdo_received && *tick_count == event_expected_tick_count) {
            // TPDO received on time
            tpdo_received = 0;
            log_write_ln("test_tpdo: TPDO %Xh received OK, tick count: %lu ms", tpdo_cob_id, *tick_count);
            break;
        }
    }
    return error;
}
// When node sends TPDO, the "message received" event will be handled in this function.
// This function will set 'tpdo_received' if the expected TPDO message is received.
static void can_message_received(can_message *msg) {
    if (test_running) {
        // Use this for catching TPDOs from the slave
        if (msg->id == tpdo_cob_id) {
            tpdo_received = 1;
            // This is the TPDO that our test slave was supposed to send
            // Check if the data is matching with what was supposed to be sent
            int expected_len = UTILS_ARRAY_SIZE(tpdo_params1_expect);
            if (msg->data_len == expected_len) {
                for (int i = 0; i < msg->data_len; i++) {
                    if (msg->data[i] != tpdo_params1_expect[i]) {
                        tpdo_received = 0;
                        log_write_ln("test_tpdo: ERROR: received TPDO data doesn't match with what was sent");
                        break;
                    }
                }
            } else {
                tpdo_received = 0;
                log_write_ln("test_tpdo: ERROR: received TPDO length is invalid (expected: %d, got: %d)", expected_len, msg->data_len);
            }
        }
    }
}