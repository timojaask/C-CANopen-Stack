//
//  can_bus.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "can_bus.h"
#include "log.h"
/***************************** Local Definitions *****************************/
#define MAX_HANDLERS 20

/****************************** Local Variables ******************************/
static int num_handlers = 0;
static void (*message_received_handlers[MAX_HANDLERS])(can_message *message);

/****************************** Local Prototypes *****************************/
static void message_received(can_message *message);

/****************************** Global Functions *****************************/
extern void can_bus_send_message(can_message *message) {
    message_received(message);
}
extern void can_bus_register_message_received_handler(void (*handler_function)(can_message *message)) {
    log_write_ln("can_bus: registering message received handler");
    if (num_handlers < MAX_HANDLERS) {
        message_received_handlers[num_handlers++] = handler_function;
    }
}

/****************************** Local Functions ******************************/
static void message_received(can_message *message) {
    log_write_ln("can_bus: message to id:%d, calling message received handlers", message->id);
    for (int i = 0; i < num_handlers; i++) {
        (*message_received_handlers[i])(message);
    }
}