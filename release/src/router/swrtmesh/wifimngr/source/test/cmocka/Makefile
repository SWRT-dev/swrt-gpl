CC		= gcc
WIFIMNGR_LIB_DIR	?= $(shell dirname $(PWD))
WIFIMNGR_LIB	= -lwifimngr -L$(WIFIMNGR_LIB_DIR)
CMOCKA_LIB	= -l cmocka
LIBS		= $(WIFIMNGR_LIB) $(CMOCKA_LIB) \
		  -lssl -lcrypto -leasy -lwifi-7 \
		  -luci -lubus -lubox -ljson-c -lblobmsg_json \
		  -lnl-genl-3 -lnl-3 \
		  -ljson-validator -ljson-schema-validator -ljson-editor

CFLAGS		= -g -Wall -I../..
LDFLAGS		= $(LIBS) -Wl,-rpath=$(WIFIMNGR_LIB_DIR) -I$(WIFIMNGR_LIB_DIR)
UNIT_TESTS	= unit_test_wifi
FUNCTIONAL_TESTS = functional_test_wifi
UTILS 		= test_utils.o

VALGRIND	= valgrind --leak-check=full --show-reachable=no \
	--show-leak-kinds=all --errors-for-leak-kinds=all \
	--error-exitcode=1 --track-origins=yes

unit_test_wifi: $(UTILS) unit_test_wifi.o
	$(CC) -o $@ $^ $(LDFLAGS)

functional_test_wifi: $(UTILS) functional_test_wifi.o
	$(CC) -o $@ $^ $(LDFLAGS)

unit-test: $(UNIT_TESTS)
	$(foreach testprog, $(UNIT_TESTS), sudo $(VALGRIND) ./$(testprog);)

functional-test: $(FUNCTIONAL_TESTS)
	$(foreach testprog, $(FUNCTIONAL_TESTS), sudo $(VALGRIND) ./$(testprog);)

.PHONY: clean
clean:
	rm $(UNIT_TESTS) $(FUNCTIONAL_TESTS) *.o -fv
