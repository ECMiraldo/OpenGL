// Pega a informação que vai pra grafica e vai processar os dados na grafica
#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat3 NormalMatrix;
uniform int deform;
uniform int deformAngle;

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 uvs;
layout (location = 2) in vec3 vNormal;


out vec2 uv;
out vec3 normal;
out vec3 vPositionEyeSpace;
out vec3 vNormalEyeSpace;

void main()
{ 
	
	normal = vNormal;
	uv = uvs;


	// Posição do vértice em coordenadas do olho.
	vPositionEyeSpace = (ModelView * vec4(vPosition, 1.0)).xyz;

	// Transformar a normal do vértice.
	vNormalEyeSpace = normalize(NormalMatrix * vNormal);

	// Posição final do vértice (em coordenadas de clip)
	if (deform == 1) {
		vec3 deformPos = vPosition;
		deformPos.x += cos(deformPos.y + radians(deformAngle));
		deformPos.z += cos(deformPos.y + radians(deformAngle));
		gl_Position = (Projection * View * Model) * vec4(deformPos,1.0f);
	}
	else {
		gl_Position = (Projection * View * Model) * vec4(vPosition,1.0f);
	}
	
	
}