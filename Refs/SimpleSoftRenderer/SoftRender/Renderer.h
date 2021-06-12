//*********************************************************
//
// Author: meng
// E-Mail: rf_marlboro@outlook.com
// GitHub: https://github.com/mversace/simpleSoftRenderer
// MyBlog: https://segmentfault.com/u/notapenny
//
// 如无必要，勿增实体。
// 人不倦，走不停。路若尽，心先行。
//
//*********************************************************

#pragma once
#include <math.h>
#include <vector>
#include <memory>

// 向量矩阵操作
namespace SoftRender
{
    // 点 向量
    struct Vector4
    {
        float x;
        float y;
        float z;
        float w;
    };

    // 矩阵
    struct Matrix
    {
        float m[4][4];
    };

    // 顶点
    struct vertex
    {
        Vector4 point;
        Vector4 color;
    };

    // 规范化
    Vector4 normallize(const Vector4& v)
    {
        float len = (float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return { v.x / len, v.y / len, v.z / len, 0.0f };
    }

    // 叉积
    Vector4 cross(const Vector4& u, const Vector4& v)
    {
        return { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x, 0.0f };
    }

    // 点积
    float dot(const Vector4& u, const Vector4& v)
    {
        return u.x * v.x + u.y * v.y + u.z * v.z;
    }

    // 矩阵乘法
    Matrix mul(const Matrix& a, const Matrix& b)
    {
        Matrix r;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                r.m[i][j] = a.m[i][0] * b.m[0][j]
                    + a.m[i][1] * b.m[1][j]
                    + a.m[i][2] * b.m[2][j]
                    + a.m[i][3] * b.m[3][j];
            }
        }
        return r;
    }

    // 点/向量转换
    Vector4 transform(const Vector4& v, const Matrix& m)
    {
        Vector4 r;
        r.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
        r.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
        r.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
        r.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
        return r;
    }

    // 透视除法 齐次除法
    void perspectiveDivede(Vector4& v)
    {
        v.x /= v.w;
        v.y /= v.w;
        v.z /= v.w;
        v.w = 1.0f;
    }

    // 转换到屏幕坐标
    void transformScreen(Vector4& v, int w, int h)
    {
        v.x = (v.x + 1.0f) * w * 0.5f;
        v.y = (1.0f - v.y) * h * 0.5f;
    }

    // 摄像机
    class Camera
    {
    public:
        Camera(Vector4 pos, Vector4 target, Vector4 up) :
            _pos(pos), _posTemp(pos), _target(target), _up(up) {}
        virtual ~Camera() noexcept {}

        void setPos(const Vector4& pos)
        {
            _pos = pos;
            calcMatrix();
        }

        void setPerspectiveForLH(float fov, float aspect, float nearZ, float farZ)
        {
            _fov = fov; _aspect = aspect; _nearZ = nearZ; _farZ = farZ;
            calcMatrix();
        }

        // 环绕
        void circle(short xMove, short yMove)
        {
            // 鼠标移动像素与弧度的比例固定
            float circleLen = 100.f;

            // 1 计算绕y轴的旋转
            float radY = xMove / circleLen;
            Matrix mScaleY = {
                (float)cos(radY), 0.0f, -(float)sin(radY), 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                (float)sin(radY), 0.0f, (float)cos(radY), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            // 2 计算绕x轴 这里需要设定一个最大角度
            float radX = yMove / circleLen;
            float maxRad = 3.1415926f * 0.45f;
            _curXRand += radX;
            if (_curXRand < -maxRad)
            {
                _curXRand = -maxRad;
                radX = 0.0f;
            }
            if (_curXRand > maxRad)
            {
                _curXRand = maxRad;
                radX = 0.0f;
            }
                
            Matrix mScaleX = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, (float)cos(radX), (float)sin(radX), 0.0f,
                0.0f, -(float)sin(radX), (float)cos(radX), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };

            _pos = transform(_pos, mScaleX);
            _pos = transform(_pos, mScaleY);
            calcMatrix();
        }

        // 缩放
        void zoom(short wParam)
        {
            float t = 0.9f;
            if (wParam > 0) t = 1.1f;
            _pos.x *= t;
            _pos.y *= t;
            _pos.z *= t;
            calcMatrix();
        }

        void reset()
        {
            _pos = _posTemp;
            _curXRand = 0.0f;
            calcMatrix();
        }

    public:
        // 世界坐标系转到投影平面
        Matrix _worldToProjection;

    private:
        void calcMatrix()
        {
            Vector4 dir = { _target.x - _pos.x, _target.y - _pos.y, _target.z - _pos.z, 0.0f };
            Vector4 w = normallize(dir);
            Vector4 u = normallize(cross(_up, w));
            Vector4 v = cross(w, u);

            _worldToView = {
                u.x, v.x, w.x, 0.0f,
                u.y, v.y, w.y, 0.0f,
                u.z, v.z, w.z, 0.0f,
                -dot(_pos, u), -dot(_pos, v), -dot(_pos, w), 1.0
            };

            float f = 1.0f / (float)tan(_fov * 0.5f);
            _viewToProjection = {
                f / _aspect, 0.0f, 0.0f, 0.0f,
                0.0f, f, 0.0f, 0.0f,
                0.0f, 0.0f, _farZ / (_farZ - _nearZ), 1.0f,
                0.0f, 0.0f, -_nearZ * _farZ / (_farZ - _nearZ), 0.0f
            };

            _worldToProjection = mul(_worldToView, _viewToProjection);
        }

    private:
        Vector4 _pos;
        Vector4 _posTemp;
        Vector4 _target;
        Vector4 _up;
        float _fov;
        float _aspect;
        float _nearZ;
        float _farZ;
        float _curXRand = 0.0f;

        // 世界坐标系转观察坐标系
        Matrix _worldToView;
        // 观察坐标系转投影坐标系
        Matrix _viewToProjection;
    };
}

