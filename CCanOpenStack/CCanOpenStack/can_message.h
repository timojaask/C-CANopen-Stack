//
//  can_message.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_can_message_h
#define CCanOpenStack_can_message_h

#include <stdint.h>

/***************************** Global Definitions ****************************/
/**
 * @brief A structure to represents a CAN message. This is not a part of CANopen protocol,
 * but underlying CAN protocol. This is the data that will be passed around the CAN network.
 */
typedef struct {
    /* @brief The CAN message identifier */
    uint16_t id;
    /* @brief The length of the message data */
    uint8_t data_len;
    /* @brief A pointer to CAN message data */
    uint8_t *data;
} can_message;

#endif
