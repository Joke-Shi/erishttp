/****************************************************************************//*
 ** @CopyRight (C) 石正贤(Shizhengxian)
 **
 ** @Brief : 
 **        : Bind a processor to cpu affinity.
 **
 ******************************************************************************/

#include "core/eris.core.h"


/**
 * @Brief: Bind a processor to cpu affinity.
 *
 * @Param: __cpu, Cpu id number.
 * @Param: __pid, A processor id.
 *
 * @Return: Ok is 0, Other is EERIS_ERROR or some errno.
 **/
eris_int_t eris_cpuset_bind( eris_int_t __cpu, eris_pid_t __pid)
{
    eris_int_t tmp_errno = errno;
    eris_int_t  rc     = 0;

#if (ERIS_LINUX_OS)
    if ( 0 <= __pid) {
        eris_long_t nprocs = eris_get_nprocessor();

        __cpu = ((0 <= __cpu) || (nprocs > __cpu)) ? __cpu : 0;

        cpu_set_t cpuset_v;

        CPU_ZERO( &cpuset_v);
        CPU_SET ( __cpu, &cpuset_v);

        /** Set affinity */
        rc = sched_setaffinity( __pid, sizeof( cpuset_v), &cpuset_v);
        if ( -1 != rc) {
            rc = EERIS_OK;

        } else { rc = errno; }
    } else { rc = EERIS_ERROR; }
#endif

    errno = tmp_errno;

    return rc;
}/// eris_cpuset_bind




