.POSIX:

TARGET = dis-esolang
OBJ = dis.o main.o dis_errno.o

all: $(TARGET)

# Now we have math.h
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm

test: $(TARGET) $(TEST_FILES)

clean:
	rm -f $(TARGET) $(OBJ)
