//
//  od_initialize.h
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 27.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#ifndef CCanOpenStack_od_initialize_h
#define CCanOpenStack_od_initialize_h

od_object od_objects[] = {
    // Data types (for reference only)
    {0x0001, 0, od_data_type_uint8, od_access_type_ro, 1}, // BOOL: 1 bit
    {0x0002, 0, od_data_type_uint8, od_access_type_ro, 8}, // INT8: 8 bits
    {0x0003, 0, od_data_type_uint8, od_access_type_ro, 16}, // INT16: 16 bits
    {0x0004, 0, od_data_type_uint8, od_access_type_ro, 32}, // INT32: 32 bits
    {0x0005, 0, od_data_type_uint8, od_access_type_ro, 8}, // UINT8: 8 bits
    {0x0006, 0, od_data_type_uint8, od_access_type_ro, 16}, // UINT16: 16 bits
    {0x0007, 0, od_data_type_uint8, od_access_type_ro, 32}, // UINT32: 32 bits
    
    // Device type : no device profile
    {0x1000, 0, od_data_type_uint32, od_access_type_ro, 0},
    // Error register : no errors
    {0x1001, 0, od_data_type_uint8, od_access_type_ro, 0},
    // Device name : 4 char string value "NODE"
    {0x1008, 0, od_data_type_uint32, od_access_type_ro, 0x4E4F4445},
    // Heartbeat producer time : 0 ms (can be changed by master)
    {0x1017, 0, od_data_type_uint16, od_access_type_rw, 0},
    // Identity object
    {0x1018, 0, od_data_type_uint8, od_access_type_ro, 1},
    {0x1018, 1, od_data_type_uint32, od_access_type_ro, 12345}, // Vendor ID
    // SDO server parameters
    {0x1200, 0, od_data_type_uint8, od_access_type_ro, 2},
    {0x1200, 1, od_data_type_uint32, od_access_type_ro, 0x600}, // RSDO COB ID : 0x600 base + node ID (to config)
    {0x1200, 2, od_data_type_uint32, od_access_type_ro, 0x580}, // TSDO COB ID : 0x580 base + node ID (to config)
    // RPDO 1-4 parameters
    {0x1400, 0, od_data_type_uint8, od_access_type_ro, 2},
    {0x1400, 1, od_data_type_uint32, od_access_type_ro, (1 << 31)}, // RPDO1 COB ID : Not in use (to be configured)
    {0x1400, 2, od_data_type_uint8, od_access_type_ro, 0xFE}, // Transmission type : 0xFE (manufacturere specific)
    {0x1401, 0, od_data_type_uint8, od_access_type_ro, 2},
    {0x1401, 1, od_data_type_uint32, od_access_type_ro, (1 << 31)}, // RPDO2 COB ID : Not in use (to be configured)
    {0x1401, 2, od_data_type_uint8, od_access_type_ro, 0xFE}, // Transmission type : 0xFE (manufacturere specific)
    {0x1402, 0, od_data_type_uint8, od_access_type_ro, 2},
    {0x1402, 1, od_data_type_uint32, od_access_type_ro, (1 << 31)}, // RPDO3 COB ID : Not in use (to be configured)
    {0x1402, 2, od_data_type_uint8, od_access_type_ro, 0xFE}, // Transmission type : 0xFE (manufacturere specific)
    {0x1403, 0, od_data_type_uint8, od_access_type_ro, 2},
    {0x1403, 1, od_data_type_uint32, od_access_type_ro, (1 << 31)}, // RPDO4 COB ID : Not in use (to be configured)
    {0x1403, 2, od_data_type_uint8, od_access_type_ro, 0xFE}, // Transmission type : 0xFE (manufacturere specific)
    // RPDO 1-4 mapping
    // All fillsed with dummy entries - replace as necessary
    {0x1600, 0, od_data_type_uint8, od_access_type_ro, 8},
    {0x1600, 1, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1600, 2, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1600, 3, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1600, 4, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1600, 5, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1600, 6, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1600, 7, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1600, 8, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1601, 0, od_data_type_uint8, od_access_type_ro, 8},
    {0x1601, 1, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1601, 2, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1601, 3, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1601, 4, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1601, 5, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1601, 6, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1601, 7, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1601, 8, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1602, 0, od_data_type_uint8, od_access_type_ro, 8},
    {0x1602, 1, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1602, 2, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1602, 3, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1602, 4, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1602, 5, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1602, 6, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1602, 7, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1602, 8, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1603, 0, od_data_type_uint8, od_access_type_ro, 8},
    {0x1603, 1, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1603, 2, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1603, 3, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1603, 4, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1603, 5, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1603, 6, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1603, 7, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1603, 8, od_data_type_uint32, od_access_type_ro, 0x00020008},
    // TPDO 1-4 parameters
    {0x1800, 0, od_data_type_uint8, od_access_type_ro, 5},
    {0x1800, 1, od_data_type_uint32, od_access_type_ro, (1 << 31)}, // RPDO1 COB ID : uint32 : Not in use (to be configured)
    {0x1800, 2, od_data_type_uint8, od_access_type_ro, 0xFE}, // Transmission type : uint8 : 0xFE (manufacturere specific)
    {0x1800, 3, od_data_type_uint16, od_access_type_ro, 100}, // Inhibit time : uint16 : 100 * 100 us (microseconds) = 10 ms
    // {0x1800, 4, 0}, // Reserved
    {0x1800, 5, od_data_type_uint16, od_access_type_ro, 200}, // Event time : uint16 : 200 ms (milliseconds)
    {0x1801, 0, od_data_type_uint8, od_access_type_ro, 5},
    {0x1801, 1, od_data_type_uint32, od_access_type_ro, (1 << 31)}, // RPDO2 COB ID : uint32 : Not in use (to be configured)
    {0x1801, 2, od_data_type_uint8, od_access_type_ro, 0xFE}, // Transmission type : uint8 : 0xFE (manufacturere specific)
    {0x1801, 3, od_data_type_uint16, od_access_type_ro, 100}, // Inhibit time : uint16 : 100 * 100 us (microseconds) = 10 ms
    // {0x1801, 4, 0}, // Reserved
    {0x1801, 5, od_data_type_uint16, od_access_type_ro, 200}, // Event time : uint16 : 200 ms (milliseconds)
    {0x1802, 0, od_data_type_uint8, od_access_type_ro, 5},
    {0x1802, 1, od_data_type_uint32, od_access_type_ro, (1 << 31)}, // RPDO3 COB ID : uint32 : Not in use (to be configured)
    {0x1802, 2, od_data_type_uint8, od_access_type_ro, 0xFE}, // Transmission type : uint8 : 0xFE (manufacturere specific)
    {0x1802, 3, od_data_type_uint16, od_access_type_ro, 100}, // Inhibit time : uint16 : 100 * 100 us (microseconds) = 10 ms
    // {0x1802, 4, 0}, // Reserved
    {0x1802, 5, od_data_type_uint16, od_access_type_ro, 200}, // Event time : uint16 : 200 ms (milliseconds)
    {0x1803, 0, od_data_type_uint8, od_access_type_ro, 5},
    {0x1803, 1, od_data_type_uint32, od_access_type_ro, (1 << 31)}, // RPDO4 COB ID : uint32 : Not in use (to be configured)
    {0x1803, 2, od_data_type_uint8, od_access_type_ro, 0xFE}, // Transmission type : uint8 : 0xFE (manufacturere specific)
    {0x1803, 3, od_data_type_uint16, od_access_type_ro, 100}, // Inhibit time : uint16 : 100 * 100 us (microseconds) = 10 ms
    // {0x1803, 4, 0}, // Reserved
    {0x1803, 5, od_data_type_uint16, od_access_type_ro, 200}, // Event time : uint16 : 200 ms (milliseconds)
    // TPDO 1-4 mapping
    // All fillsed with dummy entries - replace as necessary
    {0x1A00, 0, od_data_type_uint8, od_access_type_ro, 8},
    {0x1A00, 1, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A00, 2, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A00, 3, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A00, 4, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A00, 5, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A00, 6, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A00, 7, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A00, 8, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A01, 0, od_data_type_uint8, od_access_type_ro, 8},
    {0x1A01, 1, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A01, 2, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A01, 3, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A01, 4, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A01, 5, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A01, 6, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A01, 7, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A01, 8, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A02, 0, od_data_type_uint8, od_access_type_ro, 8},
    {0x1A02, 1, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A02, 2, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A02, 3, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A02, 4, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A02, 5, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A02, 6, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A02, 7, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A02, 8, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A03, 0, od_data_type_uint8, od_access_type_ro, 8},
    {0x1A03, 1, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A03, 2, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A03, 3, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A03, 4, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A03, 5, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A03, 6, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A03, 7, od_data_type_uint32, od_access_type_ro, 0x00020008},
    {0x1A03, 8, od_data_type_uint32, od_access_type_ro, 0x00020008},
    // NMT startup
    {0x1F80, 0, od_data_type_uint32, od_access_type_ro, (1 << 2)}, // bit 2: 0 = auto start, 1 = no auto start)
    
    // Manufacturer specific goes here
};

#endif
