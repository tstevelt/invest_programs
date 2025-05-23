PROG = invest_chk
LIB = invest_chk.a
MYSQL_DIR = /usr/lib64
MYSQL_FLAGS = -lz -lcrypt -lm -lssl -lcrypto
MLIB = $(MYSQL_DIR)/libmysqlclient.so $(MYSQL_FLAGS)

XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a
DEF = /usr/local/include
DEF1 = ../../include
DEF2 = /usr/local/include
PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(EachStock.o)	\
	$(LIB)(getargs.o)	\
	$(LIB)(invest_chk.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc -o $(PRG) $(LIB) $(XLIB) $(MLIB)

$(LIB): $(FILES)

$(FILES): invest_chk.h $(DEF)/shslib.h $(DEF)/dbylib.h $(DEF)/fileinvest.h $(DEF)/invlib.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)

