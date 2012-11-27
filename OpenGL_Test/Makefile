test: src/test.cpp InitShader.o
	g++ -g -Wall -lGL -lGLU -lGLEW -lglut -o $@ InitShader.o src/$@.cpp
	
fly_through: src/fly_through.cpp InitShader.o
	g++ -g -Wall -lGL -lGLU -lGLEW -lglut -o $@ InitShader.o src/$@.cpp 

rotating_cube: src/rotating_cube.cpp InitShader.o
	g++ -g -Wall -lGL -lGLU -lGLEW -lglut -o $@ InitShader.o src/$@.cpp 

triangle: src/triangle.cpp InitShader.o
	g++ -g -Wall -lGL -lGLU -lGLEW -lglut -o $@ InitShader.o src/$@.cpp 

InitShader.o: src/InitShader.cpp
	g++ -g -Wall -lGL -lGLU -lGLEW -lglut -c -o $@ src/InitShader.cpp

clean:
	rm -f InitShader.o
	rm -f test
	rm -f rotating_cube
	rm -f triangle