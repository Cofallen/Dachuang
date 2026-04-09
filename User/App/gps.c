#include "gps.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

volatile GPS_Data gps_data = {0};
uint8_t gps_rx_buffer[GPS_RX_SIZE];

// ================= 工具函数 =================

static double safe_atof(char *s) {
    if (!s || *s == '\0') return 0;
    return atof(s);
}

static int safe_atoi(char *s) {
    if (!s || *s == '\0') return 0;
    return atoi(s);
}

// 获取字段（自动截断）
static char* get_field(char *str, int index) {
    int i = 0;
    char *p = str;

    while (*p && i < index) {
        if (*p == ',') i++;
        p++;
    }

    if (i != index) return NULL;

    char *end = p;
    while (*end && *end != ',' && *end != '*') end++;

    *end = '\0';
    return p;
}

// ================= 解析 =================

static void parse_rmc(char *buf) {
    char *status = get_field(buf, 2);

    // ❗ 无效直接丢弃
    if (!status || status[0] != 'A') {
        gps_data.is_valid = false;
        return;
    }

    gps_data.is_valid = true;

    char *time = get_field(buf, 1);
    char *lat  = get_field(buf, 3);
    char *ns   = get_field(buf, 4);
    char *lon  = get_field(buf, 5);
    char *ew   = get_field(buf, 6);
    char *spd  = get_field(buf, 7);
    char *trk  = get_field(buf, 8);
    char *date = get_field(buf, 9);

    if (time && *time) {
        double t = atof(time);
        gps_data.hour   = (int)(t / 10000);
        gps_data.minute = ((int)t % 10000) / 100;
        gps_data.second = (int)t % 100;
    }

    if (date && *date) {
        long d = atol(date);
        gps_data.day   = d / 10000;
        gps_data.month = (d % 10000) / 100;
        gps_data.year  = 2000 + d % 100;
    }

    if (lat && ns && *lat) {
        double v = atof(lat);
        int deg = v / 100;
        double min = v - deg * 100;
        gps_data.latitude = deg + min / 60.0;
        if (ns[0] == 'S') gps_data.latitude *= -1;
    }

    if (lon && ew && *lon) {
        double v = atof(lon);
        int deg = v / 100;
        double min = v - deg * 100;
        gps_data.longitude = deg + min / 60.0;
        if (ew[0] == 'W') gps_data.longitude *= -1;
    }

    if (spd && *spd) gps_data.speed_kmh = atof(spd) * 1.852f;
    if (trk && *trk) gps_data.heading   = atof(trk);
}

static void parse_gga(char *buf) {
    char *fix = get_field(buf, 6);

    if (safe_atoi(fix) == 0) return; // ❗ 未定位

    char *sat = get_field(buf, 7);
    char *alt = get_field(buf, 9);

    gps_data.satellites = safe_atoi(sat);
    gps_data.altitude   = safe_atof(alt);
}

// ================= 行解析 =================

static void GPS_ParseLine(char *line) {
    if (line[0] != '$') return;

    if (strncmp(line + 1, "GPRMC", 5) == 0)
        parse_rmc(line);
    else if (strncmp(line + 1, "GPGGA", 5) == 0)
        parse_gga(line);
}

// ================= 核心函数 =================

void GPS_ParseBuffer(uint8_t *buf, uint16_t len) {
    char line[128];
    uint16_t idx = 0;

    for (uint16_t i = 0; i < len; i++) {
        char c = buf[i];

        if (c == '\n') {
            line[idx] = '\0';
            GPS_ParseLine(line);
            idx = 0;
        } else if (c != '\r') {
            if (idx < sizeof(line) - 1)
                line[idx++] = c;
            else
                idx = 0;
        }
    }
}