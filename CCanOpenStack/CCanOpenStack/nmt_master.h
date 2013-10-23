//
//  nmt_master.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 23.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_nmt_master_h
#define CCanOpenStack_nmt_master_h

#include <stdint.h>
#include "can_message.h"
#include "nmt.h"

/***************************** Global Prototypes *****************************/
extern void nmt_master_send_command(uint8_t node_id, nmt_command command);
extern void nmt_master_process_heartbeat(can_message *message);

#endif
