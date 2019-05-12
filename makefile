CC=gcc
src = $(wildcard *.c)
obj = $(src:.c=.o)

adtar: $(obj)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f $(obj) adtar
	rm -f *.gch
