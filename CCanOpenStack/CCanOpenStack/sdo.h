//
//  sdo.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 21.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_sdo_h
#define CCanOpenStack_sdo_h

#include <stdint.h>
#include "can_message.h"

/***************************** Global Definitions ****************************/
typedef enum {
    sdo_command_server_upload_segment = 0,
    sdo_command_server_download_segment = 1,
    sdo_command_server_upload_init = 2,
    sdo_command_server_download_init = 3,
    sdo_command_server_abort_transfer = 4,
} sdo_server_command;

typedef enum {
    sdo_command_client_download_segment = 0,
    sdo_command_client_download_init = 1,
    sdo_command_client_upload_init = 2,
    sdo_command_client_upload_segment = 3,
    sdo_command_client_abort_transfer = 4,
} sdo_client_command;

typedef enum {
    sdo_abort_toggle_bit_not_alternated = 0x05030000,
    sdo_abort_sdo_timed_out = 0x05040000,
    sdo_abort_invalid_command_specifier = 0x05040001,
    sdo_abort_invalid_block_size = 0x05040002,
    sdo_abort_invalid_sequence_number = 0x05040003,
    sdo_abort_crc_error = 0x05040004,
    sdo_abort_out_of_memory = 0x05040005,
    sdo_abort_unsupported_object_access = 0x06010000,
    sdo_abort_read_a_write_only_object = 0x06010001,
    sdo_abort_write_a_read_only_object = 0x06010002,
    sdo_abort_object_does_not_exist = 0x06020000,
    sdo_abort_object_cannot_be_mapped = 0x06040041,
    sdo_abort_object_len_bigger_than_max_pdo_len = 0x06040042,
    sdo_abort_general_parameter_incompatibility = 0x06040043,
    sdo_abort_general_internal_incompatibility = 0x06040047,
    sdo_abort_access_failed_due_hardware_error = 0x06060000,
    sdo_abort_length_of_service_parameter_does_not_match = 0x06070010,
    sdo_abort_length_of_service_parameter_is_too_high = 0x06070012,
    sdo_abort_length_of_service_parameter_is_too_low = 0x06070013,
    sdo_abort_subindex_does_not_exist = 0x06090011,
    sdo_abort_value_range_of_parameter_exceeded = 0x06090030,
    sdo_abort_value_of_parameter_written_is_too_high = 0x06090031,
    sdo_abort_value_of_parameter_written_is_too_low = 0x06090032,
    sdo_abort_maximum_value_less_than_minimum = 0x06090036,
    sdo_abort_general_error = 0x08000000,
    sdo_abort_data_transfer_error = 0x08000020,
    sdo_abort_data_transfer_error_reason_local_control = 0x08000021,
    sdo_abort_data_transfer_error_reason_device_state = 0x08000022,
    sdo_abort_od_dynamic_generation_failed = 0x08000023
} sdo_abort_code;

/***************************** Global Prototypes *****************************/
/**
 * @brief Parses a server command specifier from a raw CAN message
 * @param message A raw CAN message containing the SDO message
 * @return Returns server command specifier
 */
extern sdo_server_command sdo_get_server_command(can_message *message);
/**
 * @brief Parses a client command specifier from a raw CAN message
 * @param message A raw CAN message containing the SDO message
 * @return Returns client command specifier
 */
extern sdo_client_command sdo_get_client_command(can_message *message);
/**
 * @brief Parses a is_expedited bit from a raw CAN message
 * @param message A raw CAN message containing the SDO message
 * @return Returns the bit containing is_expedited information
 */
extern uint8_t sdo_get_expedited_bit(can_message *message);
/**
 * @brief Parses a size_indicated bit from a raw CAN message
 * @param message A raw CAN message containing the SDO message
 * @return Returns the bit containing size_indicated information
 */
extern uint8_t sdo_get_size_indicated_bit(can_message *message);
/**
 * @brief Parses data_size bits from a raw CAN message and returns actual data size in bytes
 * @param message A raw CAN message containing the SDO message
 * @return Returns actual data size in bytes
 */
extern int sdo_get_expedited_data_size(can_message *message);
/**
 * @brief Parses index bits from a raw CAN message
 * @param message A raw CAN message containing the SDO message
 * @return Returns object dictionary index
 */
extern uint16_t sdo_get_index(can_message *message);
/**
 * @brief Parses sub index bits from a raw CAN message
 * @param message A raw CAN message containing the SDO message
 * @return Returns object dictionary sub index
 */
extern uint8_t sdo_get_sub_index(can_message *message);
/**
 * @brief Parses data bits from a raw CAN message
 * @param message A raw CAN message containing the SDO message
 * @return Returns object dictionary data
 */
extern uint32_t sdo_get_expedited_data(can_message *message);
/**
 * @brief Initializes an abort transfer message and writes it into a raw CAN message
 * @param message pointer to a raw CAN message where the abort transfer message will be written
 * @param index Object dictionary index
 * @param sub_index Object dictionary sub index
 * @param abort_code SDO abort code
 */
extern void sdo_set_message_abort_transfer_data(can_message *message, uint16_t index, uint8_t sub_index, uint32_t abort_code);
/**
 * @brief Sets the is_expedited bit and writes it into the raw CAN message
 * @param message pointer to a raw CAN message where the is_expedited bit will be written
 * @param expedited The value of is_expedited bit
 */
extern void sdo_set_expedited_bit(can_message *message, uint8_t expedited);
/**
 * @brief Sets the size_indicated bit and writes it into the raw CAN message
 * @param message pointer to a raw CAN message where the size_indicated bit will be written
 * @param size_indicated The value of size_indicated bit
 */
extern void sdo_set_size_indicated_bit(can_message *message, uint8_t size_indicated);
/**
 * @brief Sets the data size bits and writes it into the raw CAN message (also sets size_indicated bit to 1)
 * @param message pointer to a raw CAN message where the data size bits will be written
 * @param data_size Data size in bytes
 */
extern void sdo_set_expedited_data_size(can_message *message, int data_size);
/**
 * @brief Sets the OD index bits and writes it into the raw CAN message
 * @param message pointer to a raw CAN message where the OD index bits will be written
 * @param index The object dictionary index
 */
extern void sdo_set_index(can_message *message, uint16_t index);
/**
 * @brief Sets the OD sub index bits and writes it into the raw CAN message
 * @param message pointer to a raw CAN message where the OD sub index bits will be written
 * @param sub_index The object dictionary sub index
 */
extern void sdo_set_sub_index(can_message *message, uint8_t sub_index);
/**
 * @brief Sets the OD object's data bits and writes it into the raw CAN message
 * @param message pointer to a raw CAN message where the OD object's data bits will be written
 * @param data The object dictionary object's data
 */
extern void sdo_set_expedited_data(can_message *message, uint32_t data);
/**
 * @brief Parses the SDO command specifier bits from a raw can message
 * @param message A raw CAN message containing the SDO
 * @return Returns SDO command specifier
 */
extern int get_sdo_command(can_message *message);
/**
 * @brief Sets the SDO command specifier bits and writes it into the raw CAN message
 * @param message pointer to a raw CAN message where the SDO command specifier bits will be written
 * @param sdo_command The SDO command specifier
 */
extern void set_sdo_command(can_message *message, int sdo_command);

#endif
