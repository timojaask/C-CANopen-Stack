//
//  main.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include "log.h"
#include "test.h"

/****************************** Local Prototypes *****************************/

/****************************** Global Functions *****************************/
int main(int argc, const char * argv[])
{
    test_run_all_tests();
    log_write_ln("main: done");
    return 0;
}

/****************************** Local Functions ******************************/