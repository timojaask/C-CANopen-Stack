//
//  test_nmt.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 24.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

// Testing NMT functions:
//  - Master sends NMT commands to a slave node.
//  - Slave node parses command, set's it's state and sends heartbeat
//  - Master receives heartbeat and makes sure NMT state is correct.
//  - repeat with another command

#include <stdint.h>
#include "test_nmt.h"
#include "can_bus.h"
#include "co_node.h"
#include "log.h"
#include "nmt.h"
#include "nmt_master.h"
#include "nmt_slave.h"

/****************************** Local Variables ******************************/
static int error = 0;
static uint8_t test_running = 0;
static co_node slave_node;
static nmt_state next_state;
static uint8_t state_changed_fired = 0;
static uint8_t reset_comm_called = 0;
static uint8_t reset_node_called = 0;
static uint32_t tick_count = 0;
static object_dictionary od;
static uint8_t heartbeat_enabled = 0;
/****************************** Local Prototypes *****************************/
static void set_heartbeat_interval(uint32_t interval);
static void test_nmt_command(nmt_command command);
static void check_node_state_from_master(co_node *node, nmt_state expected_state);
static void check_node_state_from_slave(co_node *node, nmt_state expected_state);
static void slave_message_received_handler(can_message *message);
static void master_message_received_handler(can_message *message);
static void slave_state_changed_handler(nmt_state state);
static void slave_reset_communication(void);
static void slave_reset_node(void);

/****************************** Global Functions *****************************/
extern int test_nmt_run(void) {
    test_running = 1;
    error = 0;
    // Prepare test slave node
    slave_node.node_id = 7;
    slave_node.state = nmt_state_pre_operational;
    slave_node.od = &od;
    od_initialize(slave_node.od);
    
    // Register message handlers for master and slave. Also slave's state changed handler
    can_bus_register_message_received_handler(slave_message_received_handler);
    can_bus_register_message_received_handler(master_message_received_handler);
    nmt_slave_register_state_changed_handler(slave_state_changed_handler);
    nmt_slave_set_reset_communication_function(slave_reset_communication);
    nmt_slave_set_reset_node_function(slave_reset_node);
    
    // Enable heartbeat in OD
    set_heartbeat_interval(90);
    
    // Test functionality
    tick_count = 100;
    test_nmt_command(nmt_command_operational);
    tick_count = 200;
    test_nmt_command(nmt_command_stopped);
    tick_count = 300;
    test_nmt_command(nmt_command_pre_operational);
    tick_count = 400;
    test_nmt_command(nmt_command_stopped);
    tick_count = 500;
    test_nmt_command(nmt_command_operational);
    tick_count = 600;
    test_nmt_command(nmt_command_reset_communication);
    tick_count = 700;
    test_nmt_command(nmt_command_operational);
    tick_count = 800;
    test_nmt_command(nmt_command_reset_node);
    tick_count = 900;
    test_nmt_command(nmt_command_stopped);
    
    // Test heartbeat interval, enabling and disabling
    set_heartbeat_interval(0);
    test_nmt_command(nmt_command_operational);
    check_node_state_from_slave(&slave_node, nmt_state_operational);
    tick_count = 1000;
    set_heartbeat_interval(90);
    nmt_slave_send_heartbeat(&slave_node, tick_count);
    check_node_state_from_master(&slave_node, next_state);
    
    test_running = 0;
    return error;
}

/****************************** Local Functions ******************************/
static void set_heartbeat_interval(uint32_t interval) {
    od_result result = od_write(slave_node.od, 0x1017, 0, 100);
    if (interval == 0) {
        heartbeat_enabled = 0;
    } else {
        heartbeat_enabled = 1;
    }
    if (result != OD_RESULT_OK) {
        error = 1;
        log_write_ln("test_nmt: ERROR: setting heartbeat interval failed");
    }
}
/**
 * @brief Test NMT functionality.
 *
 * - NMT master sends an NMT command to a slave.
 * - Slave receives the command, processes it and sends a heartbeat.
 * - NMT master receives the heartbeat, processes it and saves to slave node list.
 * Then check if the NMT state of the slave node is reported correctly in the
 * master's slave node list.
 *
 * @param command The NMT command to be tested
 * @note This function sets a file variable 'error' if error occurs
 */
