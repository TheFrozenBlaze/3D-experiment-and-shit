#include "3dvfunc.h"
#include "glmain.h"
#include "shader.h"
#include <vector>
#include <algorithm>
#include <glad/glad.h>
#include <cmath>
#include <cstdint>

GLuint VAO {};
GLuint pVBO {};
GLuint EBO {};
GLuint lEBO {};

std::vector<GLuint> GL::VAOvec;
std::vector<GLuint> GL::pVBOvec;
std::vector<GLuint> GL::EBOvec;
std::vector<GLuint> GL::lEBOvec;
std::vector<std::pair<float, float>> gridvert;


uint16_t GL::gWindowHeight, GL::gWindowWidth;
std::array<float,3> GL::gCamera;
std::array<std::array<float,3>, 3> GL::gCameraDesc;
std::vector<Coord> objIdent::objects;

Shader *GL::shader;
void GL::Compile(const std::vector<std::string> &files)
{
    objIdent obj;
    obj.objects.reserve(obj.objects.size() + files.size());
    
        for (size_t i{}; i < files.size(); i++)
        {
            obj.objReader(files[i]);
            Coord &cs = obj.objects.back();
            
            /*for (size_t i{}; i < cs.edge.size(); i++)
            {
                std::cout << "Edge" << " "<< cs.edge[i] << std::endl;
            }
            std::cout << "size" << " " << cs.vpc.size() << std::endl;*/
            /*for(size_t i =0; i< cs.triangles.size(); i++) {
            std::cout << "Triangle: "<< cs.triangles[i][0] << " " << cs.triangles[i][1] << " " << cs.triangles[i][2] << std::endl;
            };
            for (size_t i {}; i <cs.vpc.size(); i++) {
                std::cout << cs.vpc[i] << std::endl;
            }*/
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            GL::VAOvec.emplace_back(VAO);
            glGenBuffers(1, &pVBO);
            glGenBuffers(1, &EBO);

            glGenBuffers(1, &lEBO);
            // ebo

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, cs.triangles.size() * 3 * sizeof(uint32_t), cs.triangles.data(), GL_STATIC_DRAW);
            GL::EBOvec.emplace_back(EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, cs.edge.size() * sizeof(uint32_t), cs.edge.data(), GL_STATIC_DRAW);
            GL::lEBOvec.emplace_back(lEBO);
            // position vbo
            glBindBuffer(GL_ARRAY_BUFFER, pVBO);
            glBufferData(GL_ARRAY_BUFFER, cs.vpc.size() * sizeof(float), cs.vpc.data(), GL_DYNAMIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
            GL::pVBOvec.emplace_back(pVBO);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            pVBO = 0;
            VAO = 0;
            EBO = 0;
            lEBO = 0;
        }
}

