#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

// Include using libs
#include <windows.h>
#include <stdio.h>
#include <cmath>

// Define macroses
#define ABS(a) ( (a) < 0 ? -(a) : (a) )
#define MIN(a, b) ( (a) < (b) ? (a) : (b) )
#define MAX(a, b) ( (a) > (b) ? (a) : (b) )
#define RAD(a) (a * 0.01745329251994329577)

// Define using types
typedef unsigned char BYTE;
typedef struct RGB
{
    BYTE R, G, B;
};
typedef struct BGR4
{
    BYTE B, G, R, A;
};
typedef struct VECT2
{
    float x, y;
};
typedef struct VECT3
{
    float x, y, z;
};
typedef struct VECT4
{
    float x, y, z, t;
};
typedef struct EDGE
{
    int begin, end;
};

class FIGURE_4D
{
public:
    VECT4* vertexes4;
    VECT3* vertexes3;
    VECT2* vertexes2;
    EDGE* edges;
    int num_vert;
    int num_edg;

    FIGURE_4D(const char* fname);
    ~FIGURE_4D();

    void rotate_3d(float angle_YZ, float angle_XZ, float angle_XY, float x0, float y0, float z0);
    void rotate_4d(float angle_XT, float angle_YT, float angle_ZT, float x0, float y0, float z0, float t0);
    void translate(float dx, float dy, float dz, float dt);
};

class FRAME
{
private:
    BGR4* buffer = nullptr;
    HWND hwnd = nullptr;
    HDC hdc = nullptr;
    HDC tmpDc = nullptr;
    HBITMAP hbm = nullptr;
    short width;
    short height;

public:
    RGB pen_color;
    float camera_z, camera_t;

    FRAME(short frameWidth, short frameHeight, HWND frameHwnd);
    ~FRAME();

    void clear(RGB color = { 255, 255, 255 });
    void set_circle(short x0, short y0, short R);
    void set_pixel(short x, short y);
    void set_line(short x1, short y1, short x2, short y2);
    void print();
    VECT2 vect3_to_vect2(VECT3 vect3);
    VECT3 vect4_to_vect3(VECT4 vect4);
    void set_figure(FIGURE_4D& figure);
};

// -+-+-+-+-+-+-+-+-+-+- GLOBAL FUNC -+-+-+-+-+-+-+-+-+-+-
void vect4_rotate_3d(VECT4& vect4, float angle_YZ, float angle_XZ, float angle_XY, float x0, float y0, float z0);
void vect4_rotate_4d(VECT4& vect4, float angle_XT, float angle_YT, float angle_ZT, float x0, float y0, float z0, float t0);
void vect4_translate(VECT4& vect4, float dx, float dy, float dz, float dt);

#endif
