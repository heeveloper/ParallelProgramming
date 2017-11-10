CC = g++
CXXFLAGS = -g -Wall -std=c++11

SRCS := snapshot.cpp
OBJS := $(SRCS:.cpp=.o)
INC = ./include/
LIB = ./lib/ -lpthread

CPPFLAGS += -I$(INC)

TARGET = run
$(TARGET): $(OBJS)
		$(CC) $(CXXFLAGS) $(CPPFLAGS) -o $(TARGET) $(OBJS) -L$(LIB)

clean:
		rm $(TARGET) $(OBJS)

