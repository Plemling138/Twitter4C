CC	=	gcc
CFLAGS	=	-Os -Wall -I/usr/include
DEST	=	/usr/bin
LDFLAGS	=	-L/usr/lib
OBJS	=	main.o urlenc.o session.o extract.o
OBJS	+=	base64.o hmac-sha1.o memxor.o sha1.o twilib.o
OBJS	+=	ssl.o aes.o arc4.o asn.o coding.o cyassl_int.o cyassl_io.o des3.o hmac.o integer.o keys.o md4.o md5.o misc.o pwdbased.o rabbit.o random.o rsa.o sha.o sha256.o tls.o
PROGRAM	=	tweet

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(lDFLAGS) -o $(PROGRAM)

.c.o:	$<
	$(CC) -c $(CFLAGS) $<

clean:
		rm -f *.o *~ $(PROGRAM)
