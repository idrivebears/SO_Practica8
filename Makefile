all: filesystem vfdisk vdformat run_format
run_format:
	rm disco0.vd
	./createvd 0
	./vfdisk
	./vdformat
vdformat: vdformat.c
	gcc -o vdformat vdformat.c filesystem.o vdisk.o
vfdisk: vfdisk.c
	gcc -o vfdisk vfdisk.c vdisk.o filesystem.o
filesystem: filesystem.c filesystem.h
	gcc -c filesystem.c vdisk.o