#
# Makefile
#

CIRCLEHOME = ~/c1

OBJS	= vectors.o rpmv0.o 

LIBS	= $(CIRCLEHOME)/lib/libcircle$(RASPPI).a

include Rules.mk
