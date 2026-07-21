#include <iostream>
#include <cmath>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include "glmain.h"
#include "window.h"
#include "shader.h"
#include "3dvfunc.h"
//#include "myimgui.h"

bool Window::MMB() {
    SDL_PumpEvents();

    SDL_MouseButtonFlags buttons = SDL_GetMouseState(nullptr, nullptr);

    if(buttons & SDL_BUTTON_MMASK)
    {
        //std::cout << "trying to rotate" << std::endl;
        return true;
    } else {
        return false;
    }
};

void Window::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) == false)
    {
        std::cout << SDL_GetError() << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Initialized" << std::endl;
    }
    GL::gWindowWidth = {1280};
    GL::gWindowHeight = {720};
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("Engine",
                              GL::gWindowWidth, GL::gWindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    if (window == nullptr)
    {
        std::cout << SDL_GetError() << std::endl;
        return;
    }
    glContext = SDL_GL_CreateContext(window);
    
    if (SDL_GL_MakeCurrent(window, glContext) != true)
    {
        std::cout << SDL_GetError() << std::endl;
    }
    if (glContext == nullptr)
    {
        std::cout << SDL_GetError() << std::endl;
    }
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cout << "glad couldn't initialize" << std::endl;
    }
    
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    GL::shader = new Shader("../lib/shader/triangle.vert",
    "../lib/shader/triangle.frag"
    );
    GL::shader->Use();
    
    GL::gCamera = {0.0f, -10.0f, -20.0f};
    GL::gCameraDesc = {0.0f, -10.0f, -19.0f, 0.0f, -9.0f, -20.0f, 1.0f, -10.0f, -20.0f};
    float translation[16] = {
        GL::gCameraDesc[2][0]-GL::gCamera[0], GL::gCameraDesc[1][0]-GL::gCamera[0], -(GL::gCameraDesc[0][0]-GL::gCamera[0]), 0.0f,
        GL::gCameraDesc[2][1]-GL::gCamera[1], GL::gCameraDesc[1][1]-GL::gCamera[1], -(GL::gCameraDesc[0][1]-GL::gCamera[1]), 0.0f,
        GL::gCameraDesc[2][2]-GL::gCamera[2], GL::gCameraDesc[1][2]-GL::gCamera[2], -(GL::gCameraDesc[0][2]-GL::gCamera[2]), 0.0f,
        -(GL::gCamera[0]), -GL::gCamera[1], GL::gCamera[2], 1.0f};
    
    //float translation[16] = { 1.0, 0, 0, 0,
    //                          0, 1.0, 0, 0, 
    //                          0, 0, -1.0, 0,
    //                          0, 10.0f , -50.0, 1.0};
    float perspective[16] = {
        static_cast<float>(1 / (0.839099 * GL::gWindowWidth / GL::gWindowHeight)), 0.0f, 0.0f, 0.0f,
        0.0f, 1 / 0.839099, 0.0f, 0.0f,
        0.0f, 0.0f, -1.002002f, -1.0f,
        0.0f, 0.0f, -0.2002002f, 0.0f};
    //std::cout << "perspective[10]=" << perspective[10] << " perspective[14]=" << perspective[14] << std::endl;
    GLint cameraloc = glGetUniformLocation(GL::shader->program_index, "translation");
    glUniformMatrix4fv(cameraloc, 1, GL_FALSE, &translation[0]);
    //std::cout << "translation loc: " << cameraloc << std::endl;
    GLint perspectiveloc = glGetUniformLocation(GL::shader->program_index, "perspective");
    glUniformMatrix4fv(perspectiveloc, 1, GL_FALSE, &perspective[0]);
    //std::cout << "perspective loc: " << perspectiveloc << std::endl;

        
};
        void Window::Mainloop()
        {
            
            //GLuint tbo;
            //glGenBuffers(1, &tbo);
            //glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tbo);
            //glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, (objIdent::objects.back().vpc.size())/3 * 4 * sizeof(float), nullptr, GL_DYNAMIC_READ);
            //glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
            
            GLuint gridVAO {};
            GLuint gridVBO {};
            glGenVertexArrays(1, &gridVAO);
            glBindVertexArray(gridVAO);
            glGenBuffers(1, &gridVBO);
            glVertexAttribPointer(1, 2,  GL_FLOAT, GL_FALSE, sizeof(std::pair<float, float>), (void*)0);
            glEnableVertexAttribArray(1);

            std::vector<std::string> filenames = {"../../example/humanoid_quad.obj"};
            GL::Compile(filenames);
            bool running = true;
            SDL_Event e;
            //glEnable(GL_RASTERIZER_DISCARD); // skip actual rendering while capturing
            //glBeginTransformFeedback(GL_TRIANGLES);
            //glBindVertexArray(GL::VAOvec.back());
            //glDrawElements(GL_TRIANGLES, objIdent::objects.back().triangles.size() * 3, GL_UNSIGNED_INT, nullptr);
            //glEndTransformFeedback();
            //glDisable(GL_RASTERIZER_DISCARD);
            //std::vector<float> result((objIdent::objects.back().vpc.size())/3  * 4);
            //glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, result.size() * sizeof(float), result.data());
            //for (size_t i = 0; i < result.size(); i += 4)
                //{std::cout << result[i] << " " << result[i+1] << " " << result[i+2] << " " << result[i+3] << std::endl;}
                //const bool *mousestate = SDL_GetMouseState(nullptr);
            const bool *keyboard = SDL_GetKeyboardState(nullptr);
            float rad = 0.01745329;
            int8_t fovdeg = 40;
            float FOV = std::tan(rad*fovdeg);
            float prevx {};
            float prevy {};
            //GUIInit();
            while (running)
            {
                prevx = GL::gCamera[0];
                prevy = GL::gCamera[2];
                //MainMenu();
                while (SDL_PollEvent(&e) == true)
                {
                    if (e.type == SDL_EVENT_QUIT)
                        running = false;
                    
                            
                    else if (e.type == SDL_EVENT_WINDOW_RESIZED)
                    {
                            GL::gWindowWidth = e.window.data1;
                            GL::gWindowHeight = e.window.data2;
                            glViewport(0, 0, GL::gWindowWidth, GL::gWindowHeight);
                            float perspective[16] = {
                                static_cast<float>(1 / (FOV * GL::gWindowWidth / GL::gWindowHeight)), 0.0f, 0.0f, 0.0f,
                                0.0f, 1 / FOV, 0.0f, 0.0f,
                                0.0f, 0.0f, -1.002002f, -1.0f,
                                0.0f, 0.0f, -0.2002002f, 0.0f};
                            GLint perspectiveloc = glGetUniformLocation(GL::shader->program_index, "perspective");
                            glUniformMatrix4fv(perspectiveloc, 1, GL_FALSE, &perspective[0]);
                    }
                        else if ((e.type == SDL_EVENT_MOUSE_MOTION && MMB()))
                        {
                            //std::cout << "sensing" << std::endl;
                            //std::cout << "xrel: " << e.motion.xrel << " " << "yrel: " << e.motion.yrel << std::endl;
                            //std::cout << "xrelvec: " << (GL::gCameraDesc[0][0] - GL::gCamera[0]) << " "<<(GL::gCameraDesc[1][0] - GL::gCamera[0])<< " "<<(GL::gCameraDesc[2][0] - GL::gCamera[0])<< std::endl;
                            //std::cout << "xrot up: " << GL::gCameraDesc[1][0] << " "<<GL::gCameraDesc[1][1]<< " "<<GL::gCameraDesc[1][2]<< std::endl;
                            //std::cout << "xrot side: " << GL::gCameraDesc[2][0] << " "<<GL::gCameraDesc[2][1]<< " "<<GL::gCameraDesc[2][2]<< std::endl;
                            GL::gCameraDesc[0] = {
                                GL::gCamera[0] + (static_cast<float>((GL::gCameraDesc[0][2] - GL::gCamera[2]) * sin(rad * e.motion.xrel * 0.1) + (GL::gCameraDesc[0][0] - GL::gCamera[0]) * cos(rad * e.motion.xrel * 0.1))),
                                GL::gCameraDesc[0][1],
                                GL::gCamera[2] + (static_cast<float>((GL::gCameraDesc[0][2] - GL::gCamera[2]) * cos(rad * e.motion.xrel * 0.1) + (GL::gCameraDesc[0][0] - GL::gCamera[0]) * (-sin(rad * e.motion.xrel * 0.1))))
                                };
                            //std::cout << "xrot forw: " << GL::gCameraDesc[0][0] << " "<<GL::gCameraDesc[0][1]<< " "<<GL::gCameraDesc[0][2]<< std::endl;
                            GL::gCameraDesc[1] = {
                                GL::gCamera[0] + (static_cast<float>((GL::gCameraDesc[1][2] - GL::gCamera[2]) * sin(rad * e.motion.xrel * 0.1) + (GL::gCameraDesc[1][0] - GL::gCamera[0]) * cos(rad * e.motion.xrel * 0.1))),
                                GL::gCameraDesc[1][1],
                                GL::gCamera[2] + (static_cast<float>((GL::gCameraDesc[1][2] - GL::gCamera[2]) * cos(rad * e.motion.xrel * 0.1) + (GL::gCameraDesc[1][0] - GL::gCamera[0]) * (-sin(rad * e.motion.xrel * 0.1))))
                                };
                            //std::cout << "xrot up: " << GL::gCameraDesc[1][0] << " "<<GL::gCameraDesc[1][1]<< " "<<GL::gCameraDesc[1][2]<< std::endl;
                            GL::gCameraDesc[2] = {
                                GL::gCamera[0] + (static_cast<float>((GL::gCameraDesc[2][2] - GL::gCamera[2]) * sin(rad * e.motion.xrel * 0.1) + (GL::gCameraDesc[2][0] - GL::gCamera[0]) * cos(rad * e.motion.xrel * 0.1))),
                                GL::gCameraDesc[2][1],
                                GL::gCamera[2] + (static_cast<float>((GL::gCameraDesc[2][2] - GL::gCamera[2]) * cos(rad * e.motion.xrel * 0.1) + (GL::gCameraDesc[2][0] - GL::gCamera[0]) * (-sin(rad * e.motion.xrel * 0.1))))
                                };
                            //std::cout << "xrot side: " << GL::gCameraDesc[2][0] << " "<<GL::gCameraDesc[2][1]<< " "<<GL::gCameraDesc[2][2]<< std::endl;
                            //std::cout << "sin: " <<  sin(rad * e.motion.xrel * 0.1) << " " << "cos: "<< cos(rad * e.motion.xrel * 0.1) << std::endl;
                            //std::cout << "sin: " <<  sin(rad * e.motion.yrel * 0.1) << " " << "cos: "<< cos(rad * e.motion.yrel * 0.1) << std::endl;
                            //std::cout << "xrot forw: " << GL::gCameraDesc[0][0] << " "<<GL::gCameraDesc[0][1]<< " "<<GL::gCameraDesc[0][2]<< std::endl;
                            //std::cout << "xrot up: " << GL::gCameraDesc[1][0] << " "<<GL::gCameraDesc[1][1]<< " "<<GL::gCameraDesc[1][2]<< std::endl;
                            //std::cout << "xrot side: " << GL::gCameraDesc[2][0] << " "<<GL::gCameraDesc[2][1]<< " "<<GL::gCameraDesc[2][2]<< std::endl;
                            GL::gCameraDesc[0] = { GL::gCameraDesc[0][0],
                                                   GL::gCamera[1] + (static_cast<float>((GL::gCameraDesc[0][1] - GL::gCamera[1]) * cos(rad * e.motion.yrel * 0.1) + (GL::gCameraDesc[0][2] - GL::gCamera[2]) * (-sin(rad * e.motion.yrel * 0.1)))),
                                                   GL::gCamera[2] + (static_cast<float>((GL::gCameraDesc[0][1] - GL::gCamera[1]) * sin(rad * e.motion.yrel * 0.1) + (GL::gCameraDesc[0][2] - GL::gCamera[2]) * cos(rad * e.motion.yrel * 0.1)))
                                                };
                            //std::cout << "yrot forw: " << GL::gCameraDesc[0][0] << " " << GL::gCameraDesc[0][1]<< " " << GL::gCameraDesc[0][2]<< std::endl;
                            GL::gCameraDesc[1] = {GL::gCameraDesc[1][0],
                                                  GL::gCamera[1] + (static_cast<float>((GL::gCameraDesc[1][1] - GL::gCamera[1]) * cos(rad * e.motion.yrel * 0.1) + (GL::gCameraDesc[1][2] - GL::gCamera[2]) * (-sin(rad * e.motion.yrel * 0.1)))),
                                                  GL::gCamera[2] + (static_cast<float>((GL::gCameraDesc[1][1] - GL::gCamera[1]) * sin(rad * e.motion.yrel * 0.1) + (GL::gCameraDesc[1][2] - GL::gCamera[2]) * cos(rad * e.motion.yrel * 0.1)))
                                                };
                            //std::cout << "yrot up: " << GL::gCameraDesc[1][0] << " " << GL::gCameraDesc[1][1]<< " " << GL::gCameraDesc[1][2]<< std::endl;
                            GL::gCameraDesc[2] = {GL::gCameraDesc[2][0],
                                                  GL::gCamera[1] + (static_cast<float>((GL::gCameraDesc[2][1] - GL::gCamera[1]) * cos(rad * e.motion.yrel * 0.1) + (GL::gCameraDesc[2][2] - GL::gCamera[2]) * (-sin(rad * e.motion.yrel * 0.1)))),
                                                  GL::gCamera[2] + (static_cast<float>((GL::gCameraDesc[2][1] - GL::gCamera[1]) * sin(rad * e.motion.yrel * 0.1) + (GL::gCameraDesc[2][2] - GL::gCamera[2]) * cos(rad * e.motion.yrel * 0.1)))
                                                };
                            //std::cout << "yrot side: " << GL::gCameraDesc[2][0] << " " << GL::gCameraDesc[2][1]<< " " << GL::gCameraDesc[2][2]<< std::endl;
                            //std::cout << "yrot forw: " << GL::gCameraDesc[0][0] << " " << GL::gCameraDesc[0][1]<< " " << GL::gCameraDesc[0][2]<< std::endl;
                            //std::cout << "yrot up: " << GL::gCameraDesc[1][0] << " " << GL::gCameraDesc[1][1]<< " " << GL::gCameraDesc[1][2]<< std::endl;
                            //std::cout << "yrot side: " << GL::gCameraDesc[2][0] << " " << GL::gCameraDesc[2][1]<< " " << GL::gCameraDesc[2][2]<< std::endl;
                            
                            float translation[16] = {
                                GL::gCameraDesc[2][0]-GL::gCamera[0], GL::gCameraDesc[1][0]-GL::gCamera[0], -(GL::gCameraDesc[0][0]-GL::gCamera[0]), 0.0f,
                                GL::gCameraDesc[2][1]-GL::gCamera[1], GL::gCameraDesc[1][1]-GL::gCamera[1], -(GL::gCameraDesc[0][1]-GL::gCamera[1]), 0.0f,
                                GL::gCameraDesc[2][2]-GL::gCamera[2], GL::gCameraDesc[1][2]-GL::gCamera[2], -(GL::gCameraDesc[0][2]-GL::gCamera[2]), 0.0f,
                                -(GL::gCamera[0]*(GL::gCameraDesc[2][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[2][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[2][2]-GL::gCamera[2])),
                                -(GL::gCamera[0]*(GL::gCameraDesc[1][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[1][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[1][2]-GL::gCamera[2])),
                                (GL::gCamera[0]*(GL::gCameraDesc[0][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[0][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[0][2]-GL::gCamera[2])), 1.0f
                            };
                            //std::cout << "dot forw: " << -(GL::gCamera[0]*(GL::gCameraDesc[0][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[0][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[0][2]-GL::gCamera[2]))<< std::endl;
                            //std::cout << "dot side: " << -(GL::gCamera[0]*(GL::gCameraDesc[1][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[1][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[1][2]-GL::gCamera[2]))<< std::endl;
                            //std::cout << "dot up: " << -(GL::gCamera[0]*(GL::gCameraDesc[2][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[2][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[2][2]-GL::gCamera[2]))<< std::endl;

                            //std::cout << "forw dot up: " << (GL::gCameraDesc[0][0]-GL::gCamera[0]) * (GL::gCameraDesc[1][0]-GL::gCamera[0]) + (GL::gCameraDesc[0][1]-GL::gCamera[1]) * (GL::gCameraDesc[1][1]-GL::gCamera[1]) + (GL::gCameraDesc[0][2]-GL::gCamera[2]) * (GL::gCameraDesc[1][2]-GL::gCamera[2]) << std::endl;
                            //std::cout << "up dot side: " << (GL::gCameraDesc[2][0]-GL::gCamera[0]) * (GL::gCameraDesc[1][0]-GL::gCamera[0]) + (GL::gCameraDesc[2][1]-GL::gCamera[1]) * (GL::gCameraDesc[1][1]-GL::gCamera[1]) + (GL::gCameraDesc[2][2]-GL::gCamera[2]) * (GL::gCameraDesc[1][2]-GL::gCamera[2]) << std::endl;
                            //std::cout << "forw dot side: " << (GL::gCameraDesc[0][0]-GL::gCamera[0]) * (GL::gCameraDesc[2][0]-GL::gCamera[0]) + (GL::gCameraDesc[0][1]-GL::gCamera[1]) * (GL::gCameraDesc[2][1]-GL::gCamera[1]) + (GL::gCameraDesc[0][2]-GL::gCamera[2]) * (GL::gCameraDesc[2][2]-GL::gCamera[2]) << std::endl;
                                                        /*float translation[16] = {
                                    GL::gCameraDesc[2][0]-GL::gCamera[0], GL::gCameraDesc[2][1]-GL::gCamera[1], -(GL::gCameraDesc[2][2]-GL::gCamera[2]), 0.0f,
                                    GL::gCameraDesc[1][0]-GL::gCamera[0], GL::gCameraDesc[1][1]-GL::gCamera[1], -(GL::gCameraDesc[1][2]-GL::gCamera[2]), 0.0f,
                                    GL::gCameraDesc[0][0]-GL::gCamera[0], GL::gCameraDesc[0][1]-GL::gCamera[1], -(GL::gCameraDesc[0][2]-GL::gCamera[2]), 0.0f,
                                    -GL::gCamera[0], -GL::gCamera[1], GL::gCamera[2], 1.0f
                                };*/
    
                            
                            GLint cameraloc = glGetUniformLocation(GL::shader->program_index, "translation");
                            glUniformMatrix4fv(cameraloc, 1, GL_FALSE, &translation[0]);
                           
                        }

                        else if (e.type == SDL_EVENT_MOUSE_WHEEL)
                        {
                            if (keyboard[SDL_SCANCODE_LSHIFT]){
                                fovdeg += e.wheel.y;
                                float perspective[16] = {
                                    static_cast<float>(1 / (FOV * GL::gWindowWidth / GL::gWindowHeight)), 0.0f, 0.0f, 0.0f,
                                    0.0f, 1 / FOV, 0.0f, 0.0f,
                                    0.0f, 0.0f, -1.002002f, -1.0f,
                                    0.0f, 0.0f, -0.2002002f, 0.0f};
                                GLint perspectiveloc = glGetUniformLocation(GL::shader->program_index, "perspective");
                                glUniformMatrix4fv(perspectiveloc, 1, GL_FALSE, &perspective[0]);
                            }
                            float diffx = (GL::gCameraDesc[1][0] - GL::gCamera[0]) * e.wheel.y;
                            float diffz = (GL::gCameraDesc[1][1] - GL::gCamera[1]) * e.wheel.y;
                            float diffy = (GL::gCameraDesc[1][2] - GL::gCamera[2]) * -e.wheel.y;
                            
                                //std::cout << "horizontal: " << e.wheel.x << " " << "vertical: " << e.wheel.y << std::endl;
                                GL::gCamera[0] += diffx;
                                GL::gCamera[1] += diffy;
                                GL::gCamera[2] += diffz;
                                GL::gCameraDesc[0][0] += diffx;
                                GL::gCameraDesc[0][1] += diffy;
                                GL::gCameraDesc[0][2] += diffz;
                                GL::gCameraDesc[1][0] += diffx;
                                GL::gCameraDesc[1][1] += diffy;
                                GL::gCameraDesc[1][2] += diffz;
                                GL::gCameraDesc[2][0] += diffx;
                                GL::gCameraDesc[2][1] += diffy;
                                GL::gCameraDesc[2][2] += diffz;
                                
                                float translation[16] = {
                                GL::gCameraDesc[2][0]-GL::gCamera[0], GL::gCameraDesc[1][0]-GL::gCamera[0], -(GL::gCameraDesc[0][0]-GL::gCamera[0]), 0.0f,
                                GL::gCameraDesc[2][1]-GL::gCamera[1], GL::gCameraDesc[1][1]-GL::gCamera[1], -(GL::gCameraDesc[0][1]-GL::gCamera[1]), 0.0f,
                                GL::gCameraDesc[2][2]-GL::gCamera[2], GL::gCameraDesc[1][2]-GL::gCamera[2], -(GL::gCameraDesc[0][2]-GL::gCamera[2]), 0.0f,
                                -(GL::gCamera[0]*(GL::gCameraDesc[2][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[2][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[2][2]-GL::gCamera[2])),
                                -(GL::gCamera[0]*(GL::gCameraDesc[1][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[1][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[1][2]-GL::gCamera[2])),
                                (GL::gCamera[0]*(GL::gCameraDesc[0][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[0][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[0][2]-GL::gCamera[2])), 1.0f
                            };
                                GLint cameraloc = glGetUniformLocation(GL::shader->program_index, "translation");
                                glUniformMatrix4fv(cameraloc, 1, GL_FALSE, &translation[0]);
                        }
                    }
                    
                    if (keyboard[SDL_SCANCODE_W])
                    {
                        GL::gCameraDesc[0] = { GL::gCameraDesc[0][0],
                                                   static_cast<float>(GL::gCameraDesc[0][1] * cos(rad) + GL::gCameraDesc[0][2] * (-sin(rad))),
                                                   static_cast<float>(GL::gCameraDesc[0][1] * sin(rad) + GL::gCameraDesc[0][2] * cos(rad))
                                                };
                            //std::cout << "yrot forw: " << GL::gCameraDesc[0][0] << " " << GL::gCameraDesc[0][1]<< " " << GL::gCameraDesc[0][2]<< std::endl;
                            GL::gCameraDesc[1] = {GL::gCameraDesc[1][0],
                                                  static_cast<float>(GL::gCameraDesc[1][1] * cos(rad) + GL::gCameraDesc[1][2] * (-sin(rad))),
                                                  static_cast<float>(GL::gCameraDesc[1][1] * sin(rad) + GL::gCameraDesc[1][2] * cos(rad))
                                                };
                            //std::cout << "yrot up: " << GL::gCameraDesc[1][0] << " " << GL::gCameraDesc[1][1]<< " " << GL::gCameraDesc[1][2]<< std::endl;
                            GL::gCameraDesc[2] = {GL::gCameraDesc[2][0],
                                                  static_cast<float>(GL::gCameraDesc[2][1] * cos(rad) + GL::gCameraDesc[2][2] * (-sin(rad))),
                                                  static_cast<float>(GL::gCameraDesc[2][1] * sin(rad) + GL::gCameraDesc[2][2] * cos(rad))
                                                };
                            GL::gCamera = {
                                GL::gCamera[0],
                                static_cast<float>(GL::gCamera[1] * cos(rad) + GL::gCamera[2] * (-sin(rad))),
                                static_cast<float>(GL::gCamera[1] * sin(rad) + GL::gCamera[2] * cos(rad))

                            };
                            // std::cout << "yrot side: " << GL::gCameraDesc[2][0] << " " << GL::gCameraDesc[2][1]<< " " << GL::gCameraDesc[2][2]<< std::endl;
                            float translation[16] = {
                                GL::gCameraDesc[2][0]-GL::gCamera[0], GL::gCameraDesc[1][0]-GL::gCamera[0], -(GL::gCameraDesc[0][0]-GL::gCamera[0]), 0.0f,
                                GL::gCameraDesc[2][1]-GL::gCamera[1], GL::gCameraDesc[1][1]-GL::gCamera[1], -(GL::gCameraDesc[0][1]-GL::gCamera[1]), 0.0f,
                                GL::gCameraDesc[2][2]-GL::gCamera[2], GL::gCameraDesc[1][2]-GL::gCamera[2], -(GL::gCameraDesc[0][2]-GL::gCamera[2]), 0.0f,
                                -(GL::gCamera[0]*(GL::gCameraDesc[2][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[2][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[2][2]-GL::gCamera[2])),
                                -(GL::gCamera[0]*(GL::gCameraDesc[1][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[1][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[1][2]-GL::gCamera[2])),
                                (GL::gCamera[0]*(GL::gCameraDesc[0][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[0][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[0][2]-GL::gCamera[2])), 1.0f
                            };
                            GLint cameraloc = glGetUniformLocation(GL::shader->program_index, "translation");
                            glUniformMatrix4fv(cameraloc, 1, GL_FALSE, &translation[0]);
                    }
                    if (keyboard[SDL_SCANCODE_S])
                    {
                        GL::gCameraDesc[0] = { GL::gCameraDesc[0][0],
                                                   static_cast<float>(GL::gCameraDesc[0][1] * cos(-rad) + GL::gCameraDesc[0][2] * (-sin(-rad))),
                                                   static_cast<float>(GL::gCameraDesc[0][1] * sin(-rad) + GL::gCameraDesc[0][2] * cos(-rad))
                                                };
                            //std::cout << "yrot forw: " << GL::gCameraDesc[0][0] << " " << GL::gCameraDesc[0][1]<< " " << GL::gCameraDesc[0][2]<< std::endl;
                            GL::gCameraDesc[1] = {GL::gCameraDesc[1][0],
                                                  static_cast<float>(GL::gCameraDesc[1][1] * cos(-rad) + GL::gCameraDesc[1][2] * (-sin(-rad))),
                                                  static_cast<float>(GL::gCameraDesc[1][1] * sin(-rad) + GL::gCameraDesc[1][2] * cos(-rad))
                                                };
                            //std::cout << "yrot up: " << GL::gCameraDesc[1][0] << " " << GL::gCameraDesc[1][1]<< " " << GL::gCameraDesc[1][2]<< std::endl;
                            GL::gCameraDesc[2] = {GL::gCameraDesc[2][0],
                                                  static_cast<float>(GL::gCameraDesc[2][1] * cos(-rad) + GL::gCameraDesc[2][2]  * (-sin(-rad))),
                                                  static_cast<float>(GL::gCameraDesc[2][1] * sin(-rad) + GL::gCameraDesc[2][2] * cos(-rad))
                                                };
                            GL::gCamera = {
                                GL::gCamera[0],
                                static_cast<float>(GL::gCamera[1]  * cos(-rad) + GL::gCamera[2] * (-sin(-rad))),
                                static_cast<float>(GL::gCamera[1]  * sin(-rad) + GL::gCamera[2]  * cos(-rad))

                            };
                            //std::cout << "yrot side: " << GL::gCameraDesc[2][0] << " " << GL::gCameraDesc[2][1]<< " " << GL::gCameraDesc[2][2]<< std::endl;
                        float translation[16] = {
                                GL::gCameraDesc[2][0]-GL::gCamera[0], GL::gCameraDesc[1][0]-GL::gCamera[0], -(GL::gCameraDesc[0][0]-GL::gCamera[0]), 0.0f,
                                GL::gCameraDesc[2][1]-GL::gCamera[1], GL::gCameraDesc[1][1]-GL::gCamera[1], -(GL::gCameraDesc[0][1]-GL::gCamera[1]), 0.0f,
                                GL::gCameraDesc[2][2]-GL::gCamera[2], GL::gCameraDesc[1][2]-GL::gCamera[2], -(GL::gCameraDesc[0][2]-GL::gCamera[2]), 0.0f,
                                -(GL::gCamera[0]*(GL::gCameraDesc[2][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[2][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[2][2]-GL::gCamera[2])),
                                -(GL::gCamera[0]*(GL::gCameraDesc[1][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[1][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[1][2]-GL::gCamera[2])),
                                (GL::gCamera[0]*(GL::gCameraDesc[0][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[0][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[0][2]-GL::gCamera[2])), 1.0f
                            };
                        GLint cameraloc = glGetUniformLocation(GL::shader->program_index, "translation");
                            glUniformMatrix4fv(cameraloc, 1, GL_FALSE, &translation[0]);
                    }
                    if (keyboard[SDL_SCANCODE_A])
                    {
                        GL::gCameraDesc[0] = {
                                static_cast<float>(GL::gCameraDesc[0][2] * sin(rad) + GL::gCameraDesc[0][0] * cos(rad)),
                                GL::gCameraDesc[0][1],
                                static_cast<float>(GL::gCameraDesc[0][2] * cos(rad) + GL::gCameraDesc[0][0] * (-sin(rad)))
                                };
                                //std::cout << "xrot forw: " << GL::gCameraDesc[0][0] << " "<<GL::gCameraDesc[0][1]<< " "<<GL::gCameraDesc[0][2]<< std::endl;
                            GL::gCameraDesc[1] = {
                                static_cast<float>(GL::gCameraDesc[1][2] * sin(rad) + GL::gCameraDesc[1][0] * cos(rad)),
                                GL::gCameraDesc[1][1],
                                static_cast<float>(GL::gCameraDesc[1][2] * cos(rad) + GL::gCameraDesc[1][0] * (-sin(rad)))
                                };
                                //std::cout << "xrot up: " << GL::gCameraDesc[1][0] << " "<<GL::gCameraDesc[1][1]<< " "<<GL::gCameraDesc[1][2]<< std::endl;
                            GL::gCameraDesc[2] = {
                                static_cast<float>(GL::gCameraDesc[2][2] * sin(rad) + GL::gCameraDesc[2][0] * cos(rad)),
                                GL::gCameraDesc[2][1],
                                static_cast<float>(GL::gCameraDesc[2][2] * cos(rad) + GL::gCameraDesc[2][0] * (-sin(rad)))
                                };
                                //std::cout << "xrot side: " << GL::gCameraDesc[2][0] << " "<<GL::gCameraDesc[2][1]<< " "<<GL::gCameraDesc[2][2]<< std::endl;
                        GL::gCamera = {
                                static_cast<float>(GL::gCamera[2] * sin(rad) + GL::gCamera[0] * cos(rad)),
                                GL::gCamera[1],
                                static_cast<float>(GL::gCamera[2] * cos(rad) + GL::gCamera[0] * (-sin(rad)))
                                };

                        float translation[16] = {
                                GL::gCameraDesc[2][0]-GL::gCamera[0], GL::gCameraDesc[1][0]-GL::gCamera[0], -(GL::gCameraDesc[0][0]-GL::gCamera[0]), 0.0f,
                                GL::gCameraDesc[2][1]-GL::gCamera[1], GL::gCameraDesc[1][1]-GL::gCamera[1], -(GL::gCameraDesc[0][1]-GL::gCamera[1]), 0.0f,
                                GL::gCameraDesc[2][2]-GL::gCamera[2], GL::gCameraDesc[1][2]-GL::gCamera[2], -(GL::gCameraDesc[0][2]-GL::gCamera[2]), 0.0f,
                                -(GL::gCamera[0]*(GL::gCameraDesc[2][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[2][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[2][2]-GL::gCamera[2])),
                                -(GL::gCamera[0]*(GL::gCameraDesc[1][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[1][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[1][2]-GL::gCamera[2])),
                                (GL::gCamera[0]*(GL::gCameraDesc[0][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[0][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[0][2]-GL::gCamera[2])), 1.0f
                            };
                        GLint cameraloc = glGetUniformLocation(GL::shader->program_index, "translation");
                            glUniformMatrix4fv(cameraloc, 1, GL_FALSE, &translation[0]);
                    }
                    if (keyboard[SDL_SCANCODE_D])
                    {
                        GL::gCameraDesc[0] = {
                                static_cast<float>(GL::gCameraDesc[0][2] * sin(-rad) + GL::gCameraDesc[0][0] * cos(-rad)),
                                GL::gCameraDesc[0][1],
                                static_cast<float>(GL::gCameraDesc[0][2] * cos(-rad) + GL::gCameraDesc[0][0] * (-sin(-rad)))
                                };
                                //std::cout << "xrot forw: " << GL::gCameraDesc[0][0] << " "<<GL::gCameraDesc[0][1]<< " "<<GL::gCameraDesc[0][2]<< std::endl;
                            GL::gCameraDesc[1] = {
                                static_cast<float>(GL::gCameraDesc[1][2] * sin(-rad) + GL::gCameraDesc[1][0] * cos(-rad)),
                                GL::gCameraDesc[1][1],
                                static_cast<float>(GL::gCameraDesc[1][2] * cos(-rad) + GL::gCameraDesc[1][0] * (-sin(-rad)))
                                };
                                //std::cout << "xrot up: " << GL::gCameraDesc[1][0] << " "<<GL::gCameraDesc[1][1]<< " "<<GL::gCameraDesc[1][2]<< std::endl;
                            GL::gCameraDesc[2] = {
                                static_cast<float>(GL::gCameraDesc[2][2] * sin(-rad) + GL::gCameraDesc[2][0] * cos(-rad)),
                                GL::gCameraDesc[2][1],
                                static_cast<float>(GL::gCameraDesc[2][2] * cos(-rad) + GL::gCameraDesc[2][0] * (-sin(-rad)))
                                };
                                //std::cout << "xrot side: " << GL::gCameraDesc[2][0] << " "<<GL::gCameraDesc[2][1]<< " "<<GL::gCameraDesc[2][2]<< std::endl;
                        GL::gCamera = {
                                static_cast<float>(GL::gCamera[2] * sin(-rad) + GL::gCamera[0] * cos(-rad)),
                                GL::gCamera[1],
                                static_cast<float>(GL::gCamera[2] * cos(-rad) + GL::gCamera[0] * (-sin(-rad)))
                                };

                        float translation[16] = {
                                GL::gCameraDesc[2][0]-GL::gCamera[0], GL::gCameraDesc[1][0]-GL::gCamera[0], -(GL::gCameraDesc[0][0]-GL::gCamera[0]), 0.0f,
                                GL::gCameraDesc[2][1]-GL::gCamera[1], GL::gCameraDesc[1][1]-GL::gCamera[1], -(GL::gCameraDesc[0][1]-GL::gCamera[1]), 0.0f,
                                GL::gCameraDesc[2][2]-GL::gCamera[2], GL::gCameraDesc[1][2]-GL::gCamera[2], -(GL::gCameraDesc[0][2]-GL::gCamera[2]), 0.0f,
                                -(GL::gCamera[0]*(GL::gCameraDesc[2][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[2][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[2][2]-GL::gCamera[2])),
                                -(GL::gCamera[0]*(GL::gCameraDesc[1][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[1][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[1][2]-GL::gCamera[2])),
                                (GL::gCamera[0]*(GL::gCameraDesc[0][0]-GL::gCamera[0])+GL::gCamera[1]*(GL::gCameraDesc[0][1]-GL::gCamera[1])+GL::gCamera[2]*(GL::gCameraDesc[0][2]-GL::gCamera[2])), 1.0f
                            };
                        GLint cameraloc = glGetUniformLocation(GL::shader->program_index, "translation");
                            glUniformMatrix4fv(cameraloc, 1, GL_FALSE, &translation[0]);
                    }
                    GL::Draw(prevx, prevy, gridVAO, gridVBO);
                    SDL_GL_SwapWindow(window);
                }
        };
        void Window::Clean()
        {
            glDeleteVertexArrays(GL::VAOvec.size(), GL::VAOvec.data());
            glDeleteBuffers(GL::pVBOvec.size(), GL::pVBOvec.data());
            glDeleteBuffers(GL::EBOvec.size(), GL::EBOvec.data());
            SDL_GL_DestroyContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }


    

