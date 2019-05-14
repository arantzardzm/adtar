CC=gcc
CFLAGS = -Wall -g

BUILDIR = obj
obj = adtar.o common.o func.o struct.o

all: adtar copy

adtar: $(obj)
	$(CC) $(CFLAGS) -o adtar $(obj)
adtar: common.h func.h struct.h

copy:
	mkdir -p obj
	cp adtar $(BUILDIR)
	cp $(obj) $(BUILDIR)
	rm -f $(obj) adtar

.PHONY: clean
clean:
	rm -f $(obj) adtar
	rm -f a.out
	rm -f *.gch
