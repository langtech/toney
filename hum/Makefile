CPPFLAGS = -I../src/get_f0
LIBS = -lsndfile -L../src/get_f0 -lgetf0 -lm

synth: synth.c
	gcc -Wall -o synth synth.c $(CPPFLAGS) $(LIBS)
