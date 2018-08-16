CXX = c++
ifndef CXXFLAGS
	CXXFLAGS = -Wall -Wextra -pedantic -std=c++11
endif
ifndef CFLAGS
	CFLAGS = -Wall -Wextra -pedantic -std=c11
endif

ifneq ($(OS),Windows_NT)
	CXXFLAGS += -DHAVE_SETENV
endif

INCLUDES+=-I../../include/json/single_include
INCLUDES+=-I../../include/doctest
INCLUDES+=-I/usr/local/include

PROGS = test-calc

all: $(PROGS)

test-calc : clean
	${CXX} ${CXXFLAGS} ${INCLUDES}  -o $@ ../util.cpp ../calc.cpp test-calc.cpp

clean:
	rm -f $(PROGS)

