#========make file======= 
CC=mips_gcc
CFLAGS=-L/root/RT288x_SDK/source/lib/lib -I/root/RT288x_SDK/source/lib/libnvram
ODIR=obj

simplehome:simplehome.c wireless.c serial.c socketserver.c openAction.c
	$(CC) -o simplehome $(CFLAGS) simplehome.c wireless.c serial.c socketserver.c openAction.c -lnvram -lpthread

romfs:
	$(ROMFSINST) /bin/simplehome

clean:
	rm -f *.o simplehome
