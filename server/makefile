obj := work.o server.o pthreadpool.o

server:$(obj)
	gcc $(obj) -o server -lpthread -lsqlite3
%.o:%.c
	gcc $< -c -o $@
.PHONY:clean
clean:
	rm -rf *.o server