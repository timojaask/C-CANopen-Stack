//
//  can_bus.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_can_bus_h
#define CCanOpenStack_can_bus_h

#include "can_message.h"

/***************************** Global Prototypes *****************************/
extern void can_bus_send_message(can_message *message);
extern void can_bus_register_message_received_handler(void (*handler_function)(can_message *message));

#endif
