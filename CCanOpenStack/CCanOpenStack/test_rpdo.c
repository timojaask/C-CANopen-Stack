//
//  test_pdo.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 27.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "test_rpdo.h"
#include "can_message.h"
#include "co_node.h"
#include "log.h"
#include "nmt.h"
#include "od.h"
#include "pdo.h"
#include "utils.h"

/***************************** Local Definitions *****************************/

/****************************** Local Variables ******************************/
static pdo_mapping_param rpdo_params1[] = {
    {0x2004, 1, 8},
    {0x2004, 2, 8},
    {0x2004, 3, 8},
};
static uint32_t rpdo_params1_expect[] = {
    11, 22, 33
};
static pdo_mapping_param rpdo_params2[] = {
    {0x2005, 1, 3}, // 2
    {0x2005, 2, 16}, // 18
    {0x2005, 3, 7}, // 25
    {0x2006, 1, 9}, // 34
    {0x2006, 2, 21}, // 55
    {0x2006, 3, 5},  // 60
};
static uint32_t rpdo_params2_expect[] = {
    0x6, 0x4CC8, 0x6A, 0x98, 0x13444E, 0x16
};
static uint8_t data[8];
/****************************** Local Prototypes *****************************/
static int test_pdo(co_node *node, int cob_id, pdo_mapping_param *params, int num_params, uint32_t *expected);

/****************************** Global Functions *****************************/
extern int test_rpdo_run(void) {
    int error = 0;
    object_dictionary od;
    od_initialize(&od);
    co_node node;
    node.node_id = 15;
    node.state = nmt_state_operational;
    node.od = &od;
    
    // Test simple RPDO parameters
    data[0] = 11;
    data[1] = 22;
    data[2] = 33;
    error = test_pdo(&node, 0x201, rpdo_params1, UTILS_ARRAY_SIZE(rpdo_params1), rpdo_params1_expect);
    
    // Test some complex RPDO parameters // 0x169A227263526646 (6, 0x4CC8, 0x6A, 0x98, 0x13444E, 0x16)
    data[0] = 0x46;
    data[1] = 0x66;
    data[2] = 0x52;
    data[3] = 0x63;
    data[4] = 0x72;
    data[5] = 0x22;
    data[6] = 0x9A;
    data[7] = 0x16;
    error = test_pdo(&node, 0x301, rpdo_params2, UTILS_ARRAY_SIZE(rpdo_params2), rpdo_params2_expect);
    return error;
}

/****************************** Local Functions ******************************/
static int test_pdo(co_node *node, int cob_id, pdo_mapping_param *params, int num_params, uint32_t *expected) {
    int error = pdo_add_rpdo(node, cob_id, params, num_params);
    if (!error) {
        // Send some data and run process RPDO function, which should write data into object dictionary
        can_message msg;
        msg.id = cob_id;
        msg.data_len = 8;
        msg.data = data;
        error |= pdo_process_rpdo(node, &msg);
    }
    if (!error) {
        // See if data was written into object dictionary properly
        for (int i = 0; i < num_params; i++) {
            uint16_t index = params[i].index;
            uint8_t sub_index = params[i].sub_index;
            uint32_t value;
            od_result result = od_read(node->od, index, sub_index, &value);
            if (result == OD_RESULT_OK) {
                if (value == expected[i]) {
                    // OK
                } else {
                    log_write_ln("test_pdo: Value read from OD %04Xh:%d not what was expected (read: %X, expected: %X)", index, sub_index, value, expected[i]);
                    error = 1;
                    break;
                }
            } else {
                log_write_ln("test_pdo: Reading OD %04Xh:%d failed", index, sub_index);
                error = 1;
                break;
            }
        }
    }
    
    if (error) {
        log_write_ln("rpdo test: ERROR: pdo_add_rpdo RPDO param test");
    } else {
        log_write_ln("rpdo test: RPDO param test OK");
    }
    return error;
}