namespace SoftRender
{
    // 初始化摄像机 
    Camera camera(
        {5.0f, 5.0f, -5.0f, 1.0f},  // 位置
        {0.0f, 0.0f, 0.0f, 1.0f},   // 朝向 原点
        {0.0f, 1.0f, 0.0f, 0.0f}    // 摄像机上方向
    );

    // 目标立方体8个顶点 摄像机方向
    std::vector<vertex> vertexes = {
        // 近相机面
        {{-1.0f, +1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f}},
        {{+1.0f, +1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 0.0f}},
        {{+1.0f, -1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 0.0f}},
        {{-1.0f, -1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 0.0f}},

        // 远相机面
        {{-1.0f, +1.0f, +1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 0.0f}},
        {{+1.0f, +1.0f, +1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f}},
        {{+1.0f, -1.0f, +1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 0.0f}},
        {{-1.0f, -1.0f, +1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 0.0f}}
    };
    int g_width = 0;
    int g_height = 0;

    enum class RenderMode
    {
        RENDER_WIREFRAME,
        RENDER_COLOR,
    };
    RenderMode g_renderMode = RenderMode::RENDER_WIREFRAME;

    HDC g_tempDC = nullptr;
    HBITMAP g_tempBm = nullptr;
    HBITMAP g_oldBm = nullptr;
    unsigned int* g_frameBuff = nullptr;
    std::shared_ptr<float[]> g_depthBuff = nullptr;

    unsigned int bgColor = ((123 << 16) | (195 << 8) | 221);
    unsigned int greenColor = (255 << 8);

    // 初始化渲染器 屏幕长宽 屏幕缓冲
    void initRenderer(int w, int h, HWND hWnd);
    // 每帧绘制
    void update(HWND hWnd);
    // 清理屏幕缓冲
    void clearBuffer();
    void shutDown();
    void onMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);

    // 绘制立方体
    void drawCube();
    // 绘制面
    void drawPlane(int lt, int rt, int rb, int lb);
    // 绘制三角图元
    void drawPrimitive(const vertex& a, const vertex& b, const vertex& c);
    // 扫描线绘制三角形
    void drawPrimitiveScanLine(const vertex& a, const vertex& b, const vertex& c);
    // 绘制线
    void drawLine(int x1, int y1, int x2, int y2, unsigned int color);
    // 绘制点
    void drawPixel(int x, int y, unsigned int color);
    // 简单cvv裁剪 只要有一个顶点不在 整个三角形图元都不绘制
    bool checkCvv(const Vector4& v);
    Vector4 barycentric(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& p);
}

void SoftRender::initRenderer(int w, int h, HWND hWnd)
{
    g_width = w;
    g_height = h;

    // 1. 创建一个屏幕缓冲
    // 1.1 创建一个与当前设备兼容的DC
    HDC hDC = GetDC(hWnd);
    g_tempDC = CreateCompatibleDC(hDC);
    ReleaseDC(hWnd, hDC);
    // 1.2 创建该DC的bitmap缓冲  32位色
    BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
        (DWORD)w * h * 4, 0, 0, 0, 0 } };
    g_tempBm = CreateDIBSection(g_tempDC, &bi, DIB_RGB_COLORS, (void**)&g_frameBuff, 0, 0);
    // 1.3 选择该bitmap到dc中
    g_oldBm = (HBITMAP)SelectObject(g_tempDC, g_tempBm);
    // 1.4 创建深度缓冲区
    g_depthBuff.reset(new float[w * h]);
    
    // 清理屏幕缓冲
    clearBuffer();

    // 摄像机初始化
    camera.setPerspectiveForLH(
        3.1415926f * 0.25f,       // 上下45度视野
        (float)w / (float)h,    // 长宽比
        1.0f,
        200.0f
    );
}

