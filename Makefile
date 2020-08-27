CC=$(shell which gcc)
CXX=$(shell which g++)

# 设定目标程序和obj文件的保存路径，可以设置为空，即保存在当前目录下
OBJDIR=.
BINDIR=.

# source files:目前只扫描.c文件
SRC=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRC))
PROGRAMS=$(patsubst %.c,%,$(SRC))

# 拼接obj和目标路径
FULLOBJS=$(addprefix ${OBJDIR},${OBJS})
TARGETS=$(addprefix ${BINDIR},${PROGRAMS})

# whether has something to do
ifeq (,${TARGETS})
	$(error No ".c" file found!)
endif

#### $(call rmdir-quiet,<directory>)
## If the directory is not empty, it will failure.
define rmdir-quiet
	if [ -d $1 ]; then rmdir $1; fi
endef

# 将每个文件名作为目标，并全部定义为伪目标，方便单独编译某个模块
.PHONY : ${PROGRAMS}

# default to produce all targets
all: ${TARGETS}

${PROGRAMS}:%:${BINDIR}%

${TARGETS}: ${BINDIR}%:${OBJDIR}%.o
	$(if ${BINDIR},$(shell mkdir -p ${BINDIR}))
	$(CC) $< -o $@

${OBJDIR}%.o : %.c
	$(if $(OBJDIR),$(shell mkdir -p $(OBJDIR)))
	$(CC) -c $< -o $@

clean:
	$(foreach tag, ${TARGETS}, rm -f ${tag})
	$(foreach tag, ${FULLOBJS}, rm -f ${tag})
	$(if ${OBJDIR}, $(call rmdir-quiet,$(OBJDIR)))
	$(if ${BINDIR}, $(call rmdir-quiet,$(BINDIR)))