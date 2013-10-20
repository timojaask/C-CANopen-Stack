//
//  co_node.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 21.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_co_node_h
#define CCanOpenStack_co_node_h

#include "od.h"

typedef struct {
    uint8_t node_id;
    object_dictionary *od;
} co_node;

#endif
