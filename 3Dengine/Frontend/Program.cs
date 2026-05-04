using System;
using SDL2;
using OpenTK.Graphics.OpenGL4;

namespace Window{
    class Initialize {
        static void Main(string[] args) {
            if(SDL.SDL_Init(SDL.SDL_INIT_VIDEO) < 0) {
                Console.WriteLine(SDL.SDL_GetError());
            }else{
                Console.WriteLine("Initialized");
            }
            Renderer Re = new Renderer();

            SDL.SDL_GL_SetAttribute(SDL.SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL.SDL_GL_SetAttribute(SDL.SDL_GL_CONTEXT_MINOR_VERSION, 3);

            IntPtr Window = SDL.SDL_CreateWindow("Engine",SDL.SDL_WINDOWPOS_CENTERED,SDL.SDL_WINDOWPOS_CENTERED, 100, 100, SDL.SDL_WindowFlags.SDL_WINDOW_OPENGL, SDL.SDL_WindowFlags.SDL_WINDOW_RESIZABLE);


            SDL.SDL_GLContext glContext = SDL.SDL_GL_CreateContext(Window);

            GL.LoadBindings(new SDLBindingsContext());

            if (Window == IntPtr.Zero)
            {
                Console.WriteLine(SDL.SDL_GetError());
                return;
            }
            if (glContext == IntPtr.Zero) {
                Console.WriteLine(SDL.SDL_GetError());
            }
            bool running = true;
            SDL.SDL_Event e;
            Re.Context(100, 100);

            while (running)
            {

                while (SDL.SDL_PollEvent(out e) == 1)
                {
                    if (e.type == SDL.SDL_EventType.SDL_QUIT)
                        running = false;
                }
                SDL.SDL_GL_SwapWindow(Window);
            }

            SDL.SDL_GL_DeleteContext(glContext);
            SDL.SDL_DestroyWindow(Window);
            SDL.SDL_Quit();


        }
    }
}
