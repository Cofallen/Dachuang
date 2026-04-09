## STM32F103C8T6 简单框架

> 由Horizon战队框架简化而来。
> 

### 注意事项

- freertos报错：替换路径问题(生成代码为arm编译器，实际使用应为gcc)：

`D:\RoboMaster\2026\lidar\Horizon_frame_f1\Middlewares\Third_Party\FreeRTOS\Source\portable\RVDS\ARM_CM3` 中的两个文件替换为 `C:\Users\17273\STM32Cube\Repository\STM32Cube_FW_F1_V1.8.6\Middlewares\Third_Party\FreeRTOS\Source\portable\GCC\ARM_CM3`中的文件即可。

- eide 工具链编译失败，内存不够（keil可正常烧录）

修改申请空间大小：
1. IRAM1  0x20000000 0x20000
2. IROM1 0x08000000 0x80000

- 视觉通信协议
> 使用串口助手测试正常 云台方向左正右负 上正下负
>
> 注意单位 pos 360度 omega 度/s
1. 接收
```c
uint8_t test_packet[22] = {
    // 0. 帧头 (必须是 0xCD)
    0xCD, 
    // 1-4. Pitch 角度 (float: 10.5) -> Hex: 0x41280000 (小端: 00 00 28 41)
    0x00, 0x00, 0x28, 0x41,

    // 5-8. Yaw 角度 (float: -5.0) -> Hex: 0xC0A00000 (小端: 00 00 A0 C0)
    0x00, 0x00, 0xA0, 0xC0,

    // 9-12 Pitch 前馈 (float: 0.8) -> Hex: 0x3F4CCCCD
    0xCD, 0xCC, 0x4C, 0x3F,

    // 13-16 Yaw  前馈 (float: -0.3) -> Hex: 0xBE99999A
    0x9A, 0x99, 0x99, 0xBE,

    // 17-20 time ms
    0x00, 0x00, 0x00, 0x00, 

    // 21. 帧尾 (必须是 0xDC)
    0xDC
};
```
2. 发送
```c
uint8_t test_packet[22]={
	0xCD, 
	0x00, 0x0C, 0x28, 0x41,  // pitch float 
	0x00, 0x20, 0xA3, 0xC0,  // yaw float
	0x00, 0x00, 0x00, 0x00,  // pitch omega
    0x00, 0x00, 0x00, 0x00,  // yaw   omega
	0x00, 0x00, 0x00, 0x00,  // 时间ms
	0xDC
}
```
---
## TODO List

- [X] 更新通信协议
- [x] 添加角速度收发
- [ ] 电控对接收角度和角速度前馈后具体处理方法详述
- [ ] 添加陀螺仪对云台进行更加精细控制


---
> [!TIP]
> 角速度收发与处理

视觉：接收 **实时角速度**，发送  **角速度前馈**

电控：接收 **角速度前馈**，发送  **实时角速度**


电控接收实时角速度后处理（单位环）： 角速度前馈先乘dt积分成角度，再加到PID的ref上。

**前馈角速度 × dt → 角度增量 → 加到角度指令 → 原 PID 不动**

	


