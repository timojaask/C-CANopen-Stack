//
//  test_sdo.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 21.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "test_sdo.h"
#include "can_bus.h"
#include "log.h"
#include "sdo.h"
#include "test_util.h"

/***************************** Local Definitions *****************************/

/****************************** Local Variables ******************************/
static int error = 0;
static uint8_t msg_data[8];
static uint8_t next_is_from_client = 0;
static uint8_t next_is_from_server = 0;
static sdo_client_command next_client_command;
static sdo_server_command next_server_command;
static uint8_t next_is_expedited = 0;
static uint8_t next_is_size_indicated = 0;
static uint8_t next_data_size = 0;
static uint16_t next_index = 0;
static uint8_t next_sub_index = 0;
static uint32_t next_expedited_data = 0;
static can_message next_message;
static uint8_t test_running = 0;

/****************************** Local Prototypes *****************************/
static void test_download_request(uint16_t index, uint8_t sub_index, uint32_t data);
static void test_download_response(uint16_t index, uint8_t sub_index);
static void test_upload_request(uint16_t index, uint8_t sub_index);
static void test_upload_response(uint16_t index, uint8_t sub_index, uint32_t data);
static void test_client_abort(uint16_t index, uint8_t sub_index, sdo_abort_code abort_code);
static void test_server_abort(uint16_t index, uint8_t sub_index, sdo_abort_code abort_code);
static void message_received_handler(can_message *message);
static void check_expedited_transfer(can_message *message);
static void check_multiplexer_transfer(can_message *message);
static void check_abort_transfer(can_message *message);
static uint8_t check_multiplexer(can_message *message);
/****************************** Global Functions *****************************/
extern int test_sdo_run(void) {
    test_running = 1;
    error = 0;
    can_bus_register_message_received_handler(message_received_handler);
    next_message.id = 0x123;
    next_message.data_len = 8;
    next_message.data = msg_data;
    // Create and send various SDO messages
    test_download_request(0x2000, 1, 0x123456);
    test_download_request(0x2001, 9, 0x345);
    test_download_request(0xFF00, 0xF3, 0x9876);
    test_download_response(0x2000, 1);
    test_download_response(0x2001, 34);
    test_download_response(0xFF00, 44);
    test_upload_request(0x2000, 1);
    test_upload_request(0x2001, 9);
    test_upload_request(0xFF00, 0x56);
    test_upload_response(0x2000, 1, 0x123456);
    test_upload_response(0x2001, 9, 0x345);
    test_upload_response(0xFF00, 0xF3, 0x9876);
    test_client_abort(0x4040, 76, sdo_abort_access_failed_due_hardware_error);
    test_client_abort(0x5300, 54, sdo_abort_access_failed_due_hardware_error);
    test_client_abort(0x2031, 0x34, sdo_abort_access_failed_due_hardware_error);
    test_server_abort(0x4040, 76, sdo_abort_access_failed_due_hardware_error);
    test_server_abort(0x5300, 54, sdo_abort_access_failed_due_hardware_error);
    test_server_abort(0x2031, 0x34, sdo_abort_access_failed_due_hardware_error);
    test_running = 0;
    return error;
}

/****************************** Local Functions ******************************/
static void test_download_request(uint16_t index, uint8_t sub_index, uint32_t data) {
    next_is_from_client = 1;
    next_is_from_server = 0;
    next_client_command = sdo_command_client_download_init;
    next_is_expedited = 1;
    next_is_size_indicated = 1;
    next_data_size = 4;
    next_index = index;
    next_sub_index = sub_index;
    next_expedited_data = data;
    
    sdo_message_expedited_download_request(&next_message, next_index, next_sub_index, next_expedited_data);
    log_write_ln("test_sdo: download request: obj:%04Xh:%d data:%04Xh", index, sub_index, data);
    can_bus_send_message(&next_message);
}
static void test_download_response(uint16_t index, uint8_t sub_index) {
    next_is_from_client = 0;
    next_is_from_server = 1;
    next_server_command = sdo_command_server_download_init;
    next_is_expedited = 0;
    next_is_size_indicated = 0;
    next_data_size = 0;
    next_index = index;
    next_sub_index = sub_index;
    next_expedited_data = 0;
    
    sdo_message_download_response(&next_message, next_index, next_sub_index);
    log_write_ln("test_sdo: download response: obj:%04Xh:%d data:%04Xh", index, sub_index);
    can_bus_send_message(&next_message);
}
static void test_upload_request(uint16_t index, uint8_t sub_index) {
    next_is_from_client = 1;
    next_is_from_server = 0;
    next_client_command = sdo_command_client_upload_init;
    next_is_expedited = 0;
    next_is_size_indicated = 0;
    next_data_size = 0;
    next_index = index;
    next_sub_index = sub_index;
    next_expedited_data = 0;
    
    sdo_message_upload_request(&next_message, next_index, next_sub_index);
    log_write_ln("test_sdo: upload request: obj:%04Xh:%d data:%04Xh", index, sub_index);
    can_bus_send_message(&next_message);
}
static void test_upload_response(uint16_t index, uint8_t sub_index, uint32_t data) {
    next_is_from_client = 0;
    next_is_from_server = 1;
    next_server_command = sdo_command_server_upload_init;
    next_is_expedited = 1;
    next_is_size_indicated = 1;
    next_data_size = 4;
    next_index = index;
    next_sub_index = sub_index;
    next_expedited_data = data;
    
    sdo_message_expedited_upload_response(&next_message, next_index, next_sub_index, next_expedited_data);
    log_write_ln("test_sdo: upload response: obj:%04Xh:%d data:%04Xh", index, sub_index, data);
    can_bus_send_message(&next_message);
}
static void test_client_abort(uint16_t index, uint8_t sub_index, sdo_abort_code abort_code) {
    next_is_from_client = 1;
    next_is_from_server = 0;
    next_client_command = sdo_command_client_abort_transfer;
    next_is_expedited = 1;
    next_is_size_indicated = 1;
    next_data_size = 4;
    next_index = index;
    next_sub_index = sub_index;
    next_expedited_data = abort_code;
    
    sdo_message_client_abort_transfer(&next_message, index, sub_index, abort_code);
    log_write_ln("test_sdo: client abort transfer: obj:%04Xh:%d data:%04Xh", index, sub_index, abort_code);
    can_bus_send_message(&next_message);
}
static void test_server_abort(uint16_t index, uint8_t sub_index, sdo_abort_code abort_code) {
    next_is_from_client = 0;
    next_is_from_server = 1;
    next_server_command = sdo_command_server_abort_transfer;
    next_is_expedited = 1;
    next_is_size_indicated = 1;
    next_data_size = 4;
    next_index = index;
    next_sub_index = sub_index;
    next_expedited_data = abort_code;
    
    sdo_message_server_abort_transfer(&next_message, index, sub_index, abort_code);
    log_write_ln("test_sdo: server abort transfer: obj:%04Xh:%d data:%04Xh", index, sub_index, abort_code);
    can_bus_send_message(&next_message);
}

