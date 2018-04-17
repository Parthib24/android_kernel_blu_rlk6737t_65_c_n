/*
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2012-2014, 2016 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

/**
 * @file mali_osk_wait_queue.c
 * Implemenation of the OS abstraction layer for the kernel device driver
 */

#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include "mali_osk.h"
#include "mali_kernel_common.h"

struct _mali_osk_wait_queue_t_struct {
	wait_queue_head_t wait_queue;
};

_mali_osk_wait_queue_t *_mali_osk_wait_queue_init(void)
{
	_mali_osk_wait_queue_t *ret = NULL;

	ret = kmalloc(sizeof(_mali_osk_wait_queue_t), GFP_KERNEL);

	if (NULL == ret) {
		return ret;
	}

	init_waitqueue_head(&ret->wait_queue);
	MALI_DEBUG_ASSERT(!waitqueue_active(&ret->wait_queue));

	return ret;
}

void _mali_osk_wait_queue_wait_event(_mali_osk_wait_queue_t *queue, mali_bool(*condition)(void *), void *data)
{
	MALI_DEBUG_ASSERT_POINTER(queue);
	MALI_DEBUG_PRINT(6, ("Adding to wait queue %p\n", queue));
	wait_event(queue->wait_queue, condition(data));
}

void _mali_osk_wait_queue_wait_event_timeout(_mali_osk_wait_queue_t *queue, mali_bool(*condition)(void *), void *data, u32 timeout)
{
	MALI_DEBUG_ASSERT_POINTER(queue);
	MALI_DEBUG_PRINT(6, ("Adding to wait queue %p\n", queue));
	wait_event_timeout(queue->wait_queue, condition(data), _mali_osk_time_mstoticks(timeout));
}

void _mali_osk_wait_queue_wake_up(_mali_osk_wait_queue_t *queue)
{
	MALI_DEBUG_ASSERT_POINTER(queue);

	/* if queue is empty, don't attempt to wake up its elements */
	if (!waitqueue_active(&queue->wait_queue)) return;

	MALI_DEBUG_PRINT(6, ("Waking up elements in wait queue %p ....\n", queue));

	wake_up_all(&queue->wait_queue);

	MALI_DEBUG_PRINT(6, ("... elements in wait queue %p woken up\n", queue));
}

void _mali_osk_wait_queue_term(_mali_osk_wait_queue_t *queue)
{
	/* Parameter validation  */
	MALI_DEBUG_ASSERT_POINTER(queue);

	/* Linux requires no explicit termination of wait queues */
	kfree(queue);
}
