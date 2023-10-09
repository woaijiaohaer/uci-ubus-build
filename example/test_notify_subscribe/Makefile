
CFLAGS=-I.
LDFLAGS=-lubus -lubox -lblobmsg_json -ljson-c

all:notify_sever subscribe_client
	echo "Build OK."

clean:
	rm notify_sever subscribe_client *.o -f

notify_sever:test_notify_server.o
	$(CC) -o $@ $< $(LDFLAGS)
subscribe_client:test_subscribe_client.o
	$(CC) -o $@ $< $(LDFLAGS)
