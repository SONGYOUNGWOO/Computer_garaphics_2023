#version 330 core
// out_Color : ���ؽ� ���̴����� �Է¹޴� ���� ��
// FragColor : ����� ������ ������ ������ ���۷� ���� �� .
//in vec3 out_Color;	 //���ؽ� ���̴����Լ� ���� ����
//out vec4 FragColor;	 //���� ���

in vec3 passColor; //--- vertex shader���� �Է� ����
//in float passSize; //---  vertex shader���� �Է� ���� ũ��
out vec4 FragColor; //--- ������ ���۷� ���

void main(void)
{
	FragColor = vec4 ( passColor , 1.0);
}