//
//  nmt_slave.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 23.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_nmt_slave_h
#define CCanOpenStack_nmt_slave_h

#include "can_message.h"
#include "co_node.h"
#include "nmt.h"

/***************************** Global Prototypes *****************************/
/**
 * @brief Processes NMT message and calls approriate functions (such as setting node state)
 * @param message The raw CAN message containing the NMT command
 * @param node CANopen node, which will process the NMT message
 */
extern void nmt_slave_process_command(can_message *message, co_node *node);
/**
 * @brief Checks if it's time to send heartbeat and if yes, sends it
 * @param node CANopen node, which will process the NMT message
 * @param tick_count Current system tick count
 */
extern int nmt_slave_send_heartbeat(co_node *node, uint32_t tick_count);
/**
 * @brief Sends heartbeat immediately, even if it's not time for it yet
 * @param node CANopen node, which will process the NMT message
 */
extern void nmt_slave_send_heartbeat_immediately(co_node *node);
/**
 * @brief Registers an event handler which will be called whenver node changes state
 * @param handler_function A function pointer with signature (void)function_hander(nmt_state state);
 */
extern int nmt_slave_register_state_changed_handler(void (*handler_function)(nmt_state state));
/**
 * @brief Register a function that resets node communication. It whill be called whenever NMT reset comm command is received.
 * @param function A function pointer with signature (void)function_hander(void);
 */
extern void nmt_slave_set_reset_communication_function(void (*function)(void));
/**
 * @brief Register a function that resets node. It whill be called whenever NMT reset node command is received.
 * @param function A function pointer with signature (void)function_hander(void);
 */
extern void nmt_slave_set_reset_node_function(void (*function)(void));

#endif
