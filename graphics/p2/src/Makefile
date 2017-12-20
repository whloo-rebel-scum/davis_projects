
#OPENGL LIBS FOR MAC
#GLLIB := -framework OopenGl -frameowrk GLUT
#OPENGL LIBS FOR LINUX
GLLIB := -L/usr/lib64/ -lGL -lGLU -lglut #-lGLEW

CC := -Wall -g

whloo_p1: loo_wesley_p2.o
	g++ $(CC) $(GLLIB) -o whloo_p2 loo_wesley_p2.o

loo_wesley_p1.o: loo_wesley_p2.cpp
	g++ $(CC) -c loo_wesley_p2.cpp

clean:
	rm *.o whloo_p2
