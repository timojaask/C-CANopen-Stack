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
typedef struct {
    uint16_t id;
    uint8_t data_len;
    uint8_t *data;
} can_message;

#endif
