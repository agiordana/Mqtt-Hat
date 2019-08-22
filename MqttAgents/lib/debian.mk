CC = g++
CFLAGS = -g -I. -I../tinyxml -I/opt/include -I/usr/local/include -I/usr/local/include/httpserver -DNDEBUG 
AGENTLIB = hsrv.o NameList.o FileManager.o Attribute.o RestHttpServer.o Conf.o InterCom.o MqttClient.o MqttClientThread.o Logger.o AgentKernel.o AgentTask.o DynamicPage.o Trigger.o

XML = ../tinyxml

all:	$(AGENTLIB)

%.o:	%.cpp %.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o
