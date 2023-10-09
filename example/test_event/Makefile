
CFLAGS=-I.
LDFLAGS=-lubus -lubox -lblobmsg_json -ljson-c

all:event_recv_handler event_send
	echo "Build OK."

clean:
	rm event_recv_handler event_send *.o -f

event_recv_handler:test_event_recv_handler.o
	$(CC) -o $@ $< $(LDFLAGS)
event_send:test_event_send.o
	$(CC) -o $@ $< $(LDFLAGS)
