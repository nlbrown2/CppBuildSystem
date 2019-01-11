CXX = g++
CC = g++
LD = g++
debug_or_optimize = -g3
CXXFLAGS = -std=c++1z -Wall -Werror -pedantic $(debug_or_optimize)
PROG = cppb
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)

default: $(PROG)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

release: debug_or_optimize = -O3
release: $(PROG)

clean:
	rm -f *.o
	rm -f $(PROG)
