CC = gcc
GTKFLAGS = `pkg-config  --libs --cflags gtk+-2.0`
LDFLAGS = -lm
SRCS = gtk_ui.c hashmap.c test_main.c
MAIN = project

all:	$(MAIN)
	@echo Compilation done. Set args in properties directory and run: ./$(MAIN)

$(MAIN): $(SRCS)
	@echo Compiling $(MAIN)...
	$(CC) $(SRCS) $(GTKFLAGS) -o $@ $(LDFLAGS)

clean:
	$(RM) *~ *.o $(MAIN)

expropiative:
	@echo Compiling expropiative
	$(CC) -o project.o test_main.c arctan_func.c expropiative.c schedule.c  -lm
