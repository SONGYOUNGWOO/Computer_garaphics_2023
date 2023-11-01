#version 330 core
// in_Position : attribute index 0
// in_Color : attribute index 1

//layout (location = 0) in vec3 in_Position;	//위치 변수 attribute position 0
//layout (location = 1) in vec3 in_Color;		//컬러 변수 attribute position 1
//out vec3 out_Color;							//프래그먼트 세이더에게 전달

in vec3 vPos; //--- 메인 프로그램에서 입력 받음
in vec3 vColor; //--- 메인 프로그램에서 입력 받음
in float vSize;  // 크기
out vec3 passColor; //--- fragment shader로 전달
//out float passSize;  //---  fragment shader로 크기 전달

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;
uniform bool usev_color;
uniform vec3 ucolor;

void main(void)
{ 
	gl_Position = projectionTransform * viewTransform * modelTransform * vec4 (vPos.x, vPos.y, vPos.z, 1.0);

	passColor =  usev_color? vColor : ucolor;

}