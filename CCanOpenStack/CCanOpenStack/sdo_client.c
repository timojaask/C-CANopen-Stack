//
//  sdo_client.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 24.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "sdo_client.h"
#include "sdo.h"

/****************************** Local Prototypes *****************************/
static void sdo_set_client_command(can_message *message, sdo_client_command command);

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

extern void sdo_message_upload_request(can_message *message, uint16_t index, uint8_t sub_index) {
    sdo_set_client_command(message, sdo_command_client_upload_init);
    sdo_set_index(message, index);
    sdo_set_sub_index(message, sub_index);
}

extern void sdo_message_client_abort_transfer(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code) {
    sdo_set_client_command(message, sdo_command_client_abort_transfer);
    sdo_set_message_abort_transfer_data(message, index, sub_index, abort_code);
}

/****************************** Local Functions ******************************/
static void sdo_set_client_command(can_message *message, sdo_client_command command) {
    set_sdo_command(message, (int)command);
}