REPORT_DIR=report

SRC_DIR=src
OBJ_DIR=obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cc)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cc,$(OBJ_DIR)/%.o,$(SRC_FILES))

CXXFLAGS := -O3 -Wall -std=c++17 -Iinclude/ -g 
CC := g++

TARGET=lichens

all: obj_dir $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

obj_dir:
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

pdf:
	mkdir -p $(REPORT_DIR)/out
	pdflatex --output-directory $(REPORT_DIR)/out $(REPORT_DIR)/report.tex

clean:
	rm -rf $(REPORT_DIR)/out $(OBJ_DIR) $(TARGET)
