
PROG=getdiv

ifeq ($(LDFLAGS),"-m32")
LIB = getdiv.a
XLIB = /usr/local/lib/invlib.a /usr/local/lib/dbylib.a /usr/local/lib/shslib.a  -luuid -lcrypto -lm
else
LIB = getdiv_64.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a  /usr/lib64/libmysqlclient.so  -lz -lssl -luuid -lcrypto -lm
endif

DEF = /usr/local/include

PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(getdiv.o)		\
	$(LIB)(getargs.o)		\
	$(LIB)(cmpfuncs.o)		\
	$(LIB)(DividendCuts.o)	\
	$(LIB)(EachDividend.o)	\
	$(LIB)(EachMember.o)	\
	$(LIB)(EachStock.o)		\
	$(LIB)(EachPortfolio.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): getdiv.h $(DEF)/shslib.h  $(DEF)/dbylib.h $(DEF)/fileinvest.h $(DEF)/invlib.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)
