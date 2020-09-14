#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <vector>

//g++ main.cpp -lSDL2 -SDL2_image -lGL 

using tex_point_type=GLdouble;
struct point {int x, y;};
struct texpoint {tex_point_type x, y;};

//draws a solid box. Actually a clockwise polygon.
void draw_solid_box(const std::vector<point>& points);

//draws a textured box.
void draw_textured_box(const std::vector<point>&, const std::vector<texpoint>, GLuint);

//converts image point to opengl texture point [0-1]
texpoint to_opengl(point, double, double);

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

	//Black background...
	glClearColor(0.f, 0.f, 0., 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	//Load the texture it...
	SDL_Surface * surface=IMG_Load("sprites.png");
	GLuint index{};
	double texture_w{surface->w},
			texture_h{surface->h};
	glGenTextures(1, &index);
	glBindTexture(GL_TEXTURE_2D, index);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_w, texture_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);
	glBindTexture(GL_TEXTURE_2D, index);
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Setup texture params.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//draw a straight 1:1 texture...
	draw_textured_box(
		{{32,32}, {64, 32}, {64, 64}, {32, 64}},
		{
			to_opengl({32, 32}, texture_w, texture_h), 
			to_opengl({64, 32}, texture_w, texture_h), 
			to_opengl({64, 64}, texture_w, texture_h),
			to_opengl({32, 64}, texture_w, texture_h)
		},
		index
	);

	//draw a 64x64 box filled with a 32x32 texture (1:2)
	draw_textured_box(
		{{64,32}, {128, 32}, {128, 96}, {64, 96}},
		{
			to_opengl({32, 32}, texture_w, texture_h), 
			to_opengl({64, 32}, texture_w, texture_h), 
			to_opengl({64, 64}, texture_w, texture_h),
			to_opengl({32, 64}, texture_w, texture_h)
		},
		index
	);

	//draw a 32x32 box with the texture horizontally inverted...
	//TODO: This looks funky...
	draw_textured_box(
		{{32,64}, {64, 64}, {64, 96}, {32, 96}},
		{
			to_opengl({64, 32}, texture_w, texture_h), 
			to_opengl({32, 32}, texture_w, texture_h), 
			to_opengl({32, 64}, texture_w, texture_h),
			to_opengl({64, 64}, texture_w, texture_h)
		},
		index
	);

	//draw a 32x32 box with the texture vertically inverted...
	//TODO: This looks funky...
	draw_textured_box(
		{{128,32}, {160, 32}, {160, 64}, {128, 64}},
		{
			to_opengl({32, 64}, texture_w, texture_h), 
			to_opengl({64, 64}, texture_w, texture_h), 
			to_opengl({64, 32}, texture_w, texture_h),
			to_opengl({32, 32}, texture_w, texture_h)
		},
		index
	);

	//draw a 32x32 box with the texture inverted...
	//TODO: This looks funky...
	draw_textured_box(
		{{128,64}, {160, 64}, {160, 96}, {128, 96}},
		{
			to_opengl({64, 64}, texture_w, texture_h), 
			to_opengl({32, 64}, texture_w, texture_h), 
			to_opengl({32, 32}, texture_w, texture_h),
			to_opengl({64, 32}, texture_w, texture_h)
		},
		index
	);

	SDL_GL_SwapWindow(window);
	std::this_thread::sleep_for(std::chrono::seconds{5});

	glDeleteTextures(1, &index);

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);

	return 0;
}

void draw_textured_box(
	const std::vector<point>& geometry, 
	const std::vector<texpoint> texture_points, 
	GLuint index) {

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);

	glTexCoordPointer(2, GL_DOUBLE, 0, texture_points.data());
	glVertexPointer(2, GL_INT, 0, geometry.data());
	glDrawArrays(GL_QUADS, 0, geometry.size());

	glDisableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
	glDisableClientState(GL_VERTEX_ARRAY);
}

texpoint to_opengl(
	point pt, 
	double texture_w, 
	double texture_h) {

	tex_point_type oglx=((double)pt.x / texture_w),
				ogly=((double)pt.y / texture_h);

	return {oglx, ogly};
}

void draw_solid_box(const std::vector<point>& points) {

	glEnableClientState(GL_VERTEX_ARRAY);

	glColor3f(1.f, 1.f, 1.f);
	glVertexPointer(2, GL_INT, 0, points.data());
	glDrawArrays(GL_POLYGON, 0, points.size());

	glDisableClientState(GL_VERTEX_ARRAY);
}
