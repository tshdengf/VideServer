obj := work.o client.o

client:$(obj)
	gcc $(obj) -o client -lpthread
%.o:%.c
	gcc $< -c -o $@
.PHONY:clean
clean:
	rm -rf *.o client