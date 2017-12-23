CXX?=g++
CC=$(CXX)
INCDIR?=
CXXFLAGS?=-std=c++17 -Wall -O0 -g -I./
LDFLAGS?=
LDLIBS?=-lcryptopp -lpthread -lstdc++fs

Loader: Loader.o ExecutableHolder.o DebugChecker.o data.o

encryptor: encryptor.o

decryptor: decryptor.o

clean:
	rm -rf *~ *.o Loader encryptor decryptor
