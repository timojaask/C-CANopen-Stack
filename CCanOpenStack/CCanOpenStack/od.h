//
//  od.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_od_h
#define CCanOpenStack_od_h

#include <stdint.h>
#include "od_object.h"

/***************************** Global Definitions ****************************/
typedef enum {
    OD_RESULT_OK,
    OD_RESULT_OBJECT_NOT_FOUND
} od_result;

typedef struct {
    od_object *objects;
    int num_objects;
} object_dictionary;

/***************************** Global Prototypes *****************************/
extern od_result od_read(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t *data);
extern od_result od_write(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t data);

#endif
