/**
 * @file	: timefunc.c
 * @brief	: 时间接口
 * @version :
 * @author	: jyin
 * @date	: Jun 19, 2018
 */
#include "os.h"
#include "dbg.h"
#include "timefunc.h"

#ifdef __cplusplus
extern "C" {
#endif
void timefunc_gettime(struct tm* pst_tm)
{
	time_t st_t;
	struct tm st_tm;

	st_t = time(0);
	st_tm = *localtime(&st_t);

	*pst_tm = st_tm;

	return;
}
#ifdef __cplusplus
}
#endif
