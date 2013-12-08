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
 * @brief Add RPDO mapping to the object dictionary in order to enable receiving the given PDO.
 * @param node CANopen node, to which the RPDO mapping will be added.
 * @param cob_id RPDO COB-ID.
 * @param mapping An array of RPDO mapping parameters to be written to the OD.
 * @param num_params Number of entries in the RPDO array.
 * @return Returns error (0 = no error, otherwise error).
 */
extern int pdo_add_rpdo(co_node *node, uint16_t cob_id, pdo_mapping_param mapping[], int num_params);
// Returns error (0 = no error, otherwise error)
extern int pdo_add_tpdo(co_node *node, uint16_t cob_id, uint16_t inhibit, uint16_t event, pdo_mapping_param mapping[], int num_params);
// Returns error (0 = no error, otherwise error)
extern int pdo_process_rpdo(co_node *node, can_message *msg);
// Returns error (0 = no error, otherwise error)
extern int pdo_send_tpdo(co_node *node, uint32_t tick_count);
#endif
