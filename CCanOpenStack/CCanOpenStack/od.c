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

/****************************** Global Functions *****************************/
extern od_result od_read(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t *data) {
    od_result result = OD_RESULT_OK;
    int object_found = 0;
    for (int i = 0; i < od->num_objects; i++) {
        if (od->objects[i].index == index && od->objects[i].sub_index == sub_index) {
            *data = od->objects[i].data;
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

extern od_result od_write(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t data) {
    od_result result = OD_RESULT_OK;
    int object_found = 0;
    for (int i = 0; i < od->num_objects; i++) {
        if (od->objects[i].index == index && od->objects[i].sub_index == sub_index) {
            od->objects[i].data = data;
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

