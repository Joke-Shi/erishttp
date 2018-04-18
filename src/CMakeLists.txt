project( erishttpd)

cmake_minimum_required( VERSION 2.8)

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fPIC")

message( STATUS "The source director::: ${PROJECT_SOURCE_DIR}")
include_directories( ${PROJECT_SOURCE_DIR}/../include)

# Create config.h 
include( CheckFunctionExists)
check_function_exists( "gmtime_r"    ERIS_HAVE_GMTIME_R)
check_function_exists( "truncate"    ERIS_HAVE_TRUNCATE)
check_function_exists( "pread"       ERIS_HAVE_PREAD)
check_function_exists( "pwrite"      ERIS_HAVE_PWRITE)
check_function_exists( "getaddrinfo" ERIS_HAVE_GETADDRINFO)

include( CheckIncludeFiles)
check_include_files( "stdint.h"      ERIS_HAVE_STDINT_H)
check_include_files( "stdbool.h"     ERIS_HAVE_STDBOOL_H)
check_include_files( "malloc.h"      ERIS_HAVE_MALLOC_H)
check_include_files( "limits.h"      ERIS_HAVE_LIMITS_H)
check_include_files( "unistd.h"      ERIS_HAVE_UNISTD_H)
check_include_files( "sched.h"       ERIS_HAVE_SCHED_H)
check_include_files( "crypt.h"       ERIS_HAVE_CRYPT_H)
check_include_files( "sys/mount.h"   ERIS_HAVE_SYS_MOUNT_H)
check_include_files( "sys/param.h"   ERIS_HAVE_SYS_PARAM_H)
check_include_files( "sys/statvfs.h" ERIS_HAVE_SYS_STATVFS_H)
check_include_files( "sys/fileio.h"  ERIS_HAVE_SYS_FILEIO_H)
check_include_files( "poll.h"        ERIS_HAVE_POLL_H)
check_include_files( "sys/epoll.h"   ERIS_HAVE_SYS_EPOLL_H)
check_include_files( "sys/event.h"   ERIS_HAVE_SYS_EVENT_H)
check_include_files( "sys/devpoll.h" ERIS_HAVE_SYS_DEVPOLL_H)
check_include_files( "aio.h"         ERIS_HAVE_AIO_H)
check_include_files( "pcre.h"        ERIS_HAVE_PCRE_H)
configure_file( ${PROJECT_SOURCE_DIR}/../eris.config.h.in ${PROJECT_SOURCE_DIR}/../include/eris.config.h)


# All project source files
set ( source_files
	core/eris.buffer.c
	core/eris.cpuset.c
	core/eris.cftree.c
	core/eris.daemon.c
	core/eris.errno.c
	core/eris.http.c
	core/eris.log.c
	core/eris.petree.c
	core/eris.spinlock.c
	core/eris.string.c
	core/eris.proctitle.c
	core/eris.list.c
	core/eris.task.c
	core/eris.urlencode.c
	core/eris.time.c
	core/eris.regex.c
	core/eris.xform.c
	core/eris.module.c
	os/unix/eris.attrfs.c
	os/unix/eris.files.c
	os/unix/eris.socket.c
	event/eris.event.c
	event/eris.poll.c
	event/eris.epoll.c
	event/eris.select.c
	event/eris.kqueue.c
	event/eris.devpoll.c
	event/eris.event_queue.c
)


# Create static liberishttp.a 
add_library( erishttp
	STATIC
	${source_files}
)
set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)

# Create executable 
link_directories (
	${PROJECT_SOURCE_DIR}/../lib
)

# Add link
# target_link_libraries( pthread)

link_libraries( dl
	pcre
	pthread
)

add_executable( erishttpd 
	${source_files}
	eris/eris.util.c
	eris/eris.signal.c
	eris/eris.slave.c
	eris/erishttp.c
)

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

