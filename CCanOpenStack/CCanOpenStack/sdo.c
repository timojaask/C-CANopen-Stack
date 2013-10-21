//
//  sdo.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 21.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "sdo.h"

/****************************** Local Prototypes *****************************/
static void sdo_set_message_abort_transfer_data(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code);
static void sdo_set_server_command(can_message *message, sdo_server_command command);
static void sdo_set_client_command(can_message *message, sdo_client_command command);
static void sdo_set_expedited_bit(can_message *message, uint8_t expedited);
static void sdo_set_size_indicated_bit(can_message *message, uint8_t size_indicated);
/* Note: Also sets data size indicated bit */
static void sdo_set_expedited_data_size(can_message *message, int data_size);
static void sdo_set_index(can_message *message, uint16_t index);
static void sdo_set_sub_index(can_message *message, uint8_t sub_index);
static void sdo_set_expedited_data(can_message *message, uint32_t data);
static int get_sdo_command(can_message *message);
static void set_sdo_command(can_message *message, int sdo_command);

/****************************** Global Functions *****************************/
extern void sdo_message_expedited_download_request(can_message *message, uint16_t index, uint8_t sub_index, uint32_t data) {
    sdo_set_client_command(message, sdo_command_client_download_init);
    sdo_set_expedited_bit(message, 1);
    sdo_set_size_indicated_bit(message, 1);
    sdo_set_expedited_data_size(message, 4);
    sdo_set_index(message, index);
    sdo_set_sub_index(message, sub_index);
    sdo_set_expedited_data(message, data);
}

extern void sdo_message_download_response(can_message *message, uint16_t index, uint8_t sub_index) {
    sdo_set_server_command(message, sdo_command_server_download_init);
    sdo_set_index(message, index);
    sdo_set_sub_index(message, sub_index);
}

extern void sdo_message_upload_request(can_message *message, uint16_t index, uint8_t sub_index) {
    sdo_set_client_command(message, sdo_command_client_upload_init);
    sdo_set_index(message, index);
    sdo_set_sub_index(message, sub_index);
}

extern void sdo_message_expedited_upload_response(can_message *message, uint16_t index, uint8_t sub_index, uint32_t data) {
    sdo_set_server_command(message, sdo_command_server_upload_init);
    sdo_set_expedited_bit(message, 1);
    sdo_set_size_indicated_bit(message, 1);
    sdo_set_expedited_data_size(message, 4);
    sdo_set_index(message, index);
    sdo_set_sub_index(message, sub_index);
    sdo_set_expedited_data(message, data);
}

extern void sdo_message_server_abort_transfer(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code) {
    sdo_set_server_command(message, sdo_command_server_abort_transfer);
    sdo_set_message_abort_transfer_data(message, index, sub_index, abort_code);
}

extern void sdo_message_client_abort_transfer(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code) {
    sdo_set_client_command(message, sdo_command_client_abort_transfer);
    sdo_set_message_abort_transfer_data(message, index, sub_index, abort_code);
}

extern sdo_server_command sdo_get_server_command(can_message *message) {
    sdo_server_command command = (sdo_server_command)get_sdo_command(message);
    return command;
}

extern sdo_client_command sdo_get_client_command(can_message *message) {
    sdo_client_command command = (sdo_client_command)get_sdo_command(message);
    return command;
}

extern uint8_t sdo_get_expedited_bit(can_message *message) {
    uint8_t expedited = 0;
    if (message->data_len >= 1) {
        expedited = ((message->data[0] >> 1) & 0x1);
    }
    return expedited;
}

extern uint8_t sdo_get_size_indicated_bit(can_message *message) {
    uint8_t size_indicated = 0;
    if (message->data_len >= 1) {
        size_indicated = (message->data[0] & 0x1);
    }
    return size_indicated;
}

extern int sdo_get_expedited_data_size(can_message *message) {
    int data_size = 0;
    if (message->data_len >= 4) {
        if (sdo_get_size_indicated_bit(message)) {
            int num_bytes_not_set = ((message->data[0] >> 2) & 0x3);
            data_size = 4 - num_bytes_not_set;
        }
    }
    return data_size;
}

extern uint16_t sdo_get_index(can_message *message) {
    uint16_t index = 0;
    if (message->data_len >= 4) {
        index |= message->data[1];
        index |= ((uint16_t)message->data[2]) << 8;
    }
    return index;
}

extern uint8_t sdo_get_sub_index(can_message *message) {
    uint8_t sub_index = 0;
    if (message->data_len >= 4) {
        sub_index = message->data[3];
    }
    return sub_index;
}

extern uint32_t sdo_get_expedited_data(can_message *message) {
    uint32_t data = 0;
    if (message->data_len == 8) {
        data |= message->data[4];
        data |= ((uint32_t)message->data[5]) << 8;
        data |= ((uint32_t)message->data[6]) << 16;
        data |= ((uint32_t)message->data[7]) << 24;
    }
    return data;
}

/****************************** Local Functions ******************************/
static void sdo_set_message_abort_transfer_data(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code) {
    sdo_set_index(message, index);
    sdo_set_sub_index(message, sub_index);
    sdo_set_expedited_data(message, abort_code);
}

static void sdo_set_server_command(can_message *message, sdo_server_command command) {
    set_sdo_command(message, (int)command);
}

static void sdo_set_client_command(can_message *message, sdo_client_command command) {
    set_sdo_command(message, (int)command);
}

static void sdo_set_expedited_bit(can_message *message, uint8_t expedited) {
    if (message->data_len >= 1) {
        message->data[0] &= ~(0x1 << 1);
        message->data[0] |= ((expedited & 0x1) << 1);
    }
}

static void sdo_set_size_indicated_bit(can_message *message, uint8_t size_indicated) {
    if (message->data_len >= 1) {
        message->data[0] &= ~(0x1);
        message->data[0] |= (size_indicated & 0x1);
    }
}

/* Note: Also sets data size indicated bit */
static void sdo_set_expedited_data_size(can_message *message, int data_size) {
    if (message->data_len >= 4 && data_size <= 4 && data_size >= 0) {
        sdo_set_size_indicated_bit(message, 1);
        int num_bytes_not_set = 4 - data_size;
        message->data[0] &= ~(0x3 << 2);
        message->data[0] |= (num_bytes_not_set << 2);
    }
}

static void sdo_set_index(can_message *message, uint16_t index) {
    if (message->data_len >= 4) {
        message->data[1] = index & 0xFF;
        message->data[2] = (index >> 8) & 0xFF;
    }
}

static void sdo_set_sub_index(can_message *message, uint8_t sub_index) {
    if (message->data_len >= 4) {
        message->data[3] = sub_index;
    }
}

static void sdo_set_expedited_data(can_message *message, uint32_t data) {
    if (message->data_len == 8) {
        message->data[4] = data & 0xFF;
        message->data[5] = (data >> 8) & 0xFF;
        message->data[6] = (data >> 16) & 0xFF;
        message->data[7] = (data >> 24  ) & 0xFF;
    }
}

static int get_sdo_command(can_message *message) {
    int command = (int)sdo_command_server_abort_transfer;
    if (message->data_len >= 1) {
        command = (int)((message->data[0] >> 5) & 0x7);
    }
    return command;
}

static void set_sdo_command(can_message *message, int sdo_command) {
    if (message->data_len >= 1) {
        // first make sure the bits are empty
        message->data[0] &= ~(0x7 << 5);
        message->data[0] |= ((sdo_command & 0x7) << 5);
    }
}