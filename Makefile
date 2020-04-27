CONTIKI_PROJECT = basic_ipv6_networking
all: $(CONTIKI_PROJECT)
CONTIKI = ../..
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
include $(CONTIKI)/Makefile.include
