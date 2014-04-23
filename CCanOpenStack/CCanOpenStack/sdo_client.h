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
#include "co_node.h"

/***************************** Global Prototypes *****************************/
/**
 * @brief Initializes an expedited download request and writes it into a raw CAN message
 * @param message pointer to a raw CAN message where the download request will be written
 * @param index Object dictionary index
 * @param sub_index Object dictionary sub index
 * @param data The data to be written into the given index and sub index
 */
extern void sdo_message_expedited_download_request(can_message *message, uint16_t index, uint8_t sub_index, uint32_t data);
/**
 * @brief Initializes an upload request and writes it into a raw CAN message
 * @param message pointer to a raw CAN message where the upload request will be written
 * @param index Object dictionary index
 * @param sub_index Object dictionary sub index
 */
extern void sdo_message_upload_request(can_message *message, uint16_t index, uint8_t sub_index);
/**
 * @brief Initializes an abort transfer message and writes it into a raw CAN message
 * @param message pointer to a raw CAN message where the abort transfer message will be written
 * @param index Object dictionary index
 * @param sub_index Object dictionary sub index
 * @param abort_code SDO abort code
 */
extern void sdo_message_client_abort_transfer(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code);
#endif
