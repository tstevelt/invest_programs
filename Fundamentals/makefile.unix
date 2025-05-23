
PROG=fundamentals

ifeq ($(LDFLAGS),"-m32")
LIB = fundamentals.a
XLIB = /usr/local/lib/invlib.a /usr/local/lib/dbylib.a /usr/local/lib/weblib.a /usr/local/lib/shslib.a  -luuid -lcrypto -lm
else
LIB = fundamentals_64.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/dbylib.a /usr/local/lib64/weblib.a /usr/local/lib64/shslib.a  /usr/lib64/libmysqlclient.so /usr/local/lib64/libxlsxwriter.a  -lz -lssl -luuid -lcrypto -lm
endif

DEF = /usr/local/include

WEBDIR = /var/www/vhosts/shs_stocks/html
PRG =$(WEBDIR)/$(PROG).cgi

FILES = \
	$(LIB)(fundamentals.o)	\
	$(LIB)(GetInput.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): fundamentals.h $(DEF)/shslib.h $(DEF)/weblib.h $(DEF)/invlib.h  $(DEF)/dbylib.h $(DEF)/fileinvest.h

css:
	cp -pv fundamentals.css $(WEBDIR)/fundamentals.css

clean:
	rm -f $(LIB)

all:
	make clean
	make
	make css

.PRECIOUS: $(LIB)
