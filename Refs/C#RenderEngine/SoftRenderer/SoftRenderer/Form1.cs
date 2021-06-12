﻿using SoftRenderer.Math;
using SoftRenderer.Test;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows.Forms;

namespace SoftRenderer
{
    public partial class SoftRendererDemo : Form
    {
        //quad
        //private CVertex[] mesh = {  new CVertex(-1,1,0,0,0, 1, 0, 0),
        //                            new CVertex(-1,-1,0,0,1, 0, 1f, 0),
        //                            new CVertex(1,-1,0,1,1, 0, 0, 1f),
        //                            new CVertex(1,1,0,1,0, 0, 1, 0)
        //                         };

        //cube
        private CVertex[] mesh = {  new CVertex(1, -1, 1 ,0,0, 1, 0, 0),
                                    new CVertex(-1, -1, 1,0,1, 0, 1f, 0),
                                    new CVertex( -1,  1, 1,1,1, 0, 0, 1f),
                                    new CVertex(1, 1, 1,1,0, 0, 1, 0),
                                    new CVertex(1, -1, -1,0,1, 1, 0, 0),
                                    new CVertex(-1, -1, -1,0,0, 0, 0, 1),
                                    new CVertex( -1,  1, -1,1,0, 0, 1, 0),
                                    new CVertex(1, 1, -1,1,1, 0, 0, 1)

                                 };

                                 
        private Bitmap _texture;//纹理
        private Bitmap _frameBuff;//用一张bitmap来做帧缓冲
        private float[,] _zBuff;
        private RenderMode _currentMode;
        public SoftRendererDemo()
        {
            InitializeComponent();
            //D:/4-th_Grade/AlexSoftRenderer/Referenced/CSharpSoftRenderer/SoftRenderer/Texture
            System.Drawing.Image img = System.Drawing.Image.FromFile("texture.bmp");
            _texture = new Bitmap(img, 256, 256);
            //initTexture();
            _frameBuff = new Bitmap(this.MaximumSize.Width, this.MaximumSize.Height);
            _zBuff = new float[this.MaximumSize.Height, this.MaximumSize.Width];
            _currentMode = RenderMode.Textured;
            System.Timers.Timer mainTimer = new System.Timers.Timer(1);
    
            mainTimer.Elapsed += new ElapsedEventHandler(timer1_Tick);
            mainTimer.AutoReset = true;
            mainTimer.Enabled = true;
            mainTimer.Start();
            //
        }


        public void initTexture()
        {

            for (int j = 0; j < 256; j++)
            {
                for (int i = 0; i < 256; i++)
                {
                    int x = i / 32, y = j / 32;
                    _texture.SetPixel(j, i, System.Drawing.Color.Yellow);
                }
            }
        }



        private void ClearBuff()
        {
            for (int i = 0; i < this.MaximumSize.Height; i++)
            {
                for (int j = 0; j < this.MaximumSize.Width; j++)
                {
                    _frameBuff.SetPixel(j, i, System.Drawing.Color.Black);//清理为黑色
                    _zBuff[i, j] = 0;//计划使用1/z来做深度测试，所以初始化为0
                }
            }
        }

