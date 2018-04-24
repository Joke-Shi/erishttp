# Erishttp
erishttp is a use http protocol lightweight application server framework, and it can manager c/c++ user service.

## Platform
Support using in `Linux`, `Mac OS` and `FreeBSD`

## Build
Build it before, you can install `pcre` developor library and `cmake` version is equal and greater 2.8:
```bash
git clone https://github.com/Joke-Shi/erishttp.git
cd erishttp
make
```

## Test
```bash
cd bin
./erishttpd -h
./erishttpd -p ../
curl -i http://127.0.0.1:9432/
curl -i http://127.0.0.1:9432/hello-api/v1/hello_service
curl -i http://127.0.0.1:9432/test-api/v1/test_service
```

## Configs
pidfile

	Save erishttp.pid path file. default: var/run/erishttp.pid

daemon 

	value is (on | off). default: on

cpuset 

	Processor index +1 map cpu index. eg: 1:0 2:1

worker_n

	Worker slave processors, default: 1

worker_task_n

	A worker slave processor has task threads. default: 8

rlimit_nofile

	Set rlimit open file number. default: 4096

listen

	Erishttpd server listen ip and port. default: 127.0.0.1:9432

admin_listen

	Erishttpd admin manager server listen ip and port. default: 127.0.0.1:9433

backlog 

	Listen backlog queue size. default: 64

max_connections

	Max client request connections. default: 4096

event_type

	Event I/O type, can set (select, poll, epoll, kevent, kqueue). default: select

timeout

	Receive client request or send response to it timeout number, unit second. default: 20

keepalive

	Keep alive connection timeout number, unit second. default: 60

tcp_nodelay

	TCP protocol nodelay on or off. default: off

tcp_nopush

	TCP procotol nopush on or off. default: off

send_buffer_size

	It is socket object send buffer size. default: 8K

recv_buffer_size

	It is socket object receive buffer size. default: 8K

header_cache_size

	Receive http protocol header cache size. default: 4K

body_cache_size

	Receive http protocol entity body cache size. default: 8K

url_max_size

	Limit client request url max size. default: 1K

header_max_size

	Limit client request http protocol header max size. default: 1K

body_max_size

	Limit client request entity body data max size. default: 10M

log_max_size

	Limit log file max size, greater and truncate to zero. default: 64M

log_level

	Log level number, 0 to 9, 0 is DEBUG, 1 is INFO, 2 is NOTICE and so on. default: 2

log_path

	Dump log message file path. default: var/log

doc_root

	Static file document root path. default: var/www/htdoc

### Module configs
name

	Module name string.

language

	Module used c/c++ language. default: c

libfile

	Specify module library file pathname, you can set it in `modules/$MOD/lib$MOD.so.0.0.1` like this.

## Write Module
Create module example:
```bash
cd erishttp/modules
./erishttp.create.mod -h
./erishttp.create.mod -s xxxx -t c
ls
cd xxxx
ls
make
```

Write module code, you can read `erishttp/core/eris.http.h` and `erishttp/core/eris.module.h` apis.
You can get request query string of `GET` method, and get `POST` request body data and so on.

## Thanks!
hahaha...

