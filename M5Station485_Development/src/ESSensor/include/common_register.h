#ifndef COMMON_REGISTER_H
#define COMMON_REGISTER_H

#define UART_NUM                                        2
#define RXD                                             16
#define TXD                                             17
#define BAUDRATE                                        9600
#define RXD_TIMEOUT_MS                                  100

#define FUNCTION_CODE_READ                              0x03
#define FUNCTION_CODE_WRITE                             0x06
#define FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS          0x10

// PH sensor: ES-PH-WT-01 (potential of Hydrogen sensor)    0x02
// ORP sensor: ES-DO-WT-01 (dissolved oxygen sensor)        0x05
// TUR sensor: ES-TUR-WT-01 (turbidity sensor)              0x01
// EC sensor: ES-EC-WT-01 (electrical conductivity sensor)  0x04

#define TUR_SENSOR                                      0x01
#define PH_SENSOR                                       0x02
#define EC_SENSOR                                       0x04
#define ORP_SENSOR                                      0x05

#define TUR_READ_REGISTER                               0x0001
#define TUR_READ_MEASUREMENT                            0x0002
#define PH_READ_REGISTER                                0x0001
#define PH_READ_MEASUREMENT                             0x0002
#define EC_READ_REGISTER                                0x0000
#define EC_READ_MEASUREMENT                             0x0002
#define ORP_READ_REGISTER                               0x0001
#define ORP_READ_MEASUREMENT                            0x0002

#endif // COMMON_REGISTER_H