        /// <summary>
        /// 进行mvp矩阵变换，将顶点变换到齐次裁剪空间
        /// </summary>
        private void SetMVPTransform(CMatrix4x4 m, CMatrix4x4 v, CMatrix4x4 p, CVertex vertex)
        {
            vertex.point = vertex.point * m;
            VectorMatrixTestCase.showVector3(vertex.point);
            vertex.point = vertex.point * v;
            VectorMatrixTestCase.showVector3(vertex.point);
            vertex.point = vertex.point * p;
            VectorMatrixTestCase.showVector3(vertex.point);

                //得到齐次裁剪空间的点 v.point.w 中保存着原来的z(具体是z还是-z要看使用的投影矩阵,我们使用投影矩阵是让w中保存着z)


                //onePerZ 保存1/z，方便之后对1/z关于x’、y’插值得到1/z’ 
            vertex.onePerZ = 1 / vertex.point.w;
                //校正的推论： s/z、t/z和x’、y’也是线性关系。而我们之前知道1/z和x’、y’是线性关系。则我们得出新的思路：对1/z关于x’、y’插值得到1/z’，然后对s/z、t/z关于x’、y’进行插值得到s’/z’、t’/z’，然后用s’/z’和t’/z’分别除以1/z’，就得到了插值s’和t’
                //这里将需要插值的信息都乘以1/z 得到 s/z和t/z等，方便光栅化阶段进行插值
            vertex.u *= vertex.onePerZ;
            vertex.v *= vertex.onePerZ;
                //
            vertex.color.r *= vertex.onePerZ;
            vertex.color.g *= vertex.onePerZ;
            vertex.color.b *= vertex.onePerZ;
        }
        /// <summary>
        /// 检查是否裁剪这个顶点,简单的cvv裁剪,在透视除法之前
        /// </summary>
        /// <returns>是否通关剪裁</returns>
        private bool Clip(CVertex v)
        {
            //cvv为 x-1,1  y-1,1  z0,1
            if(v.point.x >= -v.point.w && v.point.x <= v.point.w &&
                v.point.y >= -v.point.w && v.point.y <= v.point.w && 
                v.point.z >= 0f && v.point.z <= v.point.w)
            {
                return true;
            }
            return false;
        }
        /// <summary>
        /// 从齐次剪裁坐标系转到屏幕坐标
        /// </summary>
        private void TransformToScreen(CVertex v)
        {
            if(v.point.w != 0)
            {
                //先进行透视除法，转到cvv
                v.point.x *= 1 / v.point.w;
                v.point.y *= 1 / v.point.w;
                v.point.z *= 1 / v.point.w;
                v.point.w = 1;
                //cvv到屏幕坐标
                v.point.x = (v.point.x + 1) * 0.5f * this.MaximumSize.Width;
                v.point.y = (1 - v.point.y) * 0.5f * this.MaximumSize.Height;
            }
            
        }
        /// <summary>
        /// 背面消隐
        /// </summary>
        /// <returns>是否通关背面消隐测试</returns>
        private bool BackFaceCulling(CVertex p1, CVertex p2, CVertex p3)
        {
            CVector3D v1 = p2.point - p1.point;
            CVector3D v2 = p3.point - p2.point;
            CVector3D normal = CVector3D.Cross(v1, v2);
            CVector3D viewDir = new CVector3D(0,0,1);//由于在剪裁空间，视线一点是这个方向
            if (CVector3D.Dot(normal,viewDir) > 0)
            {
                return true;
            }
            return false;
        }

