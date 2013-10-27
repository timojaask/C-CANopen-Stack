//
//  test_od.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 21.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "test_od.h"
#include "log.h"
#include "od.h"
#include "utils.h"

/***************************** Local Definitions *****************************/

/****************************** Local Variables ******************************/
static od_object objects[] = {
    // Copy the data types here:
    {0x0001, 0, od_data_type_uint8, od_access_type_ro, 1}, // BOOL: 1 bit
    {0x0002, 0, od_data_type_uint8, od_access_type_ro, 8}, // INT8: 8 bits
    {0x0003, 0, od_data_type_uint8, od_access_type_ro, 16}, // INT16: 16 bits
    {0x0004, 0, od_data_type_uint8, od_access_type_ro, 32}, // INT32: 32 bits
    {0x0005, 0, od_data_type_uint8, od_access_type_ro, 8}, // UINT8: 8 bits
    {0x0006, 0, od_data_type_uint8, od_access_type_ro, 16}, // UINT16: 16 bits
    {0x0007, 0, od_data_type_uint8, od_access_type_ro, 32}, // UINT32: 32 bits
    
    {0x1000, 0, od_data_type_uint32, od_access_type_rw, 10000},
    {0x1001, 0, od_data_type_uint32, od_access_type_rw, 10010},
    {0x2000, 0, od_data_type_uint8, od_access_type_ro, 3},
    {0x2000, 1, od_data_type_uint32, od_access_type_rw, 20001},
    {0x2000, 2, od_data_type_uint32, od_access_type_rw, 20002},
    {0x2000, 3, od_data_type_uint32, od_access_type_rw, 20003},
    
    // For testing access types
    {0x2001, 0, od_data_type_uint8, od_access_type_ro, 1},
    {0x2001, 1, od_data_type_uint32, od_access_type_ro, 20011},
    {0x2001, 2, od_data_type_uint32, od_access_type_wo, 20012},
    {0x2001, 3, od_data_type_uint32, od_access_type_const, 20013},
    
    // For testing data types
    {0x2003, 0, od_data_type_uint8, od_access_type_ro, 1},
    {0x2003, 1, od_data_type_bool, od_access_type_ro, 20031},
    {0x2003, 2, od_data_type_int8, od_access_type_ro, 20032},
    {0x2003, 3, od_data_type_int16, od_access_type_ro, 20033},
    {0x2003, 4, od_data_type_int32, od_access_type_ro, 20034},
    {0x2003, 5, od_data_type_uint8, od_access_type_ro, 20035},
    {0x2003, 6, od_data_type_uint16, od_access_type_ro, 20036},
    {0x2003, 7, od_data_type_uint32, od_access_type_ro, 20037},
};

/****************************** Local Prototypes *****************************/
static int test_read(object_dictionary *od, uint16_t index, uint8_t sub_index, int expected_result, uint32_t expected_value);
static int test_write(object_dictionary *od, uint16_t index, uint8_t sub_index, int expected_result, uint32_t value);
static int test_access(object_dictionary *od, uint16_t index, uint8_t sub_index, od_access_type expected_access);
static int test_data_type(object_dictionary *od, uint16_t index, uint8_t sub_index, od_data_type expected_type, int expected_len);

/****************************** Global Functions *****************************/
extern int test_od_run(void) {
    int error = 0;
    int num_objects = UTILS_ARRAY_SIZE(objects);
    
    // Create an object dictionary to run tests on
    object_dictionary dictionary = {
        .objects = objects,
        .num_objects = num_objects
    };
    object_dictionary *od = &dictionary;
    
    // Run tests on this object dictionary
    
    // Test reading
    error |= test_read(od, 0x1000, 0, OD_RESULT_OK, 10000);
    error |= test_read(od, 0x1001, 0, OD_RESULT_OK, 10010);
    error |= test_read(od, 0x2000, 0, OD_RESULT_OK, 3);
    error |= test_read(od, 0x2000, 1, OD_RESULT_OK, 20001);
    error |= test_read(od, 0x2000, 2, OD_RESULT_OK, 20002);
    error |= test_read(od, 0x2000, 3, OD_RESULT_OK, 20003);
    
    // Test writing
    error |= test_access(od, 0x2000, 1, od_access_type_rw);
    error |= test_write(od, 0x2000, 1, OD_RESULT_OK, 1234);
    error |= test_write(od, 0x2000, 2, OD_RESULT_OK, 9876);
    error |= test_write(od, 0x2000, 3, OD_RESULT_OK, 0);
    
    // Test read only access type
    error |= test_access(od, 0x2001, 1, od_access_type_ro);
    error |= test_write(od, 0x2001, 1, OD_RESULT_WRITING_READ_ONLY_OBJECT, 1234); // Try to write a read only object
    error |= test_read(od, 0x2001, 1, OD_RESULT_OK, 20011); // Make sure it wasn't written (old value stayed)
    // Test write only access type
    error |= test_access(od, 0x2001, 2, od_access_type_wo);
    error |= test_read(od, 0x2001, 2, OD_RESULT_READING_WRITE_ONLY_OBJECT, 0); // Try to read a write only object
    error |= test_write(od, 0x2001, 2, OD_RESULT_OK, 1234); // Write a write only object
    // Test constant access type
    error |= test_access(od, 0x2001, 3, od_access_type_const);
    error |= test_write(od, 0x2001, 3, OD_RESULT_WRITING_READ_ONLY_OBJECT, 1234); // Try to write a const object
    error |= test_read(od, 0x2001, 3, OD_RESULT_OK, 20013); // Make sure it wasn't written (old value stayed)
    
    // Test data types
    error |= test_data_type(od, 0x2003, 1, od_data_type_bool, 1);
    error |= test_data_type(od, 0x2003, 2, od_data_type_int8, 8);
    error |= test_data_type(od, 0x2003, 3, od_data_type_int16, 16);
    error |= test_data_type(od, 0x2003, 4, od_data_type_int32, 32);
    error |= test_data_type(od, 0x2003, 5, od_data_type_uint8, 8);
    error |= test_data_type(od, 0x2003, 6, od_data_type_uint16, 16);
    error |= test_data_type(od, 0x2003, 7, od_data_type_uint32, 32);
    
    return error;
}

