#include "graph.h"

// -+-+-+-+-+-+-+-+-+-+- FRAME -+-+-+-+-+-+-+-+-+-+-
FRAME::FRAME(short frameWidth, short frameHeight, HWND frameHwnd)
{
    width = frameWidth;
    height = frameHeight;

    // Create buffer array
    int size = width * height;
    buffer = new BGR4[size];

    // Copy handle to the window
    hwnd = frameHwnd;

    // Create handle to the device context
    hdc = GetDC(hwnd);
    tmpDc = CreateCompatibleDC(hdc);

    pen_color = {};

    camera_z = camera_t = 300.0f;
}
FRAME::~FRAME()
{
    delete[] buffer;
    DeleteDC(tmpDc);
    ReleaseDC(hwnd, hdc);
}

void FRAME::clear(RGB color)
{
    int i;

    for (short y = 0; y < height; y++)
    {
        for (short x = 0; x < width; x++)
        {
            i = y * width + x;
            buffer[i].R = color.R;
            buffer[i].G = color.G;
            buffer[i].B = color.B;
        }
    }
}
void FRAME::set_pixel(short x, short y)
{
    if (x > -1 && y > -1 && x < width && y < height)
    {
        int i = y * width + x;

        buffer[i].R = pen_color.R;
        buffer[i].G = pen_color.G;
        buffer[i].B = pen_color.B;
    }
}
void FRAME::set_circle(short x0, short y0, short R)
{
    for (short x = x0 - R; x < x0 + R; x++)
    {
        for (short y = y0 - R; y < y0 + R; y++)
        {
            if (((x - x0) * (x - x0) + (y - y0) * (y - y0) - R * R) < 0) set_pixel(x, y);
        }
    }
}
void FRAME::set_line(short x1, short y1, short x2, short y2)
{
    short dx = ABS(x2 - x1);
    short dy = ABS(y2 - y1);
    short sx = (x2 >= x1) ? 1 : -1;
    short sy = (y2 >= y1) ? 1 : -1;

    if (dx > dy)
    {
        short d = (dy << 1) - dx, d1 = dy << 1, d2 = (dy - dx) << 1;

        for (short x = x1 + sx, y = y1, i = 1; i < dx; i++, x += sx)
        {
            if (d > 0)
            {
                d += d2;
                y += sy;
            }
            else d += d1;

            set_pixel(x, y);
        }
    }
    else
    {
        short d = (dx << 1) - dy, d1 = dx << 1, d2 = (dx - dy) << 1;

        for (short x = x1, y = y1 + sy, i = 1; i < dy; i++, y += sy)
        {
            if (d > 0)
            {
                d += d2;
                x += sx;
            }
            else d += d1;

            set_pixel(x, y);
        }
    }

    set_pixel(x1, y1);
    set_pixel(x2, y2);
}
void FRAME::print()
{
    hbm = CreateBitmap(width, height, 1, 8 * 4, buffer);

    SelectObject(tmpDc, hbm);
    BitBlt(hdc, 0, 0, width, height, tmpDc, 0, 0, SRCCOPY);

    DeleteObject(hbm);
}
VECT2 FRAME::vect3_to_vect2(VECT3 vect3)
{
    if (vect3.z == 0)
    {
        return { vect3.x, vect3.y };
    }
    else if (vect3.z > 0)
    {
        VECT2 vect2;
        float k = camera_z / (camera_z + vect3.z);

        vect2.x = 200.0f + (vect3.x - 200.0f) * k;
        vect2.y = 200.0f + (vect3.y - 200.0f) * k;

        return vect2;
    }
    else
    {
        VECT2 vect2;
        float k = camera_z / (camera_z - vect3.z);

        vect2.x = 200.0f + (vect3.x - 200.0f) / k;
        vect2.y = 200.0f + (vect3.y - 200.0f) / k;

        return vect2;
    }
}
VECT3 FRAME::vect4_to_vect3(VECT4 vect4)
{
    if (vect4.t == 0)
    {
        return { vect4.x, vect4.y, vect4.z };
    }
    else if (vect4.t > 0)
    {
        VECT3 vect3;
        float k = camera_t / (camera_t + vect4.t);

        vect3.x = 200.0f + (vect4.x - 200.0f) * k;
        vect3.y = 200.0f + (vect4.y - 200.0f) * k;
        vect3.z = 100.0f + (vect4.z - 100.0f) * k;

        return vect3;
    }
    else
    {
        VECT3 vect3;
        float k = camera_t / (camera_t - vect4.t);

        vect3.x = 200.0f + (vect4.x - 200.0f) / k;
        vect3.y = 200.0f + (vect4.y - 200.0f) / k;
        vect3.z = 100.0f + (vect4.z - 100.0f) / k;

        return vect3;
    }
}
void FRAME::set_figure(FIGURE_4D& figure)
{
    // Convert 4D -> 2D -> 2D
    for (short i = 0; i < figure.num_vert; i++)
    {
        figure.vertexes3[i] = vect4_to_vect3(figure.vertexes4[i]);
        figure.vertexes2[i] = vect3_to_vect2(figure.vertexes3[i]);
    }

    // Draw edges
    for (short i = 0; i < figure.num_edg; i++)
    {
        if (i < 12) pen_color = { 255,0,0 };
        else if (i < 24) pen_color = { 0,255,0 };
        else pen_color = { 0,0,255 };

        set_line(figure.vertexes2[figure.edges[i].begin].x, figure.vertexes2[figure.edges[i].begin].y, figure.vertexes2[figure.edges[i].end].x, figure.vertexes2[figure.edges[i].end].y);
    }

    // Draw vertexes
    for (short i = 0; i < figure.num_vert; i++)
    {
        if (i < 8) pen_color = { 255,0,0 };
        else pen_color = { 0,255,0 };

        set_circle(figure.vertexes2[i].x, figure.vertexes2[i].y, 5);
    }
}

