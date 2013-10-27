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
    OD_RESULT_OBJECT_NOT_FOUND,
    OD_RESULT_READING_WRITE_ONLY_OBJECT,
    OD_RESULT_WRITING_READ_ONLY_OBJECT
} od_result;

typedef struct {
    od_object *objects;
    int num_objects;
} object_dictionary;

/***************************** Global Prototypes *****************************/
extern void od_initialize(object_dictionary *od);
extern od_result od_read(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t *data);
extern od_result od_write(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t data);
extern od_result od_get_access_type(object_dictionary *od, uint16_t index, uint8_t sub_index, od_access_type *access_type);
extern od_result od_get_data_type(object_dictionary *od, uint16_t index, uint8_t sub_index, od_data_type *data_type);
extern od_result od_get_data_len(object_dictionary *od, uint16_t index, uint8_t sub_index, int *num_bits);

#endif
