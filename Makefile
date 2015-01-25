all: server client
.PHONY : all

serverobj= mtserver.o TCPServerUtility.o AddressUtility.o
clientobj= client.o TCPClientUtility.o
msgobj= DieWithMessage.o
CLIBS= -pthread

$(serverobj) $(clientobj) $(msgobj): %.o: %.c Practical.h
	gcc -c $< -std=gnu99

server : $(serverobj) $(msgobj)
	gcc -o mtserver -std=gnu99 $(serverobj) $(msgobj) $(CLIBS)
client : $(clientobj) $(msgobj)
	gcc -o client -std=gnu99 $(clientobj) $(msgobj) $(CLIBS)

clean:
	rm -f *.o
	rm -f mtserver client
