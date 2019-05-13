CC=gcc
CFLAGS = -Wall -g

obj = adtar.o common.o func.o struct.o

adtar: $(obj)
	$(CC) $(CFLAGS) -o adtar $(obj)
adtar: common.h func.h struct.h

.PHONY: clean
clean:
	rm -f $(obj) adtar
	rm -f a.out
	rm -f *.gch
