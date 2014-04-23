//
//  co_stack.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 8.12.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_co_stack_h
#define CCanOpenStack_co_stack_h

#include "co_node.h"
#include "can_message.h"

/***************************** Global Prototypes *****************************/
/**
 * @brief Initializes can open node with object dictionary, SDO and PDO
 * @param node CANopen node which will be initialized
 * @param node_id
 * @param od Object dictionary that will be used for a given node
 * @param tick_count Current system tick count
 * @return Returns error (0 = no error, otherwise error).
 */
extern int co_stack_initialize(co_node *node, uint8_t node_id, object_dictionary *od, uint32_t tick_count);
/**
 * @brief Handles normal CANopen node tasks, such as heartbeat transmission and TPDO (if enabled)
 * @param node CANopen node that will be used
 * @param tick_count Current system tick count
 */
extern void co_stack_do_tasks(co_node *node, uint32_t tick_count);
/**
 * @brief Parses CAN messages and calls appropriate prosessing functions (for NMT, SDO, PDO and other CANopen messages)
 * @param node CANopen node that will be used
 * @param msg Raw CAN message
 */
extern void co_stack_can_message_received_handler(co_node *node, can_message *msg);

#endif
