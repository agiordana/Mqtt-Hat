CC = g++
FLAGS = -O2 -I. -I../lib -I../tinyxml -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd -lmosquittopp -lmosquitto
AGENTLIB = ../lib/*.o
XML = ../tinyxml/*.o
BIN = ../bin
OBJ = agent.o Rule.o RuleHub.o Precondition.o AgentTaskLocal.o

controller:	$(OBJ) main.cpp agent.h AgentTaskLocal.h
	$(CC) $(FLAGS) $(OPTLIB) -o controller main.cpp $(OBJ) $(AGENTLIB) $(LIB) $(XML)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

install:	controller
	cp controller $(BIN)/controller

clean:
	rm -f *.o controller

