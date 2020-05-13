CC=$(AFL_HOME)/afl-gcc

SRC:=src
OBJ:=obj

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

# all: $(OBJECTS)
# 	$(CC) -o $(OBJECTS) 

all:test_binary_heap test_trie

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -I$(SRC) -c $< -o $@

# test_hash_table : test-hash-table.o $(OBJECTS)
# 	$(CC) $(CFLAGS) -o $@ $^ 

# test-hash-table.o: $(OBJECTS)
# 	$(CC) -c test-hash-table.c

test_binary_heap : test-binary-heap.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ 

test-binary-heap.o: $(OBJECTS)
	$(CC) -c test-binary-heap.c

test_trie : test-trie.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ 

test-trie.o: $(OBJECTS)
	$(CC) -c test-trie.c

# test_dir:
# 	$(MAKE) -C test

clean:
	rm -f $(OBJ)/*.o *.o 
	rm -f test_trie
	rm -f test_binary_heap
	rm -f test_hash_table
	# $(MAKE) -C test_dir clean

