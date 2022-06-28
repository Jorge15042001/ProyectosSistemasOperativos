CC:=gcc
OUTPUT_1 := tiktoker
SRC_DIR_1 := src/tiktoker
OBJ_DIR_1 := build/obj/tiktoker
SRC_FILES_1 := $(wildcard $(SRC_DIR_1)/*.c)
OBJ_FILES_1 := $(patsubst $(SRC_DIR_1)/%.c,$(OBJ_DIR_1)/%.o,$(SRC_FILES_1))

CXXFLAGS_1 := -lpthread

$(OBJ_DIR_1)/%.o: $(SRC_DIR_1)/%.c
	mkdir -p $(OBJ_DIR_1)
	$(CC)   $(CXXFLAGS_1)   -c -o $@ $^  

$(OUTPUT_1): $(OBJ_FILES_1)
	$(CC)  -o $@ $^   $(CXXFLAGS_1) -lm 
clean :
	rm -rf build
	rm $(OUTPUT_1)
