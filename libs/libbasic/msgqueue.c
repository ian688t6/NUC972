/**
 * @file	: msgqueue.c
 * @brief	: 消息队列
 * @version :
 * @author	: jyin
 * @date	: Jun 19, 2018
 */

#include "os.h"
#include "dbg.h"
#include "msgqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t msgqueue_create(int32_t *pi_msgid)
{
	int32_t i_msgid = -1;

	i_msgid = msgget(IPC_PRIVATE , 0666 | IPC_CREAT | IPC_EXCL);
	if (0 > i_msgid)
	{
		loge("msgget create msgid failed!");
		return -1;
	}
	*pi_msgid = i_msgid;

	return 0;
}

int32_t msgqueue_msgsnd(int32_t i_msgid, void *pv_msg, size_t msg_sz, int32_t i_msgflg)
{
	int32_t i_ret = 0;

	i_ret = msgsnd(i_msgid, (void *)pv_msg, msg_sz, i_msgflg);
	if (0 > i_ret)
	{
		loge("msgqueue_msgsnd failed");
		return -1;
	}

	return 0;
}

int32_t msgqueue_msgrcv(int32_t i_msgid, void *pv_msg, size_t msg_sz, long int msgtype, int32_t i_msgflg)
{
	int32_t i_ret = 0;

	i_ret = msgrcv(i_msgid, (void *)pv_msg, msg_sz, msgtype, i_msgflg);
	if (0 > i_ret)
	{
//		loge("msgqueue_msgrcv failed");
		return -1;
	}

	return 0;
}

void msgqueue_destroy(int32_t i_msgid)
{
	int32_t i_ret = 0;

	i_ret = msgctl(i_msgid, IPC_RMID, 0);
	if (0 > i_ret)
	{
		loge("msgqueue_destroy failed");
		return;
	}

	return;
}

#ifdef __cplusplus
}
#endif
