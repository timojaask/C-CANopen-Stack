//
//  sdo_server.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 22.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_sdo_server_h
#define CCanOpenStack_sdo_server_h

#include "can_message.h"
#include "co_node.h"
#include "od.h"

/***************************** Global Definitions ****************************/

/***************************** Global Prototypes *****************************/
extern int sdo_server_init(co_node *node);
extern int sdo_server_get_rsdo_cob_id(co_node *node, uint32_t *cob_id);
extern int sdo_server_get_tsdo_cob_id(co_node *node, uint32_t *cob_id);
extern void sdo_server_process_request(can_message *message, co_node *node);
extern void sdo_message_download_response(can_message *message, uint16_t index, uint8_t sub_index);
extern void sdo_message_expedited_upload_response(can_message *message, uint16_t index, uint8_t sub_index, uint32_t data);
extern void sdo_message_server_abort_transfer(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code);

#endif
