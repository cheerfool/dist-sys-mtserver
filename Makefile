all: server client
.PHONY : all

serverobj= mtserver.o serverTool.o serverRespond.o addressTool.o
clientobj= client.o clientTool.o
msgobj= dieWithMsg.o
CLIBS= -pthread

$(serverobj) $(clientobj) $(msgobj): %.o: %.c tools.h
	gcc -c $< -std=gnu99

server : $(serverobj) $(msgobj)
	gcc -o mtserver -std=gnu99 $(serverobj) $(msgobj) $(CLIBS)
client : $(clientobj) $(msgobj)
	gcc -o client -std=gnu99 $(clientobj) $(msgobj) $(CLIBS)

clean:
	rm -f *.o
	rm -f mtserver client
