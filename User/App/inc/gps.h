#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#include <stdbool.h>

#define GPS_RX_SIZE 256

typedef struct {
    bool is_valid;

    uint8_t hour, minute, second;
    uint8_t day, month;
    uint16_t year;

    double latitude;
    double longitude;

    float speed_kmh;
    float heading;

    uint8_t satellites;
    float altitude;

} GPS_Data;

extern volatile GPS_Data gps_data;
extern uint8_t gps_rx_buffer[GPS_RX_SIZE];

void GPS_ParseBuffer(uint8_t *buf, uint16_t len);

#endif