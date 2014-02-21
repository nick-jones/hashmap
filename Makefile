SOURCE := src/hashmap.c src/hashmap_debug.c
OBJECTS := $(patsubst %.c,%.o, $(SOURCE))
TARGET := hashmap.so

TEST_SOURCE := tests/check_hashmap.c
TEST_OBJECTS := $(patsubst %.c,%.o, $(TEST_SOURCE))
TEST_TARGETS := $(patsubst %.c,%, $(TEST_SOURCE))

CC := gcc
CFLAGS := -Wall -fPIC

all: $(TARGET)

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

$(TARGET): $(OBJECTS)
	$(CC) $^ $(CFLAGS) -shared -o $@

$(TEST_TARGETS): $(TEST_OBJECTS) $(TARGET)
	$(CC) $< $(OBJECTS) -lcheck -o $@

test: $(TEST_TARGETS)
	sh tests/run.sh

clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(TARGET) $(TEST_TARGETS)
