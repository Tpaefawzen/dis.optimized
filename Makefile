.POSIX:

CFLAGS = -Wall -Werror -Wpedantic -O3

TARGET = dis-esolang
OBJ = main.o
LIB = libdis-esolang.a
LIBOBJ = dis.o dis_errno.o dis_math.o

LDFLAGS = -L .
LINKFLAGS = -lm -ldis-esolang

INTERNAL_TESTS = test-rot test-opr
INTERNAL_TESTS_OBJ = $(INTERNAL_TESTS:=.o)

all: $(TARGET)

$(TARGET): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LINKFLAGS)

$(LIB): $(LIB)($(LIBOBJ))

test: $(TARGET) $(TEST_FILES)

clean:
	rm -f $(OBJ) $(LIBOBJ) $(LIB)

internal-tests: $(INTERNAL_TESTS) 

$(INTERNAL_TESTS): $(INTERNAL_TESTS_OBJ) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.o $(LINKFLAGS)

clean-internal-tests:
	rm -f $(INTERNAL_TESTS) $(INTERNAL_TESTS_OBJ)
