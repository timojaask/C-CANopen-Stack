//
//  nmt_master.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 23.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_nmt_master_h
#define CCanOpenStack_nmt_master_h

#include <stdint.h>
#include "can_message.h"
#include "co_node.h"
#include "nmt.h"

/***************************** Global Prototypes *****************************/
/**
 * @brief Sends an NMT command to a given node
 * @param node_id ID of the recepient node
 * @param command The NMT command that will be sent
 */
extern void nmt_master_send_command(uint8_t node_id, nmt_command command);
/**
 * @brief Processes a heartbeat message and updates internal node state list
 * @param message Raw CAN message that contains heartbeat
 * @return Returns the NMT state of a node
 */
extern nmt_state nmt_master_process_heartbeat(can_message *message);
/**
 * @brief Returns number of nodes regiestered in the node list
 * @return Returns number of registered nodes
 */
extern int nmt_master_num_nodes(void);
/**
 * @brief Returns the registered node list
 * @return Returns the registered node list
 */
extern co_node *nmt_master_node_list(void);

#endif
