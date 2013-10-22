//
//  test_sdo_server.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 22.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include <stdint.h>
#include "test_sdo_server.h"
#include "can_bus.h"
#include "log.h"
#include "od.h"
#include "sdo.h"
#include "sdo_server.h"
#include "utils.h"

/***************************** Local Definitions *****************************/
#define RSDO_BASE 0x580
/****************************** Local Variables ******************************/
static int error = 0;
static uint8_t test_running = 0;
static uint8_t msg_data[8];
static co_node node;
static od_object od_objects[] = {
    {0x1000, 0, 10000},
    {0x1001, 0, 10010},
    {0x2000, 0, 3},
    {0x2000, 1, 20001},
    {0x2000, 2, 20002},
    {0x2000, 3, 20003},
};
can_message message;
static sdo_server_command expected_response_command;
static uint16_t expected_index;
static uint8_t expected_sub_index;
static uint32_t expected_data;
/****************************** Local Prototypes *****************************/
static void send_upload_request(uint16_t index, uint8_t sub_index, uint32_t data);
static void send_download_request(uint16_t index, uint8_t sub_index, uint32_t data);
static void message_received_handler(can_message *message);
static void parse_upload_response(can_message *message);
static void parse_download_response(can_message *message);
/****************************** Global Functions *****************************/
extern int test_sdo_server_run(void) {
    test_running = 1;
    error = 0;
    can_bus_register_message_received_handler(message_received_handler);
    // Prepare object dictionary and other variables
    int num_objects = UTILS_ARRAY_SIZE(od_objects);
    object_dictionary od = {
        .objects = od_objects,
        .num_objects = num_objects
    };
    node.node_id = 123;
    node.od = &od;
    message.data = msg_data;
    message.data_len = 8;
    
    // Send upload requests
    // Set error to 1 before each request. If test passes, message handler will set it to 0.
    error = 1;
    send_upload_request(0x1000, 0, 10000); error = 1;
    send_upload_request(0x1001, 0, 10010); error = 1;
    send_upload_request(0x2000, 0, 3); error = 1;
    send_upload_request(0x2000, 1, 20001); error = 1;
    send_upload_request(0x2000, 2, 20002); error = 1;
    send_upload_request(0x2000, 3, 20003);
    if (error) {
        log_write_ln("test_sdo_server: upload responses failed");
    }
    
    // Send download requests
    error = 1;
    send_download_request(0x1000, 0, 2); error = 1;
    send_download_request(0x1001, 0, 3); error = 1;
    send_download_request(0x2000, 0, 4); error = 1;
    send_download_request(0x2000, 1, 5); error = 1;
    send_download_request(0x2000, 2, 6); error = 1;
    send_download_request(0x2000, 3, 7);
    if (error) {
        log_write_ln("test_sdo_server: download responses failed");
    }
    
    // Send upload requests for data that was written in download requests
    error = 1;
    send_upload_request(0x1000, 0, 2); error = 1;
    send_upload_request(0x1001, 0, 3); error = 1;
    send_upload_request(0x2000, 0, 4); error = 1;
    send_upload_request(0x2000, 1, 5); error = 1;
    send_upload_request(0x2000, 2, 6); error = 1;
    send_upload_request(0x2000, 3, 7);
    if (error) {
        log_write_ln("test_sdo_server: second upload responses failed");
    }
    
    test_running = 0;
    return error;
}

/****************************** Local Functions ******************************/
static void send_upload_request(uint16_t index, uint8_t sub_index, uint32_t data) {
    expected_response_command = sdo_command_server_upload_init;
    expected_index = index;
    expected_sub_index = sub_index;
    expected_data = data;
    log_write_ln("test_sdo_server: Sending upload request %04Xh:%d", index, sub_index);
    sdo_message_upload_request(&message, index, sub_index);
    sdo_server_process_request(&message, &node);
}
static void send_download_request(uint16_t index, uint8_t sub_index, uint32_t data) {
    expected_response_command = sdo_command_server_download_init;
    expected_index = index;
    expected_sub_index = sub_index;
    expected_data = data;
    log_write_ln("test_sdo_server: Sending download request %04Xh:%d = %lu", index, sub_index, data);
    sdo_message_expedited_download_request(&message, index, sub_index, data);
    sdo_server_process_request(&message, &node);
}

static void message_received_handler(can_message *message) {
    if (message->id == RSDO_BASE + node.node_id) {
        sdo_server_command cmd = sdo_get_server_command(message);
        if (cmd == expected_response_command) {
            switch (cmd) {
                case sdo_command_server_upload_init:
                    parse_upload_response(message);
                    break;
                case sdo_command_server_download_init:
                    parse_download_response(message);
                    break;
                default:
                    log_write_ln("test_sdo_server: ERROR: test error - unsupported command, even though it matches expected");
                    error = 1;
                    break;
            }
        } else {
            log_write_ln("test_sdo_server: ERROR: wrong response command");
            error = 1;
        }
    } else {
        log_write_ln("test_sdo_server: ERROR: wrong message COB ID: %02Xh, expected: %02Xh", message->id, (RSDO_BASE + node.node_id));
        error = 1;
    }
}
static void parse_download_response(can_message *message) {
    uint16_t index = sdo_get_index(message);
    uint8_t sub_index = sdo_get_sub_index(message);
    if (index == expected_index && sub_index == expected_sub_index) {
        log_write_ln("test_sdo_server: SDO download response OK");
        error = 0;
    } else {
        if (index != expected_index) {
            log_write_ln("test_sdo_server: ERROR: index: got: %04Xh, expected: %04Xh", index, expected_index);
        } else if (sub_index != expected_sub_index) {
            log_write_ln("test_sdo_server: ERROR: sub_index: got: %d, expected: %d", sub_index, expected_sub_index);
        }
        error = 1;
    }
}
static void parse_upload_response(can_message *message) {
    uint8_t expedited = sdo_get_expedited_bit(message);
    uint8_t size_indicated = sdo_get_size_indicated_bit(message);
    if (expedited && size_indicated) {
        uint16_t index = sdo_get_index(message);
        uint8_t sub_index = sdo_get_sub_index(message);
        uint32_t data = sdo_get_expedited_data(message);
        if (index == expected_index && sub_index == expected_sub_index && data == expected_data) {
            log_write_ln("test_sdo_server: SDO upload response OK");
            error = 0;
        } else {
            if (index != expected_index) {
                log_write_ln("test_sdo_server: ERROR: index: got: %04Xh, expected: %04Xh", index, expected_index);
            } else if (sub_index != expected_sub_index) {
                log_write_ln("test_sdo_server: ERROR: sub_index: got: %d, expected: %d", sub_index, expected_sub_index);
            } else if (data != expected_data) {
                log_write_ln("test_sdo_server: ERROR: data: got: %lu, expected: %lu", data, expected_data);
            }
            error = 1;
        }
    } else {
        log_write_ln("test_sdo_server: ERROR: transfer was not expedited - expedited expected");
        error = 1;
    }
}