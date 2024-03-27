.POSIX:

TARGET = dis-esolang
LIB = libdis-esolang.a
LIBOBJ = dis.o dis_errno.o dis_fmt.o

LDFLAGS = -L .
LINKFLAGS = -lm -ldis-esolang

INTERNAL_TESTS = test-dis-fmt  # test-rot test-opr
INTERNAL_TESTS_OBJ = $(INTERNAL_TESTS:=.o)

all: $(TARGET)

$(TARGET): main.o $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.o $(LINKFLAGS)

$(LIB): $(LIB)($(LIBOBJ))

test: $(TARGET) $(TEST_FILES)

clean:
	rm -f $(TARGET) $(LIBOBJ) $(LIB)

internal-tests: $(INTERNAL_TESTS) 

$(INTERNAL_TESTS): $(INTERNAL_TESTS_OBJ) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.o $(LINKFLAGS)

clean-internal-tests:
	rm -f $(INTERNAL_TESTS) $(INTERNAL_TESTS_OBJ)
