
PROG=wlupld

LIB = wlupld_64.a
XLIB = /usr/local/lib64/invlib.a /usr/local/lib64/weblib.a /usr/local/lib64/dbylib.a /usr/local/lib64/shslib.a  /usr/lib64/libmysqlclient.so /usr/local/lib64/libxlsxwriter.a -lz -lssl -luuid -lcrypto -lm

DEF = /usr/local/include

WEBDIR = /var/www/vhosts/shs_stocks/html
PRG =$(WEBDIR)/$(PROG).cgi

FILES = \
	$(LIB)(wlupld.o)		\
	$(LIB)(DoFile.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc $(LDFLAGS) -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

$(FILES): wlupld.h $(DEF)/shslib.h $(DEF)/invlib.h $(DEF)/weblib.h $(DEF)/dbylib.h $(DEF)/fileinvest.h

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)
