PROG = UpdateFundamental
LIB = UpdateFundamental.a
MYSQL_DIR = /usr/lib64
MYSQL_FLAGS = -lz -lcrypt -lm -lssl -lcrypto
MLIB = $(MYSQL_DIR)/libmysqlclient.so $(MYSQL_FLAGS)

# CFLAGS = -g -fsanitize=address -Wall -Wformat-truncation=0 -I../../include -I/usr/local/include

XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a
DEF = /usr/local/include
PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(UpdateFundamental.o)	\
	$(LIB)(EachStock.o)	\
	$(LIB)(getargs.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	# gcc  -g -fsanitize=address -o $(PRG) $(LIB) $(XLIB) $(MLIB)
	gcc  -g -o $(PRG) $(LIB) $(XLIB) $(MLIB)

$(LIB): $(FILES)

$(FILES): UpdateFundamental.h $(DEF)/shslib.h $(DEF)/dbylib.h $(DEF)/fileinvest.h $(DEF)/invlib.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)

