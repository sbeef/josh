all: josh

josh: josh.c josh.h
	gccx -o josh josh.c

clean: 
	rm josh
