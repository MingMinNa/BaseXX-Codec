CC = g++
FLAGS = -Wall -O2 -std=c++20
EXT = cpp

SRC_DIR   = src
BUILD_DIR = build

TARGET = main
SRCS = $(wildcard $(SRC_DIR)/*.$(EXT) $(TARGET).$(EXT))
OBJS = $(patsubst $(SRC_DIR)/%.$(EXT), $(BUILD_DIR)/%.o, $(SRCS))


$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(TARGET) -lm

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(EXT)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TARGET).exe