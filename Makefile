.POSIX:

TARGET = dis-esolang
OBJ = dis.o main.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $?

test: $(TARGET) $(TEST_FILES)

clean:
	rm -f $(TARGET) $(OBJ)
