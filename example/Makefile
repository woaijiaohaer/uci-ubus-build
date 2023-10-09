
dir_y =
dir_y += test_invoke
dir_y += test_notify_subscribe
dir_y += test_event

all clean:
	@for i in $(dir_y) ; \
		do \
		echo "**********************Build $$i $@**********************" ; \
		make -C $$i $@ ; \
	done

