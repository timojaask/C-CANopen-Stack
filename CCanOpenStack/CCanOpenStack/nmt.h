//
//  nmt.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 23.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_nmt_h
#define CCanOpenStack_nmt_h

/***************************** Global Definitions ****************************/
typedef enum {
    nmt_state_boot_up = 0,
    nmt_state_stopped = 4,
    nmt_state_operational = 5,
    nmt_state_pre_operational = 127
} nmt_state;
typedef enum {
    nmt_command_operational = 1,
    nmt_command_stopped = 2,
    nmt_command_pre_operational = 128,
    nmt_command_reset_node = 129,
    nmt_command_reset_communication = 130
} nmt_command;

#endif
