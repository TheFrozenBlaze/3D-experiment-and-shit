#include "3dvfunc.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <utility>
#include <chrono>
#include <thread>

void objIdent::objReader(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error reading file: " << filename << std::endl;
            return;
        }

            std::string line;
            while (std::getline(file, line)) {
                if (line.size() > 1 && line[0] == 'v' && line[1] == ' ') {
                    std::istringstream iss(line);
                    std::string v;
                    float x, y, z;
                    iss >> v >> x >> y >> z;
                    cs.vxc.push_back(x);
                    cs.vyc.push_back(y);
                    cs.vzc.push_back(z);
                }   else if (line.size() > 1 && line[0]=='f' && line[1]==' ') {
                    faceindex += 1; //self explanatory
                    vectorindex = 0;//vertex index actually
                    line.erase(0, 2); // cuts of the 'f' and the ' ' from the beginning
                    cs.fvi.emplace_back(); // next line wouldnt work without this
                    int point = 2; //where the f-function writes its outputs (((2=v, 3=vt, 4=vn))
                    int number = 0;
                    std::string numberstring;//loads the integers here
                    std::istringstream iss(line);
                    for (size_t i = 0; i < line.size(); i++) {
                        if(line[i] == ' ' || i == line.size() - 1) { //where to write
                            if (i == line.size() - 1 && line[i] != ' ') {//end of line write
                                numberstring.push_back(line[i]);
                            }
                            if(point == 2){
                                number = std::stoi(numberstring); // check for the v v v format
                                if (vectorindex >= cs.fvi[faceindex-1].ver.size()) {
                                    cs.fvi[faceindex-1].ver.push_back({0,0,0});
                                }
                                cs.fvi[faceindex-1].ver[vectorindex].v = number;
                                cs.fvi[faceindex-1].ver[vectorindex].vt = -1;
                                cs.fvi[faceindex-1].ver[vectorindex].vn = -1;
                                numberstring.clear();
                                number = 0;
                                vectorindex++;
                                point = 2;
                                continue;
                            } else if(point==3){
                                number = std::stoi(numberstring); // checks for v/vt format
                                if (vectorindex >= cs.fvi[faceindex-1].ver.size()) {
                                    cs.fvi[faceindex-1].ver.push_back({0,0,0});
                                }
                                cs.fvi[faceindex-1].ver[vectorindex].vt = number;
                                cs.fvi[faceindex-1].ver[vectorindex].vn = -1;
                                number = 0;
                                numberstring.clear();
                                vectorindex++;
                                point = 2;

                                continue;
                            }else{
                                number = std::stoi(numberstring); //checks for vn
                                if (vectorindex >= cs.fvi[faceindex-1].ver.size()) {
                                    cs.fvi[faceindex-1].ver.push_back({0,0,0});
                                }
                                cs.fvi[faceindex-1].ver[vectorindex].vn = number;
                                vectorindex++;
                                numberstring.clear();
                                number = 0;
                                point = 2;
                            }
                            if (i != line.size() - 1 && line[i] == ' ' ) {
                                cs.fvi.back().ver.push_back({0,0,0});
                            }
                            continue;
                        } else {
                        if (i > 0 && line[i] == '/' && line[i-1]== '/') { //check for v//vn format
                            cs.fvi[faceindex-1].ver[vectorindex].vt = -1;
                            point++;
                            continue;

                        } else if  (line[i] == '/') { //the standard v/vt/vn format
                            if(point == 2){
                                number = std::stoi(numberstring);
                                if (vectorindex >= cs.fvi[faceindex-1].ver.size()) {
                                    cs.fvi.back().ver.push_back({0,0,0});
                                }
                                cs.fvi[faceindex-1].ver[vectorindex].v = number;
                                point++;
                                numberstring.clear();
                                number = 0;
                                continue;
                            } else {
                                number = std::stoi(numberstring);
                                cs.fvi[faceindex-1].ver[vectorindex].vt = number;
                                number = 0;
                                numberstring.clear();
                                point++;
                                continue;
                            }
                        } else {
                            numberstring.push_back(line[i]);
                        }
                    }
                }
            } else if (line.size() > 1 && line[0] == 'v' && line[1] == 'n' && line[2] == ' ') { //vn
                std::istringstream iss(line);
                std::string v;
                float x, y, z;
                iss >> v >> x >> y >> z;
                cs.vnxc.push_back(x);
                cs.vnyc.push_back(y);
                cs.vnzc.push_back(z);
            } else {
                continue;
            }
        }

    };
    void objIdent::Triangulator() {
        float limit = 1e-6; //basicaly zero
        std::array<float, 3> normal = {0.0, 0.0, 0.0};
        uint8_t projection = 0;
        for(size_t i = 0; i < cs.fvi.size(); i++) {
            if(cs.fvi[i].ver.size() > 3) {
                /*std::cout << "\nFace " << i
          << " vertex count: " << cs.fvi[i].ver.size() << std::endl;*/
                normal = { 0.0, 0.0 , 0.0}; 
                float x, y, z, coordy, coordx, coordz;
                int f = 1;
                while(normal == std::array<float, 3>{0.0f, 0.0f, 0.0f}) { //calculatin the normal of the face based on it's vertices, if the normal is 0, then the face is degenerate and we will try to calculate the normal with different vertices until we get a non degenerate normal or we run out of vertices to try
                    if(f+1 == cs.fvi[i].ver.size()) {
                        std::cout << "INVALID FACE" << std::endl;
                        break;  
                    } else {
                    coordx = (cs.vyc[cs.fvi[i].ver[f].v-1]-cs.vyc[cs.fvi[i].ver[0].v-1])*(cs.vzc[cs.fvi[i].ver[1+f].v-1]-cs.vzc[cs.fvi[i].ver[0].v-1])-(cs.vyc[cs.fvi[i].ver[1+f].v-1]-cs.vyc[cs.fvi[i].ver[0].v-1])*(cs.vzc[cs.fvi[i].ver[0+f].v-1]-cs.vzc[cs.fvi[i].ver[0].v-1]); 
                    coordy = (cs.vzc[cs.fvi[i].ver[f].v-1]-cs.vzc[cs.fvi[i].ver[0].v-1])*(cs.vxc[cs.fvi[i].ver[1+f].v-1]-cs.vxc[cs.fvi[i].ver[0].v-1])-(cs.vzc[cs.fvi[i].ver[1+f].v-1]-cs.vzc[cs.fvi[i].ver[0].v-1])*(cs.vxc[cs.fvi[i].ver[0+f].v-1]-cs.vxc[cs.fvi[i].ver[0].v-1]);
                    coordz = (cs.vxc[cs.fvi[i].ver[f].v-1]-cs.vxc[cs.fvi[i].ver[0].v-1])*(cs.vyc[cs.fvi[i].ver[1+f].v-1]-cs.vyc[cs.fvi[i].ver[0].v-1])-(cs.vxc[cs.fvi[i].ver[1+f].v-1]-cs.vxc[cs.fvi[i].ver[0].v-1])*(cs.vyc[cs.fvi[i].ver[0+f].v-1]-cs.vyc[cs.fvi[i].ver[0].v-1]);
                    x = limit > std::fabs(coordx) && std::fabs(coordx) >= 0 ? 0.0f : coordx; 
                    y = limit > std::fabs(coordy) && std::fabs(coordy) >= 0 ? 0.0f : coordy;
                    z = limit > std::fabs(coordz) && std::fabs(coordz) >= 0 ? 0.0f : coordz;
                    normal = {
                        x, y, z
                    };
                        f++;
                    } 
                }
                /*std::cout << "Normal" << normal[0] << " Normal0" <<  normal[1] << " Normal1" << normal[2] << "Normal2" << std::endl;*/    
                f = 1;
                float min =std::fabs(*std::min_element(normal.begin(), normal.end())), max = std::fabs(*std::max_element(normal.begin(), normal.end()));//the absolute biggest element defines, which way would the projection distort the least, because of perpendicularity
                //if a normal is more parallel with the, let's say, x, then the projection on the yz plane would be distorted the least => eliminates the risk of wrong detection
                int8_t dor = limit >= max-min && max-min >= 0 ? 0 : (max-min > 0 ? 1 : -1);
                projection = dor == 0 ? std::distance(normal.begin(), std::max_element(normal.begin(), normal.end())) : dor > 0 ? std::distance(normal.begin(), std::max_element(normal.begin(), normal.end())) : std::distance(normal.begin(), std::min_element(normal.begin(), normal.end()));// if the absolutes of the min and max are equal, then it's just easier to choose max
                /*std::cout << "projection" << (int)projection << std::endl;*/
                std::vector<int> vindices;
                uint32_t v0, v1, v2;
                size_t k = 0;
                switch(projection) {
                    case 0:
                    if(x > 0) { //we are ordering here based on winding. The normal tells us, which way are the vertices ordered
                        for(int j = 0; j < cs.fvi[i].ver.size(); j++) {
                    vindices.emplace_back(j);
                    }
                    } else {
                        for(int j = 0; j < cs.fvi[i].ver.size(); j++) {
                    vindices.emplace_back(cs.fvi[i].ver.size()-1-j);
                    }
                    }
                    /*std::cout << "Initial winding: ";
                    for (auto v : vindices) std::cout << v << " ";
                    std::cout << std::endl;*/
                        while(vindices.size() >= 3) {
                            if(k == vindices.size())  {
                                k = 0;
                            }   size_t r;
                                v0 = cs.fvi[i].ver[vindices[k]].v-1;
                                v1 = cs.fvi[i].ver[vindices[(k+1) % vindices.size()]].v-1;
                                v2 = v2 = cs.fvi[i].ver[vindices[(k+2) % vindices.size()]].v - 1;//(k+2) % vindices.size() is good, because it allws us to loop around, because this gives us the modulo, and at k+2 = vindices.size() it becomes 0, etc.
                                //we are chhosing the points of the potential triangle
                                std::array<float, 2>rv1 = {cs.vyc[v1] - cs.vyc[v0],cs.vzc[v1] - cs.vzc[v0]};
                                std::array<float, 2>rv2 = {cs.vyc[v2] - cs.vyc[v0],cs.vzc[v2] - cs.vzc[v0]};
                                bool reflexcheck = rv1[0] * rv2[1] - rv1[1] * rv2[0] <= 0 ? true : false;//concaveness => this sadly currently makes the algoritm O(n^3), and could be implemented in the for loop

                                //NOTES FOR MYSELF: CHECK ON THIS LATER(INCLUDE REFLEX IN FOR LOOP)

                                /*std::cout << "k=" << k
                                << " size=" << vindices.size()
                                << " v0=" << v0
                                << " v1=" << v1
                                << " v2=" << v2 << std::endl;
                                std::this_thread::sleep_for(std::chrono::milliseconds(500));*/
                                if(reflexcheck == true) {
                                    k++;
                                    //std::cout << "reflexcheck: " << reflexcheck << std::endl;
                                    continue;
                                } else {
                                    //std::cout << "reflexcheck: " << reflexcheck << std::endl;
                                    //std::cout << "I have entered else" << std::endl;
                                    bool removed =false;
                                    bool valid = true;
                                    size_t before = vindices.size();
                                    for(size_t step = 0; step < vindices.size(); step++) { 
                                            size_t l = (k + 3 + step) % vindices.size();
                                            std::array<float, 2> p = {cs.vyc[cs.fvi[i].ver[vindices[l]].v-1], cs.vzc[cs.fvi[i].ver[vindices[l]].v-1]};//the vertex of the polygon
                                            std::array<float, 2> tp1 = {cs.vyc[v0], cs.vzc[v0]};
                                            std::array<float, 2> tp2 = {cs.vyc[v1], cs.vzc[v1]};
                                            std::array<float, 2> tp3 = {cs.vyc[v2], cs.vzc[v2]};
                                            struct CPpair {
                                                float ad = 0;
                                                float bc = 0;
                                                float product = ad - bc;
                                            } ABP, ACP, BCP;
                                            ABP.ad = (tp2[0] - tp1[0]) * (p[1] - tp1[1]);
                                            ABP.bc = (tp2[1] - tp1[1]) * (p[0] - tp1[0]);
                                            ACP.ad = (tp1[0] - tp3[0]) * (p[1] - tp3[1]); 
                                            ACP.bc = (tp1[1] - tp3[1]) * (p[0] - tp3[0]);
                                            BCP.ad = (tp3[0] - tp2[0]) * (p[1] - tp2[1]);
                                            BCP.bc = (tp3[1] - tp2[1]) * (p[0] - tp2[0]);
                                            ABP.product = ABP.ad - ABP.bc;
                                            ACP.product = ACP.ad - ACP.bc;
                                            BCP.product = BCP.ad - BCP.bc;//we are calculating the normals of the triangle's sides with an another polygon vertex(obv. we are taking each side and the current vertex to it),
                                            // we are taking the normal relative to a vertex of the potential triangle , and rotate through the triangle in ccw
                                            valid = (ABP.product > limit) && (ACP.product > limit) && (BCP.product > limit); //because of ccw, if all of the normals are positive, then the point is inside, if one of them is negative, then it's outside, if it's 0, then it's on the edge, but we will consider it as outside, because of precision issues
                                        if(valid == true) {// we exclude go step 1 forward in potential triangle vertices, because the last 1 was invalid
                                                //std::cout<< "face invalid" << std::endl;
                                                k++;
                                                break;
                                            }
                                        }
                                        if (valid == false) {
                                            //std::cout << "v1+1: " << v1+1 << std::endl;
                                            //std::cout << "k+1 shit: " << cs.fvi[i].ver[vindices[(k+1) % vindices.size()]].v << std::endl;
                                            cs.triangles.emplace_back(std::array<long unsigned int, 3>{v0, v1, v2}); //we add this because of indexing later at drawing the object
                                            vindices.erase(vindices.begin() + (k+1) % vindices.size());//we exclude the used, v1 vertex, because that's the point of ear clipping, simplifying the polygon
                                            /*std::cout<< "face valid" << std::endl;
                                            std::cout << "I have removed: " << v1+1 << std::endl;
                                            std::cout << "Triangle "<<cs.triangles[cs.triangles.size()-1][0] << " " << cs.triangles[cs.triangles.size()-1][1] << " "<< cs.triangles[cs.triangles.size()-1][2]<< std::endl;*/
                                            removed = true;
                                            k = 0;
                                        }
                                        
                                       /*if (!removed) {
                                            
                                            std::cout << "INVALID FACE\n";
                                            break;
                                        } */ // Fix this later
                                        if (vindices.size() == before) { //invalid face, prob. not coplanar or smth
                                            std::cout << "NO PROGRESS THIS ITERATION\n";
                                        }
                                    }                                   
                                }
                    break;
                    case 1:
                    if(y < 0) {
                        for(int j = 0; j < cs.fvi[i].ver.size(); j++) {
                    vindices.emplace_back(j);
                    }
                    } else {
                        for(int j = 0; j < cs.fvi[i].ver.size(); j++) {
                    vindices.emplace_back(cs.fvi[i].ver.size()-1-j);
                    }
                    }
                    /*std::cout << "Initial winding: ";
                    for (auto v : vindices) std::cout << v << " ";
                    std::cout << std::endl;*/

                        while(vindices.size() >= 3) {
                            if(k == vindices.size())  {
                                k = 0;
                            }   size_t r;
                                v0 = cs.fvi[i].ver[vindices[k]].v-1;
                                v1 = cs.fvi[i].ver[vindices[(k+1) % vindices.size()]].v-1;
                                v2 = v2 = cs.fvi[i].ver[vindices[(k+2) % vindices.size()]].v - 1;
                                
                                std::array<float, 2>rv1 = {cs.vxc[v1] - cs.vxc[v0],cs.vzc[v1] - cs.vzc[v0]};
                                std::array<float, 2>rv2 = {cs.vxc[v2] - cs.vxc[v0],cs.vzc[v2] - cs.vzc[v0]};
                                bool reflexcheck = rv1[0] * rv2[1] - rv1[1] * rv2[0] <= 0 ? true : false;
                                /*std::cout << "k=" << k
                                << " size=" << vindices.size()
                                << " v0=" << v0
                                << " v1=" << v1
                                << " v2=" << v2 << std::endl;
                                std::this_thread::sleep_for(std::chrono::milliseconds(500));*/
                                if(reflexcheck == true) {
                                    k++;
                                    /*std::cout << "reflexcheck: " << reflexcheck << std::endl;*/
                                    continue;
                                } else {
                                    /*std::cout << "reflexcheck: " << reflexcheck << std::endl;
                                    std::cout << "I have entered else" << std::endl;*/
                                    bool removed =false;
                                    bool valid = true;
                                    size_t before = vindices.size();
                                    for(size_t step = 0; step < vindices.size(); step++) { 
                                            size_t l = (k + 3 + step) % vindices.size();
                                            std::array<float, 2> p = {cs.vxc[cs.fvi[i].ver[vindices[l]].v-1], cs.vzc[cs.fvi[i].ver[vindices[l]].v-1]};
                                            std::array<float, 2> tp1 = {cs.vxc[v0], cs.vzc[v0]};
                                            std::array<float, 2> tp2 = {cs.vxc[v1], cs.vzc[v1]};
                                            std::array<float, 2> tp3 = {cs.vxc[v2], cs.vzc[v2]};
                                            struct CPpair {
                                                float ad = 0;
                                                float bc = 0;
                                                float product = ad - bc;
                                            } ABP, ACP, BCP;
                                            ABP.ad = (tp2[0] - tp1[0]) * (p[1] - tp1[1]);
                                            ABP.bc = (tp2[1] - tp1[1]) * (p[0] - tp1[0]);
                                            ACP.ad = (tp1[0] - tp3[0]) * (p[1] - tp3[1]); 
                                            ACP.bc = (tp1[1] - tp3[1]) * (p[0] - tp3[0]);
                                            BCP.ad = (tp3[0] - tp2[0]) * (p[1] - tp2[1]);
                                            BCP.bc = (tp3[1] - tp2[1]) * (p[0] - tp2[0]);
                                            ABP.product = ABP.ad - ABP.bc;
                                            ACP.product = ACP.ad - ACP.bc;
                                            BCP.product = BCP.ad - BCP.bc;
                                            valid = (ABP.product > limit) && (ACP.product > limit) && (BCP.product > limit);
                                        if(valid == true) {
                                                //std::cout<< "face invalid" << std::endl;
                                                k++;
                                                break;
                                            }
                                        }
                                        if (valid == false) {
                                            //std::cout << "v1+1: " << v1+1 << std::endl;
                                            //std::cout << "k+1 shit: " << cs.fvi[i].ver[vindices[(k+1) % vindices.size()]].v << std::endl;
                                            cs.triangles.emplace_back(std::array<long unsigned int, 3>{v0, v1, v2});
                                            vindices.erase(vindices.begin() + (k+1) % vindices.size());
                                            //std::cout<< "face valid" << std::endl;
                                            //std::cout << "I have removed: " << v1+1 << std::endl;
                                            //std::cout << "Triangle "<<cs.triangles[cs.triangles.size()-1][0] << " " << cs.triangles[cs.triangles.size()-1][1] << " "<< cs.triangles[cs.triangles.size()-1][2]<< std::endl;
                                            removed = true;
                                            k = 0;
                                        }
                                        
                                       /*if (!removed) {
                                            
                                            std::cout << "INVALID FACE\n";
                                            break;
                                        } */ // Fix this later
                                        if (vindices.size() == before) {
                                            std::cout << "NO PROGRESS THIS ITERATION\n";
                                        }
                                    }                                   
                                }
                    break;
                    case 2:
                    if(z > 0) {
                        for(int j = 0; j < cs.fvi[i].ver.size(); j++) {
                    vindices.emplace_back(j);
                    }
                    } else {
                        for(int j = 0; j < cs.fvi[i].ver.size(); j++) {
                    vindices.emplace_back(cs.fvi[i].ver.size()-1-j);
                    }
                    }
                    /*std::cout << "Initial winding: ";
                    for (auto v : vindices) std::cout << v << " ";
                    std::cout << std::endl;*/
                        while(vindices.size() >= 3) {
                            if(k == vindices.size())  {
                                k = 0;
                            }   size_t r;
                                v0 = cs.fvi[i].ver[vindices[k]].v-1;
                                v1 = cs.fvi[i].ver[vindices[(k+1) % vindices.size()]].v-1;
                                v2 = cs.fvi[i].ver[vindices[(k+2) % vindices.size()]].v - 1;
                                
                                std::array<float, 2>rv1 = {cs.vxc[v1] - cs.vxc[v0],cs.vyc[v1] - cs.vyc[v0]};
                                std::array<float, 2>rv2 = {cs.vxc[v2] - cs.vxc[v0],cs.vyc[v2] - cs.vyc[v0]};
                                bool reflexcheck = rv1[0] * rv2[1] - rv1[1] * rv2[0] <= 0 ? true : false;
                                /*std::cout << "k=" << k
                                << " size=" << vindices.size()
                                << " v0=" << v0
                                << " v1=" << v1
                                << " v2=" << v2 << std::endl;*/ 
                                //std::this_thread::sleep_for(std::chrono::milliseconds(500));
                                if(reflexcheck == true) {
                                    k++;
                                    //std::cout << "reflexcheck: " << reflexcheck << std::endl;
                                    continue;
                                } else {
                                    //std::cout << "reflexcheck: " << reflexcheck << std::endl;
                                    //std::cout << "I have entered else" << std::endl;
                                    bool removed =false;
                                    bool valid = true;
                                    size_t before = vindices.size();
                                    for(size_t step = 0; step < vindices.size(); step++) { 
                                            size_t l = (k + 3 + step) % vindices.size();
                                            std::array<float, 2> p = {cs.vxc[cs.fvi[i].ver[vindices[l]].v-1], cs.vyc[cs.fvi[i].ver[vindices[l]].v-1]};
                                            std::array<float, 2> tp1 = {cs.vxc[v0], cs.vyc[v0]};
                                            std::array<float, 2> tp2 = {cs.vxc[v1], cs.vyc[v1]};
                                            std::array<float, 2> tp3 = {cs.vxc[v2], cs.vyc[v2]};
                                            struct CPpair {
                                                float ad = 0;
                                                float bc = 0;
                                                float product = ad - bc;
                                            } ABP, ACP, BCP;
                                            ABP.ad = (tp2[0] - tp1[0]) * (p[1] - tp1[1]);
                                            ABP.bc = (tp2[1] - tp1[1]) * (p[0] - tp1[0]);
                                            ACP.ad = (tp1[0] - tp3[0]) * (p[1] - tp3[1]); 
                                            ACP.bc = (tp1[1] - tp3[1]) * (p[0] - tp3[0]);
                                            BCP.ad = (tp3[0] - tp2[0]) * (p[1] - tp2[1]);
                                            BCP.bc = (tp3[1] - tp2[1]) * (p[0] - tp2[0]);
                                            ABP.product = ABP.ad - ABP.bc;
                                            ACP.product = ACP.ad - ACP.bc;
                                            BCP.product = BCP.ad - BCP.bc;
                                            valid = (ABP.product > limit) && (ACP.product > limit) && (BCP.product > limit);
                                        if(valid == true) {
                                                //std::cout<< "face invalid" << std::endl;
                                                k++;
                                                break;
                                            }
                                        }
                                        if (valid == false) {
                                            /*std::cout << "v1+1: " << v1+1 << std::endl;
                                            std::cout << "k+1 shit: " << cs.fvi[i].ver[vindices[(k+1) % vindices.size()]].v << std::endl;*/
                                            cs.triangles.emplace_back(std::array<long unsigned int, 3>{v0, v1, v2});
                                            vindices.erase(vindices.begin() + (k+1) % vindices.size());
                                            //std::cout<< "face valid" << std::endl;
                                            //std::cout << "I have removed: " << v1+1 << std::endl;
                                            //std::cout << "Triangle "<<cs.triangles[cs.triangles.size()-1][0] << " " << cs.triangles[cs.triangles.size()-1][1] << " "<< cs.triangles[cs.triangles.size()-1][2]<< std::endl;                                            removed = true;
                                            k = 0;
                                        }
                                        
                                       /*if (!removed) {
                                            
                                            std::cout << "INVALID FACE\n";
                                            break;
                                        } */ // Fix this later
                                        if (vindices.size() == before) {
                                            std::cout << "NO PROGRESS THIS ITERATION\n";
                                        }
                                    }                                   
                                }
                    break;
                
                }
            } else {
                cs.triangles.emplace_back(std::array<long unsigned int, 3>{cs.fvi[i].ver[0].v, cs.fvi[i].ver[1].v, cs.fvi[i].ver[2].v}); // we pass on shapes, that are triangles
                continue;
            }
        }  
    }
                
int main() {
    objIdent obj;

    obj.objReader("/home/matyas/Downloads/humanoid_quad.obj");
    for(size_t i = 0; i < obj.cs.fvi.size(); i++) {
        for(size_t j =0; j < obj.cs.fvi[i].ver.size(); j++) {
            std::cout << obj.cs.fvi[i].ver[j].v << std::endl;
        }
    }
    obj.Triangulator();
    for(size_t i =0; i< obj.cs.triangles.size(); i++) {
        std::cout << "Triangle: "<< obj.cs.triangles[i][0] << " " << obj.cs.triangles[i][1] << " " << obj.cs.triangles[i][2] << std::endl;
    }
}