void SoftRender::update(HWND hWnd)
{
    // 1. clear frameBuffer
    clearBuffer();

    // draw frameBuffer
    drawCube();

    // present frameBuffer to screen
    HDC hDC = GetDC(hWnd);
    BitBlt(hDC, 0, 0, g_width, g_height, g_tempDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, hDC);
}

void SoftRender::onMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    static bool isPressed = false;
    static LPARAM lpCur = 0;
    switch (message)
    {
    case WM_MOUSEWHEEL:
        camera.zoom(GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    case WM_LBUTTONDOWN:
        isPressed = true;
        lpCur = lParam;
        break;
    case WM_LBUTTONUP:
        isPressed = false;
        break;
    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON)
        {
            short xMove = LOWORD(lParam) - LOWORD(lpCur);
            short yMove = HIWORD(lParam) - HIWORD(lpCur);
            lpCur = lParam;
            camera.circle(xMove, yMove);
        }
        break;
    case WM_KEYDOWN:
        if (wParam == VK_SPACE)
            camera.reset();
        else if (wParam == VK_F1)
        {
            if (g_renderMode == RenderMode::RENDER_COLOR) g_renderMode = RenderMode::RENDER_WIREFRAME;
            else g_renderMode = RenderMode::RENDER_COLOR;
        }
        break;
    default:
        break;
    }
}

void SoftRender::clearBuffer()
{
    for (int row = 0; row < g_height; ++row)
    {
        for (int col = 0; col < g_width; ++col)
        {
            int idx = row * g_width + col;
            // 默认背景色浅蓝 R123 G195 B221
            g_frameBuff[idx] = bgColor;
            // 深度缓冲区 1.0f
            g_depthBuff[idx] = 1.0f;
        }
    }
}

void SoftRender::shutDown()
{
    if (g_tempDC)
    {
        if (g_oldBm)
        {
            SelectObject(g_tempDC, g_oldBm);
            g_oldBm = nullptr;
        }
        DeleteDC(g_tempDC);
        g_tempDC = nullptr;
    }

    if (g_tempBm)
    {
        DeleteObject(g_tempBm);
        g_tempBm = nullptr;
    }
}

void SoftRender::drawCube()
{
    drawPlane(0, 1, 2, 3);  // 正面
    drawPlane(1, 5, 6, 2);  // 右面
    drawPlane(4, 0, 3, 7);  // 左面
    drawPlane(4, 5, 1, 0);  // 上面
    drawPlane(3, 2, 6, 7);  // 下面
    drawPlane(5, 4, 7, 6);  // 后面
}

void SoftRender::drawPlane(int lt, int rt, int rb, int lb)
{
    drawPrimitive(vertexes[lt], vertexes[rt], vertexes[rb]);
    drawPrimitive(vertexes[lt], vertexes[rb], vertexes[lb]);
}

void SoftRender::drawPrimitive(const vertex& a, const vertex& b, const vertex& c)
{
    // 1. 转换顶点到齐次剪裁空间
    // point * modelToWorld * worldToView * viewToProjection
    //
    // 1.1 计算modelToWorld矩阵 SRT
    // 简单做 该物理的坐标系与世界坐标系相同
    // 则modelToWorld就是单位矩阵
    // do nothing
    //
    // 1.2 计算worldToView viewToProjection
    // 这一部分在camera中计算了 直接读取出来
    //
    // 1.3 顺路做简单的cvv裁剪
    Matrix m = camera._worldToProjection;
    Vector4 p1 = transform(a.point, m); if (checkCvv(p1)) return;
    Vector4 p2 = transform(b.point, m); if (checkCvv(p2)) return;
    Vector4 p3 = transform(c.point, m); if (checkCvv(p3)) return;

    // 2. 透视除法 归一到NDC坐标系
    // x[-1, 1] y[-1, 1] z[near, far]
    perspectiveDivede(p1);
    perspectiveDivede(p2);
    perspectiveDivede(p3);

    // 3. 转换到屏幕坐标
    transformScreen(p1, g_width, g_height);
    transformScreen(p2, g_width, g_height);
    transformScreen(p3, g_width, g_height);

    // 4. 绘制线框
    if (g_renderMode == RenderMode::RENDER_WIREFRAME)
    {
        int x1 = (int)(p1.x + 0.5f), x2 = (int)(p2.x + 0.5f), x3 = (int)(p3.x + 0.5f);
        int y1 = (int)(p1.y + 0.5f), y2 = (int)(p2.y + 0.5f), y3 = (int)(p3.y + 0.5f);
        drawLine(x1, y1, x2, y2, greenColor);
        drawLine(x2, y2, x3, y3, greenColor);
        drawLine(x1, y1, x3, y3, greenColor);
    }
    else
    {
        // 5 重心坐标绘制三角形
        drawPrimitiveScanLine({ p1, a.color }, { p2, b.color }, { p3, c.color });
    }

}

