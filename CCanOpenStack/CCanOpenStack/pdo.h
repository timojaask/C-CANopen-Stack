//
//  pdo.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 27.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_pdo_h
#define CCanOpenStack_pdo_h

#include <stdint.h>
#include "can_message.h"
#include "co_node.h"

/***************************** Global Definitions ****************************/
typedef struct {
    uint16_t index;
    uint8_t sub_index;
    uint8_t length;
} pdo_mapping_param;

/***************************** Global Prototypes *****************************/
/**
 * @brief Initializes system PDO's. Call this before using PDO functions.
 * @param tick_count Current system tick count
 */
extern void pdo_initialize(uint32_t tick_count);
/**
 * @brief Add RPDO mapping to the object dictionary in order to enable receiving the given PDO.
 * @param node CANopen node, to which the RPDO mapping will be added.
 * @param cob_id RPDO COB-ID.
 * @param mapping An array of RPDO mapping parameters to be written to the OD.
 * @param num_params Number of entries in the RPDO array.
 * @return Returns error (0 = no error, otherwise error).
 */
extern int pdo_add_rpdo(co_node *node, uint16_t cob_id, pdo_mapping_param mapping[], int num_params);
/**
 * @brief Add TPDO mapping to the object dictionary in order to enable transmitting the given PDO.
 * @param node CANopen node, to which the TPDO mapping will be added.
 * @param cob_id TPDO COB-ID.
 * @param inhibit Minimum time between PDO transmissions in 100 us units.
 * @param event Maximum time between PDO trasmission in 1 ms units.
 * @param mapping An array of TPDO mapping parameters to be written to the OD.
 * @param num_params Number of entries in the TPDO array.
 * @return Returns error (0 = no error, otherwise error).
 */
extern int pdo_add_tpdo(co_node *node, uint16_t cob_id, uint16_t inhibit, uint16_t event, pdo_mapping_param mapping[], int num_params);
/**
 * @brief Processes a received PDO message
 * @param node CANopen node, which is supposed to receive the PDO
 * @param msg Raw CAN message, which contains PDO data
 * @return Returns error (0 = no error, otherwise error).
 */
extern int pdo_process_rpdo(co_node *node, can_message *msg);
/**
 * @brief Checks if any of TPDOs on a given node are ready for sending and sends them
 * @param node CANopen node that contains the TPDOs
 * @param tick_count Current system tick count
 * @return Returns error (0 = no error, otherwise error).
 */
extern int pdo_send_tpdo(co_node *node, uint32_t tick_count);
#endif
