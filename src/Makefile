SRC_DIR := .
BUILD_DIR := ../build/src
INCLUDE_DIR := ./include

SRCS := $(wildcard $(SRC_DIR)/*.c)
DEPS := $(wildcard $(INCLUDE_DIR)/*.h)
_OBJ := $(shell basename -a $(SRCS:.c=.o))
BIN := mqtt_subscriber
OBJ := $(patsubst %, $(BUILD_DIR)/%, $(_OBJ))
LDFLAGS:= -lmosquitto -lsqlite3 -luci -largp -ljson-c -lcurl
CFLAGS := -Wall -g -I$(INCLUDE_DIR)

.PHONY: all
all: $(SRC) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)/*.o