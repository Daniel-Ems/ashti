CLAGS += -std=c11
CFLAFS += -Wall -Wextra -Wpedantic -Wwrite-strings -Winline
CFLAGS += -Wstack-usage=1024 -Wfloat-equal -Waggregate-return 
LDLIBS += -lssl -lcrypto
ASHTI_BIN=ashti
ASHTI_DEPS=ashti.o 

all:$(ASHTI_BIN)

$(ASHTI_BIN):$(ASHTI_DEPS)

.PHONY: clean debug profile

debug: CFLAGS+=-g
debug: all

profile: CFLAGS+=-pg
profile: all

clean:
	$(RM) $(ASHTI_BIN) $(ASHTI_DEPS)

