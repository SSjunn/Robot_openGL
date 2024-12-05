#pragma once
#include <GL/glew.h>
#include "stb_image.h"

class Texture
{
public:
	Texture(const char* fileLoc);

	void LoadTexture(); // ��ũ�� ����� �̹����� �޸𸮿� �ؽ��ķ� �ε�
	void UseTexture(); // �������� �� �� �ؽ��ĸ� ���ڴٰ� ����
	void ClearTexture(); // �޸𸮿��� �ؽ��� ������

	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;

	char fileLocation[128]; // ���� ���
};