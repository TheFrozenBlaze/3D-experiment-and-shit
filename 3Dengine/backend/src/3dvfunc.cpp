#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdint>


struct Coord {
public:
    //endpoint
    std::vector<float> xcoords;
    std::vector<float> ycoords;
    std::vector<float> zcoords;
    //startpoint
    std::vector<float> stpxcoords;
    std::vector<float> stpycoords;
    std::vector<float> stpzcoords;
    // length
    std::vector<float> xlen;
    std::vector<float> ylen;
    std::vector<float> zlen;
    // vertices
    std::vector<float> vxc;
    std::vector<float> vyc;
    std::vector<float> vzc;
    //vertex normals
    std::vector<float> vnxc;
    std::vector<float> vnyc;
    std::vector<float> vnzc;
    // transformed vertices
    std::vector<float> tvxc;
    std::vector<float> tvyc;
    std::vector<float> tvzc;
    //usable faces(that could be intersected)
    std::vector<int> usable;


    struct Face {
        struct Vertex { int v, vt, vn; };
        std::vector<Face::Vertex> ver;
    };

    std::vector<Face> fvi;
};

class Vector {
public:
    Coord cs;
    float xl = 0.0f;
    float yl = 0.0f;
    float zl = 0.0f;
    float a = 0.0f;
    float b = 0.0f;
    float c = 0.0f;
    float m = 0.0f; // steepness
    float cc = 0.0f; // function crossing the given axis

    enum class crdsysdim {
        XY,
        XZ,
        ZY
    };

    // Calculate vector length
    float lengthVector(float x, float y, float z, float stpx, float stpy, float stpz) {
        xl = std::fabs(stpx - x);
        cs.xlen.push_back(xl);

        yl = std::fabs(stpy - y);
        cs.ylen.push_back(yl);

        zl = std::fabs(stpz - z);
        cs.zlen.push_back(zl);

        float xzd = std::sqrt(xl * xl + zl * zl);
        float ydxz = std::sqrt(xzd * xzd + yl * yl);

        return ydxz;
    }

    // Convert vector line to function form
    /*void funcVector(int num, std::string level) {
        crdsysdim state;
        std::transform(level.begin(), level.end(), level.begin(), [](unsigned char c) { return std::toupper(c); });
        if (level == "XY") {state = crdsysdim::XY;}
        else if (level == "XZ") {state = crdsysdim::XZ;}
        else {state = crdsysdim::ZY;}

        float stpdom = 0.0f;
        float dx = 0.0f;
        float dy = 0.0f;

        switch (state) {
            case crdsysdim::XY:
                dx = cs.stpxcoords[num] - cs.xcoords[num];
                dy = cs.stpycoords[num] - cs.ycoords[num];
                stpdom = cs.stpycoords[num];
                nondom = cs.stpxcoords[num];
                break;
            case crdsysdim::XZ:
                dx = cs.stpxcoords[num] - cs.xcoords[num];
                dy = cs.stpzcoords[num] - cs.zcoords[num];
                stpdom = cs.stpzcoords[num];
                nondom = cs.stpxcoords[num];
                break;
            case crdsysdim::ZY:
                dx = cs.stpzcoords[num] - cs.zcoords[num];
                dy = cs.stpycoords[num] - cs.ycoords[num];
                stpdom = cs.stpycoords[num];
                nondom = cs.stpzcoords[num];
                break;
        }


        if (dx == 0.0f) {
            m = INFINITY;
        } else {
            m = dy / dx;
        }

        float cc = stpdom-m*nondom;

    }*/
    /*correction:my solution was after fixes: double degree = std::atan(dy/dx); float cc= stpdom - std::tan(degree)*nondom; but chatgpt made me realize, that tan(degree) is just 'm', and I don't have to deal with division by zero and trig...(mentioned, because in this challenge ai isn't allowed to make my code, at max give me hints) - this will be later repurposed and refined, but this is one of my greatest creations for now, and I want to preserve it in its almost original form in its original place*/

 };

class objIdent {
public:
    Vector vec;
    Coord cs;

    int vectorindex = 0;
    int faceindex = 0;

