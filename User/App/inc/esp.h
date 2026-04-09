#ifndef ESP_H
#define ESP_H

#include <stdint.h>
#include <stdbool.h>

// 配置参数
#define ESP_AP_SSID     "GPS_Tracker"   // WiFi 热点名称
#define ESP_AP_PASSWORD "12345678"      // 密码（至少8位）
#define ESP_TCP_PORT    80              // TCP 服务器端口

// 状态机
typedef enum {
    ESP_STATE_IDLE,
    ESP_STATE_AP_READY,
    ESP_STATE_SERVER_READY,
    ESP_STATE_CLIENT_CONNECTED
} ESP_State;

// 外部访问状态
extern volatile ESP_State esp_state;


// 获取当前 GPS 数据的 JSON 字符串（由用户提供 GPS 数据接口）
// 这个函数需要您根据实际 GPS 结构体实现，或者直接在 esp.c 中引用全局 gps_data
// 简单起见，我们在 esp.c 中直接使用外部全局变量 gps_data（需要在 gps.h 中声明）
void ESP_SendJsonData(void);
void ESP_UART_IdleCallback(void);
void esp_task_entry();

#endif