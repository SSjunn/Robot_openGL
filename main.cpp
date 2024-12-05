/*
#define STB_IMAGE_IMPLEMENTATION

#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>

using namespace std;

//로봇 관절의 각도 변화량
#define STEP 0.1
//로봇 신체 정의하기
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

//노드 지우기
#define NULL 0



int option = -1; // 메뉴에서 선택한 애니메이션 동작 나타내기
int animation = -3; //현재 실행 중이거나 마지막 애니메이션 상태 / 초기값은 -3으로 설정
int done = 0; // 애니메이션이 완료되었는지 확인하는 플래그 변수 , 0은 완료 안된거야
double rotate = 0; // 회전값을 저장하는 변수, 객체의 회전 상태
double horizontal = 0; // 수평 이동값을 저장하는 변수, 객체의 수평 위치를 나타냄

void head(); //머리 그리기
void torso(); // 몸통 그리기
void left_upper_arm(); // 왼쪽 상완(왼쪽 팔) 그리기
void right_upper_arm(); // 오른쪽 상완(위쪽 팔) 그리기
void left_upper_leg(); // 왼쪽 상퇴 (위쪽 다리) 그리기
void right_upper_leg(); // 오른쪽 상퇴 ( 위쪽 다리)

//자료형 point 선언
//float 타입 배열로 3개의 요소(x,y,z 좌표)를 가지는 배열을 point라는 이름으로 정의
// 주로 3D 공간에서 좌표를 표현하는데 사용
typedef float point[3]; 


typedef struct treenode // 트리 노드 구주체 정의하기
{
	GLfloat m[16]; // 4x4 변환 행렬을 저장하기 위해 배열(OpenGL에서 좌표 변환에 사용)
	void(*f)(); // 노드가 수행할 함수의 포인터 (예 : 신체 부위를 그리는 함수)
	struct treenode* sibling; // 동일 계층(수평적 관계)에 있는 다음 노드를 가리키는 포인터
	struct treenode* child; // 하위 계층(부속된 구조)에 있는 첫 번째 자식 노드를 가리키는 포인터
}treenode;

typedef treenode* t_ptr;

static GLfloat theta[11] = { 0.0,0.0,0.0,180.0,0.0,180.0,0.0,
180.0,0.0,180.0,0.0 }; // 신체 각 관절의 초기 회전 각도 배열

// 회전을 조정하는 데 사용되는 변수
// 특정 관절의 각도를 변경하거나 회전할 때 사용
static GLint angle = 2;


//GLUquadricObj : 원, 구, 원통 등을 그리는거
GLUquadricObj* t, * h, * lua, * lla, * rua, * rla, * lll, * rll, * rul, * lul;
// 몸통(t) 머리(h) 왼쪽 위 팔(Lua) 왼쪽 아래 팔(lla) 오른쪽 위팔(rua) 오른쪽 아래 팔(rla) 왼쪽 위 다리(lll) 오른쪽 위 다리(rll) 오른쪽 아래 다리(rul) 왼쪽 아래 다리(lul)
GLUquadricObj* relb, * lelb, * rknee, * lknee, * nk, * lhand, * rhand, * lfoot, * rfoot, * rsh, * lsh;
// relb: 오른쪽 팔꿈치 (right elbow)
//lelb: 왼쪽 팔꿈치 (left elbow)
// rknee: 오른쪽 무릎 (right knee)
// lknee: 왼쪽 무릎 (left knee)
// nk: 목 (neck)
// lhand: 왼손 (left hand)
// rhand: 오른손 (right hand)
// lfoot: 왼발 (left foot)
// rfoot: 오른발 (right foot)
// rsh: 오른쪽 어깨 (right shoulder)
// lsh: 왼쪽 어깨 (left shoulder)

//전체 신체 모델의 기본 크기 설정하기
double size = 1.0;


treenode torso_node, head_node, lua_node, rua_node, lll_node, rll_node,
lla_node, rla_node, rul_node, lul_node,
relb_node, lelb_node, rknee_node, lknee_node, nk_node, lhand_node, rhand_node, lfoot_node, rfoot_node,
rsh_node, lsh_node;
// 신체 각 부위를 계층적으로 구성하기 위한 treenode 구조체 변수들
// 각 노드는 특정 신체 부위를 나타내며, 트리 구조를 형성





void traverse(treenode* root)
{
	if (root == NULL) return; // 기조 조건 : 노트가 NULL 이면 함수 종료
	glPushMatrix(); // 현재 변환 행렬을 스택에 저장
	glMultMatrixf(root->m); //현재 변환 행렬에 노드의 변환 행렬 'm'을 곱함
	root->f(); // 현재 노드의 작업을 실행하기
	if (root->child != NULL) traverse(root->child); // 자식 노드가 있으면 자식 노드를 재귀적으로 탐색
	glPopMatrix(); // 스택에서 이전 변환 행렬을 복원 (자식 노드로 인한 변환 영향 제거)
	if (root->sibling != NULL) traverse(root->sibling); // 형제 노드가 있으면 형제 노드를 재귀적으로 탐색
}

void torso()
{
	glPushMatrix();
	// 현재 변환 행렬을 스택에 저장.
    // 몸통의 그리기가 다른 신체 부위에 영향을 미치지 않도록 현재 상태를 보존.
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// 몸통을 -90도 회전.
    // 이 회전은 OpenGL 좌표계에서 기본적으로 z축 방향으로 생성되는 원통을 세로 방향(y축)으로 배치하기 위해 필요.
    // 파라미터: 
    //   -90.0: 회전 각도
    //   1.0, 0.0, 0.0: 회전 축 (x축 기준으로 회전)
	gluCylinder(t, TORSO_RADIUS / 1.2, TORSO_RADIUS, TORSO_HEIGHT, 10, 10);
	// OpenGL의 GLU 라이브러리를 사용하여 원통을 생성.
	// 파라미터:
	//   t: GLUquadricObj 객체로, 원통 그리기를 정의.
	//   TORSO_RADIUS / 1.2: 원통의 아래쪽 반지름 (몸통의 좁은 아래쪽 부분 표현)
	//   TORSO_RADIUS: 원통의 위쪽 반지름 (몸통의 넓은 위쪽 부분 표현)
	//   TORSO_HEIGHT: 원통의 높이 (몸통의 세로 길이)
	//   10: 원통의 수평 방향 세그먼트 개수 (메쉬의 정밀도 결정)
	//   10: 원통의 수직 방향 세그먼트 개수 (메쉬의 정밀도 결정)
	glPopMatrix();
	// 스택에서 이전 변환 행렬을 복원.
    // `glPushMatrix` 이전의 상태로 돌아가, 몸통 그리기가 다른 객체에 영향을 미치지 않도록 함.
}

void head()
{
	glPushMatrix();
	// 현재 변환 행렬을 스택에 저장. 
	// 머리 부분의 변환이 다른 신체 부위에 영향을 주지 않도록 보존.
	glTranslatef(0.0, HEAD_HEIGHT, 0.0);
	// 머리 위치를 이동.
	// (0.0, HEAD_HEIGHT, 0.0) 위치로 y축 방향으로 HEAD_HEIGHT만큼 이동.
	// 이는 머리가 몸통 위에 배치되도록 함.
	glScalef(HEAD_RADIUS, HEAD_HEIGHT, HEAD_RADIUS);
	// 머리의 크기를 조정.
	// x축과 z축 방향으로 HEAD_RADIUS 크기, y축 방향으로 HEAD_HEIGHT 크기를 설정.
	// 구 형태를 타원형으로 변환.
	gluSphere(h, HEAD_RADIUS, 10, 10);
	// 구체(머리)를 생성.
	// 파라미터:
	//   - h: GLUquadricObj* 객체로 구를 그리는 데 사용.
	//   - HEAD_RADIUS: 구의 반지름.
	//   - 10, 10: 세그먼트 개수로, 구의 매끄러움을 결정 (수평 및 수직 세그먼트).
	
	//선글라스
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// x축 기준으로 -90도 회전.
	// 구의 z축 중심에 원통(안경 다리)을 세로 방향으로 배치.
	glTranslatef(0.0f, -0.4f, -0.15f);
	// 안경 다리의 위치를 약간 조정.
	// y축으로 -0.4f, z축으로 -0.15f 이동하여 머리에 자연스럽게 위치하도록 설정.
	gluCylinder(h, 0.9 * HEAD_RADIUS, 0.9 * HEAD_RADIUS, HEAD_HEIGHT / 5, 10, 10);
	// 원통(안경 다리)을 생성.
	// 파라미터:
	// h: GLUquadricObj* 객체로 원통을 그리는 데 사용.
	// 0.9 * HEAD_RADIUS: 원통의 아래쪽 및 위쪽 반지름.
	// HEAD_HEIGHT / 5: 원통의 높이 (안경 다리의 길이).
	// 10, 10: 원통의 세그먼트 개수로 매끄러움을 결정.
	glPopMatrix();
}

void neck()
{
	glPushMatrix();
	// 현재 변환 행렬을 스택에 저장
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// 목을 -90도로 회전하여 z축을 따라 생성되는 원통을 세로 방향(y)로 배치
	gluCylinder(nk, NECK_RADIUS, NECK_RADIUS, NECK_HEIGHT, 10, 10);
	// 원통으로 목을 생성.
	// 파라미터:
	//   nk: GLUquadricObj* 객체로 원통을 그리기 위한 도형.
	//   NECK_RADIUS: 목의 반지름.
	//   NECK_HEIGHT: 목의 높이.
	//   10, 10: 원통의 메쉬 세그먼트 개수.
	glPopMatrix();
	// 스택에서 이전 변환 행렬을 복
}

void rightShoulder()
{
	glPushMatrix();
	gluSphere(relb, SHOULDER_RADIUS, 10, 10);
	// 구체로 오른쪽 어깨를 생성.
	// relb: GLUquadricObj* 객체.
	// SHOULDER_RADIUS: 어깨의 반지름.
	glPopMatrix();
}

void leftShoulder()
{
	glPushMatrix();
	gluSphere(lelb, SHOULDER_RADIUS, 10, 10);
	// 구체로 왼쪽 어깨를 생성.
	glPopMatrix();
}

void rightElbow()
{
	glPushMatrix();
	gluSphere(relb, ELBOW_RADIUS, 10, 10);
	// 구체로 오른쪽 팔꿈치를 생성.
	// ELBOW_RADIUS: 팔꿈치의 반지름.
	glPopMatrix();
}

void leftElbow()
{
	glPushMatrix();
	gluSphere(lelb, ELBOW_RADIUS, 10, 10);
	// 구체로 왼쪽 팔꿈치를 생성.
	glPopMatrix();
}

void rightKnee()
{
	glPushMatrix();
	gluSphere(rknee, KNEE_RADIUS, 10, 10);
	// 구체로 오른쪽 무릎을 생성.
	// KNEE_RADIUS: 무릎의 반지름.
	glPopMatrix();
}

void leftKnee()
{
	glPushMatrix();
	gluSphere(lknee, KNEE_RADIUS, 10, 10);
	// 구체로 왼쪽 무릎을 생성.
	glPopMatrix();
}

void leftFoot()
{
	glPushMatrix();
	gluSphere(lknee, FOOT_RADIUS, 10, 10);
	// 구체로 왼쪽 발을 생성.
	// FOOT_RADIUS: 발의 반지름.
	glPopMatrix();
}

void rightFoot()
{
	glPushMatrix();
	gluSphere(lknee, FOOT_RADIUS, 10, 10);
	// 구체로 오른쪽 발을 생성.
	glPopMatrix();
}

void rightHand()
{
	glPushMatrix();
	gluSphere(lknee, HAND_RADIUS, 10, 10);
	// 구체로 오른쪽 손을 생성.
	// HAND_RADIUS: 손의 반지름.
	glPopMatrix();
}

void leftHand()
{
	glPushMatrix();
	gluSphere(lknee, HAND_RADIUS, 10, 10);
	// 구체로 왼쪽 손을 생성.
	glPopMatrix();

}

void left_upper_arm()
{
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// 원통을 세로 방향으로 배치.
	gluCylinder(lua, UPPER_ARM_RADIUS, UPPER_ARM_RADIUS - 0.1, UPPER_ARM_HEIGHT, 10, 10);
	// 원통으로 왼쪽 위팔을 생성.
	// lua: GLUquadricObj* 객체.
	// UPPER_ARM_RADIUS: 위팔의 윗부분 반지름.
	// UPPER_ARM_RADIUS - 0.1: 아래쪽 반지름 (점점 좁아짐).
	// UPPER_ARM_HEIGHT: 위팔의 길이.
	glPopMatrix();
}

void left_lower_arm()
{
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	// 원통을 세로 방향으로 배치.
	gluCylinder(lla, LOWER_ARM_RADIUS - 0.1, LOWER_ARM_RADIUS - 0.15, LOWER_ARM_HEIGHT, 10, 10);
	// 원통으로 왼쪽 아래팔을 생성.
	// lla: GLUquadricObj* 객체.
	// LOWER_ARM_RADIUS - 0.1: 아래팔의 윗부분 반지름.
	// LOWER_ARM_RADIUS - 0.15: 아래팔의 아랫부분 반지름 (점점 더 좁아짐).
	// LOWER_ARM_HEIGHT: 아래팔의 길이.
	glPopMatrix();
}

//오른쪽 위팔을 그리는 함수
void right_upper_arm()
{
	glPushMatrix(); // 현재 변환 행렬을 저장
	glRotatef(-90.0, 1.0, 0.0, 0.0); // x축을 기준으로 -90도 회전
	gluCylinder(rua, UPPER_ARM_RADIUS, UPPER_ARM_RADIUS - 0.1, UPPER_ARM_HEIGHT, 10, 10);
	//원기둥을 그리기, 시작 반지름(UPPER_ARM_RADIUS), 끝 반지름(UPPER_ARM_RADIUS - 0.1), 
    // 높이(UPPER_ARM_HEIGHT), 가로 세분화(10), 세로 세분화(10)
	glPopMatrix(); // 저장된 행렬로 복구
}

//오른쪽 아래팔을 그리는 함수
void right_lower_arm()
{
	glPushMatrix(); // 현재 변환 행렬을 저장
	glRotatef(-90.0, 1.0, 0.0, 0.0); // x축을 기준으로 -90도 회전
	gluCylinder(rla, LOWER_ARM_RADIUS - 0.1, LOWER_ARM_RADIUS - 0.15, LOWER_ARM_HEIGHT, 10, 10);
	// 원기둥을 그리기, 시작 반지름(LOWER_ARM_RADIUS - 0.1), 끝 반지름(LOWER_ARM_RADIUS - 0.15),
    // 높이(LOWER_ARM_HEIGHT), 가로 세분화(10), 세로 세분화(10)
	glPopMatrix(); //저장된 행렬로 복구
}

//왼쪽 허벅다리를 그리는 함수
void left_upper_leg()
{
	glPushMatrix(); //현재 변환 행렬을 저장
	glRotatef(-90.0, 1.0, 0.0, 0.0); // x축을 기준으로 -90도 회전
	gluCylinder(lul, UPPER_LEG_RADIUS, UPPER_LEG_RADIUS - 0.1, UPPER_LEG_HEIGHT, 10, 10);
	// 원기둥을 그리기, 시작 반지름(UPPER_LEG_RADIUS), 끝 반지름(UPPER_LEG_RADIUS - 0.1),
    // 높이(UPPER_LEG_HEIGHT), 가로 세분화(10), 세로 세분화(10)
	glPopMatrix(); // 저장된 행렬로 복구
}

//왼쪽 종아리를를 그리는 함수
void left_lower_leg()
{
	glPushMatrix(); // 현재 변환 행렬을 저장
	glRotatef(-90.0, 1.0, 0.0, 0.0); //x축을 기준으로 -90도 회전
	gluCylinder(lll, LOWER_LEG_RADIUS - 0.1, LOWER_LEG_RADIUS - 0.2, LOWER_LEG_HEIGHT, 10, 10);
	// 원기둥을 그리기, 시작 반지름(LOWER_LEG_RADIUS - 0.1), 끝 반지름(LOWER_LEG_RADIUS - 0.2),
	// 높이(LOWER_LEG_HEIGHT), 가로 세분화(10), 세로 세분화(10)
	glPopMatrix(); //저장된 행렬로 복구
}

//오른쪽 허벅다리를 그리는 함수
void right_upper_leg()
{
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(rul, UPPER_LEG_RADIUS, UPPER_LEG_RADIUS - 0.1, UPPER_LEG_HEIGHT, 10, 10);
	glPopMatrix();
}
//오른쪽 종아리를 그리는 함수
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
	// 투영 행렬 모드로 전환.
	// 투영 행렬은 3D 좌표를 2D 화면에 투영하는 방법을 정의.
	glLoadIdentity();
	// 투영 행렬을 초기화.
	// 이전에 적용된 변환을 제거하여 기본 상태로 설정.
	glOrtho(-15, 15, -15, 15, -15, 15);
	// 직교 투영 설정.
	// 화면에 표시되는 공간의 크기를 설정:
	// - x축: [-15, 15]
	// - y축: [-15, 15]
	// - z축: [-15, 15]
	// 직교 투영은 원근감 없이 모든 객체를 동일한 크기로 표현.
	glRotatef(rotate, 0, 1, 0);
	// y축을 기준으로 `rotate`만큼 회전.
	// `rotate`는 외부에서 변경될 수 있는 전역 변수로, 애니메이션이나 사용자 입력을 통해 회전각을 조정.
	glMatrixMode(GL_MODELVIEW);
	// 모델뷰 행렬 모드로 전환.
	// 이후의 변환은 객체의 위치와 방향에 영향을 미침.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 화면을 초기화.
	// GL_COLOR_BUFFER_BIT: 화면의 색상 버퍼를 지움.
	// GL_DEPTH_BUFFER_BIT: 깊이 버퍼(3D 객체의 깊이 정보)를 지움.
	glLoadIdentity();
	// 모델뷰 행렬을 초기화하여 기본 상태로 설정.
	glColor3f(0.4, 0.4, 0.4);
	// 렌더링 색상을 설정.
	// RGB 값을 각각 0.4로 설정하여 회색으로 렌더링.

	traverse(&torso_node);
	// 트리 구조의 루트 노드인 `torso_node`부터 트리를 순회하며 신체 부위를 렌더링.
	// `traverse` 함수는 계층적 트리 구조를 따라 재귀적으로 모든 신체 부위를 그림.
	glutSwapBuffers();
	// 프레임 버퍼를 교체.
	// 이중 버퍼링을 사용하여 렌더링된 프레임을 화면에 출력.
	// 기존 화면이 깜박이는 현상을 방지.
}

//움직임 동작 함수
void inverseKinematics()
{
	switch (animation) // 애니메이션 상태에 따른 다른 동작 수행
	{
	case 0: //애니메이션 상태가 0일 경우
		//오른쪽, 왼팔, 목의 각도를 조정
		if (theta[5] < 180.0) // 오른팔의 각도가 180도 이하인 경우
		{
			theta[5] += STEP; //오른팔 각도를 증가
			theta[3] += STEP; // 왼팔 각도를 증가
			theta[1] -= 0.2 * STEP; // 오른팔의 각도가 180도 이하인 경우
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