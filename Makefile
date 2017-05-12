all: vfdisk vdformat setup
setup:
	rm disco0.vd
	./createvd 0
	./vfdisk
	./vdformat
vdformat:
	gcc -o vdformat vdformat.c
vfdisk:
	gcc -o vfdisk vfdisk.c vdisk.o