    // Read .obj file
    void objReader(const std::string& filename) {
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
                    cs.fvi.emplace_back();
                    cs.fvi.back().ver.push_back({0,0,0});//creates space
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
                                cs.fvi[faceindex-1].ver[vectorindex].vt = number;
                                cs.fvi[faceindex-1].ver[vectorindex].vn = -1;
                                number = 0;
                                numberstring.clear();
                                vectorindex++;
                                point = 2;
                                continue;
                            }else{
                                number = std::stoi(numberstring); //checks for vn
                                cs.fvi[faceindex-1].ver[vectorindex].vn = number;
                                vectorindex++;
                                numberstring.clear();
                                number = 0;
                                point = 2;
                            }
                            if (i != line.size() - 1 && line[i] == ' ' ) {
                                cs.fvi.emplace_back();
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

    // Check vector intersection
    /*bool vectorCheck(const std::string& level, int vecnum) {
        Vector::crdsysdim sst;
         std::transform(level.begin(), level.end(), level.begin(), [](unsigned char c) { return std::toupper(c); });
        if (level == "XY") {sst = Vector::crdsysdim::XY;
        }
        else if (level == "XZ")
        {sst = Vector::crdsysdim::XZ;
        }
        else {sst = Vector::crdsysdim::ZY;}


        float check;
        int above = 0;
        int below = 0;

        vec.funcVector(vecnum, level);

        for (size_t i = 0; i < cs.vxc.size(); i++) {
            switch (sst) {
                case Vector::crdsysdim::XY:
                    check = (cs.vxc[i] * vec.m) + vec.cc;
                    if (check == cs.vyc[i]) return true;
                    else if (check > cs.vyc[i]) above++;
                    else below++;
                    break;


                case Vector::crdsysdim::XZ:
                    check = (cs.vxc[i] * vec.m) + vec.cc;
                    if (check == cs.vzc[i]) return true;
                    else if (check > cs.vzc[i]) above++;
                    else below++;
                    break;

                case Vector::crdsysdim::ZY:
                    check = (cs.vzc[i] * vec.m) + vec.cc;
                    if (check == cs.vyc[i]) return true;
                    else if (check > cs.vyc[i]) above++;
                    else below++;
                    break;
            }

            if (above > 0 && below > 0)
                return true;
        }
        return false;
    }*/

    /*this would be good if this was either 2d, or 3d with simple objects like cubes or whatever*/

    void AABB(uint32_t vecnum) {


        for(size_t i = 0; i < cs.fvi.size(); i++) {
            std::vector<float> temporaryx;
            std::vector<float> temporaryz;
            std::vector<float> temporaryy;
            for(size_t j = 0; j < cs.fvi[i].ver.size(); j++) {
                temporaryx.emplace_back(cs.vxc[cs.fvi[i].ver[j].v]-1);
                temporaryy.emplace_back(cs.vyc[cs.fvi[i].ver[j].v]-1);
                temporaryz.emplace_back(cs.vzc[cs.fvi[i].ver[j].v]-1);
            }
            auto mmx = std::minmax_element(temporaryx.begin(), temporaryx.end());
            auto mmy = std::minmax_element(temporaryy.begin(), temporaryy.end());
            auto mmz = std::minmax_element(temporaryz.begin(), temporaryz.end());
            float temp;
            float xmin = *mmx.first, xmax = *mmx.second;
            float ymin = *mmy.first, ymax = *mmy.second;
            float zmin = *mmz.first, zmax = *mmz.second;

            float vxmin = std::min(cs.xcoords[vecnum], cs.stpxcoords[vecnum]);
            float vxmax = std::max(cs.xcoords[vecnum], cs.stpxcoords[vecnum]);
            float vymin = std::min(cs.ycoords[vecnum], cs.stpycoords[vecnum]);
            float vymax = std::max(cs.ycoords[vecnum], cs.stpycoords[vecnum]);
            float vzmin = std::min(cs.zcoords[vecnum], cs.stpzcoords[vecnum]);
            float vzmax = std::max(cs.zcoords[vecnum], cs.stpzcoords[vecnum]);

            if (xmax < vxmin || xmin > vxmax) continue;
            if (ymax < vymin || ymin > vymax) continue;
            if (zmax < vzmin || zmin > vzmax) continue;

            cs.usable.push_back(i);
        }
    };
};

//ONLY FOR RENDERED OBJECTS
class Transformation {
public:
    Coord cs;
    void linearTrans(int lane, float length) {
        enum class dimlane {
            X,
            Y,
            Z
        };
        dimlane dimension;
        if (lane==1) {dimension=dimlane::X;}
        else if (lane==2) {dimension=dimlane::Y;}
        else {dimension=dimlane::Z;}

	//applies the same transition in one dimension on every single vertex
        switch (dimension) {
            case dimlane::X:
                for(size_t i = 0; i < cs.vxc.size(); i++) {
                    float trans = cs.vxc[i] + length;
                    cs.tvxc.push_back(trans);
                }
                cs.vxc.swap(cs.tvxc);
                cs.tvxc.clear();
                break;

            case dimlane::Y:
                for(size_t i = 0; i < cs.vyc.size(); i++) {
                    float trans = cs.vyc[i] + length;
                    cs.tvyc.push_back(trans);
                }
                cs.vyc.swap(cs.tvyc);
                cs.tvyc.clear();
                break;

            case dimlane::Z:
                for(size_t i = 0; i < cs.vzc.size(); i++) {
                    float trans = cs.vzc[i] + length;
                    cs.tvzc.push_back(trans);
                }
                cs.vzc.swap(cs.tvzc);
                cs.tvzc.clear();
                break;
        }
    };
    //rotates the object with 2D matrices(just like freecad)
    void turnTrans(int type, int degree) {
        enum class axis {
            XY,
            YZ,
            ZX
        };
        axis chosen;
        if (type==1) {chosen=axis::XY;}
        else if (type==2) {chosen=axis::YZ;}
        else {chosen=axis::ZX;}
        float Radian = static_cast<float>(degree * M_PI / 180);
	float vertical = std::sin(Radian);
	float horizontal = std::cos(Radian);

	switch (chosen) {
            case axis::XY:
                    for(size_t i =0; i < cs.vxc.size(); i++) {
                    float transvert = cs.vyc[i]*horizontal + cs.vxc[i]*vertical;
                    float transhor = cs.vyc[i]*vertical*(-1) + cs.vxc[i]*horizontal;
                    cs.tvxc.push_back(transhor);
                    cs.tvyc.push_back(transvert);
                    }
                    cs.vxc.swap(cs.tvxc);
                    cs.tvxc.clear();
                    cs.vyc.swap(cs.tvyc);
                    cs.tvyc.clear();


                break;

            case axis::YZ:
                for(size_t i =0; i < cs.vyc.size(); i++) {
                    float transvert = cs.vzc[i]*horizontal + cs.vyc[i]*vertical;
                    float transhor = cs.vzc[i]*vertical*(-1) + cs.vyc[i]*horizontal;
                    cs.tvyc.push_back(transhor);
                    cs.tvzc.push_back(transvert);
                    }
                    cs.vzc.swap(cs.tvzc);
                    cs.tvzc.clear();
                    cs.vyc.swap(cs.tvyc);
                    cs.tvyc.clear();
                break;

            case axis::ZX:
                for(size_t i =0; i < cs.vxc.size(); i++) {
                    float transvert = cs.vxc[i]*horizontal + cs.vzc[i]*vertical;
                    float transhor = cs.vxc[i]*vertical*(-1) + cs.vzc[i]*horizontal;
                    cs.tvxc.push_back(transhor);
                    cs.tvzc.push_back(transvert);
                    }
                    cs.vxc.swap(cs.tvxc);
                    cs.tvxc.clear();
                    cs.vzc.swap(cs.tvzc);
                    cs.tvzc.clear();
                break;
            }
        }
    };
};


/*float transvert = cs.vyc[i]*horizontal + cs.vyc[i]*vertical;
 float transhor = cs.vxc[i]*vertical*(-1) + cs.vxc[i]*horizontal;*/
