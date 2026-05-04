using System;
using OpenTK.Graphics.OpenGL4;
using SDL2;
using Render;

class Initialize {


        private static IntPtr GetProcAddress(string name) =>
        SDL.SDL_GL_GetProcAddress(name);

        private static void LoadGL()
        {
            var getProc = new Func<string, IntPtr>(GetProcAddress);
            // OpenTK exposes GL.LoadBindings(Func<string, IntPtr>) in some builds:
            var mi = typeof(GL).GetMethod("LoadBindings", new[] { typeof(Func<string, IntPtr>) });
            if (mi != null) mi.Invoke(null, new object[] { getProc });
        }

        static void Main(string[] args) {


            if(SDL.SDL_Init(SDL.SDL_INIT_VIDEO) < 0) {
                Console.WriteLine(SDL.SDL_GetError());
            }else{
                Console.WriteLine("Initialized");
            }

            SDL.SDL_GL_SetAttribute(SDL.SDL_GLattr.SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL.SDL_GL_SetAttribute(SDL.SDL_GLattr.SDL_GL_CONTEXT_MINOR_VERSION, 3);


            IntPtr Window = SDL.SDL_CreateWindow("Engine",SDL.SDL_WINDOWPOS_CENTERED,SDL.SDL_WINDOWPOS_CENTERED, 100, 100, SDL.SDL_WindowFlags.SDL_WINDOW_OPENGL | SDL.SDL_WindowFlags.SDL_WINDOW_RESIZABLE);


            IntPtr glContext = SDL.SDL_GL_CreateContext(Window);
            LoadGL();

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
            Objects.Context(100, 100);

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

