//
//  nmt_slave.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 24.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "nmt_slave.h"
#include "can_bus.h"
#include "log.h"
#include "nmt.h"

/***************************** Local Definitions *****************************/

/****************************** Local Variables ******************************/
static nmt_state current_state = nmt_state_boot_up;

/****************************** Local Prototypes *****************************/
static void change_state(nmt_state state);

/****************************** Global Functions *****************************/
extern void nmt_slave_process_command(can_message *message) {
    // Get command
    nmt_command command = (nmt_command)message->data[0];
    // Change state
    switch (command) {
        case nmt_command_operational:
            change_state(nmt_state_operational);
            break;
        case nmt_command_pre_operational:
            change_state(nmt_state_pre_operational);
            break;
        case nmt_command_reset_communication:
            // TODO
            log_write_ln("nmt_slave: TODO: reset communication command not implemented");
            break;
        case nmt_command_reset_node:
            // TODO
            log_write_ln("nmt_slave: TODO: reset node command not implemented");
            break;
        case nmt_command_stopped:
            change_state(nmt_state_stopped);
            break;
        default:
            log_write_ln("nmt_slave: ERROR: unknown command: %d", (int)command);
            break;
    }
}
extern void nmt_slave_send_heartbeat(uint8_t node_id) {
    can_message message;
    message.id = 0x700 + node_id;
    message.data_len = 1;
    message.data[0] = (uint8_t)current_state;
    can_bus_send_message(&message);
}

/****************************** Local Functions ******************************/
static void change_state(nmt_state state) {
    if (state != current_state) {
        switch (state) {
            case nmt_state_operational:
                log_write_ln("nmt_slave: TODO: state changes not implemented");
                // TODO
                break;
            case nmt_state_pre_operational:
                log_write_ln("nmt_slave: TODO: state changes not implemented");
                // TODO
                break;
            case nmt_state_stopped:
                log_write_ln("nmt_slave: TODO: state changes not implemented");
                // TODO
                break;
            default:
                log_write_ln("nmt_slave: ERROR: unknown state: %d", (int)state);
                break;
        }
    }
}