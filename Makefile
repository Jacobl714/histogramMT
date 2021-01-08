CC = cc
CFLAGS = -g
all: histoSeq histo

histoSeq : histoSeq.o
	$(CC) $(CFLAGS) -o $@ $< -lpthread -lm

histo : histo.o
	$(CC) $(CFLAGS) -o $@ $< -lpthread -lm

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o histoSeq histo *~