static void message_received_handler(can_message *message) {
    if (test_running) {
        if (next_is_from_client) {
            sdo_client_command r_command = sdo_get_client_command(message);
            if (r_command == next_client_command) {
                if (r_command == sdo_command_client_download_init) {
                    // Download init command
                    check_expedited_transfer(message);
                } else if (r_command == sdo_command_client_upload_init) {
                    // Upload init command
                    check_multiplexer_transfer(message);
                } else if (r_command == sdo_command_client_abort_transfer) {
                    // Abort transfer command
                    check_abort_transfer(message);
                } else {
                    // Other client command
                    log_write_ln("test_sdo: test not implemented");
                    error = 1;
                }
            } else {
                log_write_ln("test_sdo: wrong command");
                error = 1;
            }
        } else if (next_is_from_server) {
            // From server
            sdo_server_command r_command = sdo_get_server_command(message);
            if (r_command == next_server_command) {
                if (r_command == sdo_command_server_download_init) {
                    // Download init command
                    check_multiplexer_transfer(message);
                } else if (r_command == sdo_command_server_upload_init) {
                    // Upload init command
                    check_expedited_transfer(message);
                } else if (r_command == sdo_command_server_abort_transfer) {
                    // Abort transfer command
                    check_abort_transfer(message);
                } else {
                    // Other server command
                    log_write_ln("test_sdo: test not implemented");
                    error = 1;
                }
            } else {
                log_write_ln("test_sdo: wrong command");
                error = 1;
            }
        } else {
            log_write_ln("test_sdo: test setup error");
            error = 1;
        }
    }
}

static void check_expedited_transfer(can_message *message) {
    uint8_t r_expedited = sdo_get_expedited_bit(message);
    if (r_expedited == next_is_expedited) {
        uint8_t r_size_indicated = sdo_get_size_indicated_bit(message);
        if (r_size_indicated == next_is_size_indicated) {
            if (r_expedited && r_size_indicated) {
                // Expedited
                uint8_t r_size = sdo_get_expedited_data_size(message);
                if (r_size == next_data_size) {
                    if (check_multiplexer(message) == 0) {
                        uint32_t r_data = sdo_get_expedited_data(message);
                        if (r_data == next_expedited_data) {
                            // OK
                            log_write_ln("test_sdo: download init request OK");
                        } else {
                            log_write_ln("test_sdo: wrong data");
                            error = 1;
                        }
                    }
                } else {
                    log_write_ln("test_sdo: wrong data size");
                    error = 1;
                }
            } else if (!r_expedited && !r_size_indicated) {
                // Not expedited
                log_write_ln("test_sdo: test not implemented");
                error = 1;
            } else {
                log_write_ln("test_sdo: test setup error");
                error = 1;
            }
        } else {
            log_write_ln("test_sdo: wrong size indicated");
            error = 1;
        }
    } else {
        log_write_ln("test_sdo: wrong expedited");
        error = 1;
    }
}

static void check_multiplexer_transfer(can_message *message) {
    uint16_t r_index = sdo_get_index(message);
    if (r_index == next_index) {
        uint8_t r_sub_index = sdo_get_sub_index(message);
        if (r_sub_index == next_sub_index) {
            // OK
            log_write_ln("test_sdo: upload init request OK");
        } else {
            log_write_ln("test_sdo: wrong sub index");
            error = 1;
        }
    } else {
        log_write_ln("test_sdo: wrong index");
        error = 1;
    }
}

static void check_abort_transfer(can_message *message) {
    if (check_multiplexer(message) == 0) {
        uint32_t r_data = sdo_get_expedited_data(message);
        if (r_data == next_expedited_data) {
            // OK
            log_write_ln("test_sdo: abort transfer OK");
        } else {
            log_write_ln("test_sdo: wrong error code");
            error = 1;
        }
    }
}

static uint8_t check_multiplexer(can_message *message) {
    uint16_t r_index = sdo_get_index(message);
    if (r_index == next_index) {
        uint8_t r_sub_index = sdo_get_sub_index(message);
        if (r_sub_index == next_sub_index) {
            // index and sub_index OK
        } else {
            log_write_ln("test_sdo: wrong sub index");
            error = 1;
        }
    } else {
        log_write_ln("test_sdo: wrong index");
        error = 1;
    }
    return error;
}