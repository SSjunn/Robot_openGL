/*
#define STB_IMAGE_IMPLEMENTATION

#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>

using namespace std;

//�κ� ������ ���� ��ȭ��
#define STEP 0.1
//�κ� ��ü �����ϱ�
#define TORSO_HEIGHT 6.0
#define TORSO_RADIUS 2.0
#define UPPER_ARM_HEIGHT 3.0
#define UPPER_ARM_RADIUS  0.5
#define LOWER_ARM_HEIGHT 2.5
#define LOWER_ARM_RADIUS  0.5
#define UPPER_LEG_HEIGHT 4.0
#define UPPER_LEG_RADIUS  0.6
#define LOWER_LEG_HEIGHT 3.5
#define LOWER_LEG_RADIUS  0.6
#define ELBOW_RADIUS 0.6
#define KNEE_RADIUS 0.7
#define HAND_RADIUS 0.6
#define FOOT_RADIUS 0.7
#define SHOULDER_RADIUS 0.8
#define HEAD_HEIGHT 1.5
#define HEAD_RADIUS 1.0
#define NECK_HEIGHT 1.0
#define NECK_RADIUS 0.4

//��� �����
#define NULL 0



int option = -1; // �޴����� ������ �ִϸ��̼� ���� ��Ÿ����
int animation = -3; //���� ���� ���̰ų� ������ �ִϸ��̼� ���� / �ʱⰪ�� -3���� ����
int done = 0; // �ִϸ��̼��� �Ϸ�Ǿ����� Ȯ���ϴ� �÷��� ���� , 0�� �Ϸ� �ȵȰž�
double rotate = 0; // ȸ������ �����ϴ� ����, ��ü�� ȸ�� ����
double horizontal = 0; // ���� �̵����� �����ϴ� ����, ��ü�� ���� ��ġ�� ��Ÿ��

void head(); //�Ӹ� �׸���
void torso(); // ���� �׸���
void left_upper_arm(); // ���� ���(���� ��) �׸���
void right_upper_arm(); // ������ ���(���� ��) �׸���
void left_upper_leg(); // ���� ���� (���� �ٸ�) �׸���
void right_upper_leg(); // ������ ���� ( ���� �ٸ�)

//�ڷ��� point ����
//float Ÿ�� �迭�� 3���� ���(x,y,z ��ǥ)�� ������ �迭�� point��� �̸����� ����
// �ַ� 3D �������� ��ǥ�� ǥ���ϴµ� ���
typedef float point[3]; 


typedef struct treenode // Ʈ�� ��� ����ü �����ϱ�
{
	GLfloat m[16]; // 4x4 ��ȯ ����� �����ϱ� ���� �迭(OpenGL���� ��ǥ ��ȯ�� ���)
	void(*f)(); // ��尡 ������ �Լ��� ������ (�� : ��ü ������ �׸��� �Լ�)
	struct treenode* sibling; // ���� ����(������ ����)�� �ִ� ���� ��带 ����Ű�� ������
	struct treenode* child; // ���� ����(�μӵ� ����)�� �ִ� ù ��° �ڽ� ��带 ����Ű�� ������
}treenode;

typedef treenode* t_ptr;

static GLfloat theta[11] = { 0.0,0.0,0.0,180.0,0.0,180.0,0.0,
180.0,0.0,180.0,0.0 }; // ��ü �� ������ �ʱ� ȸ�� ���� �迭

// ȸ���� �����ϴ� �� ���Ǵ� ����
// Ư�� ������ ������ �����ϰų� ȸ���� �� ���
static GLint angle = 2;


//GLUquadricObj : ��, ��, ���� ���� �׸��°�
GLUquadricObj* t, * h, * lua, * lla, * rua, * rla, * lll, * rll, * rul, * lul;
// ����(t) �Ӹ�(h) ���� �� ��(Lua) ���� �Ʒ� ��(lla) ������ ����(rua) ������ �Ʒ� ��(rla) ���� �� �ٸ�(lll) ������ �� �ٸ�(rll) ������ �Ʒ� �ٸ�(rul) ���� �Ʒ� �ٸ�(lul)
GLUquadricObj* relb, * lelb, * rknee, * lknee, * nk, * lhand, * rhand, * lfoot, * rfoot, * rsh, * lsh;
// relb: ������ �Ȳ�ġ (right elbow)
//lelb: ���� �Ȳ�ġ (left elbow)
// rknee: ������ ���� (right knee)
// lknee: ���� ���� (left knee)
// nk: �� (neck)
// lhand: �޼� (left hand)
// rhand: ������ (right hand)
// lfoot: �޹� (left foot)
// rfoot: ������ (right foot)
// rsh: ������ ��� (right shoulder)
// lsh: ���� ��� (left shoulder)

//��ü ��ü ���� �⺻ ũ�� �����ϱ�
double size = 1.0;


treenode torso_node, head_node, lua_node, rua_node, lll_node, rll_node,
lla_node, rla_node, rul_node, lul_node,
relb_node, lelb_node, rknee_node, lknee_node, nk_node, lhand_node, rhand_node, lfoot_node, rfoot_node,
rsh_node, lsh_node;
// ��ü �� ������ ���������� �����ϱ� ���� treenode ����ü ������
// �� ���� Ư�� ��ü ������ ��Ÿ����, Ʈ�� ������ ����





void traverse(treenode* root)
{
	if (root == NULL) return; // ���� ���� : ��Ʈ�� NULL �̸� �Լ� ����
	glPushMatrix(); // ���� ��ȯ ����� ���ÿ� ����
	glMultMatrixf(root->m); //���� ��ȯ ��Ŀ� ����� ��ȯ ��� 'm'�� ����
	root->f(); // ���� ����� �۾��� �����ϱ�
	if (root->child != NULL) traverse(root->child); // �ڽ� ��尡 ������ �ڽ� ��带 ��������� Ž��
	glPopMatrix(); // ���ÿ��� ���� ��ȯ ����� ���� (�ڽ� ���� ���� ��ȯ ���� ����)
	if (root->sibling != NULL) traverse(root->sibling); // ���� ��尡 ������ ���� ��带 ��������� Ž��
}

void torso()
{
	glPushMatrix();
	// ���� ��ȯ ����� ���ÿ� ����.
    // ������ �׸��Ⱑ �ٸ� ��ü ������ ������ ��ġ�� �ʵ��� ���� ���¸� ����.
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// ������ -90�� ȸ��.
    // �� ȸ���� OpenGL ��ǥ�迡�� �⺻������ z�� �������� �����Ǵ� ������ ���� ����(y��)���� ��ġ�ϱ� ���� �ʿ�.
    // �Ķ����: 
    //   -90.0: ȸ�� ����
    //   1.0, 0.0, 0.0: ȸ�� �� (x�� �������� ȸ��)
	gluCylinder(t, TORSO_RADIUS / 1.2, TORSO_RADIUS, TORSO_HEIGHT, 10, 10);
	// OpenGL�� GLU ���̺귯���� ����Ͽ� ������ ����.
	// �Ķ����:
	//   t: GLUquadricObj ��ü��, ���� �׸��⸦ ����.
	//   TORSO_RADIUS / 1.2: ������ �Ʒ��� ������ (������ ���� �Ʒ��� �κ� ǥ��)
	//   TORSO_RADIUS: ������ ���� ������ (������ ���� ���� �κ� ǥ��)
	//   TORSO_HEIGHT: ������ ���� (������ ���� ����)
	//   10: ������ ���� ���� ���׸�Ʈ ���� (�޽��� ���е� ����)
	//   10: ������ ���� ���� ���׸�Ʈ ���� (�޽��� ���е� ����)
	glPopMatrix();
	// ���ÿ��� ���� ��ȯ ����� ����.
    // `glPushMatrix` ������ ���·� ���ư�, ���� �׸��Ⱑ �ٸ� ��ü�� ������ ��ġ�� �ʵ��� ��.
}

void head()
{
	glPushMatrix();
	// ���� ��ȯ ����� ���ÿ� ����. 
	// �Ӹ� �κ��� ��ȯ�� �ٸ� ��ü ������ ������ ���� �ʵ��� ����.
	glTranslatef(0.0, HEAD_HEIGHT, 0.0);
	// �Ӹ� ��ġ�� �̵�.
	// (0.0, HEAD_HEIGHT, 0.0) ��ġ�� y�� �������� HEAD_HEIGHT��ŭ �̵�.
	// �̴� �Ӹ��� ���� ���� ��ġ�ǵ��� ��.
	glScalef(HEAD_RADIUS, HEAD_HEIGHT, HEAD_RADIUS);
	// �Ӹ��� ũ�⸦ ����.
	// x��� z�� �������� HEAD_RADIUS ũ��, y�� �������� HEAD_HEIGHT ũ�⸦ ����.
	// �� ���¸� Ÿ�������� ��ȯ.
	gluSphere(h, HEAD_RADIUS, 10, 10);
	// ��ü(�Ӹ�)�� ����.
	// �Ķ����:
	//   - h: GLUquadricObj* ��ü�� ���� �׸��� �� ���.
	//   - HEAD_RADIUS: ���� ������.
	//   - 10, 10: ���׸�Ʈ ������, ���� �Ų������� ���� (���� �� ���� ���׸�Ʈ).
	
	//���۶�
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// x�� �������� -90�� ȸ��.
	// ���� z�� �߽ɿ� ����(�Ȱ� �ٸ�)�� ���� �������� ��ġ.
	glTranslatef(0.0f, -0.4f, -0.15f);
	// �Ȱ� �ٸ��� ��ġ�� �ణ ����.
	// y������ -0.4f, z������ -0.15f �̵��Ͽ� �Ӹ��� �ڿ������� ��ġ�ϵ��� ����.
	gluCylinder(h, 0.9 * HEAD_RADIUS, 0.9 * HEAD_RADIUS, HEAD_HEIGHT / 5, 10, 10);
	// ����(�Ȱ� �ٸ�)�� ����.
	// �Ķ����:
	// h: GLUquadricObj* ��ü�� ������ �׸��� �� ���.
	// 0.9 * HEAD_RADIUS: ������ �Ʒ��� �� ���� ������.
	// HEAD_HEIGHT / 5: ������ ���� (�Ȱ� �ٸ��� ����).
	// 10, 10: ������ ���׸�Ʈ ������ �Ų������� ����.
	glPopMatrix();
}

void neck()
{
	glPushMatrix();
	// ���� ��ȯ ����� ���ÿ� ����
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// ���� -90���� ȸ���Ͽ� z���� ���� �����Ǵ� ������ ���� ����(y)�� ��ġ
	gluCylinder(nk, NECK_RADIUS, NECK_RADIUS, NECK_HEIGHT, 10, 10);
	// �������� ���� ����.
	// �Ķ����:
	//   nk: GLUquadricObj* ��ü�� ������ �׸��� ���� ����.
	//   NECK_RADIUS: ���� ������.
	//   NECK_HEIGHT: ���� ����.
	//   10, 10: ������ �޽� ���׸�Ʈ ����.
	glPopMatrix();
	// ���ÿ��� ���� ��ȯ ����� ��
}

void rightShoulder()
{
	glPushMatrix();
	gluSphere(relb, SHOULDER_RADIUS, 10, 10);
	// ��ü�� ������ ����� ����.
	// relb: GLUquadricObj* ��ü.
	// SHOULDER_RADIUS: ����� ������.
	glPopMatrix();
}

void leftShoulder()
{
	glPushMatrix();
	gluSphere(lelb, SHOULDER_RADIUS, 10, 10);
	// ��ü�� ���� ����� ����.
	glPopMatrix();
}

void rightElbow()
{
	glPushMatrix();
	gluSphere(relb, ELBOW_RADIUS, 10, 10);
	// ��ü�� ������ �Ȳ�ġ�� ����.
	// ELBOW_RADIUS: �Ȳ�ġ�� ������.
	glPopMatrix();
}

void leftElbow()
{
	glPushMatrix();
	gluSphere(lelb, ELBOW_RADIUS, 10, 10);
	// ��ü�� ���� �Ȳ�ġ�� ����.
	glPopMatrix();
}

void rightKnee()
{
	glPushMatrix();
	gluSphere(rknee, KNEE_RADIUS, 10, 10);
	// ��ü�� ������ ������ ����.
	// KNEE_RADIUS: ������ ������.
	glPopMatrix();
}

void leftKnee()
{
	glPushMatrix();
	gluSphere(lknee, KNEE_RADIUS, 10, 10);
	// ��ü�� ���� ������ ����.
	glPopMatrix();
}

void leftFoot()
{
	glPushMatrix();
	gluSphere(lknee, FOOT_RADIUS, 10, 10);
	// ��ü�� ���� ���� ����.
	// FOOT_RADIUS: ���� ������.
	glPopMatrix();
}

void rightFoot()
{
	glPushMatrix();
	gluSphere(lknee, FOOT_RADIUS, 10, 10);
	// ��ü�� ������ ���� ����.
	glPopMatrix();
}

void rightHand()
{
	glPushMatrix();
	gluSphere(lknee, HAND_RADIUS, 10, 10);
	// ��ü�� ������ ���� ����.
	// HAND_RADIUS: ���� ������.
	glPopMatrix();
}

void leftHand()
{
	glPushMatrix();
	gluSphere(lknee, HAND_RADIUS, 10, 10);
	// ��ü�� ���� ���� ����.
	glPopMatrix();

}

void left_upper_arm()
{
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// ������ ���� �������� ��ġ.
	gluCylinder(lua, UPPER_ARM_RADIUS, UPPER_ARM_RADIUS - 0.1, UPPER_ARM_HEIGHT, 10, 10);
	// �������� ���� ������ ����.
	// lua: GLUquadricObj* ��ü.
	// UPPER_ARM_RADIUS: ������ ���κ� ������.
	// UPPER_ARM_RADIUS - 0.1: �Ʒ��� ������ (���� ������).
	// UPPER_ARM_HEIGHT: ������ ����.
	glPopMatrix();
}

void left_lower_arm()
{
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// ������ ���� �������� ��ġ.
	gluCylinder(lla, LOWER_ARM_RADIUS - 0.1, LOWER_ARM_RADIUS - 0.15, LOWER_ARM_HEIGHT, 10, 10);
	// �������� ���� �Ʒ����� ����.
	// lla: GLUquadricObj* ��ü.
	// LOWER_ARM_RADIUS - 0.1: �Ʒ����� ���κ� ������.
	// LOWER_ARM_RADIUS - 0.15: �Ʒ����� �Ʒ��κ� ������ (���� �� ������).
	// LOWER_ARM_HEIGHT: �Ʒ����� ����.
	glPopMatrix();
}

//������ ������ �׸��� �Լ�
void right_upper_arm()
{
	glPushMatrix(); // ���� ��ȯ ����� ����
	glRotatef(-90.0, 1.0, 0.0, 0.0); // x���� �������� -90�� ȸ��
	gluCylinder(rua, UPPER_ARM_RADIUS, UPPER_ARM_RADIUS - 0.1, UPPER_ARM_HEIGHT, 10, 10);
	//������� �׸���, ���� ������(UPPER_ARM_RADIUS), �� ������(UPPER_ARM_RADIUS - 0.1), 
    // ����(UPPER_ARM_HEIGHT), ���� ����ȭ(10), ���� ����ȭ(10)
	glPopMatrix(); // ����� ��ķ� ����
}

//������ �Ʒ����� �׸��� �Լ�
void right_lower_arm()
{
	glPushMatrix(); // ���� ��ȯ ����� ����
	glRotatef(-90.0, 1.0, 0.0, 0.0); // x���� �������� -90�� ȸ��
	gluCylinder(rla, LOWER_ARM_RADIUS - 0.1, LOWER_ARM_RADIUS - 0.15, LOWER_ARM_HEIGHT, 10, 10);
	// ������� �׸���, ���� ������(LOWER_ARM_RADIUS - 0.1), �� ������(LOWER_ARM_RADIUS - 0.15),
    // ����(LOWER_ARM_HEIGHT), ���� ����ȭ(10), ���� ����ȭ(10)
	glPopMatrix(); //����� ��ķ� ����
}

//���� ����ٸ��� �׸��� �Լ�
void left_upper_leg()
{
	glPushMatrix(); //���� ��ȯ ����� ����
	glRotatef(-90.0, 1.0, 0.0, 0.0); // x���� �������� -90�� ȸ��
	gluCylinder(lul, UPPER_LEG_RADIUS, UPPER_LEG_RADIUS - 0.1, UPPER_LEG_HEIGHT, 10, 10);
	// ������� �׸���, ���� ������(UPPER_LEG_RADIUS), �� ������(UPPER_LEG_RADIUS - 0.1),
    // ����(UPPER_LEG_HEIGHT), ���� ����ȭ(10), ���� ����ȭ(10)
	glPopMatrix(); // ����� ��ķ� ����
}

//���� ���Ƹ����� �׸��� �Լ�
void left_lower_leg()
{
	glPushMatrix(); // ���� ��ȯ ����� ����
	glRotatef(-90.0, 1.0, 0.0, 0.0); //x���� �������� -90�� ȸ��
	gluCylinder(lll, LOWER_LEG_RADIUS - 0.1, LOWER_LEG_RADIUS - 0.2, LOWER_LEG_HEIGHT, 10, 10);
	// ������� �׸���, ���� ������(LOWER_LEG_RADIUS - 0.1), �� ������(LOWER_LEG_RADIUS - 0.2),
	// ����(LOWER_LEG_HEIGHT), ���� ����ȭ(10), ���� ����ȭ(10)
	glPopMatrix(); //����� ��ķ� ����
}

//������ ����ٸ��� �׸��� �Լ�
void right_upper_leg()
{
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(rul, UPPER_LEG_RADIUS, UPPER_LEG_RADIUS - 0.1, UPPER_LEG_HEIGHT, 10, 10);
	glPopMatrix();
}
//������ ���Ƹ��� �׸��� �Լ�
void right_lower_leg()
{
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(rll, LOWER_LEG_RADIUS - 0.1, LOWER_LEG_RADIUS - 0.2, LOWER_LEG_HEIGHT, 10, 10);
	glPopMatrix();
}

void display(void)
{
	glMatrixMode(GL_PROJECTION);
	// ���� ��� ���� ��ȯ.
	// ���� ����� 3D ��ǥ�� 2D ȭ�鿡 �����ϴ� ����� ����.
	glLoadIdentity();
	// ���� ����� �ʱ�ȭ.
	// ������ ����� ��ȯ�� �����Ͽ� �⺻ ���·� ����.
	glOrtho(-15, 15, -15, 15, -15, 15);
	// ���� ���� ����.
	// ȭ�鿡 ǥ�õǴ� ������ ũ�⸦ ����:
	// - x��: [-15, 15]
	// - y��: [-15, 15]
	// - z��: [-15, 15]
	// ���� ������ ���ٰ� ���� ��� ��ü�� ������ ũ��� ǥ��.
	glRotatef(rotate, 0, 1, 0);
	// y���� �������� `rotate`��ŭ ȸ��.
	// `rotate`�� �ܺο��� ����� �� �ִ� ���� ������, �ִϸ��̼��̳� ����� �Է��� ���� ȸ������ ����.
	glMatrixMode(GL_MODELVIEW);
	// �𵨺� ��� ���� ��ȯ.
	// ������ ��ȯ�� ��ü�� ��ġ�� ���⿡ ������ ��ħ.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// ȭ���� �ʱ�ȭ.
	// GL_COLOR_BUFFER_BIT: ȭ���� ���� ���۸� ����.
	// GL_DEPTH_BUFFER_BIT: ���� ����(3D ��ü�� ���� ����)�� ����.
	glLoadIdentity();
	// �𵨺� ����� �ʱ�ȭ�Ͽ� �⺻ ���·� ����.
	glColor3f(0.4, 0.4, 0.4);
	// ������ ������ ����.
	// RGB ���� ���� 0.4�� �����Ͽ� ȸ������ ������.

	traverse(&torso_node);
	// Ʈ�� ������ ��Ʈ ����� `torso_node`���� Ʈ���� ��ȸ�ϸ� ��ü ������ ������.
	// `traverse` �Լ��� ������ Ʈ�� ������ ���� ��������� ��� ��ü ������ �׸�.
	glutSwapBuffers();
	// ������ ���۸� ��ü.
	// ���� ���۸��� ����Ͽ� �������� �������� ȭ�鿡 ���.
	// ���� ȭ���� �����̴� ������ ����.
}

//������ ���� �Լ�
void inverseKinematics()
{
	switch (animation) // �ִϸ��̼� ���¿� ���� �ٸ� ���� ����
	{
	case 0: //�ִϸ��̼� ���°� 0�� ���
		//������, ����, ���� ������ ����
		if (theta[5] < 180.0) // �������� ������ 180�� ������ ���
		{
			theta[5] += STEP; //������ ������ ����
			theta[3] += STEP; // ���� ������ ����
			theta[1] -= 0.2 * STEP; // �������� ������ 180�� ������ ���
		}
		else animation = option; //an exei antistrafei tote eimaste stin arxiki thesi kai to neo animation einai to option

		glPushMatrix();

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[5], 1.0, 0.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rua_node.m);

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[3], 1.0, 0.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, lua_node.m);

		glLoadIdentity();
		glTranslatef(0.0, TORSO_HEIGHT - 0.25 * NECK_HEIGHT, 0.0);
		glRotatef(theta[1], 1.0, 0.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, nk_node.m);

		glPopMatrix();
		break;
	case 1:
		if (theta[9] < 180.0)
		{
			theta[9] += STEP;
			theta[10] -= STEP;
		}
		else animation = option;

		glPushMatrix();

		glLoadIdentity();
		glTranslatef(TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
		glRotatef(theta[9], 1.0, 0.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rul_node.m);

		glLoadIdentity();
		glTranslatef(0.0, UPPER_LEG_HEIGHT, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rknee_node.m);

		glLoadIdentity();
		glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
		glRotatef(theta[10], 1.0, 0.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rll_node.m);

		glPopMatrix();
		break;
	case 6: case -1:
		if (theta[9] < 180.0)
		{
			theta[9] += STEP;
			theta[10] -= STEP;
			theta[7] += STEP;
			theta[8] -= STEP;
			theta[5] -= 1.3 * STEP;
			theta[6] += STEP;
			theta[3] += STEP;
			theta[4] -= STEP;
			horizontal -= 0.03 * STEP;
		}
		else animation = option;

		glPushMatrix();

		glLoadIdentity();
		glTranslatef(TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
		glRotatef(theta[9], 1.0, 0.0, -1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rul_node.m);

		glLoadIdentity();
		glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
		glRotatef(theta[10], 1.0, 0.0, -1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rll_node.m);

		glLoadIdentity();
		glTranslatef(0, horizontal, 0);
		glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[5], -1.0, 0.0, -1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rua_node.m);

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[6], 1.0, 1.0, 1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rla_node.m);

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[3], 1.0, 0.0, -1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, lua_node.m);

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[4], -1.0, -1.0, 1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, lla_node.m);

		glPopMatrix();
		break;
	case 7: case -2:
		if (theta[9] < 180.0)
		{
			theta[9] += STEP;
			theta[10] -= STEP;
			theta[7] += STEP;
			theta[8] -= STEP;
			theta[5] -= STEP;
			theta[6] += STEP;
			theta[3] += STEP;
			theta[4] -= STEP;
			horizontal -= 0.03 * STEP;
		}
		else animation = option;

		glPushMatrix();

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[5], 0.0, 0.0, 1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rua_node.m);

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[6], 0.0, 0.0, 1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rla_node.m);

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[3], 0.0, 0.0, 1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, lua_node.m);

		glLoadIdentity();
		glTranslatef(0, 0, 0.0);
		glRotatef(theta[4], 0.0, 0.0, 1.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, lla_node.m);

		glLoadIdentity();
		glTranslatef(TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
		glRotatef(theta[9], 1.0, 0.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rul_node.m);

		glLoadIdentity();
		glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
		glRotatef(theta[10], 1.0, 0.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, rll_node.m);

		glLoadIdentity();
		glTranslatef(-TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
		glRotatef(theta[7], 1.0, 0.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, lul_node.m);

		glLoadIdentity();
		glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
		glRotatef(theta[8], 1.0, 0.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, lll_node.m);

		glLoadIdentity();
		glTranslatef(0, -horizontal, -horizontal);
		glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);

		glPopMatrix();
		break;
	default: animation = option;
	}
}

void idle()
{
	switch (option)
	{
	case 0:
		if (animation != option) inverseKinematics(); //an to teleytaio animation einai diaforetiko antistrepse to
		else if (!done) //kane animation mexri na oloklirwthei
		{
			if (theta[5] > 30.0)
			{
				theta[5] -= STEP;
				theta[3] -= STEP;
				theta[1] += 0.2 * STEP;
			}
			else done = 1;

			glPushMatrix();

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[5], 1.0, 0.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rua_node.m);

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[3], 1.0, 0.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, lua_node.m);

			glLoadIdentity();
			glTranslatef(0.0, TORSO_HEIGHT - 0.25 * NECK_HEIGHT, 0.0);
			glRotatef(theta[1], 1.0, 0.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, nk_node.m);

			glPopMatrix();
		}
		break;
	case 1:
		if (animation != option) inverseKinematics();
		else if (!done)
		{
			if (theta[9] > 110.0)
			{
				theta[9] -= STEP;
				theta[10] += STEP;
			}
			else done = 1;

			glPushMatrix();

			glLoadIdentity();
			glTranslatef(TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
			glRotatef(theta[9], 1.0, 0.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rul_node.m);

			glLoadIdentity();
			glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
			glRotatef(theta[10], 1.0, 0.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rll_node.m);

			glLoadIdentity();
			glTranslatef(0, -horizontal, -horizontal);
			glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);

			glPopMatrix();
		}
		break;
	case 2:
		inverseKinematics();
		break;
	case 6:
		if (animation != option) inverseKinematics();
		else
		{
			if (theta[9] > 130.0)
			{
				theta[9] -= STEP;
				theta[10] += STEP;
				theta[7] -= STEP;
				theta[8] += STEP;
				theta[5] += 1.3 * STEP;
				theta[6] -= STEP;
				theta[3] -= STEP;
				theta[4] += STEP;
				horizontal += 0.03 * STEP;
			}
			else animation = -1;

			glPushMatrix();

			glLoadIdentity();
			glTranslatef(TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
			glRotatef(theta[9], 1.0, 0.0, -1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rul_node.m);

			glLoadIdentity();
			glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
			glRotatef(theta[10], 1.0, 0.0, -1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rll_node.m);

			glLoadIdentity();
			glTranslatef(0, horizontal, 0);
			glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[5], -1.0, 0.0, -1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rua_node.m);

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[6], 1.0, 1.0, 1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rla_node.m);

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[3], 1.0, 0.0, -1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, lua_node.m);

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[4], -1.0, -1.0, 1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, lla_node.m);

			glPopMatrix();
		}
		break;
	case 7:
		if (animation != option) inverseKinematics();
		else
		{
			if (theta[9] > 130.0)
			{
				theta[9] -= STEP;
				theta[10] += STEP;
				theta[7] -= STEP;
				theta[8] += STEP;
				theta[5] += STEP;
				theta[6] -= STEP;
				theta[3] -= STEP;
				theta[4] += STEP;
				horizontal += 0.03 * STEP;
			}
			else animation = -2;

			glPushMatrix();

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[5], 0.0, 0.0, 1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rua_node.m);

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[6], 0.0, 0.0, 1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rla_node.m);

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[3], 0.0, 0.0, 1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, lua_node.m);

			glLoadIdentity();
			glTranslatef(0, 0, 0.0);
			glRotatef(theta[4], 0.0, 0.0, 1.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, lla_node.m);

			glLoadIdentity();
			glTranslatef(TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
			glRotatef(theta[9], 1.0, 0.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rul_node.m);

			glLoadIdentity();
			glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
			glRotatef(theta[10], 1.0, 0.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, rll_node.m);

			glLoadIdentity();
			glTranslatef(-TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
			glRotatef(theta[7], 1.0, 0.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, lul_node.m);

			glLoadIdentity();
			glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
			glRotatef(theta[8], 1.0, 0.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, lll_node.m);

			glLoadIdentity();
			glTranslatef(0, -horizontal, -horizontal);
			glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);

			glPopMatrix();
		}
		break;
	}
	glutPostRedisplay();
}

void menu(int id)
{
	option = id;
	done = 0;
	if (id == 3) rotate = -90;
	if (id == 4) rotate = 0;
	if (id == 5) rotate = -30;
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-10.0, 10.0, -10.0 * (GLfloat)h / (GLfloat)w,
			10.0 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-10.0 * (GLfloat)w / (GLfloat)h,
			10.0 * (GLfloat)w / (GLfloat)h, 0.0, 10.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void myinit()
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess = { 100.0 };
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 10.0, 10.0, 10.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(1.0, 0.0, 0.0);

	//allocate quadrics with filled drawing style

	h = gluNewQuadric();
	gluQuadricDrawStyle(h, GLU_FILL);
	t = gluNewQuadric();
	gluQuadricDrawStyle(t, GLU_FILL);
	lua = gluNewQuadric();
	gluQuadricDrawStyle(lua, GLU_FILL);

	lelb = gluNewQuadric();
	gluQuadricDrawStyle(lelb, GLU_FILL);
	lla = gluNewQuadric();
	gluQuadricDrawStyle(lla, GLU_FILL);
	rua = gluNewQuadric();
	gluQuadricDrawStyle(rua, GLU_FILL);
	rla = gluNewQuadric();
	gluQuadricDrawStyle(rla, GLU_FILL);
	lul = gluNewQuadric();
	gluQuadricDrawStyle(lul, GLU_FILL);
	lll = gluNewQuadric();
	gluQuadricDrawStyle(lll, GLU_FILL);
	rul = gluNewQuadric();
	gluQuadricDrawStyle(rul, GLU_FILL);
	rll = gluNewQuadric();
	gluQuadricDrawStyle(rll, GLU_FILL);
	rknee = gluNewQuadric();
	gluQuadricDrawStyle(rknee, GLU_FILL);
	lknee = gluNewQuadric();
	gluQuadricDrawStyle(lknee, GLU_FILL);
	relb = gluNewQuadric();
	gluQuadricDrawStyle(relb, GLU_FILL);
	nk = gluNewQuadric();
	gluQuadricDrawStyle(nk, GLU_FILL);
	rhand = gluNewQuadric();
	gluQuadricDrawStyle(rhand, GLU_FILL);
	lhand = gluNewQuadric();
	gluQuadricDrawStyle(lhand, GLU_FILL);
	lfoot = gluNewQuadric();
	gluQuadricDrawStyle(lfoot, GLU_FILL);
	rfoot = gluNewQuadric();
	gluQuadricDrawStyle(rfoot, GLU_FILL);
	rsh = gluNewQuadric();
	gluQuadricDrawStyle(rsh, GLU_FILL);
	lsh = gluNewQuadric();
	gluQuadricDrawStyle(lsh, GLU_FILL);

	//Set up tree

	glLoadIdentity();
	glRotatef(theta[0], 0.0, 1.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, torso_node.m);
	torso_node.f = torso;
	torso_node.sibling = NULL;
	torso_node.child = &nk_node;

	glLoadIdentity();
	glTranslatef(0.0, TORSO_HEIGHT - 0.25 * NECK_HEIGHT, 0.0);
	glRotatef(theta[1], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, nk_node.m);
	nk_node.f = neck;
	nk_node.sibling = &lsh_node;
	nk_node.child = &head_node;

	glLoadIdentity();
	glTranslatef(-(TORSO_RADIUS + UPPER_ARM_RADIUS), 0.9 * TORSO_HEIGHT, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, lsh_node.m);
	lsh_node.f = leftShoulder;
	lsh_node.sibling = &rsh_node;
	lsh_node.child = &lua_node;

	glLoadIdentity();
	glTranslatef(TORSO_RADIUS + UPPER_ARM_RADIUS, 0.9 * TORSO_HEIGHT, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, rsh_node.m);
	rsh_node.f = rightShoulder;
	rsh_node.sibling = &lul_node;
	rsh_node.child = &rua_node;

	glLoadIdentity();
	glTranslatef(0.0, 0.75 * NECK_HEIGHT, 0.0);
	glRotatef(theta[2], 0.0, 1.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, head_node.m);
	head_node.f = head;
	head_node.sibling = NULL;
	head_node.child = NULL;

	glLoadIdentity();
	glTranslatef(0, 0, 0.0);
	glRotatef(theta[3], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, lua_node.m);
	lua_node.f = left_upper_arm;
	lua_node.sibling = NULL;
	lua_node.child = &lelb_node;

	glLoadIdentity();
	glTranslatef(0.0, UPPER_ARM_HEIGHT, 0.0);
	glRotatef(theta[4], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, lelb_node.m);
	lelb_node.f = leftElbow;
	lelb_node.sibling = NULL;
	lelb_node.child = &lla_node;

	glLoadIdentity();
	glTranslatef(0, 0, 0.0);
	glRotatef(theta[5], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, rua_node.m);
	rua_node.f = right_upper_arm;
	rua_node.sibling = NULL;
	rua_node.child = &relb_node;

	glLoadIdentity();
	glTranslatef(0.0, UPPER_ARM_HEIGHT, 0.0);
	glRotatef(theta[4], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, relb_node.m);
	relb_node.f = rightElbow;
	relb_node.sibling = NULL;
	relb_node.child = &rla_node;

	glLoadIdentity();
	glTranslatef(-TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
	glRotatef(theta[7], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, lul_node.m);
	lul_node.f = left_upper_leg;
	lul_node.sibling = &rul_node;
	lul_node.child = &lknee_node;

	glLoadIdentity();
	glTranslatef(TORSO_RADIUS / 2, 0.1 * UPPER_LEG_HEIGHT, 0.0);
	glRotatef(theta[9], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, rul_node.m);
	rul_node.f = right_upper_leg;
	rul_node.sibling = NULL;
	rul_node.child = &rknee_node;

	glLoadIdentity();
	glTranslatef(0.0, ELBOW_RADIUS / 2, 0.0);
	glRotatef(theta[4], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, lla_node.m);
	lla_node.f = left_lower_arm;
	lla_node.sibling = NULL;
	lla_node.child = &lhand_node;

	glLoadIdentity();
	glTranslatef(0.0, ELBOW_RADIUS / 2, 0.0);
	glRotatef(theta[4], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, rla_node.m);
	rla_node.f = right_lower_arm;
	rla_node.sibling = NULL;
	rla_node.child = &rhand_node;

	glLoadIdentity();
	glTranslatef(0.0, LOWER_ARM_HEIGHT, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, rhand_node.m);
	rhand_node.f = rightHand;
	rhand_node.sibling = NULL;
	rhand_node.child = NULL;

	glLoadIdentity();
	glTranslatef(0.0, LOWER_ARM_HEIGHT, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, lhand_node.m);
	lhand_node.f = leftHand;
	lhand_node.sibling = NULL;
	lhand_node.child = NULL;

	glLoadIdentity();
	glTranslatef(0.0, UPPER_LEG_HEIGHT, 0.0);
	glRotatef(theta[10], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, lknee_node.m);
	lknee_node.f = leftKnee;
	lknee_node.sibling = NULL;
	lknee_node.child = &lll_node;

	glLoadIdentity();
	glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
	glRotatef(theta[8], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, lll_node.m);
	lll_node.f = left_lower_leg;
	lll_node.sibling = NULL;
	lll_node.child = &lfoot_node;

	glLoadIdentity();
	glTranslatef(0.0, LOWER_LEG_HEIGHT, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, lfoot_node.m);
	lfoot_node.f = leftFoot;
	lfoot_node.sibling = NULL;
	lfoot_node.child = NULL;

	glLoadIdentity();
	glTranslatef(0.0, UPPER_LEG_HEIGHT, 0.0);
	glRotatef(theta[10], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, rknee_node.m);
	rknee_node.f = rightKnee;
	rknee_node.sibling = NULL;
	rknee_node.child = &rll_node;

	glLoadIdentity();
	glTranslatef(0.0, KNEE_RADIUS / 2, 0.0);
	glRotatef(theta[10], 1.0, 0.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, rll_node.m);
	rll_node.f = right_lower_leg;
	rll_node.sibling = NULL;
	rll_node.child = &rfoot_node;

	glLoadIdentity();
	glTranslatef(0.0, LOWER_LEG_HEIGHT, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, rfoot_node.m);
	rfoot_node.f = rightFoot;
	rfoot_node.sibling = NULL;
	rfoot_node.child = NULL;

	glLoadIdentity();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Final-Project");
	myinit();
	glutReshapeFunc(myReshape);
	glutIdleFunc(idle);
	glutDisplayFunc(display);

	glutCreateMenu(menu);
	glutAddMenuEntry(" Raise Hands ", 0);
	glutAddMenuEntry(" Raise Leg ", 1);
	glutAddMenuEntry(" Dance ", 7);
	glutAddMenuEntry(" Gangnam Style Dance ", 6);
	glutAddMenuEntry(" Reset ", 2);
	glutAddMenuEntry(" Left View ", 3);
	glutAddMenuEntry(" Front View ", 4);
	glutAddMenuEntry(" 3/4 View ", 5);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}
*/