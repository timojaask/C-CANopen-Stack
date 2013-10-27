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
typedef enum {
    od_data_type_bool = 1,
    od_data_type_int8 = 2,
    od_data_type_int16 = 3,
    od_data_type_int32 = 4,
    od_data_type_uint8 = 5,
    od_data_type_uint16 = 6,
    od_data_type_uint32 = 7
} od_data_type;

typedef enum {
    od_access_type_rw = 0,
    od_access_type_wo = 1,
    od_access_type_ro = 2,
    od_access_type_const = 3
} od_access_type;

typedef struct {
    uint16_t index;
    uint8_t sub_index;
    od_data_type data_type;
    od_access_type access_type;
    uint32_t data;
} od_object;

#endif
