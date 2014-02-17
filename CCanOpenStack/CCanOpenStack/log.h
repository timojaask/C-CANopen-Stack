//
//  log.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_log_h
#define CCanOpenStack_log_h

/***************************** Global Prototypes *****************************/

/**
 * @brief Writes a string to a log with an additional line break.
 * @param format C printf format style
 */
extern void log_write_ln(const char *format, ...);
/**
 * @brief Writes a string to a log as it is (witout additional line break).
 * @param format C printf format style
 */
extern void log_write(const char *format, ...);

#endif
