//
//  test.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "test.h"
#include "test_can_bus.h"

/****************************** Global Functions *****************************/
extern void test_run_all_tests(void) {
    test_can_bus_run();
}