static void test_nmt_command(nmt_command command) {
    state_changed_fired = 0;
    reset_comm_called = 0;
    reset_node_called = 0;
    switch (command) {
        case nmt_command_operational:
            next_state = nmt_state_operational;
            break;
        case nmt_command_pre_operational:
            next_state = nmt_state_pre_operational;
            break;
        case nmt_command_stopped:
            next_state = nmt_state_stopped;
            break;
        case nmt_command_reset_communication:
            next_state = nmt_state_pre_operational;
            break;
        case nmt_command_reset_node:
            next_state = nmt_state_pre_operational;
            break;
        default:
            log_write_ln("test_nmt: INTERNAL ERROR: unknown nmt_command");
            error = 1;
            return;
    }
    nmt_master_send_command(slave_node.node_id, command);
    // At this point, since the test system is not multithreaded, the nmt command should be procesed and heartbeat sent by the slave node and then processed by the master node.
    // Check master node to see if correct heartbeat was received
    if (heartbeat_enabled) {
        check_node_state_from_master(&slave_node, next_state);
        if (state_changed_fired == 0 && reset_comm_called == 0 && reset_node_called == 0 && heartbeat_enabled == 1) {
            error = 1;
            log_write_ln("test_nmt: ERROR: either heartbeat not received or slave state changed didn't fire or both");
        }
        if (reset_node_called && command != nmt_command_reset_node) {
            log_write_ln("test_nmt: ERROR: slave reset node was called, event thought command was %d", command);
            error = 1;
        } else if (command == nmt_command_reset_node && reset_node_called == 0) {
            log_write_ln("test_nmt: ERROR: command was reset node, but slate reset node function was not called", command);
            error = 1;
        } else if (reset_comm_called && command != nmt_command_reset_communication) {
            log_write_ln("test_nmt: ERROR: slave reset comm was called, event thought command was %d", command);
            error = 1;
        } else if (command == nmt_command_reset_communication && reset_comm_called == 0) {
            log_write_ln("test_nmt: ERROR: command was reset communication, but slate reset comm function was not called", command);
            error = 1;
        }
    }
}
/**
 * @brief See if node has reported an expected state.
 *
 * Reads a node list from NMT master and checks if the selected
 * node is on the list and if the node has the correct NMT state.
 *
 * @param node The CANopen node the NMT state of which is being checked.
 * @param expected_state The expected NMT state of the selected node.
 * @note This function sets a file variable 'error' if error occurs
 */
static void check_node_state_from_master(co_node *node, nmt_state expected_state) {
    if (nmt_master_num_nodes() == 1) {
        if (nmt_master_node_list()[0].node_id == slave_node.node_id) {
            // OK - the correct node ID was saved
            if (nmt_master_node_list()[0].state == expected_state) {
                // OK - the correct state was received and saved
                log_write_ln("test_nmt: state %d OK", expected_state);
            } else {
                error = 1;
                log_write_ln("test_nmt: ERROR: wrong nmt state saved to master's node list");
            }
        } else {
            error = 1;
            log_write_ln("test_nmt: ERROR: wrong node ID saved to master's node list");
        }
    } else {
        error = 1;
    }
}
static void check_node_state_from_slave(co_node *node, nmt_state expected_state) {
    if (node->state != expected_state) {
        error = 1;
        log_write_ln("test_nmt: ERROR: wrong nmt state saved to slave node");
    } else {
        log_write_ln("test_nmt: state %d OK", expected_state);
    }
}
static void slave_message_received_handler(can_message *message) {
    if (test_running) {
        if (message->id == 0 || message->id == slave_node.node_id) {
            // This is NMT command, either broadcase or directed to this node
            nmt_slave_process_command(message, &slave_node);
            // Send heartbeat
            nmt_slave_send_heartbeat(&slave_node, tick_count);
        }
    }
}
static void slave_state_changed_handler(nmt_state state) {
    if (test_running) {
        if (state == next_state) {
            state_changed_fired = 1;
        } else {
            log_write_ln("test_nmt: state_changed: wrong state: got %d, expected %d", (int)state, (int)next_state);
            error = 1;
        }
    }
}
static void slave_reset_communication(void) {
    slave_node.state = nmt_state_pre_operational;
    reset_comm_called = 1;
    // Send heartbeat
    nmt_slave_send_heartbeat(&slave_node, tick_count);
}
static void slave_reset_node(void) {
    slave_node.state = nmt_state_pre_operational;
    reset_node_called = 1;
    // Send heartbeat
    nmt_slave_send_heartbeat(&slave_node, tick_count);
}
static void master_message_received_handler(can_message *message) {
    if (test_running) {
        if (message->id == 0x700 + slave_node.node_id) {
            // This is heard beat from the slave node
            nmt_master_process_heartbeat(message);
        }
    }
}

