TARGET := heat-index
src    := util.cpp calc.cpp cgi.cpp main.cpp
obj    := $(src:.cpp=.o)
dep    := $(obj:.o=.d)
CXX    := c++
HEADERS:= $(wildcard *.o)

ifndef CXXFLAGS
	CXXFLAGS = -Wall -Wextra -pedantic -std=c++11
endif
ifndef CFLAGS
	CFLAGS = -Wall -Wextra -pedantic -std=c11
endif

ifneq ($(OS),Windows_NT)
	CXXFLAGS += -DHAVE_SETENV
endif

INCLUDES += -I../include/json/single_include
LDFLAGS  += -lm

.PHONY: clean cleandep

all: $(TARGET)

-include $(dep)   # include all dep files in the makefile

$(TARGET): $(obj)
	${CXX} ${CXXFLAGS} ${INCLUDES}  -o $@ $^ ${LDFLAGS}

%.o: %.cpp
	${CXX} ${CXXFLAGS} ${INCLUDES}  -c $< -o $@

%.d: %.cpp
	${CXX} ${CXXFLAGS} ${INCLUDES} $< -MM -MT $(@:.d=.o) >$@

cleanall: clean cleandep

clean:
	rm -f $(TARGET) $(obj)

cleandep:
	rm -f $(dep)

