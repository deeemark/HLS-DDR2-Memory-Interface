CC = gcc
CFLAGS = -DC_SIM -std=c99 -Wall -Wextra -pedantic

TARGET = ddr2_test

SOURCES = \
	main.c \
	tb_ddr2_top.c \
	ddr2_top.c \
	ddr2_controller.c \
	ddr2_write_phy.c \
	ddr2_read_phy.c

HEADERS = \
	tb_ddr2_top.h \
	ddr2_top.h \
	ddr2_controller.h \
	ddr2_write_phy.h \
	ddr2_read_phy.h

.PHONY: all clean

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
