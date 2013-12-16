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
extern int co_stack_initialize(co_node *node, uint8_t node_id, object_dictionary *od, uint32_t tick_count);
extern void co_stack_do_tasks(co_node *node, uint32_t tick_count);
extern void co_stack_can_message_received_handler(co_node *node, can_message *msg);

#endif
