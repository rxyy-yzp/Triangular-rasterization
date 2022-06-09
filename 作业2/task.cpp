#include"GL/freeglut.h"
#include"iostream"
#include"math.h"
#include"stdlib.h"
#include"time.h"
using namespace std;

#define WINDOW_WIDTH 600 //���ڿ��
#define WINDOW_HEIGHT 600 //���ڸ߶�

//�����νṹ��
struct Triangle
{
	int a[3][2];//������3������a1,a2,a3��x,y����
	float L[3][3];//������3����L1,L2,L3��Ӧֱ�߷��̵�����ϵ����L1Ϊa2a3��ɵı�
}Tri[5];

float RGB[5][3] = //������ɫ����
{
	{0,0,1},//��ɫ
	{0,0.5,0},//��ɫ
	{0,1,1},//����
	{1,0,1},//��ɫ
	{1,1,0},//��ɫ
};

//�����νṹ���ʼ��
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
	//����L����(3���߷���)
	for (int p = 0;p < 5;p++)
	{
		for (int Li = 1; Li <= 3; Li++)
		{//�ߵı��
			int dX = Tri[p].a[(Li + 1) % 3][0] - Tri[p].a[Li % 3][0];//�ñ������������֮�����y���ϵ��
			int dY = Tri[p].a[(Li + 1) % 3][1] - Tri[p].a[Li % 3][1];//�ñ�������������֮�����x���ϵ��
			int x = dX,y = dY,t;
			while (y != 0)//�����Լ��
			{
				t = x % y;
				x = y;
				y = t;
			}
			dX /= x;dY /= x;//Լ��
			Tri[p].L[Li - 1][1] = (float)dX;//ϵ��A
			Tri[p].L[Li - 1][0] = (float)(-dY);//ϵ��B
			//������һ�������L[Li-1][2]����ϵ��C
			Tri[p].L[Li - 1][2] = 0 - Tri[p].a[Li % 3][0] * Tri[p].L[Li - 1][0] - Tri[p].a[Li % 3][1] * Tri[p].L[Li - 1][1];
			//��Li������Եĵ����ֱ�߷��̣�>0�޲�����<0������ϵ��ȡ�෴������֤���ص����ֱ�߷���ʱ>0�������οɼ���
			if (Tri[p].a[Li - 1][0] * Tri[p].L[Li - 1][0] + Tri[p].a[Li - 1][1] * Tri[p].L[Li - 1][1] + Tri[p].L[Li - 1][2] < 0)
				for (int i = 0; i < 3; i++)
					Tri[p].L[Li - 1][i] = -Tri[p].L[Li - 1][i];
		}
	}
};

//�����ΰ�Χ�вü�
int bboxcut(struct Triangle Tri[])
{
	int worldSize = 0;//��������ϵ�ߴ�
	for (int i = 0; i < 5; i++)//�ü�ÿ��������
		for (int j = 0; j < 3; j++)//��ȡ������
			for (int k = 0; k < 2; k++)//��ȡÿ����ĺ�������
				if (abs(Tri[i].a[j][k]) > worldSize)
					worldSize = Tri[i].a[j][k];
	//ȷ����������ϵ ���������꼫ֵ��Ϊ(-worldSize, worldSize)
	gluOrtho2D(-worldSize, worldSize, -worldSize, worldSize);//��������
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);//�ӿ�
	return worldSize;
}

//����������㷨1��ÿ�����ص�ֱ���������θ��ߵ�ֱ�߷������
void triangleFilling1(struct Triangle Tri[], int worldSize) 
{
	bool visible = true;
	glBegin(GL_POINTS);//��ʼ��ͼ����ӦglEnd()
	for (int x = -worldSize; x <= worldSize; x++)
		for (int y = -worldSize; y <= worldSize; y++)
			for (int i = 0; i < 5; i++)//�ֱ����ÿ��������
			{
				glColor3f(RGB[i][0], RGB[i][1], RGB[i][2]);//����ɫ������ѡȡ��ɫ
				visible = true;//����ĳ���Ƿ��������εĿɼ���
				for (int ci = 0; ci < 3; ci++) 
				{
					float result = Tri[i].L[ci][0] * x + Tri[i].L[ci][1] * y + Tri[i].L[ci][2];//���ص����ֱ�߷���
					if (result < 0)
					{
						visible = false;//���ò��ɼ�������
						break;
					}
				}
				if (visible)//����ɼ�
					glVertex2i(x, y);//ָ�����Ƹõ�
			}
	glEnd();//������ͼ����ӦglBegin()
}

