.POSIX:

TARGET = dis

all: $(TARGET)

test: $(TARGET) $(TEST_FILES)

clean:
	rm -f $(TARGET)
