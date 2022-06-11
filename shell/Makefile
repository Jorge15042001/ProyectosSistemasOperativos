CC:=gcc
OUTPUT := shell
SRC_DIR := src
OBJ_DIR := build/obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := 
CPPFLAGS := 
CXXFLAGS := 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p build
	mkdir -p build/obj
	$(CC)   $(CXXFLAGS)   -c -o $@ $^  

$(OUTPUT): $(OBJ_FILES)
	$(CC)  -o $@ $^   $(CXXFLAGS) 
clean :
	rm -rf build
	rm $(OUTPUT)
build:
	mkdir build
	mkdir build/obj
