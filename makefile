CC = g++
CCFLAGS = -g -Wall

rd_view: libcs631.a rd_direct.o pnm_display.o
	$(CC) -o rd_view $(CCFLAGS) libcs631.a rd_direct.o pnm_display.o -lm -lX11

# Add whatever additional files you want with a rule here, and also
# in the final linking rule above.

rd_direct.o: rd_direct.cc rd_direct.h  
	$(CC) $(CCFLAGS) -c rd_direct.cc 

pnm_display.o: pnm_display.cc pnm_display.h
	$(CC) $(CCFLAGS) -c pnm_display.cc

clean:
	-rm *.o rd_view *.gch


