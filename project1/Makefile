CC=g++
CFLAGS=-g -O3 --std=c++11
LIBS = -lpthread
run: signal.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f *.o run
