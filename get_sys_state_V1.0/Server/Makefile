CC               = gcc 			
ELF              = ../server	#指定生成的可执行文件及路径
SRC              = $(wildcard ./src/*.c)	#匹配查找所有的.c文件
LIB              = -L ./lib
INC              = -I ./inc 
OBJ              = $(patsubst %.c,%.o,$(SRC)) #把所有的SRC中的.c 替换成.o
CFLAG			 = -lpthread  -lssl -lcrypto -lm


$(ELF):$(OBJ)
	$(CC) $^ -o $@ $(CONFIG)  $(LIB) $(INC) $(CFLAG)
%.o:%.c
	$(CC) $< -o $@  -c $(CONFIG)  $(LIB) $(INC) $(CFLAG)


clean:
	$(RM) $(ELF) $(OBJ)