void GL::Draw(float prevx, float prevy, GLuint& grVAO, GLuint& grVBO)
    {

    objIdent obj;
    glClearColor(0.2f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, GL::gWindowWidth, GL::gWindowHeight);
    GLint colorLoc = glGetUniformLocation(GL::shader->program_index, "uColor");
    GLint boolloc = glGetUniformLocation(GL::shader->program_index, "meshvgrid");
   
    /*float minx= std::min({GL::gCameraDesc[0][0]-GL::gCamera[0],GL::gCameraDesc[1][0]-GL::gCamera[0], GL::gCameraDesc[2][0]-GL::gCamera[0], -GL::gCameraDesc[0][0]-GL::gCamera[0],-GL::gCameraDesc[1][0]-GL::gCamera[0], -GL::gCameraDesc[2][0]-GL::gCamera[0]});
    float maxx= std::max({GL::gCameraDesc[0][0]-GL::gCamera[0],GL::gCameraDesc[1][0]-GL::gCamera[0], GL::gCameraDesc[2][0]-GL::gCamera[0], -GL::gCameraDesc[0][0]-GL::gCamera[0],-GL::gCameraDesc[1][0]-GL::gCamera[0], -GL::gCameraDesc[2][0]-GL::gCamera[0]});
    float minz= std::min({GL::gCameraDesc[0][2]-GL::gCamera[2],GL::gCameraDesc[1][2]-GL::gCamera[2], GL::gCameraDesc[2][2]-GL::gCamera[2], -GL::gCameraDesc[0][2]-GL::gCamera[2],-GL::gCameraDesc[1][2]-GL::gCamera[2], -GL::gCameraDesc[2][2]-GL::gCamera[2]});
    float maxz= std::max({GL::gCameraDesc[0][2]-GL::gCamera[2],GL::gCameraDesc[1][2]-GL::gCamera[2], GL::gCameraDesc[2][2]-GL::gCamera[2], -GL::gCameraDesc[0][2]-GL::gCamera[2],-GL::gCameraDesc[1][2]-GL::gCamera[2], -GL::gCameraDesc[2][2]-GL::gCamera[2]});

    minx = std::roundf(minx * 100);
    maxx = std::roundf(maxx * 100);
    minz = std::roundf(minz * 100);
    maxz = std::roundf(maxz * 100);*/
    glBindVertexArray(grVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grVBO);

    if(prevx - GL::gCamera[0] > 10e-2 || prevy-GL::gCamera[2] > 10e-2 )
    {
    gridvert.clear();
    int minx = std::floor(GL::gCamera[0]-100)/10;
    int maxx = std::floor(GL::gCamera[0]+100)/10;
    int minz = std::floor(GL::gCamera[2]-100)/10;
    int maxz = std::floor(GL::gCamera[2]+100)/10;
    for(uint i{}; i < 20; i++) {
        gridvert.emplace_back(std::pair<float, float>{(minx + i)*10, minz*10});
        gridvert.emplace_back(std::pair<float, float>{(minx + i) * 10, maxz* 10});
        gridvert.emplace_back(std::pair<float, float>{minx* 10, (minz + i) * 10}); 
        gridvert.emplace_back(std::pair<float, float>{maxx* 10, (minz + i) * 10});
    }

    }
    //std::cout << "vertices = " << gridvert.size() << std::endl;
    glUniform1i(boolloc, GL_TRUE);
    glUniform3f(colorLoc, 1.f,1.f,1.f);
    glBufferData(GL_ARRAY_BUFFER,gridvert.size() * sizeof(std::pair<float, float>), gridvert.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, gridvert.size());
    for (size_t i{}; i < obj.objects.size(); i++) {
        Coord &cs = obj.objects[i];

        
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        glUniform1i(boolloc, GL_FALSE);
        //std::cout << colorLoc << std::endl;
        glUniform3f(colorLoc, 0.75f,0.75f,0.75f);
        glBindVertexArray(GL::VAOvec[i]);
        
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL::EBOvec[i]);
        glDrawElements(GL_TRIANGLES, cs.triangles.size() * 3, GL_UNSIGNED_INT, nullptr);
        //std::cout << colorLoc << std::endl;
        glUniform3f(colorLoc, 0.f,0.f,0.f);
        glDisable(GL_POLYGON_OFFSET_FILL);
        //glDisable(GL_DEPTH_TEST);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL::lEBOvec[i]);
        glLineWidth(2.0f);
        glDrawElements(GL_LINES, cs.edge.size(), GL_UNSIGNED_INT, nullptr);

        /*float minx= std::min({GL::gCameraDesc[0][0]-GL::gCamera[0],GL::gCameraDesc[1][0]-GL::gCamera[0], GL::gCameraDesc[2][0]-GL::gCamera[0], -GL::gCameraDesc[0][0]-GL::gCamera[0],-GL::gCameraDesc[1][0]-GL::gCamera[0], -GL::gCameraDesc[2][0]-GL::gCamera[0]});
        float maxx= std::max({GL::gCameraDesc[0][0]-GL::gCamera[0],GL::gCameraDesc[1][0]-GL::gCamera[0], GL::gCameraDesc[2][0]-GL::gCamera[0], -GL::gCameraDesc[0][0]-GL::gCamera[0],-GL::gCameraDesc[1][0]-GL::gCamera[0], -GL::gCameraDesc[2][0]-GL::gCamera[0]});
        float minz= std::min({GL::gCameraDesc[0][2]-GL::gCamera[2],GL::gCameraDesc[1][2]-GL::gCamera[2], GL::gCameraDesc[2][2]-GL::gCamera[2], -GL::gCameraDesc[0][2]-GL::gCamera[2],-GL::gCameraDesc[1][2]-GL::gCamera[2], -GL::gCameraDesc[2][2]-GL::gCamera[2]});
        float maxz= std::max({GL::gCameraDesc[0][2]-GL::gCamera[2],GL::gCameraDesc[1][2]-GL::gCamera[2], GL::gCameraDesc[2][2]-GL::gCamera[2], -GL::gCameraDesc[0][2]-GL::gCamera[2],-GL::gCameraDesc[1][2]-GL::gCamera[2], -GL::gCameraDesc[2][2]-GL::gCamera[2]});

        minx = std::roundf(minx * 100);
        maxx = std::roundf(maxx * 100);
        minz = std::roundf(minz * 100);
        maxz = std::roundf(maxz * 100);*/
        //glEnable(GL_DEPTH_TEST);
    }
    }




