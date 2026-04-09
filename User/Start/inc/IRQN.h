#ifndef __IRQN_H
#define __IRQN_H

#include "main.h"
#include "can.h"
#include "usart.h"

#include "All_init.h"

void User_IRQHandler(UART_HandleTypeDef *huart);

#endif // !__IRQN_H