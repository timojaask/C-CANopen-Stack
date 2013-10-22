//
//  sdo_server.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 22.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_sdo_server_h
#define CCanOpenStack_sdo_server_h

#include "can_message.h"
#include "co_node.h"
#include "od.h"

/***************************** Global Definitions ****************************/

/***************************** Global Prototypes *****************************/

extern void sdo_server_process_request(can_message *message, co_node *node);


#endif
