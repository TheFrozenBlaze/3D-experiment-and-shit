using OpenTK.Graphics.OpenGL4;
namespace Render{
public class Objects {
    public static void Context(int w, int h) {
        GL.Viewport(0, 0, w, h);
        GL.ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        GL.Clear(ClearBufferMask.ColorBufferBit);
    }
}
}
