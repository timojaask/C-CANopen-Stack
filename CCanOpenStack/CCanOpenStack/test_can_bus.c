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
#include "test_util.h"

/****************************** Local Variables ******************************/
static uint16_t next_id = 0;
static uint8_t next_len = 0;
static uint8_t next_data[8];
static int error = 0;
static uint8_t test_running = 0;

/****************************** Local Prototypes *****************************/
static void send_message(uint16_t id, uint8_t len, uint32_t data);
static void test_message_received_handler1(can_message *message);
static void test_message_received_handler2(can_message *message);
static void check_received_message(can_message *message, int handler_num);

/****************************** Global Functions *****************************/
extern int test_can_bus_run(void) {
    test_running = 1;
    error |= can_bus_register_message_received_handler(test_message_received_handler1);
    error |= can_bus_register_message_received_handler(test_message_received_handler2);
    
    send_message(0x123, 4, 0x12345);
    send_message(0x456, 6, 0x98765);
    send_message(0x789, 8, 0x23452);
    send_message(0x123, 2, 0x3456);
    test_running = 0;
    return error;
}

/****************************** Local Functions ******************************/
static void send_message(uint16_t id, uint8_t len, uint32_t data) {
    next_id = id;
    next_len = len;
    for (int i = 0; i < len; i++) {
        next_data[i] = (data >> (i*8)) & 0xFF;
    }
    can_message msg = {.id = next_id, .data_len = next_len, .data = next_data};
    can_bus_send_message(&msg);
}
static void test_message_received_handler1(can_message *message) {
    check_received_message(message, 1);

}
static void test_message_received_handler2(can_message *message) {
    check_received_message(message, 2);
}
static void check_received_message(can_message *message, int handler_num) {
    if (test_running) {
        if (message->id != next_id) {
            error = 1;
            log_write("test_can_bus: message received handler %d: wrong id, id:%Xh, len:%d, data:", handler_num, message->id, message->data_len);
        }
        if (!error) {
            if (message->data_len != next_len) {
                error = 1;
                log_write("test_can_bus: message received handler %d: wrong len, id:%Xh, len:%d, data:", handler_num, message->id, message->data_len);
            }
        }
        if (!error) {
            // Compare each byte in data
            for (int i = 0; i < message->data_len; i++) {
                if (message->data[i] != next_data[i]) {
                    error = 1;
                    log_write("test_can_bus: message received handler %d: wrong data, id:%Xh, len:%d, data:", handler_num, message->id, message->data_len);
                    break;
                }
            }
        }
        if (!error) {
            log_write("test_can_bus: receive %d OK id:%Xh, len:%d, data:", handler_num, message->id, message->data_len);
        }
        print_message_data(message);
        log_write_ln("");
    }
}