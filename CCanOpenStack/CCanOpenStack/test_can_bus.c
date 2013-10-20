//
//  test_can_bus.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "test_can_bus.h"
#include "can_bus.h"
#include "log.h"

/****************************** Local Variables ******************************/
static uint16_t next_id = 0;
static uint8_t next_len = 0;
static uint32_t next_data = 0;
static int error = 0;
/****************************** Local Prototypes *****************************/
static void send_message(uint16_t id, uint8_t len, uint32_t data);
static void test_message_received_handler1(can_message *message);
static void test_message_received_handler2(can_message *message);

/****************************** Global Functions *****************************/
extern int test_can_bus_run(void) {
    error |= can_bus_register_message_received_handler(test_message_received_handler1);
    error |= can_bus_register_message_received_handler(test_message_received_handler2);
    
    send_message(0x123, 4, 0x12345);
    send_message(0x456, 6, 0x98765);
    send_message(0x789, 8, 0x23452);
    send_message(0x123, 2, 0x3456);
    return error;
}

/****************************** Local Functions ******************************/
static void send_message(uint16_t id, uint8_t len, uint32_t data) {
    next_id = id;
    next_len = len;
    next_data = data;
    can_message msg = {.id = next_id, .data_len = next_len, .data = next_data};
    can_bus_send_message(&msg);
}
static void test_message_received_handler1(can_message *message) {
    if (message->id != next_id) {
        error = 1;
        log_write_ln("test_can_bus: message received handler 1: wrong id, id:%Xh, len:%d, data:[%Xh]", message->id, message->data_len, message->data);
    }
    if (message->data_len != next_len) {
        error = 1;
        log_write_ln("test_can_bus: message received handler 1: wrong len, id:%Xh, len:%d, data:[%Xh]", message->id, message->data_len, message->data);
    }
    if (message->data != next_data) {
        error = 1;
        log_write_ln("test_can_bus: message received handler 1: wrong data, id:%Xh, len:%d, data:[%Xh]", message->id, message->data_len, message->data);
    }
}
static void test_message_received_handler2(can_message *message) {
    if (message->id != next_id) {
        error = 1;
        log_write_ln("test_can_bus: message received handler 2: wrong id, id:%Xh, len:%d, data:[%Xh]", message->id, message->data_len, message->data);
    }
    if (message->data_len != next_len) {
        error = 1;
        log_write_ln("test_can_bus: message received handler 2: wrong len, id:%Xh, len:%d, data:[%Xh]", message->id, message->data_len, message->data);
    }
    if (message->data != next_data) {
        error = 1;
        log_write_ln("test_can_bus: message received handler 2: wrong data, id:%Xh, len:%d, data:[%Xh]", message->id, message->data_len, message->data);
    }
}