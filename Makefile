include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk

ps5-webkit-cache-remove.elf: main.c
	$(CC) -Wall -Werror -g -o $@ $^
