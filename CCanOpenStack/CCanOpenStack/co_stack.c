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
#include "nmt_slave.h"
#include "pdo.h"

extern void co_stack_do_tasks(co_node *node, uint32_t tick_count) {
    // Send heartbeat
    nmt_slave_send_heartbeat(node, tick_count);
    // Send TPDOs
    pdo_send_tpdo(node, tick_count);
}