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
/**
 * @brief Sends a given message to CAN bus.
 * @param message A pointer to the CAN message the will be sent
 * @return 0: no error, greater than 0: error occurred
 */
extern void can_bus_send_message(can_message *message);
/**
 * @brief Register a function that will be called whenever there is a new message on CAN bus
 * The message must be of a type void and take pointer to can_message as a paramter.
 * When a CAN message is received, this function is called with the received CAN message as a parameter.
 * @param handler_function A pointer to a function that will be called when a new message is received
 * @return 0: no error, greater than 0: error occurred
 */
extern int can_bus_register_message_received_handler(void (*handler_function)(can_message *message));

#endif
