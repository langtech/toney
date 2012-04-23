SRC = get_f0.c get_cands.c sigproc.c dp_f0.c
OBJ = $(SRC:.c=.o)
LIBS = -lm -lsndfile
CPPFLAGS = -I/opt/local/include
LDFLAGS = -L/opt/local/lib

all: libgetf0.a
	gcc -Wall -g -o test_get_f0 test.c libgetf0.a $(LIBS) $(CPPFLAGS) $(LDFLAGS)
	gcc -Wall -g -o get_f0_60_650_10_5 get_f0_60_650_10_5.c libgetf0.a $(LIBS) $(CPPFLAGS) $(LDFLAGS)
	gcc -Wall -g -o get_f0_60_650_10_1 get_f0_60_650_10_1.c libgetf0.a $(LIBS) $(CPPFLAGS) $(LDFLAGS)

libgetf0.a: $(SRC)
	gcc -Wall -g -c $(SRC) $(CPPFLAGS)
	rm -f libgetf0.a
	ar r libgetf0.a $(SRC:.c=.o)
	ranlib libgetf0.a
