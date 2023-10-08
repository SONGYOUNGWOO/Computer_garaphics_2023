#version 330 core
// in_Position : attribute index 0
// in_Color : attribute index 1

//layout (location = 0) in vec3 in_Position;	//��ġ ���� attribute position 0
//layout (location = 1) in vec3 in_Color;		//�÷� ���� attribute position 1
//out vec3 out_Color;							//�����׸�Ʈ ���̴����� ����

in vec3 vPos; //--- ���� ���α׷����� �Է� ����
in vec3 vColor; //--- ���� ���α׷����� �Է� ����
in float vSize;  // ũ��
out vec3 passColor; //--- fragment shader�� ����
//out float passSize;  //---  fragment shader�� ũ�� ����

uniform mat4 modelTransform;

void main(void)
{ 
	gl_Position = modelTransform * vec4 (vPos.x, vPos.y, vPos.z, 1.0);

	passColor=  vColor;

}