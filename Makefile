all: project

project: project.c gfx.o
        gcc project.c gfx.o -lX11 -lm -o project

clean:
        rm project
