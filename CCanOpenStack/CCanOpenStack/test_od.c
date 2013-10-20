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
static od_object od[] = {
    {0x1000, 0, 10000},
    {0x1001, 0, 10010},
    {0x2000, 0, 3},
    {0x2000, 1, 20001},
    {0x2000, 2, 20002},
    {0x2000, 3, 20003},
};

/****************************** Local Prototypes *****************************/
static int test_read(uint16_t index, uint8_t sub_index, uint32_t expected_value);
static int test_write(uint16_t index, uint8_t sub_index, uint32_t value);

/****************************** Global Functions *****************************/
extern int test_od_run(void) {
    int error = 0;
    od_init(od, UTILS_ARRAY_SIZE(od));
    error |= test_read(0x1000, 0, 10000);
    error |= test_read(0x1001, 0, 10010);
    error |= test_read(0x2000, 0, 3);
    error |= test_read(0x2000, 1, 20001);
    error |= test_read(0x2000, 2, 20002);
    error |= test_read(0x2000, 3, 20003);
    
    error |= test_write(0x2000, 1, 1234);
    error |= test_write(0x2000, 2, 9876);
    error |= test_write(0x2000, 3, 0);
    return error;
}

/****************************** Local Functions ******************************/
static int test_read(uint16_t index, uint8_t sub_index, uint32_t expected_value) {
    int error = 0;
    uint32_t data;
    if (od_read(index, sub_index, &data) == OD_RESULT_OK) {
        if (data != expected_value) {
            // Data is incorrect - test failed
            log_write_ln("test_od: od_read failed: wrong data @%04Xh:%d, expected:%lu, got:%lu", index, sub_index, expected_value, data);
            error = 1;
        } else {
            // Data was correct - test passed
        }
    } else {
        // od_read returned something else than OK - test failed
        log_write_ln("test_od: od_read failed: od_read returned error");
        error = 1;
    }
    if (!error) {
        log_write_ln("test_od: od_read OK %04Xh:%d %lu", index, sub_index, expected_value);
    }
    return error;
}
static int test_write(uint16_t index, uint8_t sub_index, uint32_t value) {
    int error = 0;
    if (od_write(index, sub_index, value) == OD_RESULT_OK) {
        error |= test_read(index, sub_index, value);
        if (error) {
            log_write_ln("test_od: od_write failed: od_write was ok, but test_read returned error");
        }
    } else {
        error = 1;
        log_write_ln("test_od: od_write failed: od_read returned error");
    }
    if (!error) {
        log_write_ln("test_od: od_write OK %04Xh:%d %lu", index, sub_index, value);
    }
    return error;
}