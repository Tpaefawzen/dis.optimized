.POSIX:

CFLAGS = -Wall -Werror -Wpedantic -O3

TARGETDIR = .
TARGET = $(TARGETDIR)/dis-esolang
OBJ = main.o
LIB = libdis-esolang.a
LIBOBJ = dis.o dis_errno.o dis_math.o

LDFLAGS = -L .
LINKFLAGS = -lm -ldis-esolang

all: $(TARGET)

$(TARGET): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LINKFLAGS)

$(LIB): $(LIB)($(LIBOBJ))

clean:
	rm -f $(OBJ) $(LIBOBJ) $(LIB)

# INTERNAL
#
INTERNAL_TESTS = test-rot test-opr
INTERNAL_TESTS_OBJ = $(INTERNAL_TESTS:=.o)

internal-tests: $(INTERNAL_TESTS) 

$(INTERNAL_TESTS): $(INTERNAL_TESTS_OBJ) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.o $(LINKFLAGS)

clean-internal-tests:
	rm -f $(INTERNAL_TESTS) $(INTERNAL_TESTS_OBJ)

# TESTS
#
.SUFFIXES: .dis .dis.test

TEST_FILES = prog_examples/just_one_nop.dis \
prog_examples/syntax-error/59050_bangs.dis \
prog_examples/syntax-error/unclosed_comment.dis \
prog_examples/cgcc-user100411/infinite-nulls.dis \
prog_examples/cgcc-user100411/print-123-without-123.dis \
prog_examples/cgcc-user100411/short_cat.dis \
prog_examples/cgcc-user100411/truth-machine.dis \
prog_examples/59049_rots.dis \
prog_examples/from-OG/cat_expl.dis \
prog_examples/from-OG/666.dis \
prog_examples/from-OG/cat.dis \
prog_examples/59049_bangs.dis \
prog_examples/cat_1to59047_is_nop.dis \
prog_examples/hello_with_noncmds.dis \
prog_examples/cgcc-2503-mark-jayxcela/hello-83.dis \
prog_examples/cgcc-2503-mark-jayxcela/hello-102.dis
TESTS = $(TEST_FILES:=.test)
	     
test: $(TARGET) $(TESTS)
	echo $(TESTS)

prog_examples/just_one_nop.dis.test:
	@echo == Tests infinite loop ==
	$(TARGET) -k 1000000 prog_examples/just_one_nop.dis; case $$? in 0) false;; *) true;; esac
