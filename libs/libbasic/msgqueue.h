/**
 * @file	: msgqueue.h
 * @brief	: 消息队列
 * @version :
 * @author	: jyin
 * @date	: Jun 19, 2018
 */

#ifndef __MSGQUEUE_H__
#define __MSGQUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    long int msg_type;
    uint8_t *pv_data;
} msgqueue_data_t;

extern int32_t msgqueue_create(int32_t *pi_msgid);

extern void msgqueue_destroy(int32_t i_msgid);

extern int32_t msgqueue_msgsnd(int32_t i_msgid, void *pv_msg, size_t msg_sz, int32_t i_msgflg);

extern int32_t msgqueue_msgrcv(int32_t i_msgid, void *pv_msg, size_t msg_sz, long int msgtype, int32_t i_msgflg);

#ifdef __cplusplus
}
#endif
#endif /* MSGQUEUE_H_ */