//����������㷨2������ɨ�������
void triangleFilling2(struct Triangle Tri[], int worldSize) {
	glBegin(GL_POINTS);
	for (int y = -worldSize; y <= worldSize; y++)
		for (int i = 0; i < 5; i++) 
		{
			glColor3f(RGB[i][0], RGB[i][1], RGB[i][2]);//����ɫ������ѡȡ��ɫ
			int x0 = -worldSize;//ÿ��ɨ������ߵ���ʼxֵ
			float Ei[3];//�洢һ����������ɨ�����������ص��������ֱ�߷��̵ĳ�ֵ
			for (int ci = 0; ci < 3; ci++)
				Ei[ci] = Tri[i].L[ci][0] * x0 + Tri[i].L[ci][1] * y + Tri[i].L[ci][2];//���ص�(x0,y)����ֱ�߷���
			for (int x = x0; x <= worldSize; x++) 
			{
				if (Ei[0] >= 0 && Ei[1] >= 0 && Ei[2] >= 0)//���ص��������������ߵĿɼ���
					glVertex2i(x, y);
				for (int j = 0; j < 3; j++)
					Ei[j] += Tri[i].L[j][0];
			}
		}
	glEnd();
}

//ͼ����ʾ
void display()
{
	glLoadIdentity();//�ָ�ԭʼ����ϵ
	int worldSize = bboxcut(Tri);//�����ΰ�Χ�вü�
	glClear(GL_COLOR_BUFFER_BIT);//�ָ�������ɫ
	double dur;//��ʱ��
	clock_t start, end;
	start = clock();//��ʼ��ʱ
	//triangleFilling1(Tri, worldSize);//����������㷨1
	triangleFilling2(Tri, worldSize);//����������㷨2
	end = clock();//������ʱ
	dur = (double)(end - start);//�����ͼʱ��
	cout << "��ͼ����ʱ��:" << dur / CLOCKS_PER_SEC << endl;//�����ͼ����ʱ��
	for (int i = 0; i < 5; i++)//����5������������������ʶ�� 
	{
		glColor3f(RGB[i][0], RGB[i][1], RGB[i][2]);//����ɫ������ѡȡ��ɫ
		glBegin(GL_LINE_LOOP);//��ʼ��ͼ����ӦglEnd()
		for (int j = 0; j < 3; j++)
			glVertex2i(Tri[i].a[j][0], Tri[i].a[j][1]);
		glEnd();//������ͼ����ӦglBegin()
	}
	glFlush();//ǿ��ˢ�»��壬��֤��ͼ��ִ��
}

int main(int argc, char** argv)
{
	InitTriangle(Tri);
	glutInit(&argc, argv);//��ʼ��GLUT��
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);//����ͼ����ʾģʽ
	glutInitWindowPosition(100, 100);//�����ڴ��ڵ�����λ��
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);//���ô��ڴ�С
	gluOrtho2D(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_WIDTH / 2, WINDOW_WIDTH / 2);//������ʾ����
	glutCreateWindow("�����εĹ�դ���㷨"); //��������
	glMatrixMode(GL_PROJECTION);//����͸��
	glLoadIdentity();//�ָ�ԭʼ����ϵ
	glClearColor(1.0, 1.0, 1.0, 1.0);//�����ɫ����Ϊ��ɫ
	glutDisplayFunc(&display);//����һ������
	glutMainLoop();//����GLUT�¼�����ѭ��
	system("pause");
	return 0;
}