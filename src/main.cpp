#include <SDL.h>
#include "glad/glad.h"

#include "nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

int main(int argc, char **argv)
{
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed %s\n", SDL_GetError());
		exit(1);
	}

	SDL_Window *wnd = SDL_CreateWindow("SDL - NanoVG test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!wnd) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed %s\n", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_GLContext ctx = SDL_GL_CreateContext(wnd);
	if (!ctx) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext failed %s\n", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetSwapInterval(1);

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to resolve OpenGL functions");
		exit(1);
	}

	auto vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
	auto renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
	auto version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Using OpenGL context with vendor '%s', renderer '%s', version '%s'",
		vendor, renderer, version);

	NVGcontext *vg = nvgCreateGL2(NVG_ANTIALIAS);
	if (!vg) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize NanoVG");
		exit(1);
	}

	bool done = false;
	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				done = true;
				break;
			}
		}

		SDL_GL_MakeCurrent(wnd, ctx);

		int w, h;
		SDL_GL_GetDrawableSize(wnd, &w, &h);
		glViewport(0, 0, w, h);

		nvgBeginFrame(vg, w, h, 1);

		nvgBeginPath(vg);
		nvgRoundedRect(vg, 100, 100, 300, 300, 10);
		nvgFillColor(vg, nvgRGB(255, 192, 0));
		nvgFill(vg);
		nvgStrokeWidth(vg, 4);
		nvgStrokeColor(vg, nvgRGB(0, 255, 0));
		nvgStroke(vg);

		nvgBeginPath(vg);
		nvgEllipse(vg, 800, 400, 100, 50);
		NVGpaint grad = nvgLinearGradient(vg, 700, 350, 900, 450, nvgRGB(0, 255, 0), nvgRGB(255, 255, 0));
		nvgFillPaint(vg, grad);
		nvgFill(vg);

		nvgEndFrame(vg);

		SDL_GL_SwapWindow(wnd);
	}

	nvgDeleteGL2(vg);
	SDL_GL_DeleteContext(ctx);
	SDL_DestroyWindow(wnd);
	return 0;
}
