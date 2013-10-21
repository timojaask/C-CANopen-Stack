//
//  test_util.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 22.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "test_util.h"
#include "log.h"

extern void print_message_data(can_message *message) {
    log_write("[");
    for (int i = 0; i < message->data_len; i++) {
        log_write("%02Xh", message->data[i]);
        if (i < message->data_len - 1) {
            log_write(" ");
        }
    }
    log_write("]");
}
