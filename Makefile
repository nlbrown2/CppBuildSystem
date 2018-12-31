CXX = g++
CC = g++
LD = g++
CXXFLAGS = -std=c++1z -Wall -Werror -pedantic -g3
PROG = cppb
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)

default: $(PROG)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

clean:
	rm -f *.o
	rm -f $(PROG)
