CXX=gcc
CXXFLAGS=-g -Wall
.PHONY: all clean
target=libtest.so.0.0.1
sources=test.c
objects=test.o


inc_opts=-I../../include

lib_opts=-L../../lib

libs=-lerishttp

all:$(target)
$(target): $(objects)
	$(CXX) $(CXXFLAGS) -shared $^ -o $@ -L../../lib $(lib_opts) $(libs)


$(objects):%.o:%.c
	$(CXX) $(CXXFLAGS) -fPIC $(inc_opts) -c $^ -o $@


clean:
	@rm $(target) $(objects)


