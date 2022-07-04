CC:=gcc

###########################################################################
# Oouput 1
###########################################################################
OUTPUT_1 := tiktoker
SRC_DIR_1 := src/tiktoker
OBJ_DIR_1 := build/obj/tiktoker
SRC_FILES_1 := $(wildcard $(SRC_DIR_1)/*.c)
OBJ_FILES_1 := $(patsubst $(SRC_DIR_1)/%.c,$(OBJ_DIR_1)/%.o,$(SRC_FILES_1))

CXXFLAGS_1 := 

$(OBJ_DIR_1)/%.o: $(SRC_DIR_1)/%.c
	mkdir -p $(OBJ_DIR_1)
	$(CC)   $(CXXFLAGS_1)   -c -o $@ $^  

$(OUTPUT_1): $(OBJ_FILES_1)
	$(CC)  -o $@ $^   $(CXXFLAGS_1) -lm 
###########################################################################
# Oouput 2
###########################################################################
OUTPUT_2 := reciver
SRC_DIR_2 := src/reciver
OBJ_DIR_2 := build/obj/reciver
SRC_FILES_2 := $(wildcard $(SRC_DIR_2)/*.c)
OBJ_FILES_2 := $(patsubst $(SRC_DIR_2)/%.c,$(OBJ_DIR_2)/%.o,$(SRC_FILES_2))

CXXFLAGS_2 := -lpthread -lm -g

$(OBJ_DIR_2)/%.o: $(SRC_DIR_2)/%.c
	mkdir -p $(OBJ_DIR_2)
	$(CC)   $(CXXFLAGS_2)   -c -o $@ $^  

$(OUTPUT_2): $(OBJ_FILES_2)
	$(CC)  -o $@ $^   $(CXXFLAGS_2)  

###########################################################################
# Oouput 3
###########################################################################
OUTPUT_3 := monitor
SRC_DIR_3 := src/monitor
OBJ_DIR_3 := build/obj/monitor
SRC_FILES_3 := $(wildcard $(SRC_DIR_3)/*.c)
OBJ_FILES_3 := $(patsubst $(SRC_DIR_3)/%.c,$(OBJ_DIR_3)/%.o,$(SRC_FILES_3))

CXXFLAGS_3 := -g

$(OBJ_DIR_3)/%.o: $(SRC_DIR_3)/%.c
	mkdir -p $(OBJ_DIR_3)
	$(CC)   $(CXXFLAGS_3)   -c -o $@ $^  

$(OUTPUT_3): $(OBJ_FILES_3)
	$(CC)  -o $@ $^   $(CXXFLAGS_3)  

###########################################################################
# all 
###########################################################################
all: $(OUTPUT_1) $(OUTPUT_2) $(OUTPUT_3)

clean :
	rm -rf build
	rm $(OUTPUT_1)
	rm $(OUTPUT_2)
	rm $(OUTPUT_3)