/****************************** Local Functions ******************************/
static int test_read(object_dictionary *od, uint16_t index, uint8_t sub_index, int expected_result, uint32_t expected_value) {
    int error = 0;
    uint32_t data;
    int result = od_read(od, index, sub_index, &data);
    if (result != expected_result) {
        // Was expecting different result
        log_write_ln("test_od: od_read failed: wrong result @%04Xh:%d, expected:%d, got:%d", index, sub_index, expected_result, result);
        error = 1;
    } else {
        if (result == OD_RESULT_OK) {
            if (data != expected_value) {
                // Data is incorrect - test failed
                log_write_ln("test_od: od_read failed: wrong data @%04Xh:%d, expected:%lu, got:%lu", index, sub_index, expected_value, data);
                error = 1;
            } else {
                // Data was correct - test passed
            }
        } else {
            // Some other error result was expected, so no need to compare data here - test passed
        }
    }
    if (!error) {
        log_write_ln("test_od: od_read OK %04Xh:%d %lu", index, sub_index, expected_value);
    }
    return error;
}
static int test_write(object_dictionary *od, uint16_t index, uint8_t sub_index, int expected_result, uint32_t value) {
    int error = 0;
    int result = od_write(od, index, sub_index, value);
    if (result != expected_result) {
        // Was expecting different result
        log_write_ln("test_od: od_write failed: wrong result @%04Xh:%d, expected:%d, got:%d", index, sub_index, expected_result, result);
        error = 1;
    } else {
        if (result == OD_RESULT_OK) {
            od_access_type access_type;
            result = od_get_access_type(od, index, sub_index, &access_type);
            // Read back the value to make sure it was saved correctly
            if (result == OD_RESULT_OK) {
                if (access_type == od_access_type_wo) {
                    // This is a write only object, so we can't use normal read funciton here.
                    // Instead, check the value manually
                    int object_found = 0;
                    for (int i = 0; i < od->num_objects; i++) {
                        if (od->objects[i].index == index && od->objects[i].sub_index == sub_index) {
                            if (od->objects[i].data != value) {
                                log_write_ln("test_od: failed: write only object wasn't written @%04Xh:%d, expected:%lu, got:%lu", index, sub_index, value, od->objects[i].data);
                                error = 1;
                            } else {
                                // Data was written OK
                            }
                            object_found = 1;
                            break;
                        }
                    }
                    if (!object_found) {
                        log_write_ln("test_od: od_write failed: od_write was ok, but failed to find object manually to check the value");
                        error = 1;
                    }
                } else {
                    error |= test_read(od, index, sub_index, OD_RESULT_OK, value);
                    if (error) {
                        log_write_ln("test_od: od_write failed: od_write was ok, but test_read returned error");
                        error = 1;
                    }
                }
            } else {
                log_write_ln("test_od: od_write failed: od_write was ok, but getting access type failed");
                error = 1;
            }
        } else {
            // Some other error result was expected, so no need to compare data here - test passed
        }
    }
    if (!error) {
        log_write_ln("test_od: od_write OK %04Xh:%d %lu", index, sub_index, value);
    }
    return error;
}
static int test_access(object_dictionary *od, uint16_t index, uint8_t sub_index, od_access_type expected_access) {
    int error = 0;
    od_access_type read_access;
    int result = od_get_access_type(od, index, sub_index, &read_access);
    if (result == OD_RESULT_OK) {
        if (read_access == expected_access) {
            // OK - access type matches
        } else {
            log_write_ln("test_od: test_access failed: wrong access type @%04Xh:%d, , expected:%d, got:%d", index, sub_index, expected_access, read_access);
            error = 1;
        }
    } else {
        log_write_ln("test_od: test_access failed: od_get_access_type @%04Xh:%d returned an error", index, sub_index);
        error = 1;
    }
    if (!error) {
        log_write_ln("test_od: test_access OK %04Xh:%d", index, sub_index);
    }
    return error;
}
static int test_data_type(object_dictionary *od, uint16_t index, uint8_t sub_index, od_data_type expected_type, int expected_len) {
    int error = 0;
    od_data_type read_type;
    int result = od_get_data_type(od, index, sub_index, &read_type);
    if (result == OD_RESULT_OK) {
        if (read_type == expected_type) {
            int read_len;
            result = od_get_data_len(od, index, sub_index, &read_len);
            if (result == OD_RESULT_OK) {
                if (read_len == expected_len) {
                    // OK - length matches
                } else {
                    log_write_ln("test_od: test_data_type failed: od_get_data_len @%04Xh:%d returned wrong len", index, sub_index);
                    error = 1;
                }
            } else {
                log_write_ln("test_od: test_data_type failed: od_get_data_len @%04Xh:%d returned an error", index, sub_index);
                error = 1;
            }
        } else {
            log_write_ln("test_od: test_data_type failed: od_get_data_type @%04Xh:%d returned wrong type", index, sub_index);
            error = 1;
        }
    } else {
        log_write_ln("test_od: test_data_type failed: od_get_data_type @%04Xh:%d returned an error", index, sub_index);
        error = 1;
    }
    if (!error) {
        log_write_ln("test_od: test_data_type OK %04Xh:%d", index, sub_index);
    }
    return error;
}