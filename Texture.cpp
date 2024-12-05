/*
#include <string>
#include "Texture.h"

Texture::Texture(const char* fileLoc)
{
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;

	strcpy(fileLocation, fileLoc);
}

void Texture::LoadTexture()
{
	// ��ũ���� �̹��� �ε�, ������ unsigned char[]
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);
	if (!texData)
	{
		printf("�ؽ��� �ε� ���� : %s\n", fileLocation);
		return;
	}
	else
	{
		printf("�ؽ��� �ε� ����\n");
	}

	// VRAM ���ο� �ؽ��ĸ� ��� ���� ����
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// �Ķ���͵� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// VRAM���� �ؽ��� ���ֱ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);

	// �Ӹ� ����
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);
}

void Texture::UseTexture()
{
	glActiveTexture(GL_TEXTURE0); //0�� �ؽ��� ���� Ȱ��ȭ
	glBindTexture(GL_TEXTURE_2D, textureID); // VRAM ���� �ִ� �� �ؽ��ĸ� 0�� �ؽ��� ���ֿ� bind

}

void Texture::ClearTexture()
{
	glDeleteTextures(1, &textureID);
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;

	fileLocation[0] = '\0';
}

Texture::~Texture()
{
	ClearTexture();
}
*/