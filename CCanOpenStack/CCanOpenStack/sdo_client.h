//
//  sdo_client.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 24.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_sdo_client_h
#define CCanOpenStack_sdo_client_h


#include <stdint.h>
#include "can_message.h"

/***************************** Global Prototypes *****************************/
extern void sdo_message_expedited_download_request(can_message *message, uint16_t index, uint8_t sub_index, uint32_t data);
extern void sdo_message_upload_request(can_message *message, uint16_t index, uint8_t sub_index);
extern void sdo_message_client_abort_transfer(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code);

#endif
