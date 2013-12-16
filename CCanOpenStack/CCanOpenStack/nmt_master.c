//
//  nmt_master.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 24.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "nmt_master.h"
#include "can_bus.h"
#include "co_node.h"

/***************************** Local Definitions *****************************/
#define MAX_NODES 16

/****************************** Local Variables ******************************/
static uint8_t data[2];
static co_node nodes[MAX_NODES];
static int num_nodes = 0;

/****************************** Local Prototypes *****************************/

/****************************** Global Functions *****************************/
extern void nmt_master_send_command(uint8_t node_id, nmt_command command)
{
    can_message message;
    // NMT master message has COB ID 0
    message.id = 0;
    message.data_len = 2;
    message.data = data;
    // Set command byte (byte 0)
    message.data[0] = (uint8_t)command;
    // Set node id byte (byte 1)
    message.data[1] = (uint8_t)node_id;
    // Send nmt command
    can_bus_send_message(&message);
}
extern nmt_state nmt_master_process_heartbeat(can_message *message)
{
    // Get source node ID (COB ID - 700h)
    uint16_t node_id = message->id - 0x700;
    // Get node NMT state
    nmt_state state = (nmt_state)message->data[0];
    
    // Look through nodes list. If node is on the list, update it's state,
    // If node is not on the list, add it to the list
    uint8_t is_on_list = 0;
    for (int i = 0; i < num_nodes; i++)
    {
        if (nodes[i].node_id == node_id)
        {
            // Found the node on the list. Update it's state
            is_on_list = 1;
            nodes[i].state = state;
            break;
        }
    }
    if (!is_on_list)
    {
        // Node not yet on the list - add it
        nodes[num_nodes].node_id = node_id;
        nodes[num_nodes].state = state;
        num_nodes++;
    }
    return state;
}
extern int nmt_master_num_nodes(void)
{
    return num_nodes;
}
extern co_node *nmt_master_node_list(void)
{
    return nodes;
}

/****************************** Local Functions ******************************/