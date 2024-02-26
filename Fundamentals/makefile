PROG = Fundamentals
LIB = Fundamentals.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/shslib.a
DEF = /usr/local/include
PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(Fundamentals.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): $(DEF)/shslib.h

clean:
	rm -f $(LIB)
	rm -f *.raw
	rm -f *.txt
	rm -f *.csv

all:
	make clean
	make

.PRECIOUS: $(LIB)

