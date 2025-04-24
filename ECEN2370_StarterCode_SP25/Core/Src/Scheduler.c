/*
 * Scheduler.c
 *
 *  Created on: Jan 28, 2025
 *      Author: shapi
 */

#include "Scheduler.h"

static uint32_t scheduledEvents;

void addSchedulerEvent(uint32_t tb_sched)
{
	scheduledEvents |= (tb_sched);
}

void removeSchedulerEvent(uint32_t tb_sched)
{
	scheduledEvents &= ~(tb_sched);
}

uint32_t getScheduledEvents()
{
	return scheduledEvents;
}
