//
//  nmt_slave.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 24.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "nmt_slave.h"
#include "can_bus.h"
#include "co_node.h"
#include "log.h"
#include "nmt.h"

/***************************** Local Definitions *****************************/
#define MAX_HANDLERS 10

/****************************** Local Variables ******************************/
static int num_handlers = 0;
static void (*state_changed_handlers[MAX_HANDLERS])(nmt_state state);
static void (*reset_communication_function)(void);
static void (*reset_node_function)(void);
static uint32_t previous_heartbeat_tick = 0;

/****************************** Local Prototypes *****************************/
static void change_state(nmt_state state, co_node *node);
static void state_changed(co_node *node);
static void send_heartbeat(co_node *node);

/****************************** Global Functions *****************************/
extern void nmt_slave_process_command(can_message *message, co_node *node) {
    // Get command
    nmt_command command = (nmt_command)message->data[0];
    // Change state
    switch (command) {
        case nmt_command_operational:
            change_state(nmt_state_operational, node);
            break;
        case nmt_command_pre_operational:
            change_state(nmt_state_pre_operational, node);
            break;
        case nmt_command_reset_communication:
            if (reset_communication_function) {
                reset_communication_function();
            } else {
                log_write_ln("nmt_slave: ERROR: reset communication function is not set");
            }
            break;
        case nmt_command_reset_node:
            if (reset_node_function) {
                reset_node_function();
            } else {
                log_write_ln("nmt_slave: ERROR: reset node function is not set");
            }
            break;
        case nmt_command_stopped:
            change_state(nmt_state_stopped, node);
            break;
        default:
            log_write_ln("nmt_slave: ERROR: unknown command: %d", (int)command);
            break;
    }
}
/* Returns error (0 = no error, otherwise error occurred) */
extern int nmt_slave_send_heartbeat(co_node *node, uint32_t tick_count) {
    int error = 0;
    // Check object dictionary to see if heartbeat is enabled and if so what is it's frequency
    // The value is found in OD index 0x1017, sub index 0, data type uint16
    uint32_t interval;
    od_result result = od_read(node->od, 0x1017, 0, &interval);
    if (result == OD_RESULT_OK) {
        if (interval != 0) {
            if (tick_count - previous_heartbeat_tick >= interval) {
                send_heartbeat(node);
                previous_heartbeat_tick = tick_count;
            }
        }
    } else {
        log_write_ln("nmt_slave: ERROR: reading heartbeat OD entry failed");
        error = 1;
    }
    return error;
}
extern void nmt_slave_send_heartbeat_immediately(co_node *node) {
    send_heartbeat(node);
}
extern int nmt_slave_register_state_changed_handler(void (*handler_function)(nmt_state state)) {
    int error = 0;
    if (num_handlers < MAX_HANDLERS) {
        state_changed_handlers[num_handlers++] = handler_function;
    } else {
        error = 1;
    }
    return error;
}

extern void nmt_slave_set_reset_communication_function(void (*function)(void)) {
    reset_communication_function = function;
}
extern void nmt_slave_set_reset_node_function(void (*function)(void)) {
    reset_node_function = function;
}

/****************************** Local Functions ******************************/
static void change_state(nmt_state state, co_node *node) {
    if (state != node->state) {
        switch (state) {
            case nmt_state_operational:
                node->state = nmt_state_operational;
                state_changed(node);
                break;
            case nmt_state_pre_operational:
                node->state = nmt_state_pre_operational;
                state_changed(node);
                break;
            case nmt_state_stopped:
                node->state = nmt_state_stopped;
                state_changed(node);
                break;
            default:
                log_write_ln("nmt_slave: ERROR: unknown state: %d", (int)state);
                break;
        }
    }
}
static void state_changed(co_node *node) {
    for (int i = 0; i < num_handlers; i++) {
        (*state_changed_handlers[i])(node->state);
    }
}
static void send_heartbeat(co_node *node) {
    can_message message;
    uint8_t data = (uint8_t)node->state;
    message.id = 0x700 + node->node_id;
    message.data_len = 1;
    message.data = &data;
    can_bus_send_message(&message);
}