        private void Draw(CMatrix4x4 m, CMatrix4x4 v, CMatrix4x4 p)
        {
            DrawPanel(0, 1, 2, 3, m, v, p);
            DrawPanel(4, 5, 6, 7, m, v, p);
            DrawPanel(0, 4, 5, 1, m, v, p);
            DrawPanel(1, 5, 6, 2, m, v, p);
            DrawPanel(2, 6, 7, 3, m, v, p);
            DrawPanel(3, 7, 4, 0, m, v, p);
        }
        /// <summary>
        /// 绘制平面
        /// </summary>
        /// <param name="vIndex1">顶点索引</param>
        /// <param name="vIndex2">顶点索引</param>
        /// <param name="vIndex3">顶点索引</param>
        /// <param name="vIndex4">顶点索引</param>
        /// <param name="mvp">mvp矩阵</param>
        private void DrawPanel(int vIndex1, int vIndex2, int vIndex3, int vIndex4, CMatrix4x4 m, CMatrix4x4 v, CMatrix4x4 p)
        {
            CVertex p1 = new CVertex(mesh[vIndex1]);
            CVertex p2 = new CVertex(mesh[vIndex2]);
            CVertex p3 = new CVertex(mesh[vIndex3]);
            //
            DrawTriangle(p1, p2, p3, m, v, p);
            p1 = new CVertex(mesh[vIndex1]);
            p3 = new CVertex(mesh[vIndex3]);
            CVertex p4 = new CVertex(mesh[vIndex4]);

            DrawTriangle(p3, p4, p1, m, v, p);
        }
        /// <summary>
        /// 绘制三角形
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <param name="p3"></param>
        /// <param name="mvp"></param>
        private void DrawTriangle(CVertex p1, CVertex p2, CVertex p3, CMatrix4x4 m, CMatrix4x4 v, CMatrix4x4 p)
        {
            //--------------------几何阶段---------------------------
            //变换到齐次裁剪空
            SetMVPTransform(m, v, p, p1);
            SetMVPTransform(m, v, p, p2);
            SetMVPTransform(m, v, p, p3);

            ////裁剪
            if (Clip(p1) == false || Clip(p2) == false || Clip(p3) == false)
            {
                return;
            }
            //背面消隐
            //if (BackFaceCulling(p1, p2, p3) == false)
            //{
            //    return;
            //}
            //变换到屏幕坐标
            TransformToScreen(p1);
            VectorMatrixTestCase.showVector3(p1.point);
            TransformToScreen(p2);
            VectorMatrixTestCase.showVector3(p2.point);
            TransformToScreen(p3);
            VectorMatrixTestCase.showVector3(p3.point);

            //--------------------光栅化阶段---------------------------
            //todo test
            //_frameBuff.SetPixel((int)p1.point.x, (int)p1.point.y, System.Drawing.Color.White);
            //_frameBuff.SetPixel((int)p2.point.x, (int)p2.point.y, System.Drawing.Color.White);
            //_frameBuff.SetPixel((int)p3.point.x, (int)p3.point.y, System.Drawing.Color.White);

            TriangleRasterization(p1, p2, p3);
        }
        /// <summary>
        /// 光栅化三角形
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <param name="p3"></param>
        private void TriangleRasterization(CVertex p1, CVertex p2, CVertex p3)
        {
            if(p1.point.y == p2.point.y)
            {
                if(p1.point.y < p3.point.y)
                {//平顶
                    DrawTriangleTop(p1, p2, p3);
                }
                else
                {//平底
                    DrawTriangleBottom(p3, p1, p2);
                }
            }
            else if (p1.point.y == p3.point.y)
            {
                if (p1.point.y < p2.point.y)
                {//平顶
                    DrawTriangleTop(p1, p3, p2);
                }
                else
                {//平底
                    DrawTriangleBottom(p2, p1, p1);
                }
            }
            else if (p2.point.y == p3.point.y)
            {
                if (p2.point.y < p1.point.y)
                {//平顶
                    DrawTriangleTop(p2, p3, p1);
                }
                else
                {//平底
                    DrawTriangleBottom(p1, p2, p3);
                }
            }
            else
            {//分割三角形
                CVertex top = null;

                CVertex bottom = null;
                CVertex middle = null;
                if(p1.point.y > p2.point.y && p2.point.y > p3.point.y)
                {
                    top = p3;
                    middle = p2;
                    bottom = p1;
                }
                else if (p3.point.y > p2.point.y && p2.point.y > p1.point.y)
                {
                    top = p1;
                    middle = p2;
                    bottom = p3;
                }
                else if (p2.point.y > p1.point.y && p1.point.y > p3.point.y)
                {
                    top = p3;
                    middle = p1;
                    bottom = p2;
                }
                else if (p3.point.y > p1.point.y && p1.point.y > p2.point.y)
                {
                    top = p2;
                    middle = p1;
                    bottom = p3;
                }
                else if(p1.point.y > p3.point.y && p3.point.y > p2.point.y)
                {
                    top = p2;
                    middle = p3;
                    bottom = p1;
                }
                else if (p2.point.y > p3.point.y && p3.point.y > p1.point.y)
                {
                    top = p1;
                    middle = p3;
                    bottom = p2;
                }
                //插值求中间点x
                float middlex = (middle.point.y - top.point.y) * (bottom.point.x - top.point.x) / (bottom.point.y - top.point.y) + top.point.x;
                float dy = middle.point.y - top.point.y;
                float t = dy / (bottom.point.y - top.point.y);
                //插值生成左右顶点
                CVertex newMiddle = new CVertex();
                newMiddle.point.x = middlex;
                newMiddle.point.y = middle.point.y;
                MathUntil.ScreenSpaceLerpVertex(ref newMiddle, top, bottom, t);

                //平底
                DrawTriangleBottom(top, newMiddle, middle);
                //平顶
                DrawTriangleTop(newMiddle, middle, bottom);     
            }
        }
        //x = (y-y1) * (x2-x1) / (y2-y1) + x1
        /// <summary>
        /// 平顶，p1,p2,p3为下顶点
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <param name="p3"></param>
        private void DrawTriangleTop(CVertex p1, CVertex p2, CVertex p3)
        {
            for (float y = p1.point.y; y <= p3.point.y; y++)
            {
                int yIndex = (int)(System.Math.Round(y, MidpointRounding.AwayFromZero)); 
                if (yIndex >= 0 && yIndex < this.MaximumSize.Height)
                {
                    float xl = (y - p1.point.y) * (p3.point.x - p1.point.x) / (p3.point.y - p1.point.y) + p1.point.x;
                    float xr = (y - p2.point.y) * (p3.point.x - p2.point.x) / (p3.point.y - p2.point.y) + p2.point.x;
 
                    float dy = y - p1.point.y;
                    float t = dy / (p3.point.y - p1.point.y);
                    //插值生成左右顶点
                    CVertex new1 = new CVertex();
                    new1.point.x = xl;
                    new1.point.y = y;
                    MathUntil.ScreenSpaceLerpVertex(ref new1, p1, p3, t);
                    //
                    CVertex new2 = new CVertex();
                    new2.point.x = xr;
                    new2.point.y = y;
                    MathUntil.ScreenSpaceLerpVertex(ref new2, p2, p3, t);
                    //扫描线填充
                    if (new1.point.x < new2.point.x)
                    {
                        ScanlineFill(new1, new2, yIndex);
                    }
                    else
                    {
                        ScanlineFill(new2, new1, yIndex);
                    }
                }
            }
        }
        /// <summary>
        /// 平底，p1为上顶点,p2，p3
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <param name="p3"></param>

