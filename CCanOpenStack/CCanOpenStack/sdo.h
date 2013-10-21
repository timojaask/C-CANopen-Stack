//
//  sdo.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 21.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_sdo_h
#define CCanOpenStack_sdo_h

#include <stdint.h>
#include "can_message.h"

/***************************** Global Definitions ****************************/
typedef enum {
    sdo_command_server_upload_segment = 0,
    sdo_command_server_download_segment = 1,
    sdo_command_server_upload_init = 2,
    sdo_command_server_download_init = 3,
    sdo_command_server_abort_transfer = 4,
} sdo_server_command;

typedef enum {
    sdo_command_client_download_segment = 0,
    sdo_command_client_download_init = 1,
    sdo_command_client_upload_init = 2,
    sdo_command_client_upload_segment = 3,
    sdo_command_client_abort_transfer = 4,
} sdo_client_command;

/***************************** Global Prototypes *****************************/
extern void sdo_message_expedited_download_request(can_message *message, uint16_t index, uint8_t sub_index, uint32_t data);
extern void sdo_message_download_response(can_message *message, uint16_t index, uint8_t sub_index);
extern void sdo_message_upload_request(can_message *message, uint16_t index, uint8_t sub_index);
extern void sdo_message_expedited_upload_response(can_message *message, uint16_t index, uint8_t sub_index, uint32_t data);
extern void sdo_message_server_abort_transfer(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code);
extern void sdo_message_client_abort_transfer(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code);
extern sdo_server_command sdo_get_server_command(can_message *message);
extern sdo_client_command sdo_get_client_command(can_message *message);
extern uint8_t sdo_get_expedited_bit(can_message *message);
extern uint8_t sdo_get_size_indicated_bit(can_message *message);
extern int sdo_get_expedited_data_size(can_message *message);
extern uint16_t sdo_get_index(can_message *message);
extern uint8_t sdo_get_sub_index(can_message *message);
extern uint32_t sdo_get_expedited_data(can_message *message);

#endif
