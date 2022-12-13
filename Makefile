CFLAGS = -Wall -g
CC = mpic++
INCLUDE = includes

ALL_OBJS = main.o primality_test.o worker.o utils.o

primetime: $(ALL_OBJS)
	$(CC) $(CFLAGS) -I $(INCLUDE) -o primetime $(ALL_OBJS)

debug: CFLAGS += -DDEBUG_BUILD
debug: primetime

utils.o: utils.cpp $(INCLUDE)/utils.h
	$(CC) $(CFLAGS) -I $(INCLUDE) -c utils.cpp

primality_test.o: primality_test.cpp $(INCLUDE)/primality_test.h $(INCLUDE)/utils.h
	$(CC) $(CFLAGS) -I $(INCLUDE) -c primality_test.cpp

worker.o: worker.cpp $(INCLUDE)/worker.h $(INCLUDE)/primality_test.h $(INCLUDE)/utils.h
	$(CC) $(CFLAGS) -I $(INCLUDE) -c worker.cpp

main.o: main.cpp $(INCLUDE)/main.h $(INCLUDE)/worker.h $(INCLUDE)/primality_test.h $(INCLUDE)/utils.h
	$(CC) $(CFLAGS) -I $(INCLUDE) -c main.cpp

clean:
	rm *.o primetime