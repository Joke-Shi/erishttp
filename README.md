# Erishttp
erishttp is a use http protocol lightweight application server framework, and it can manager c/c++ user service.

## Build
	Build it before, you can install pcre developor library and cmake version is equal and greater 2.8:
```bash
	git clone https://github.com/Joke-Shi/erishttp.git
	cd erishttp
	make
```

## Test
```bash
	cd bin
	./erishttp -h
	./erishttp -p ../
```

## Configs
	pidfile
		Save erishttp.pid path file. default: var/run/erishttp.pid

	daemon 
		value is (on | off). default: on

	cpuset 
		Processor index +1 map cpu index. eg: 1:0 2:1


