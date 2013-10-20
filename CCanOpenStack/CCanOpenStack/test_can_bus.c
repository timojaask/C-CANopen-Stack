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

/****************************** Local Prototypes *****************************/
static void test_message_received_handler1(can_message *message);
static void test_message_received_handler2(can_message *message);

/****************************** Global Functions *****************************/
extern void test_can_bus_run(void) {
    can_bus_register_message_received_handler(test_message_received_handler1);
    can_bus_register_message_received_handler(test_message_received_handler2);
    uint8_t data[] = { 1, 2, 3, 4};
    can_message msg = {.id = 123, .data_len = 4, .data = data};
    can_bus_send_message(&msg);
}

/****************************** Local Functions ******************************/

static void test_message_received_handler1(can_message *message) {
    log_write("test_can_bus: message received handler 1, id:%Xh, len:%d, data:[", message->id, message->data_len);
    for (int i = 0; i < message->data_len; i++) {
        log_write("%02Xh", message->data[i]);
        if (i < message->data_len - 1) {
            log_write(" ");
        }
    }
    log_write_ln("]");
}

static void test_message_received_handler2(can_message *message) {
    log_write("test_can_bus: message received handler 2, id:%Xh, len:%d, data:[", message->id, message->data_len);
    for (int i = 0; i < message->data_len; i++) {
        log_write("%02Xh", message->data[i]);
        if (i < message->data_len - 1) {
            log_write(" ");
        }
    }
    log_write_ln("]");
}