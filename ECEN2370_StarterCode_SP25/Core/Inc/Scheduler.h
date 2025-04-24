/*
 * Scheduler.h
 *
 *  Created on: Jan 28, 2025
 *      Author: shapi
 */

//#include "STM32F429i.h"

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>

#define LED_TOGGLE_EVENT   (1 << 0)
#define LED_DELAY_EVENT    (1 << 1)
#define BUTTON_EVENT       (1 << 2)
#define DEVICE_ID_AND_TEMP_EVENT       (1 << 3)
#define DROP_CHIP          (1 << 4)
#define DELAY 2

uint32_t getScheduledEvents();

void addSchedulerEvent(uint32_t tb_sched);

void removeSchedulerEvent(uint32_t tb_rem);

#endif /* SCHEDULER_H_ */
