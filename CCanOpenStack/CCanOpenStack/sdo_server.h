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
/**
 * @brief Initializes SDO server. Call this before using SDO server functionality.
 * @param node CANopen node, on which the SDO server will be initialized.
 * @return Returns error (0 = no error, otherwise error).
 */
extern int sdo_server_init(co_node *node);
/**
 * @brief Returns given node's RSDO COB-ID
 * @param node CANopen node
 * @param cob_id Pointer to a place where the COB-ID will be written
 * @return Returns error (0 = no error, otherwise error).
 */
extern int sdo_server_get_rsdo_cob_id(co_node *node, uint32_t *cob_id);
/**
 * @brief Returns given node's TSDO COB-ID
 * @param node CANopen node
 * @param cob_id Pointer to a place where the COB-ID will be written
 * @return Returns error (0 = no error, otherwise error).
 */
extern int sdo_server_get_tsdo_cob_id(co_node *node, uint32_t *cob_id);
/**
 * @brief Processes an SDO request and sends a response
 * @param message Raw CAN message containing SDO request
 * @param node CANopen node
 */
extern void sdo_server_process_request(can_message *message, co_node *node);
/**
 * @brief Initializes a download response and writes it into a raw CAN message
 * @param message pointer to a raw CAN message where the download response will be written
 * @param index Object dictionary index
 * @param sub_index Object dictionary sub index
 */
extern void sdo_message_download_response(can_message *message, uint16_t index, uint8_t sub_index);
/**
 * @brief Initializes an expedited upload response and writes it into a raw CAN message
 * @param message pointer to a raw CAN message where the upload response will be written
 * @param index Object dictionary index
 * @param sub_index Object dictionary sub index
 * @param data The data that was read from the given index and sub index
 */
extern void sdo_message_expedited_upload_response(can_message *message, uint16_t index, uint8_t sub_index, uint32_t data);
/**
 * @brief Initializes an abort transfer message and writes it into a raw CAN message
 * @param message pointer to a raw CAN message where the abort transfer message will be written
 * @param index Object dictionary index
 * @param sub_index Object dictionary sub index
 * @param abort_code SDO abort code
 */
extern void sdo_message_server_abort_transfer(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code);

#endif
