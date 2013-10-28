//
//  pdo.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 27.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "pdo.h"
#include "log.h"
#include "od.h"

/***************************** Local Definitions *****************************/

/****************************** Local Variables ******************************/
static int num_rpdos = 0;
static int num_tpdos = 0;

/****************************** Local Prototypes *****************************/
static int set_pdo_mapping(co_node *node, pdo_mapping_param mapping[], int num_params, uint16_t mapping_index);

/****************************** Global Functions *****************************/
extern int pdo_add_rpdo(co_node *node, uint16_t cob_id, pdo_mapping_param mapping[], int num_params) {
    int error = 0;
    uint16_t rpdo_index = 0x1400 + num_rpdos;
    uint16_t mapping_index = 0x1600 + num_rpdos;
    od_result result = od_internal_write(node->od, rpdo_index, 1, cob_id);
    if (result == OD_RESULT_OK) {
        error = set_pdo_mapping(node, mapping, num_params, mapping_index);
    } else {
        error = 1;
    }
    if (error) {
        log_write_ln("pdo: ERROR add RPDO %04Xh failed", cob_id);
    } else {
        num_rpdos++;
        log_write_ln("pdo: add RPDO %04Xh OK", cob_id);
    }
    return error;
}
extern int pdo_add_tpdo(co_node *node, uint16_t cob_id, uint16_t inhibit, uint16_t event, pdo_mapping_param mapping[], int num_params) {
    int error = 0;
    uint16_t tpdo_index = 0x1800 + num_tpdos;
    uint16_t mapping_index = 0x1A00 + num_tpdos;
    od_result result = od_internal_write(node->od, tpdo_index, 1, cob_id);
    if (result == OD_RESULT_OK) {
        result = od_internal_write(node->od, tpdo_index, 3, inhibit);
        if (result == OD_RESULT_OK) {
            result = od_internal_write(node->od, tpdo_index, 5, event);
            if (result == OD_RESULT_OK) {
                error = set_pdo_mapping(node, mapping, num_params, mapping_index);
            } else {
                error = 1;
            }
        } else {
            error = 1;
        }
    } else {
        error = 1;
    }
    if (error) {
        log_write_ln("pdo: ERROR add TPDO %04Xh failed", cob_id);
    } else {
        num_tpdos++;
        log_write_ln("pdo: add TPDO %04Xh OK", cob_id);
    }
    return error;
}
extern int pdo_process_rpdo(co_node *node, can_message *msg) {
    int error = 0;
    int entry_num = -1;
    // See if this RPDO is enabled
    for (int i = 0; i < num_rpdos; i++) {
        uint16_t index = 0x1400 + i;
        uint32_t data;
        od_read(node->od, index, 1, &data);
        if (data == msg->id) {
            entry_num = i;
            break;
        }
    }
    if (entry_num >= 0) {
        // RPDO entry found, so it is enabled. Now map data into OD entries
        uint16_t index = 0x1600 + entry_num;
        int num_params = 0;
        uint32_t data;
        // Find the number of mapping parameters
        od_result result = od_read(node->od, index, 0, &data);
        if (result == OD_RESULT_OK) {
            num_params = data;
            uint32_t data1 = msg->data[0] | (msg->data[1] << 8) | (msg->data[2] << 16) | (msg->data[3] << 24);
            uint32_t data2 = msg->data[4] | (msg->data[5] << 8) | (msg->data[6] << 16) | (msg->data[7] << 24);
            uint8_t current_bit = 0;
            for (int i = 1; i <= num_params; i++) {
                // Read the RPDO mapping parameter to find out where to store this piece of data
                result = od_read(node->od, index, i, &data);
                if (result != OD_RESULT_OK) {
                    error = 1;
                    break;
                }
                // dest_index/sub_index is where this piece of incoming data should be saved according to RPDO mapping parameter
                uint16_t dest_index = (data >> 16) & 0xFFFF;
                uint8_t dest_sub_index = (data >> 8) & 0xFF;
                uint8_t dest_num_bits = data & 0xFF;
                // Make sure this is not a dummy entry, because dummy entries are not supposed to be saved anywhere
                if (dest_index >= 0x1000) {
                    // Extract n bit long value from the message data (n = dest_num_bits and offset = current_bit)
                    uint32_t value = 0;
                    if (current_bit < 32) {
                        if (current_bit + dest_num_bits < 32) {
                            uint32_t mask = (1 << dest_num_bits) - 1;
                            value = (data1 >> current_bit) & mask;
                        } else {
                            int val1bits = 32 - current_bit;
                            int val2bits = dest_num_bits - val1bits;
                            uint32_t mask1 = (1 << val1bits) - 1;
                            uint32_t mask2 = (1 << val2bits) - 1;
                            value = (data1 >> current_bit) & mask1;
                            value += (data2 & mask2) << val1bits;
                        }
                    } else {
                        uint32_t mask = (1 << dest_num_bits) - 1;
                        value = data2 >> (current_bit - 32) & mask;
                    }
                    // Write value to OD
                    result = od_write(node->od, dest_index, dest_sub_index, value);
                    if (result != OD_RESULT_OK) {
                        error = 1;
                        break;
                    }
                }
                current_bit += dest_num_bits;
            }
        }
    }
    return error;
}
/****************************** Local Functions ******************************/
static int set_pdo_mapping(co_node *node, pdo_mapping_param mapping[], int num_params, uint16_t mapping_index) {
    int error = 0;
    od_result result;
    int total_len_bits = 0;
    for (int i = 0; i < num_params; i++) {
        // Check if mapped object exists
        od_access_type access_type;
        if (od_get_access_type(node->od, mapping[i].index, mapping[i].sub_index, &access_type) == OD_RESULT_OK) {
            // OK - mapped object exists
            total_len_bits += mapping[i].length;
            if (total_len_bits > 64) {
                error = 1;
                log_write_ln("pdo: ERROR: mapping failed, because mapped data exceeds 64 bit limit");
                break;
            }
            uint32_t value = ((uint32_t)mapping[i].index << 16) + ((uint32_t)mapping[i].sub_index << 8) + mapping[i].length;
            result = od_internal_write(node->od, mapping_index, i + 1, value);
            if (result != OD_RESULT_OK) {
                error = 1;
                break;
            }
        } else {
            // Error - mapped object doesn't exist
            log_write_ln("pdo: ERROR: mapping failed, because mapped object doesn't exist: %04Xh:%d", mapping[i].index, mapping[i].sub_index);
            error = 1;
            break;
        }
    }
    if (!error) {
        result = od_internal_write(node->od, mapping_index, 0, num_params);
    }
    if (result != OD_RESULT_OK) {
        error = 1;
    }
    return error;
}