        private void DrawTriangleBottom(CVertex p1, CVertex p2, CVertex p3)
        {
            for (float y = p1.point.y; y <= p2.point.y; y++)
            {
                int yIndex = (int)(System.Math.Round(y, MidpointRounding.AwayFromZero)); 
                if (yIndex >= 0 && yIndex < this.MaximumSize.Height)
                {
                    float xl = (y - p1.point.y) * (p2.point.x - p1.point.x) / (p2.point.y - p1.point.y) + p1.point.x;
                    float xr = (y - p1.point.y) * (p3.point.x - p1.point.x) / (p3.point.y - p1.point.y) + p1.point.x;

                    float dy = y - p1.point.y;
                    float t = dy / (p2.point.y - p1.point.y);
                    //插值生成左右顶点
                    CVertex new1 = new CVertex();
                    new1.point.x = xl;
                    new1.point.y = y;
                    MathUntil.ScreenSpaceLerpVertex(ref new1, p1, p2, t);
                    //
                    CVertex new2 = new CVertex();
                    new2.point.x = xr;
                    new2.point.y = y;
                    MathUntil.ScreenSpaceLerpVertex(ref new2, p1, p3, t);
                    //扫描线填充
                    if(new1.point.x < new2.point.x)
                    {
                        ScanlineFill(new1, new2, yIndex);
                    }
                    else
                    {
                        ScanlineFill(new2, new1, yIndex);
                    }
                }
            }
        }