// -+-+-+-+-+-+-+-+-+-+- FIGURE 4D -+-+-+-+-+-+-+-+-+-+-
FIGURE_4D::FIGURE_4D(const char* fname)
{
    FILE* fp;

    // Open file
    fopen_s(&fp, fname, "rt");

    if (fp)
    {
        // Read vertexes
        fscanf_s(fp, "%d", &num_vert);

        vertexes4 = new VECT4[num_vert];
        vertexes3 = new VECT3[num_vert];
        vertexes2 = new VECT2[num_vert];

        for (short i = 0; i < num_vert; i++)
        {
            fscanf_s(fp, "%f %f %f %f", &vertexes4[i].x, &vertexes4[i].y, &vertexes4[i].z, &vertexes4[i].t);
            vertexes4[i].x = 100 + 200 * vertexes4[i].x;
            vertexes4[i].y = 100 + 200 * vertexes4[i].y;
            vertexes4[i].z = 200 * vertexes4[i].z;
            vertexes4[i].t = 200 * vertexes4[i].t;
        }

        // Read edges
        fscanf_s(fp, "%d", &num_edg);

        edges = new EDGE[num_edg];

        for (short i = 0; i < num_edg; i++)
        {
            fscanf_s(fp, "%d %d", &edges[i].begin, &edges[i].end);
        }

        // Close file
        fclose(fp);
        printf("%s readed.", fname);
    }
    else
    {
        // If error
        printf("%s read error.", fname);
        num_vert = num_edg = 0;
    }
}
FIGURE_4D::~FIGURE_4D()
{
    delete[] vertexes4;
    delete[] vertexes3;
    delete[] vertexes2;
    delete[] edges;
}

void FIGURE_4D::rotate_3d(float angle_YZ, float angle_XZ, float angle_XY, float x0, float y0, float z0)
{
    for (short i = 0; i < num_vert; i++)
    {
        vect4_rotate_3d(vertexes4[i], angle_YZ, angle_XZ, angle_XY, x0, y0, z0);
    }
}
void FIGURE_4D::rotate_4d(float angle_XT, float angle_YT, float angle_ZT, float x0, float y0, float z0, float t0)
{
    for (short i = 0; i < num_vert; i++)
    {
        vect4_rotate_4d(vertexes4[i], angle_XT, angle_YT, angle_ZT, x0, y0, z0, t0);
    }
}
void FIGURE_4D::translate(float dx, float dy, float dz, float dt)
{
    for (short i = 0; i < num_vert; i++)
    {
        vect4_translate(vertexes4[i], dx, dy, dz, dt);
    }
}

