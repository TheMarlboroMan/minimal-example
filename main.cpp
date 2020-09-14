#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <vector>

//g++ main.cpp -lSDL2 -SDL2_image -lGL 

int main(int argc, char ** argv) {

	int window_w=200, window_h=200;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

	SDL_Window * window=SDL_CreateWindow("",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		window_w, window_h, SDL_WINDOW_OPENGL);

	SDL_GLContext context=SDL_GL_CreateContext(window);

	//Set logical size...
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//left, right, top, bottom, near and far.
	//Sets 0.0 to be top left. Adjust 0.5f to get the pixel centre.
	//TODO: Is this, perhaps the fuck up???
	glOrtho(-0.5f, (float)window_w-.5f, (float)window_h-.5f, -0.5f, 1.f, -1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0.f, 0.f, window_w, window_h);

	glClearColor(1.f, 0.5f, 0.5f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT);

//	SDL_GL_SwapWindow(window);
//	std::this_thread::sleep_for(std::chrono::seconds{2});

	//Draw a solid box, first the geometry, clockwise.
	struct point {int x, y;};

	std::vector<point>	points{
		{32, 32}, {64, 32}, {64, 64}, {32, 64}
	};

	glDisable(GL_BLEND);
	glColor3f(1.f, 1.f, 1.f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_INT, 0, points.data());
	glDrawArrays(GL_POLYGON, 0, points.size());
	glDisableClientState(GL_VERTEX_ARRAY);

	SDL_GL_SwapWindow(window);
	std::this_thread::sleep_for(std::chrono::seconds{2});

	//Then the texture... first we load it...
	SDL_Surface * surface=IMG_Load("sprites.png");
	GLuint index{};
	double texture_w{surface->w},
			texture_h{surface->h};
	glGenTextures(1, &index);
	glBindTexture(GL_TEXTURE_2D, index);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_w, texture_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);

	//Next we map it...
	using point_type=GLdouble;
	struct texpoint {point_type x, y;};

	glBindTexture(GL_TEXTURE_2D, index);
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//This would be the full texture...
//	std::vector<texpoint>	tex_points{{0., 0.}, {1., 0.}, {1., 1.}, {0., 1.}};

	auto to_opengl=[texture_w, texture_h](int x, int y) {

		point_type oglx=((double)x / texture_w),
					ogly=((double)y / texture_h);
		return texpoint{oglx, ogly};
	};

	//This would be the texture points {32, 32}, {64, 32}, {64, 64}, {32, 64}
	std::vector<texpoint>	tex_points{
		to_opengl(32, 32), to_opengl(64, 32), to_opengl(64, 64), to_opengl(32, 64)
	};
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
	glVertexPointer(2, GL_INT, 0, points.data());
	glTexCoordPointer(2, GL_DOUBLE, 0, tex_points.data());
	glDrawArrays(GL_QUADS, 0, points.size());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
	glDisable(GL_TEXTURE_2D);

	SDL_GL_SwapWindow(window);
	std::this_thread::sleep_for(std::chrono::seconds{2});

	glDeleteTextures(1, &index);

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);

	return 0;
}
