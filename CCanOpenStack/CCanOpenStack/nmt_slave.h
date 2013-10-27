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
extern void nmt_slave_process_command(can_message *message, co_node *node);
extern int nmt_slave_send_heartbeat(co_node *node, uint32_t tick_count);
extern int nmt_slave_register_state_changed_handler(void (*handler_function)(nmt_state state));
extern void nmt_slave_set_reset_communication_function(void (*function)(void));
extern void nmt_slave_set_reset_node_function(void (*function)(void));

#endif
