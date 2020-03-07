#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "geometry.h"

struct Triangle {
    Vec3f p1;
    Vec3f p2;
    Vec3f p3;

    Triangle(const Vec3f& pt1, const Vec3f& pt2, const Vec3f& pt3) : p1(pt1), p2(pt2), p3(pt3) {}

    bool rayIntersect(Vec3f orig,Vec3f dir) const{
        const float p = 0.01;
        Vec3f edge1, edge2, h, s, q;
        float a, f, u, v;
        edge1 = p2 - p1;
        edge2 = p3 - p1;
        h = cross(dir,edge2);
        a = edge1*h;
        if (a > -p && a < p)
            return false;

        f = 1.0 / a;
        s = orig - p1;
        u = f * (s*h);

        if (u < 0.0 || u > 1.0)
            return false;

        q = cross(s,edge1);
        v = f * (dir*q);
        if (v < 0.0 || u + v > 1.0)
            return false;

        float t = f * (edge2*q);

        if (t > p){
            return true;
        }
        else return false;
    }

};

Vec3f cast_ray(const Vec3f& orig, const Vec3f& dir, const Triangle& tr) {
    if (!tr.rayIntersect(orig,dir)){
        return Vec3f(0.2, 0.7, 0.8); // background color
    }
    return Vec3f(0.4, 0.4, 0.3);
}

void render() {

    const int width = 1024;
    const int height = 768;
    const float fov = M_PI / 3.;
    std::vector<Vec3f> framebuffer(width * height);

    Triangle tr = Triangle(Vec3f(-100,50,-100), Vec3f(0,0,-100), Vec3f(100, 0, -100));

    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            /*float x = (2 * (i + 0.5) / (float)width - 1) * tan(fov / 2.) * width / (float)height;
            float y = -(2 * (j + 0.5) / (float)height - 1) * tan(fov / 2.);*/
            float dir_x = (i + 0.5) - width / 2.;
            float dir_y = -(j + 0.5) + height / 2.;    // this flips the image at the same time
            float dir_z = -height / (2. * tan(fov / 2.));
            Vec3f dir = Vec3f(dir_x, dir_y, dir_z);
            framebuffer[i + j * width] = cast_ray(Vec3f(0, 0, 0), dir,tr);
        }
    }

    std::ofstream ofs; // save the framebuffer to file
    ofs.open("./out.ppm", std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height * width; ++i) {
        Vec3f& c = framebuffer[i];
        float max = std::max(c[0], std::max(c[1], c[2]));
        if (max > 1) c = c * (1. / max);
        for (size_t j = 0; j < 3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

int main() {
    render();
    return 0;
}
