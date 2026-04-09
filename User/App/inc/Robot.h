#ifndef __ROBOT_H
#define __ROBOT_H

#include "RUI_DBUS.h"
#include "Vision.h"
#include "DJI_Motor.h"

void RobotTask(uint8_t mode, DBUS_Typedef *DBUS, CONTAL_Typedef *CONTAL);


#endif
