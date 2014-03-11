BIN_NAME:=test
BIN_ARGS:=

LIBS:=rt
DEBUG:=-g

CFLAGS:=
LFLAGS:=

### shouldn't need to change anything below

CFLAGS+=-I../include
CFLAGS+=-Wall
CFLAGS+=$(DEBUG)
LFLAGS+=-L../lib
LFLAGS+=$(DEBUG)
LFLAGS+=$(addprefix -l,$(LIBS))

BIN_DIR:=bin
OBJ_DIR:=obj
DIRS:=$(BIN_DIR) $(OBJ_DIR)

SRCS:=$(wildcard *.c)
OBJS:=$(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

.PRECIOUS: $(OBJ_DIR)/%.d


all: $(BIN_DIR)/$(BIN_NAME)

new: clean
	$(MAKE) all

clean:
	rm -rf $(addsuffix /*,$(DIRS))

distclean:
	rm -rf $(DIRS) $(addprefix .,$(addsuffix .dir,$(DIRS)))

run: $(BIN_DIR)/$(BIN_NAME)
	LD_LIBRARY_PATH=../lib $^ $(BIN_ARGS)

lib:
	$(MAKE) -C .. all


.%.dir:
	mkdir $*
	touch $@

$(BIN_DIR)/$(BIN_NAME): $(OBJS) .$(BIN_DIR).dir
	gcc -o $@ $(filter %.o,$^) $(addprefix -l,$(patsubst ../lib/lib%.so,%,$(wildcard ../lib/*.so))) $(LFLAGS)

$(OBJS): $(OBJ_DIR)/%.o: .$(OBJ_DIR).dir
	gcc -MMD $*.c -c -o $@ $(CFLAGS)
	
-include $(wildcard $(OBJ_DIR)/*.d)
