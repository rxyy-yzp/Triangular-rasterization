#include"GL/freeglut.h"
#include"iostream"
#include"math.h"
#include"stdlib.h"
#include"time.h"
using namespace std;

#define WINDOW_WIDTH 600 //窗口宽度
#define WINDOW_HEIGHT 600 //窗口高度

//三角形结构体
struct Triangle
{
	int a[3][2];//三角形3个顶点a1,a2,a3的x,y坐标
	float L[3][3];//三角形3条边L1,L2,L3对应直线方程的三个系数，L1为a2a3组成的边
}Tri[5];

float RGB[5][3] = //定义颜色数组
{
	{0,0,1},//蓝色
	{0,0.5,0},//绿色
	{0,1,1},//淡蓝
	{1,0,1},//粉色
	{1,1,0},//黄色
};

//三角形结构体初始化
void InitTriangle(struct Triangle Tri[])
{
	Tri[0].a[0][0] = 100;Tri[0].a[0][1] = 50;
	Tri[0].a[1][0] = 200;Tri[0].a[1][1] = 150;
	Tri[0].a[2][0] = 300;Tri[0].a[2][1] = 100;
	Tri[1].a[0][0] = 150;Tri[1].a[0][1] = -100;
	Tri[1].a[1][0] = 200;Tri[1].a[1][1] = -200;
	Tri[1].a[2][0] = 300;Tri[1].a[2][1] = -200;
	Tri[2].a[0][0] = -100;Tri[2].a[0][1] = -150;
	Tri[2].a[1][0] = -200;Tri[2].a[1][1] = -250;
	Tri[2].a[2][0] = -100;Tri[2].a[2][1] = -300;
	Tri[3].a[0][0] = -100;Tri[3].a[0][1] = 300;
	Tri[3].a[1][0] = -200;Tri[3].a[1][1] = 400;
	Tri[3].a[2][0] = -300;Tri[3].a[2][1] = 200;
	Tri[4].a[0][0] = -100;Tri[4].a[0][1] = -100;
	Tri[4].a[1][0] = 100;Tri[4].a[1][1] = -100;
	Tri[4].a[2][0] = 0;Tri[4].a[2][1] = 100;
	//计算L数组(3条边方程)
	for (int p = 0;p < 5;p++)
	{
		for (int Li = 1; Li <= 3; Li++)
		{//边的编号
			int dX = Tri[p].a[(Li + 1) % 3][0] - Tri[p].a[Li % 3][0];//该边两顶点横坐标之差，用作y项的系数
			int dY = Tri[p].a[(Li + 1) % 3][1] - Tri[p].a[Li % 3][1];//该边两顶点纵坐标之差，用作x项的系数
			int x = dX,y = dY,t;
			while (y != 0)//求最大公约数
			{
				t = x % y;
				x = y;
				y = t;
			}
			dX /= x;dY /= x;//约分
			Tri[p].L[Li - 1][1] = (float)dX;//系数A
			Tri[p].L[Li - 1][0] = (float)(-dY);//系数B
			//代入任一顶点求出L[Li-1][2]，求系数C
			Tri[p].L[Li - 1][2] = 0 - Tri[p].a[Li % 3][0] * Tri[p].L[Li - 1][0] - Tri[p].a[Li % 3][1] * Tri[p].L[Li - 1][1];
			//将Li与其相对的点代入直线方程，>0无操作，<0则三个系数取相反数，保证像素点代入直线方程时>0在三角形可见侧
			if (Tri[p].a[Li - 1][0] * Tri[p].L[Li - 1][0] + Tri[p].a[Li - 1][1] * Tri[p].L[Li - 1][1] + Tri[p].L[Li - 1][2] < 0)
				for (int i = 0; i < 3; i++)
					Tri[p].L[Li - 1][i] = -Tri[p].L[Li - 1][i];
		}
	}
};

//三角形包围盒裁剪
int bboxcut(struct Triangle Tri[])
{
	int worldSize = 0;//世界坐标系尺寸
	for (int i = 0; i < 5; i++)//裁剪每个三角形
		for (int j = 0; j < 3; j++)//获取三个点
			for (int k = 0; k < 2; k++)//获取每个点的横纵坐标
				if (abs(Tri[i].a[j][k]) > worldSize)
					worldSize = Tri[i].a[j][k];
	//确定世界坐标系 横纵向坐标极值均为(-worldSize, worldSize)
	gluOrtho2D(-worldSize, worldSize, -worldSize, worldSize);//设置区域
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);//视口
	return worldSize;
}

