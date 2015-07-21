CC=gcc
CFLAGS=-g -Wall
SPECLIBS=
EXECUTABLES=hkmeans

# hkmeans: the h stands for either Hartigan or hardcoded.
# the hardcoding only refers to the label and the number of features, 3.
# this can be corrected later. In any case, hkmenas is only a prototype.
hkmeans: hkmeans.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
