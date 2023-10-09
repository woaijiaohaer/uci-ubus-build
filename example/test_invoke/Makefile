
CFLAGS=-I.
LDFLAGS=-lubus -lubox -lblobmsg_json -ljson-c

all:invoke_server invoke_client
	echo "Build OK."

clean:
	rm invoke_sever invoke_client *.o -f

invoke_server:test_invoke_server.o
	$(CC) -o $@ $< $(LDFLAGS)
invoke_client:test_invoke_client.o
	$(CC) -o $@ $< $(LDFLAGS)
