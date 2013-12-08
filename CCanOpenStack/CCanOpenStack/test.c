//
//  test.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "test.h"
#include "log.h"
#include "test_can_bus.h"
#include "test_nmt.h"
#include "test_od.h"
#include "test_rpdo.h"
#include "test_tpdo.h"
#include "test_sdo.h"
#include "test_sdo_server.h"

/****************************** Global Functions *****************************/
extern void test_run_all_tests(void) {
    int error = 0;
    error |= test_can_bus_run();
    error |= test_od_run();
    error |= test_sdo_run();
    error |= test_sdo_server_run();
    error |= test_nmt_run();
    error |= test_rpdo_run();
    error |= test_tpdo_run();
    if (!error) {
        log_write_ln("test: ALL OK");
    } else {
        log_write_ln("test: ERRORS");
    }
}