// -+-+-+-+-+-+-+-+-+-+- GLOBAL FUNC -+-+-+-+-+-+-+-+-+-+-
void vect4_rotate_3d(VECT4& vect4, float angle_YZ, float angle_XZ, float angle_XY, float x0, float y0, float z0)
{
    /*
    float Mx[3][3] =
    {
        { 1,               0,                0 },
        { 0, cos(RAD(angle)), -sin(RAD(angle)) },
        { 0, sin(RAD(angle)),  cos(RAD(angle)) }
    }
    float My[3][3] =
    {
        {  cos(RAD(angle)), 0, sin(RAD(angle)) },
        {                0, 1,               0 },
        { -sin(RAD(angle)), 0, cos(RAD(angle)) }
    }
    float Mz[3][3] =
    {
        { cos(RAD(angle)), -sin(RAD(angle)), 0 },
        { sin(RAD(angle)), cos(RAD(angle)),  0 },
        { 0,               0,                1 }
    }
    */

    vect4.x -= x0;
    vect4.y -= y0;
    vect4.z -= z0;

    float x, y, z;

    if (angle_YZ)
    {
        angle_YZ = RAD(angle_YZ);
        y = vect4.y;
        z = vect4.z;

        vect4.y = cos(angle_YZ) * y - sin(angle_YZ) * z;
        vect4.z = sin(angle_YZ) * y + cos(angle_YZ) * z;
    }
    if (angle_XZ)
    {
        angle_XZ = RAD(angle_XZ);
        x = vect4.x;
        z = vect4.z;

        vect4.x = cos(angle_XZ) * x - sin(angle_XZ) * z;
        vect4.z = sin(angle_XZ) * x + cos(angle_XZ) * z;
    }
    if (angle_XY)
    {
        angle_XY = RAD(angle_XY);
        x = vect4.x;
        y = vect4.y;

        vect4.x = cos(angle_XY) * x - sin(angle_XY) * y;
        vect4.y = sin(angle_XY) * x + cos(angle_XY) * y;
    }

    vect4.x += x0;
    vect4.y += y0;
    vect4.z += z0;
}
void vect4_rotate_4d(VECT4& vect4, float angle_XT, float angle_YT, float angle_ZT, float x0, float y0, float z0, float t0)
{
    vect4.x -= x0;
    vect4.y -= y0;
    vect4.z -= z0;
    vect4.t -= t0;

    float x, y, z, t;

    if (angle_XT)
    {
        angle_XT = RAD(angle_XT);
        x = vect4.x;
        t = vect4.t;

        vect4.x = cos(angle_XT) * x - sin(angle_XT) * t;
        vect4.t = sin(angle_XT) * x + cos(angle_XT) * t;
    }
    if (angle_YT)
    {
        angle_YT = RAD(angle_YT);
        y = vect4.y;
        t = vect4.t;

        vect4.y = cos(angle_YT) * y - sin(angle_YT) * t;
        vect4.t = sin(angle_YT) * y + cos(angle_YT) * t;
    }
    if (angle_ZT)
    {
        angle_ZT = RAD(angle_ZT);
        z = vect4.z;
        t = vect4.t;

        vect4.z = cos(angle_ZT) * z - sin(angle_ZT) * t;
        vect4.t = sin(angle_ZT) * z + cos(angle_ZT) * t;
    }

    vect4.x += x0;
    vect4.y += y0;
    vect4.z += z0;
    vect4.t += t0;
}
void vect4_translate(VECT4& vect4, float dx, float dy, float dz, float dt)
{
    vect4.x += dx;
    vect4.y += dy;
    vect4.z += dz;
    vect4.t += dt;
}