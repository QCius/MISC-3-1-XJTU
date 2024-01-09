all : p t system_call spinlock
.PHONY : all

p: p.c 
	gcc -o p p.c 

system_call: system_call.c 
	gcc -o system_call system_call.c 

t: t.c
	gcc -o t t.c -lpthread

spinlock: spinlock.c
	gcc -o spinlock spinlock.c -lpthread

