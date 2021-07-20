CC=g++
CPPFLAGS=-Wall
LDLIBS=-ljsoncpp -lpthread -lssl -lcrypto -lcurl

example2.out: BAPI.o example2.o
	$(CC) $(CPPFLAGS) BAPI.o example2.o -o example2.out $(LDLIBS)

BAPI.o: BAPI.cpp
	$(CC) -c $(CPPFLAGS) BAPI.cpp -o BAPI.o $(LDLIBS)

example2.o: example2.cpp
	$(CC) -c $(CPPFLAGS) example2.cpp -o example2.o $(LDLIBS)

clean:
	rm *.o *.out