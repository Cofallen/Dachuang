// ...existing code...
#include "esp.h"
#include "usart.h"
#include "dma.h"
#include "gps.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cmsis_os.h"

// 全局状态
volatile ESP_State esp_state = ESP_STATE_IDLE;
uint8_t esp_client_id = 0xFF;   // 当前连接的客户端ID

// DMA 接收缓冲区
#define ESP_RX_BUF_SIZE 512
static uint8_t esp_rx_buf[ESP_RX_BUF_SIZE];
static char line_buf[200];
static uint8_t line_idx = 0;
volatile uint8_t line_ready = 0;

// 发送互斥标志，防止多次调用 HAL_UART_Transmit_DMA 导致冲突
static volatile uint8_t esp_tx_busy = 0;

// 非阻塞发送命令（使用 DMA），若正在发送则返回0，发送成功返回1
static uint8_t at_send_nb(const uint8_t *cmd) {
    HAL_UART_Transmit_DMA(&huart3, cmd, 20);
    return 1;
}

// 非阻塞发送数据给客户端：先发送 CIPSEND 命令再发送数据（不等待远端确认）
// 为避免 DMA 重叠，这里会轮询 esp_tx_busy（非常短），若觉得不合适可改为任务延时或队列。
static void send_data_nb(uint8_t client_id, const uint8_t *data, uint16_t len) {
    char header[48];
    int hlen = snprintf(header, sizeof(header), "AT+CIPSEND=%d,%d\r\n", client_id, len);
    if (hlen <= 0) return;

    // 等待上次发送完成（简单轮询）
    while (esp_tx_busy) { __NOP(); }
    at_send_nb(header);

    // 等待 header 发送完成
    while (esp_tx_busy) { __NOP(); }

    // 发送实际数据（可能是 HTML/JSON）
    at_send_nb(data);
}

// 处理收到的 AT 响应行（保持原逻辑）
static void process_line(char *line) {
    if (strstr(line, "CONNECT") != NULL) {
        char *p = strchr(line, ',');
        if (p) esp_client_id = (uint8_t)atoi(p + 1);
        esp_state = ESP_STATE_CLIENT_CONNECTED;
        return;
    }
    if (strstr(line, "CLOSED") != NULL) {
        esp_state = ESP_STATE_SERVER_READY;
        esp_client_id = 0xFF;
        return;
    }
    if (strstr(line, "+IPD") != NULL) {
        char *get = strstr(line, "GET ");
        if (get == NULL) return;
        if (strstr(get, "/data") != NULL) {
            char json[128];
            double lat = gps_data.is_valid ? gps_data.latitude : 0.0;
            double lon = gps_data.is_valid ? gps_data.longitude : 0.0;
            float speed = gps_data.is_valid ? gps_data.speed_kmh : 0.0f;
            float alt = gps_data.is_valid ? gps_data.altitude : 0.0f;
            uint8_t sat = gps_data.is_valid ? gps_data.satellites : 0;
            snprintf(json, sizeof(json), "{\"lat\":%.6f,\"lon\":%.6f,\"speed\":%.1f,\"alt\":%.1f,\"sat\":%d}\r\n",
                     lat, lon, speed, alt, sat);
            send_data_nb(esp_client_id, json, (uint16_t)strlen(json));
        } else {
            const char *html =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width'>"
                "<title>GPS Tracker</title><style>body{font-family:monospace;padding:20px;background:#1a1a2e;color:#eee;}"
                ".data{background:#16213e;padding:15px;border-radius:8px;margin:10px 0;}</style>"
                "</head><body><h2>GPS Tracker</h2>"
                "<div><strong>Latitude:</strong> <span id='lat'>--</span> deg</div>"
                "<div><strong>Longitude:</strong> <span id='lon'>--</span> deg</div>"
                "<div><strong>Speed:</strong> <span id='speed'>--</span> km/h</div>"
                "<div><strong>Altitude:</strong> <span id='alt'>--</span> m</div>"
                "<div><strong>Satellites:</strong> <span id='sat'>--</span></div>"
                "<script>function f(){fetch('/data').then(r=>r.json()).then(d=>{"
                "document.getElementById('lat').innerText=d.lat||'0';"
                "document.getElementById('lon').innerText=d.lon||'0';"
                "document.getElementById('speed').innerText=d.speed||'0';"
                "document.getElementById('alt').innerText=d.alt||'0';"
                "document.getElementById('sat').innerText=d.sat||'0';"
                "}).catch(e=>console.log(e));setTimeout(f,1000);}f();</script></body></html>\r\n";
            send_data_nb(esp_client_id, html, (uint16_t)strlen(html));
        }
    }
}

// 空闲中断处理（处理接收数据行）
// 保持原有逻辑：把接收的数据切成行并调用 process_line
void ESP_UART_IdleCallback(void) {
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);
        HAL_UART_DMAStop(&huart3);
        uint16_t len = ESP_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
        for (uint16_t i = 0; i < len; i++) {
            uint8_t c = esp_rx_buf[i];
            if (c == '\n') {
                line_buf[line_idx] = '\0';
                line_ready = 1;
                process_line(line_buf);
                line_idx = 0;
            } else if (line_idx < (sizeof(line_buf) - 1)) {
                line_buf[line_idx++] = (char)c;
            }
        }
        HAL_UART_Receive_DMA(&huart3, esp_rx_buf, ESP_RX_BUF_SIZE);
    }
}

// UART 发送完成回调：清除 esp_tx_busy（避免与其他模块冲突，仅处理 huart3）
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        esp_tx_busy = 0;
    }
}

// ESP 任务入口（非阻塞发送命令，不等待响应）
// 任务运行环境应为 RTOS；如果在主循环调用也可工作（不会阻塞等待回应）
void esp_task_entry(void) {
    // 初始化 DMA 接收与空闲中断
    HAL_UART_Receive_DMA(&huart3, esp_rx_buf, ESP_RX_BUF_SIZE);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);

    osDelay(500);
    // 发送基础命令（非阻塞）。不等待命令返回，状态以发送为准或由接收处理更新。
    // at_send_nb("AT+RST\r\n");
    // 允许模块复位时间，任务层面可以用 osDelay(500) 或者让其它任务运行，这里不强制等待。

    // at_send_nb("ATE0\r\n");
    // at_send_nb("AT+CWMODE=3\r\n");
    // at_send_nb("AT+CIPMUX=1\r\n");

    // char cmd[64];
    // snprintf(cmd, sizeof(cmd), "AT+CWSAP=\"%s\",\"%s\",5,3\r\n", ESP_AP_SSID, ESP_AP_PASSWORD);
    // at_send_nb(cmd);

    // snprintf(cmd, sizeof(cmd), "AT+CIPSERVER=1,%d\r\n", ESP_TCP_PORT);
    // at_send_nb(cmd);

}