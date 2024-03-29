CC = g++
FLAGS = -I. -I../lib -I../tinyxml -I/opt/include -g -DNDEBUG 
LIB = -lstdc++ 
OPTLIB = -L/opt/lib -lboost_system -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex -lboost_date_time -lboost_random 
AGENTLIB = ../lib/hsrv.o ../lib/Conf.o ../lib/FileManager.o ../lib/InterCom.o ../lib/Attribute.o

BIN = ../bin
XML = ../tinyxml/*.o

all:	main.cpp 
	$(CC) $(FLAGS) $(OPTLIB) -o starter main.cpp $(AGENTLIB) $(XML) $(LIB)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

openssl/%.o : openssl/%.c
	$(CC) -c $<
	
install:	all
	cp starter $(BIN)/starter

clean:	
	rm starter
