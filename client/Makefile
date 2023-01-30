CONTIKI_PROJECT = client
all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky z1

CONTIKI=../../../../..

include ../Makefile.common

include $(CONTIKI)/Makefile.include
