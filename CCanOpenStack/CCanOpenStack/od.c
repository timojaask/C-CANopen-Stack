//
//  od.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "od.h"
#include "log.h"

/****************************** Local Variables ******************************/
static od_object *objects;
static int num_objects;

/****************************** Global Functions *****************************/
extern void od_init(od_object *od_objects, int num_od_objects) {
    objects = od_objects;
    num_objects = num_od_objects;
    log_write_ln("od: initialized with %d objects", num_objects);
}

extern od_result od_read(uint16_t index, uint8_t sub_index, uint32_t *data) {
    od_result result = OD_RESULT_OK;
    int object_found = 0;
    for (int i = 0; i < num_objects; i++) {
        if (objects[i].index == index && objects[i].sub_index == sub_index) {
            *data = objects[i].data;
            object_found = 1;
            break;
        }
    }
    if (!object_found) {
        result = OD_RESULT_OBJECT_NOT_FOUND;
        log_write_ln("od: object %04Xh:%d not found", index, sub_index);
    }
    return result;
}

extern od_result od_write(uint16_t index, uint8_t sub_index, uint32_t data) {
    od_result result = OD_RESULT_OK;
    int object_found = 0;
    for (int i = 0; i < num_objects; i++) {
        if (objects[i].index == index && objects[i].sub_index == sub_index) {
            objects[i].data = data;
            object_found = 1;
            break;
        }
    }
    if (!object_found) {
        result = OD_RESULT_OBJECT_NOT_FOUND;
        log_write_ln("od: object %04Xh:%d not found", index, sub_index);
    }
    return result;
}

