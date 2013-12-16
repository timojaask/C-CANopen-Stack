//
//  delay.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 8.12.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

/*******************************************************************
 * THIS FILE SHOULD IMPLEMENT FUNCTIONS DECLARED IN delay.h.
 * THIS SHOULD BE IMPLEMENTED ACCORDING TO PLATFORM THIS PROGRAM
 * IS INTENDED TO RUN ON.
 * FOR DEMONSTRATION PURPOSES AN EXAMPLE IMPLEMENTATION IS PROVIDED
 * FOR MAC OS X OPERATING SYSTEM.
 *******************************************************************/

#include <stdint.h>
#include <unistd.h>
#include "delay.h"

extern void delay_ms(uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
}
