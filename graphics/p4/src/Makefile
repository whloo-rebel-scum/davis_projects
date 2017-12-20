#OPENGL LIBS FOR MAC
#GLLIB := -framework OopenGl -frameowrk GLUT
#OPENGL LIBS FOR LINUX
GLLIB := -L/usr/lib64/ -lGL -lGLU -lglut #-lGLEW

CC := -Wall -g

whloo_p4: loo_wesley_p4.o
	g++ $(CC) $(GLLIB) -o whloo_p4 loo_wesley_p4.o
	
loo_wesley_p4.o: loo_wesley_p4.cpp
	g++ $(CC) -c loo_wesley_p4.cpp

clean: 
	rm *.o whloo_p4
