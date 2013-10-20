//
//  od_object.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_od_object_h
#define CCanOpenStack_od_object_h

#include <stdint.h>

/***************************** Global Definitions ****************************/
typedef struct {
    uint16_t index;
    uint8_t sub_index;
    uint32_t data;
} od_object;

#endif
