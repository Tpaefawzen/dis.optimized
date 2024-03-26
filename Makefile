.POSIX:

TARGET = dis-esolang
OBJ = dis.o main.o dis_errno.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

test: $(TARGET) $(TEST_FILES)

clean:
	rm -f $(TARGET) $(OBJ)
