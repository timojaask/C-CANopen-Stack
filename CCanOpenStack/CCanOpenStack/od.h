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
/**
 * @brief Initializes object dictionary. Call this before using any other OD functions.
 * @param od The object dictionary in use
 * @return Returns error (0 = no error, otherwise error).
 */
extern int od_initialize(object_dictionary *od);
/**
 * @brief Read a value from a given object dictionary object
 * @param od The object dictionary in use
 * @param index Object's index
 * @param sub_index Object's sub-index
 * @param data Pointer to a place where the object's data will be put
 * @return Returns od_result
 */
extern od_result od_read(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t *data);
/**
 * @brief Write a value to a given object dictionary object
 * @param od The object dictionary in use
 * @param index Object's index
 * @param sub_index Object's sub-index
 * @param data The data that will be writted to into the object
 * @return Returns od_result
 */
extern od_result od_write(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t data);
/**
 * @brief Write a value to a given object dictionary object, even if it's read only
 * @param od The object dictionary in use
 * @param index Object's index
 * @param sub_index Object's sub-index
 * @param data The data that will be writted to into the object
 * @return Returns od_result
 */
extern od_result od_internal_write(object_dictionary *od, uint16_t index, uint8_t sub_index, uint32_t data);
/**
 * @brief Get object dictionary object's access type
 * @param od The object dictionary in use
 * @param index Object's index
 * @param sub_index Object's sub-index
 * @param access_type Pointer to a place where the object's access type will be written
 * @return Returns od_result
 */
extern od_result od_get_access_type(object_dictionary *od, uint16_t index, uint8_t sub_index, od_access_type *access_type);
/**
 * @brief Get object dictionary object's data type
 * @param od The object dictionary in use
 * @param index Object's index
 * @param sub_index Object's sub-index
 * @param data_type Pointer to a place where the object's data type will be written
 * @return Returns od_result
 */
extern od_result od_get_data_type(object_dictionary *od, uint16_t index, uint8_t sub_index, od_data_type *data_type);
/**
 * @brief Get object dictionary object's data length
 * @param od The object dictionary in use
 * @param index Object's index
 * @param sub_index Object's sub-index
 * @param num_bits Pointer to a place where the object's data length in bytes will be written
 * @return Returns od_result
 */
extern od_result od_get_data_len(object_dictionary *od, uint16_t index, uint8_t sub_index, int *num_bits);

#endif
