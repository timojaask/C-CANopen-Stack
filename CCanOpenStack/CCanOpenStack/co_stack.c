//
//  co_stack.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 8.12.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include <stdint.h>
#include "co_stack.h"
#include "co_node.h"
#include "log.h"
#include "nmt_slave.h"
#include "pdo.h"
#include "sdo_server.h"

/****************************** Local Variables ******************************/

/****************************** Local Prototypes *****************************/

/****************************** Global Functions *****************************/
extern int co_stack_initialize(co_node *node, uint8_t node_id, object_dictionary *od, uint32_t tick_count)
{
    int error = 0;
    node->node_id = node_id;
    node->state = nmt_state_boot_up;
    node->od = od;
    error |= od_initialize(node->od);
    // Add some TPDO and RPDO mapping
    pdo_initialize(tick_count);
    // Initialize SDO server
    error |= sdo_server_init(node);
    node->state = nmt_state_pre_operational;
    return error;
}

extern void co_stack_do_tasks(co_node *node, uint32_t tick_count)
{
    // Send heartbeat
    nmt_slave_send_heartbeat(node, tick_count);
    // Send TPDOs
    if (node->state == nmt_state_operational)
    {
        pdo_send_tpdo(node, tick_count);
    }
}

extern void co_stack_can_message_received_handler(co_node *node, can_message *msg)
{
    uint32_t rsdo_cob_id = 0;
    int error = sdo_server_get_rsdo_cob_id(node, &rsdo_cob_id);
    if (error)
    {
        log_write_ln("co_stack: ERROR: unable to get SDO server RSDO COB-ID");
    }
    else
    {
        // See if this is an SDO request
        if (msg->id == rsdo_cob_id)
        {
            // SDO request received
            sdo_server_process_request(msg, node);
        }
        // See if this is an NMT command
        else if (msg->id == 0 || msg->id == node->node_id)
        {
            // This is NMT command, either broadcase or directed to this node
            nmt_slave_process_command(msg, node);
            // Send heartbeat
            nmt_slave_send_heartbeat_immediately(node);
        }
        // See if this is an RPDO message
        else
        {
            if (node->state == nmt_state_operational)
            {
                // pdo_process_rpdo automatically makes sure the message is RPDO before processing it,
                // and if not, the message will be simply ignored.
                error |= pdo_process_rpdo(node, msg);
                if (error)
                {
                    log_write_ln("co_stack: ERROR: unable to process RPDO");
                }
            }
        }
    }
}

/****************************** Local Functions ******************************/