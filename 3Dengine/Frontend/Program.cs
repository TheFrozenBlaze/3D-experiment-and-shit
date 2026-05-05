using System;
using OpenTK.Graphics.OpenGL4;
using OpenTK;
using SDL3;
using Render;

class Initialize {
        class SDLBindingsContext : IBindingsContext
        {
            public IntPtr GetProcAddress(string procName)
            {
                return SDL.GLGetProcAddress(procName);
            }
        }

        static void Main() {


            if(SDL.Init(SDL.InitFlags.Video) == false) {
                Console.WriteLine(SDL.GetError());
            }else{
                Console.WriteLine("Initialized");
            }
         
            SDL.GLSetAttribute(SDL.GLAttr.ContextMajorVersion, 3);
            SDL.GLSetAttribute(SDL.GLAttr.ContextMinorVersion, 3);
            SDL.GLSetAttribute(SDL.GLAttr.ContextProfileMask, (int)SDL.GLProfile.Core);

            IntPtr Window = SDL.CreateWindow("Engine",
            100, 100, SDL.WindowFlags.OpenGL | SDL.WindowFlags.Resizable);
            SDL.SetWindowPosition(Window, Convert.ToInt32(SDL.WindowPosCentered()),Convert.ToInt32(SDL.WindowPosCentered()));

            if (Window == IntPtr.Zero)
            {
                Console.WriteLine(SDL.GetError());
                return;
            }
            IntPtr glContext = SDL.GLCreateContext(Window);

            if(SDL.GLMakeCurrent(Window, glContext) != true) {Console.WriteLine(SDL.GetError());}
            GL.LoadBindings(new SDLBindingsContext());
            if (glContext == IntPtr.Zero) {
                Console.WriteLine(SDL.GetError());
            }
            bool running = true;
            SDL.Event e;

            while (running)
            {
                while (SDL.PollEvent(out e) == true)
                {
                    if (e.Type == (uint)SDL.EventType.Quit)
                        running = false;
                }
                SDL.GetWindowSize(Window, out int w, out int h);
                Render.Objects.Context(w, h);
                SDL.GLSwapWindow(Window);
            }

            SDL.GLDestroyContext(glContext);
            SDL.DestroyWindow(Window);
            SDL.Quit();


        }
}

