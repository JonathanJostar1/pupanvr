include ../../Makefile.param


TARGET := xvr_demo


SRCS  := $(wildcard *.c) $(COMM_SRC)
ifeq ($(filter $(CHIP), " MC6650 "),)
SRCS := $(filter-out nna_sample.c,$(SRCS))
endif

SRCS += ad/tp/ad_tp2830.c ad/yj/ad_yj.c

ifneq ($(filter $(CHIP), " MC3312 "),)
SRCS += $(wildcard smh_sample/*.c)
endif

OBJS  := $(SRC:%.c=%.o)

# intermediate directory for generated object files
OBJDIR := .o
# intermediate directory for generated dependency files
DEPDIR := .d

# object files, auto generated from source files
OBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRCS)))
# dependency files, auto generated from source files
DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

# compilers (at least gcc and clang) don't create the subdirectories automatically
$(shell mkdir -p $(dir $(OBJS)) >/dev/null)
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)



# C flags
INC_FLAGS := -I$(REL_INC) -I$(MPP_PATH)/test/common -I$(MPP_PATH)/drv_include
CFLAGS := -Wall  $(INC_FLAGS) $(LIBS_CFLAGS) $(MPP_CFLAGS)
# C++ flags
CXXFLAGS := -std=c++11
# C/C++ flags
CPPFLAGS := #-g -Wall -Wextra -pedantic
# linker flags
LDLIBS  := -l:libmpi.a -l:libaudio_codec.a -lpthread -lm -ldl

ifneq ($(filter $(CHIP), " MC6650 "),)
LDLIBS += -l:libnna.a
endif

ifneq ($(filter $(CHIP), " MC3312 "),)
CFLAGS += -I./ -DISP_ENABLE
LDLIBS += -l:libisp.a  -l:libispcore.a  -l:libgc1054.a -l:libdbi.a -l:libdci.a
endif

LDFLAGS := $(LIBS_LD_CFLAGS) -L$(REL_LIB)
# flags required for dependency generation; passed to compilers
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td


# compile C source files
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# compile C++ source files
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@
# link object files to binary
LINK.share  = $(CC)  -shared $(LDFLAGS) $^ -o $@ $(LDLIBS)
LINK.static = $(CC)  -static $(LDFLAGS) $^ -o $@ $(LDLIBS)

# archieve object files to static library
AR.o = $(AR) rcs $@
# strip the object
STRIP.o = $(STRIP) $@
# precompile step
PRECOMPILE =
# postcompile step
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all: $(TARGET)

.PHONY: clean
clean:
	$(RM) -r $(TARGET) $(OBJDIR) $(DEPDIR)



$(TARGET): $(OBJS)
	$(LINK.static)
	mv $(TARGET) $(REL_TEST)


$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.c) $<
	$(POSTCOMPILE)


.PRECIOUS = $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

-include $(DEPS)

