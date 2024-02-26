PROG = getfundSEC
LIB = getfundSEC.a
MYSQL_DIR = /usr/lib64
MYSQL_FLAGS = -lz -lcrypt -lm -lssl -lcrypto
MLIB = $(MYSQL_DIR)/libmysqlclient.so $(MYSQL_FLAGS)

XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a
DEF = /usr/local/include
PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(getfundSEC.o)	\
	$(LIB)(getargs.o)		\
	$(LIB)(EachFundamental.o)	\
	$(LIB)(EachStock.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc -o $(PRG) $(LIB) $(XLIB) $(MLIB)

$(LIB): $(FILES)

$(FILES): getfundSEC.h $(DEF)/shslib.h $(DEF)/dbylib.h $(DEF)/fileinvest.h $(DEF)/invlib.h

python:
	cp -pv getfundSEC.py /usr/local/bin/getfundSEC.py
	cp -pv FCF_Stats.py /usr/local/bin/FCF_Stats.py

clean:
	rm -f $(LIB)

all:
	make clean
	make
	make python

.PRECIOUS: $(LIB)

