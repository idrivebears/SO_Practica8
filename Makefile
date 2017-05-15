all: filesystem vfdisk vdformat user_fs shell run_format
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
	gcc -c filesystem.c 
dumpseclog: dumpseclog.c
	gcc -o dumpseclog dumpseclog.c filesystem.o vdisk.o
user_fs: user_fs.c
	gcc -o user_fs user_fs.c vdisk.o filesystem.o
shell: shell.c
	gcc -o shell shell.c filesystem.o vdisk.o