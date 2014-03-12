SYS_INC_DIR:=/usr/include
SYS_LIB_DIR:=/usr/lib

LIB_VERSION:=0
LIB_FULLREV:=$(LIB_VERSION).0.1
LIB_NAME:=magicsock

DEF_PREFIX:=MY
#DEFS+=NO_VALID_CHECKS

LIBS:=rt pthread
DEBUG:=-g

CFLAGS:=
LFLAGS:=

### shouldn't need to change anything below

CFLAGS+=-Iinclude
CFLAGS+=-Wall
CFLAGS+=$(addprefix -D$(DEF_PREFIX)_,$(DEFS))
CFLAGS+=$(DEBUG)
CFLAGS+=-fPIC
CFLAGS+=-fvisibility=hidden
CFLAGS+=-Wno-variadic-macros
LFLAGS+=$(DEBUG)
LFLAGS+=-fPIC
LFLAGS+=$(addprefix -l,$(LIBS))

LIB_DIR:=lib
OBJ_DIR:=obj
DIRS:=$(LIB_DIR) $(OBJ_DIR)

SRCS:=$(wildcard *.c)
OBJS:=$(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

.PRECIOUS: $(OBJ_DIR)/%.d

INCLUDE_FILES:=$(patsubst include/%,%,$(wildcard include/*.h))
INSTALL_FILES:=$(addprefix $(SYS_INC_DIR)/,$(INCLUDE_FILES)) $(SYS_LIB_DIR)/lib$(LIB_NAME).so $(SYS_LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV) $(SYS_LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg

all: $(LIB_DIR)/lib$(LIB_NAME).so $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg

new: clean
	$(MAKE) all

clean:
	rm -rf $(addsuffix /*,$(DIRS))
	make -C test clean

distclean:
	rm -rf $(DIRS) $(addprefix .,$(addsuffix .dir,$(DIRS)))

test: all
	make -C test all

run: all
	make -C test run


install: $(INSTALL_FILES)

uninstall:
	rm -rf $(INSTALL_FILES)

$(SYS_INC_DIR)/%: $(wildcard include/*.h)
	install -g root -o root -DT -m 644 $^ $@
$(SYS_LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV): $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV)
	install -g root -o root -DT -m 755 $^ $@
$(SYS_LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg: $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg
	install -g root -o root -DT -m 755 $^ $@
$(SYS_LIB_DIR)/lib$(LIB_NAME).so: $(SYS_LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV)
	ln -fs $(shell basename $^) $@


.%.dir:
	mkdir $*
	touch $@

$(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV).dbg: $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV)
	objcopy --only-keep-debug $^ $@
	objcopy --add-gnu-debuglink=$@ $^
	objcopy --strip-debug $^
	touch $@

$(LIB_DIR)/lib$(LIB_NAME).so: $(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV)
	ln -sf `basename $^` $@

$(LIB_DIR)/lib$(LIB_NAME).so.$(LIB_FULLREV): $(LIB_DIR)/lib$(LIB_NAME).o
	gcc -shared -Wl,--no-undefined -Wl,-soname,lib$(LIB_NAME).so.$(LIB_FULLREV) $(filter %.o,$^) $(LFLAGS) -o $@

$(LIB_DIR)/lib$(LIB_NAME).o: $(OBJS) .$(LIB_DIR).dir
	ld -r -o $@ $(filter %.o,$^)

$(OBJS): $(OBJ_DIR)/%.o: .$(OBJ_DIR).dir
	gcc $(CFLAGS) -MMD $*.c -c -o $@
	
-include $(wildcard $(OBJ_DIR)/*.d)
