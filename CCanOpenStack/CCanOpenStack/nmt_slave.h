//
//  nmt_slave.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 23.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_nmt_slave_h
#define CCanOpenStack_nmt_slave_h

#include "can_message.h"

/***************************** Global Prototypes *****************************/
extern void nmt_slave_process_command(can_message *message);
extern void nmt_slave_send_heartbeat(void);

#endif