void SoftRender::drawPrimitiveScanLine(const vertex& a, const vertex& b, const vertex& c)
{
    float xl = a.point.x; if (b.point.x < xl) xl = b.point.x; if (c.point.x < xl) xl = c.point.x;
    float xr = a.point.x; if (b.point.x > xr) xr = b.point.x; if (c.point.x > xr) xr = c.point.x;
    float yt = a.point.y; if (b.point.y < yt) yt = b.point.y; if (c.point.y < yt) yt = c.point.y;
    float yb = a.point.y; if (b.point.y > yb) yb = b.point.y; if (c.point.y > yb) yb = c.point.y;

    int xMin = (int)(xl + 0.5f), xMax = (int)(xr + 0.5f), yMin = (int)(yt + 0.5f), yMax = (int)(yb + 0.5f);
    for (int x = xMin; x <= xMax; ++x)
    {
        for (int y = yMin; y <= yMax; ++y)
        {
            // 计算是否在三角形内部
            Vector4 ret = barycentric(a.point, b.point, c.point, { (float)x, (float)y, 0.0f, 0.0f });
            if (ret.x < 0 || ret.y < 0 || ret.z < 0) continue;
            unsigned int colorR = (unsigned int)((a.color.x * ret.x + b.color.x * ret.y + c.color.x * ret.z) * 255);
            unsigned int colorG = (unsigned int)((a.color.y * ret.x + b.color.y * ret.y + c.color.y * ret.z) * 255);
            unsigned int colorB = (unsigned int)((a.color.z * ret.x + b.color.z * ret.y + c.color.z * ret.z) * 255);
            float depth = (a.point.z * ret.x + b.point.z * ret.y + c.point.z * ret.z);
            if (g_depthBuff[x + y * g_width] < depth)continue;
            g_depthBuff[x + y * g_width] = depth;
            drawPixel(x, y, (colorR << 16) | (colorG << 8) | colorB);
        }
    }
}

void SoftRender::drawLine(int x1, int y1, int x2, int y2, unsigned int color)
{
    if (x1 == x2 && y1 == y2)
    {
        drawPixel(x1, y1, color);
    }
    else if (x1 == x2)
    {
        if (y1 > y2) std::swap(y1, y2);
        for (int y = y1; y <= y2; ++y)
            drawPixel(x1, y, color);
    }
    else if (y1 == y2)
    {
        if (x1 > x2) std::swap(x1, x2);
        for (int x = x1; x <= x2; ++x)
            drawPixel(x, y1, color);
    }
    else
    {
        // Bresenham
        int diff = 0;
        int dx = std::abs(x1 - x2);
        int dy = std::abs(y1 - y2);
        if (dx > dy)
        {
            if (x1 > x2) std::swap(x1, x2), std::swap(y1, y2);
            for (int x = x1, y = y1; x < x2; ++x)
            {
                drawPixel(x, y, color);
                diff += dy;
                if (diff >= dx)
                {
                    diff -= dx;
                    y += (y1 < y2) ? 1 : -1;
                }
            }
            drawPixel(x2, y2, color);
        }
        else
        {
            if (y1 > y2) std::swap(x1, x2), std::swap(y1, y2);
            for (int y = y1, x = x1; y < y2; ++y)
            {
                drawPixel(x, y, color);
                diff += dx;
                if (diff >= dy)
                {
                    diff -= dy;
                    x += (x1 < x2) ? 1 : -1;
                }
            }
            drawPixel(x2, y2, color);
        }
    }
}

void SoftRender::drawPixel(int x, int y, unsigned int color)
{
    if (x < 0 || x >= g_width || y < 0 || y >= g_height) return;

    int idx = y * g_width + x;
    g_frameBuff[idx] = color;
}

bool SoftRender::checkCvv(const Vector4& v)
{
    if (v.z < 0.0f) return true;
    if (v.z > v.w) return true;
    if (v.x < -v.w) return true;
    if (v.x > v.w) return true;
    if (v.y < -v.w) return true;
    if (v.y > v.w) return true;
    return false;
}

SoftRender::Vector4 SoftRender::barycentric(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& p)
{
    Vector4 v1 = { c.x - a.x, b.x - a.x, a.x - p.x };
    Vector4 v2 = { c.y - a.y, b.y - a.y, a.y - p.y };

    Vector4 u = cross(v1, v2);
    if (std::abs(u.z) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return { 1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z };
    return { -1, 1, 1, 0 }; // in this case generate negative coordinates, it will be thrown away by the rasterizator
}