        /// <summary>
        /// 扫描线填充
        /// </summary>
        /// <param name="left">左端点，值已经经过插值</param>
        /// <param name="right">右端点，值已经经过插值</param>
        private void ScanlineFill(CVertex left, CVertex right, int yIndex)
        {
            float dx = right.point.x - left.point.x;
            for (float x = left.point.x; x <= right.point.x; x++)
            {
                int xIndex = (int)(System.Math.Round(x, MidpointRounding.AwayFromZero)); 
                if (xIndex >= 0 && xIndex < this.MaximumSize.Width)
                {
                     float lerpFactor = 0;
                    if(dx != 0)
                    {
                        lerpFactor = (x - left.point.x) / dx;
                    }
                    //1/z’与x’和y'是线性关系的
                    float onePreZ = MathUntil.Lerp(left.onePerZ, right.onePerZ, lerpFactor);
                    if (onePreZ >= _zBuff[yIndex, xIndex])//使用1/z进行深度测试
                    {//通过测试
                        float w = 1 / onePreZ;
                        _zBuff[yIndex, xIndex] = onePreZ;
                        //uv 插值，求纹理颜色
                        float u = MathUntil.Lerp(left.u, right.u, lerpFactor) * w * (_texture.Width - 1);
                        float v = MathUntil.Lerp(left.v, right.v, lerpFactor) * w * (_texture.Height - 1);
                        int uIndex = (int)(u + 0.5f);
                        int vIndex = (int)(v + 0.5f);
                        uIndex = MathUntil.Range(uIndex, 0, _texture.Width - 1);
                        vIndex = MathUntil.Range(vIndex, 0, _texture.Height - 1);
                        //uv坐标系采用dx风格
                        System.Drawing.Color textrueColor = _texture.GetPixel(vIndex, uIndex);


                        //插值顶点颜色
                        float r = MathUntil.Lerp(left.color.r, right.color.r, lerpFactor) * w * 255;
                        float g = MathUntil.Lerp(left.color.g, right.color.g, lerpFactor) * w * 255;
                        float b = MathUntil.Lerp(left.color.b, right.color.b, lerpFactor) * w * 255;


                        //更新渲染模式渲染
                        if (RenderMode.Textured == _currentMode)
                        {
                            _frameBuff.SetPixel(xIndex, yIndex, textrueColor);
                        }
                        else if (RenderMode.VertexColor == _currentMode)
                        {
                            _frameBuff.SetPixel(xIndex, yIndex, System.Drawing.Color.FromArgb((int)r, (int)g, (int)b));
                        }
                    }
                }
            }

        }

        private float theta = 0;
        private float pos = 0;
        private float x = 0;
        protected override void OnPaint(PaintEventArgs e)
        {
        }

        protected override void OnPaintBackground(PaintEventArgs e)
        {
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            switch(keyData)
            {
                case Keys.A:
                    theta += 0.1f;
                    break;
                case Keys.S:
                    pos -= 0.1f;
                    break;
                case Keys.D:
                    theta -= 0.1f;

                    break;
                case Keys.W:
                    pos += 0.1f;
                    break;
   
                default:
                    break;

            }
            return false;
        }

        private float rot = 0;
        Graphics g1 = null;

        private void timer1_Tick(object sender, EventArgs e)
        {
            lock (_frameBuff)
            {
                ClearBuff();
                CMatrix4x4 m = new CMatrix4x4();
                m.Identity();
                m[3, 2] = 5;
                rot += 0.1f;
                m = MathUntil.GetRotateY(rot) * m;
                CMatrix4x4 v = MathUntil.GetView(new CVector3D(0, 0, 0, 1), new CVector3D(0, 0, 1, 1), new CVector3D(0, 1, 0, 1));
                CMatrix4x4 p = MathUntil.GetProjection((float)System.Math.PI / 4, this.MaximumSize.Width / (float)this.MaximumSize.Height, 1f, 500f);
                //
                Draw(m, v, p);
                //
                if (g1 == null)
                {
                    g1 = this.CreateGraphics();
                }
                g1.Clear(System.Drawing.Color.Black);
                g1.DrawImage(_frameBuff, 0, 0); 
            }
            this.Invalidate();
        }

    }
}
