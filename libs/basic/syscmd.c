/**
 * @file	: syscmd.c
 * @brief	: 系统命令
 * @version :
 * @author	: jyin
 * @date	: Jun 19, 2018
 */
#include "os.h"
#include "dbg.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t syscmd(const char *pc_cmd)
{
	int32_t i_pid 		= 0;
	int32_t i_ret 		= 0;
	int32_t i_status 	= 0;

	i_pid = fork();
	if (0 > i_pid)
	{
		loge("syscmd fork failed!");
		return -1;
	}
	else if (0 == i_pid)
	{
		 execl("/bin/sh", "sh", "-c", pc_cmd, (char *) 0);
	     exit(-1);
	}
	else
	{
		for (;;)
		{
			i_ret = waitpid(i_pid, &i_status, 0);
			if (i_ret > 0)
			{
                if (WIFEXITED(i_status)) {
                    //子进程正常结束: exit(任意值无论正负), execv()执行结束没有崩溃
                    //1. execv()执行成功结束: status = 0
                    //2. execv()执行结束，但可能没有执行成功，内部调用了exit(-1): status = 0xFF00
                    //3. 子进程直接调用 exit(-1): status = 0xFF00
                    //4. 子进程直接调用 exit(-10): status = 0xF600
                    if (WEXITSTATUS(i_status) == 0xFF) {
                        //TODO
                    	logw("syscmd exit status 0xff!");
                    } else if (WEXITSTATUS(i_status) == 0xF6) {
                        //TODO
                    	logw("syscmd exit status 0xf6!");
                    } else {
                        //TODO
                    }
                }
                break;
			}
			else
			{
	             if (errno != EINTR) {
	            	 i_status = -1;    /* error other than EINTR from waitpid() */
	                break;
	            }
			}
		}
	}
    return 0;
}

#ifdef __cplusplus
}
#endif
