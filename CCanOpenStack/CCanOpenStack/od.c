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

/****************************** Local Prototypes *****************************/
static int get_data_type_num_bits(object_dictionary *od, od_data_type data_type);
static void print_object_not_found_error(uint16_t index, uint8_t sub_index);

/****************************** Global Functions *****************************/
extern od_result od_read(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t *data) {
    od_result result = OD_RESULT_OK;
    int object_found = 0;
    for (int i = 0; i < od->num_objects; i++) {
        if (od->objects[i].index == index && od->objects[i].sub_index == sub_index) {
            if (od->objects[i].access_type != od_access_type_wo) {
                *data = od->objects[i].data;
            } else {
                result = OD_RESULT_READING_WRITE_ONLY_OBJECT;
                log_write_ln("od: object %04Xh:%d cannot be read as it's write only", index, sub_index);
            }
            object_found = 1;
            break;
        }
    }
    if (!object_found) {
        result = OD_RESULT_OBJECT_NOT_FOUND;
        print_object_not_found_error(index, sub_index);
    }
    return result;
}

extern od_result od_write(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t data) {
    od_result result = OD_RESULT_OK;
    int object_found = 0;
    for (int i = 0; i < od->num_objects; i++) {
        if (od->objects[i].index == index && od->objects[i].sub_index == sub_index) {
            if (od->objects[i].access_type == od_access_type_wo || od->objects[i].access_type == od_access_type_rw) {
                od->objects[i].data = data;
            } else {
                result = OD_RESULT_WRITING_READ_ONLY_OBJECT;
                log_write_ln("od: object %04Xh:%d cannot be written as it's read only", index, sub_index);
            }
            object_found = 1;
            break;
        }
    }
    if (!object_found) {
        result = OD_RESULT_OBJECT_NOT_FOUND;
        print_object_not_found_error(index, sub_index);
    }
    return result;
}

extern od_result od_get_access_type(object_dictionary *od, uint16_t index, uint8_t sub_index, od_access_type *access_type) {
    od_result result = OD_RESULT_OK;
    int object_found = 0;
    for (int i = 0; i < od->num_objects; i++) {
        if (od->objects[i].index == index && od->objects[i].sub_index == sub_index) {
            *access_type = od->objects[i].access_type;
            object_found = 1;
            break;
        }
    }
    if (!object_found) {
        result = OD_RESULT_OBJECT_NOT_FOUND;
        print_object_not_found_error(index, sub_index);
    }
    return result;
}

extern od_result od_get_data_type(object_dictionary *od, uint16_t index, uint8_t sub_index, od_data_type *data_type) {
    od_result result = OD_RESULT_OK;
    int object_found = 0;
    for (int i = 0; i < od->num_objects; i++) {
        if (od->objects[i].index == index && od->objects[i].sub_index == sub_index) {
            *data_type = od->objects[i].data_type;
            object_found = 1;
            break;
        }
    }
    if (!object_found) {
        result = OD_RESULT_OBJECT_NOT_FOUND;
        print_object_not_found_error(index, sub_index);
    }
    return result;
}

extern od_result od_get_data_len(object_dictionary *od, uint16_t index, uint8_t sub_index, int *num_bits) {
    od_result result = OD_RESULT_OK;
    int object_found = 0;
    for (int i = 0; i < od->num_objects; i++) {
        if (od->objects[i].index == index && od->objects[i].sub_index == sub_index) {
            *num_bits = get_data_type_num_bits(od, od->objects[i].data_type);
            object_found = 1;
            break;
        }
    }
    if (!object_found) {
        result = OD_RESULT_OBJECT_NOT_FOUND;
        print_object_not_found_error(index, sub_index);
    }
    return result;
}

/****************************** Local Functions ******************************/
// Returns number of bits or 0 if error
static int get_data_type_num_bits(object_dictionary *od, od_data_type data_type) {
    int num_bits = 0;
    for (int i = 0; i < od->num_objects; i++) {
        // The OD index of data type is the od_data_type enum converted to integer
        if (od->objects[i].index == (uint16_t)data_type && od->objects[i].sub_index == 0) {
            // The value of data type OD entry is number of bits
            num_bits = (int)od->objects[i].data;
            break;
        }
    }
    if (num_bits == 0) {
        log_write_ln("od: ERROR: data type length could not be found");
    }
    return num_bits;
}
static void print_object_not_found_error(uint16_t index, uint8_t sub_index) {
    log_write_ln("od: ERROR: object %04Xh:%d not found", index, sub_index);
}