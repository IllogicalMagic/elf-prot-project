CXX?=g++
CC=$(CXX)
INCDIR?=
CXXFLAGS?=-std=c++17 -Wall -O0 -g -I$(INCDIR)
LDLIBS?=-lcryptopp -lpthread

Loader: Loader.o ExecutableHolder.o DebugChecker.o

encryptor: encryptor.o

decryptor: decryptor.o

clean:
	rm -rf *~ *.o Loader encryptor decryptor