//三角形填充算法1：每个像素点分别带入三角形各边的直线方程求解
void triangleFilling1(struct Triangle Tri[], int worldSize) 
{
	bool visible = true;
	glBegin(GL_POINTS);//开始绘图，对应glEnd()
	for (int x = -worldSize; x <= worldSize; x++)
		for (int y = -worldSize; y <= worldSize; y++)
			for (int i = 0; i < 5; i++)//分别绘制每个三角形
			{
				glColor3f(RGB[i][0], RGB[i][1], RGB[i][2]);//从颜色数组中选取颜色
				visible = true;//计算某点是否在三角形的可见侧
				for (int ci = 0; ci < 3; ci++) 
				{
					float result = Tri[i].L[ci][0] * x + Tri[i].L[ci][1] * y + Tri[i].L[ci][2];//像素点代入直线方程
					if (result < 0)
					{
						visible = false;//设置不可见不绘制
						break;
					}
				}
				if (visible)//如果可见
					glVertex2i(x, y);//指定绘制该点
			}
	glEnd();//结束绘图，对应glBegin()
}

//三角形填充算法2：利用扫描线求解
void triangleFilling2(struct Triangle Tri[], int worldSize) {
	glBegin(GL_POINTS);
	for (int y = -worldSize; y <= worldSize; y++)
		for (int i = 0; i < 5; i++) 
		{
			glColor3f(RGB[i][0], RGB[i][1], RGB[i][2]);//从颜色数组中选取颜色
			int x0 = -worldSize;//每条扫描线左边的起始x值
			float Ei[3];//存储一个三角形内扫描线最左像素点代入三个直线方程的初值
			for (int ci = 0; ci < 3; ci++)
				Ei[ci] = Tri[i].L[ci][0] * x0 + Tri[i].L[ci][1] * y + Tri[i].L[ci][2];//像素点(x0,y)代入直线方程
			for (int x = x0; x <= worldSize; x++) 
			{
				if (Ei[0] >= 0 && Ei[1] >= 0 && Ei[2] >= 0)//像素点在三角形三条边的可见侧
					glVertex2i(x, y);
				for (int j = 0; j < 3; j++)
					Ei[j] += Tri[i].L[j][0];
			}
		}
	glEnd();
}

//图像显示
void display()
{
	glLoadIdentity();//恢复原始坐标系
	int worldSize = bboxcut(Tri);//三角形包围盒裁剪
	glClear(GL_COLOR_BUFFER_BIT);//恢复背景颜色
	double dur;//计时器
	clock_t start, end;
	start = clock();//开始计时
	//triangleFilling1(Tri, worldSize);//三角形填充算法1
	triangleFilling2(Tri, worldSize);//三角形填充算法2
	end = clock();//结束计时
	dur = (double)(end - start);//计算绘图时间
	cout << "绘图所用时间:" << dur / CLOCKS_PER_SEC << endl;//输出绘图所用时间
	for (int i = 0; i < 5; i++)//画出5个三角形做轮廓方便识别 
	{
		glColor3f(RGB[i][0], RGB[i][1], RGB[i][2]);//从颜色数组中选取颜色
		glBegin(GL_LINE_LOOP);//开始绘图，对应glEnd()
		for (int j = 0; j < 3; j++)
			glVertex2i(Tri[i].a[j][0], Tri[i].a[j][1]);
		glEnd();//结束绘图，对应glBegin()
	}
	glFlush();//强制刷新缓冲，保证绘图命执行
}

int main(int argc, char** argv)
{
	InitTriangle(Tri);
	glutInit(&argc, argv);//初始化GLUT库
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);//设置图形显示模式
	glutInitWindowPosition(100, 100);//设置在窗口的中心位置
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);//设置窗口大小
	gluOrtho2D(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_WIDTH / 2, WINDOW_WIDTH / 2);//设置显示区域
	glutCreateWindow("三角形的光栅化算法"); //创建窗口
	glMatrixMode(GL_PROJECTION);//增加透视
	glLoadIdentity();//恢复原始坐标系
	glClearColor(1.0, 1.0, 1.0, 1.0);//清除颜色缓存为白色
	glutDisplayFunc(&display);//设置一个函数
	glutMainLoop();//进入GLUT事件处理循环
	system("pause");
	return 0;
}