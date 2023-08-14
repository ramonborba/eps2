/*
 * config.h
 * 
 * Copyright The EPS 2.0 Contributors.
 * 
 * This file is part of EPS 2.0.
 * 
 * EPS 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * EPS 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EPS 2.0. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/**
 * \brief Configuration parameters definition.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * \author Augusto Cezar Boldori Vassoler <augustovassoler@gmail.com>
 * \author João Cláudio Elsen Barcellos <joaoclaudiobarcellos@gmail.com>
 * \author Ramon de Araujo Borba <ramonborba97@gmail.com>
 * 
 * \version 0.4.0
 * 
 * \date 2021/01/25
 * 
 * \defgroup config Configuration
 * \{
 */

#include "system/sys_log/sys_log.h"

#ifndef CONFIG_H_
#define CONFIG_H_

/* System */
#define CONFIG_GLOBAL_LOG_LEVEL                         SYS_LOG_INFO

/* Tasks */
#define CONFIG_TASK_STARTUP_ENABLED                     1
#define CONFIG_TASK_WATCHDOG_RESET_ENABLED              1
#define CONFIG_TASK_HEARTBEAT_ENABLED					1
#define CONFIG_TASK_SYSTEM_RESET_ENABLED				1
#define CONFIG_TASK_READ_SENSORS_ENABLED                1
#define CONFIG_TASK_PARAM_SERVER_ENABLED                1
#define CONFIG_TASK_MPPT_ALGORITHM_ENABLED              1
#define CONFIG_TASK_HEATER_CONTROLLER_ENABLED           1
#define CONFIG_TASK_TIME_CONTROL_ENABLED                1
#define CONFIG_TASK_DEVICE_RESPONSE_ENABLED             1

/* Tasks Debug Logs*/
#define CONFIG_TASK_READ_SENSORS_DEBUG_ENABLED          0

/* Devices */
#define CONFIG_SET_DUMMY_EPS                            0
#define CONFIG_DEV_LEDS_ENABLED                         1
#define CONFIG_DEV_HEATER_ENABLED                       1
#define CONFIG_DEV_MPPT_ENABLED                         1
#define CONFIG_DEV_BAT_MANAGER_ENABLED                  0
#define CONFIG_DEV_BATTERY_MONITOR_ENABLED              1
#define CONFIG_DEV_MEDIA_ENABLED                        1
#define CONFIG_DEV_CURRENT_SENSOR_ENABLED               1
#define CONFIG_DEV_VOLTAGE_SENSOR_ENABLED               1
#define CONFIG_DEV_TEMP_SENSOR_ENABLED                  1
#define CONFIG_DEV_OBDH_ENABLED                         1
#define CONFIG_DEV_TTC_ENABLED                          1

/* Drivers */
#define CONFIG_DRIVERS_DEBUG_ENABLED                    0
#define CONFIG_DRIVERS_DS277X_ONEWIRE_VERSION           0

#define MAX_BATTERY_CHARGE                              2450    /* [mAh] */
#define BAT_MONITOR_CHARGE_VALUE                        (uint16_t)(MAX_BATTERY_CHARGE/0.625)    /* 0.625 is a conversion factor for the  battery monitor */
#define RESET_BATTERY_TO_INITIAL_STATE                  0

#endif /* CONFIG_H_ */

/** \} End of config group */
