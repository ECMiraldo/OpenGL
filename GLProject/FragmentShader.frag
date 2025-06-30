#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model
uniform sampler2D textura;
uniform int deform;
uniform int deformAngle;

// Estrutura da fonte de luz ambiente global
struct AmbientLight {
	vec3 ambient;	// Componente de luz ambiente global
};

uniform AmbientLight ambientLight; // Fonte de luz ambiente global

// Estrutura de uma fonte de luz direcional
struct DirectionalLight	{
	vec3 direction;		// Direção da luz, espaço do mundo
	
	vec3 ambient;		// Componente de luz ambiente
	vec3 diffuse;		// Componente de luz difusa
	vec3 specular;		// Componente de luz especular
};

uniform DirectionalLight directionalLight; // Fonte de luz direcional

// Estrutura de uma fonte de luz pontual
//Luz Pontual
struct PointLight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform PointLight pointLight;

// Estrutura de uma fonte de luz cónica
struct SpotLight {
	vec3 position;		// Posição do foco de luz, espaço do mundo
	
	vec3 ambient;		// Componente de luz ambiente
	vec3 diffuse;		// Componente de luz difusa
	vec3 specular;		// Componente de luz especular
	
	float constant;		// Coeficiente de atenuação constante
	float linear;		// Coeficiente de atenuação linear
	float quadratic;	// Coeficiente de atenuação quadrática

	float angle, spotExponent;
	vec3 spotDirection;
};

uniform SpotLight spotLight; // Fonte de luz cónica

struct Material{
	vec3 emissive;
	vec3 ambient;		// Ka
	vec3 diffuse;		// kd
	vec3 specular;		// ke
	float shininess;
};

uniform Material material;

in vec3 vPositionEyeSpace;
in vec3 vNormalEyeSpace;
in vec2 uv;

layout (location = 0) out vec4 fColor; // Cor final do fragmento

vec4 calcAmbientLight(AmbientLight light);	
vec4 calcDirectionalLight(DirectionalLight light);
vec4 calcPointLight(PointLight light);
vec4 calcSpotLight(SpotLight light);

void main()
{
	// Cálculo da componente emissiva do material.
	vec4 emissive = vec4(material.emissive, 1.0);

	// Cálculo do efeito da iluminação no fragmento.
	vec4 light[5];
	// Contribuição da fonte de luz ambiente
	light[0] = calcAmbientLight(ambientLight);
	// Contribuição da fonte de luz direcional
	light[1] = calcDirectionalLight(directionalLight);
	// Contribuição de cada fonte de luz Pontual
	light[2] = calcPointLight(pointLight);
	light[3] = calcSpotLight(spotLight);
	// Contribuição da fonte de luz cónica
	light[4] = /**/vec4(0.0)/**/;

	

	// Cálculo da cor final do fragmento.
	// Com CubeMap
	fColor = (emissive + light[0] + light[1] + light[2] + light[3] + light[4]) * texture(textura, uv);
	// Com cor de fragmento
	//fColor = (emissive + light[0] + light[1] + light[2] + light[3] + light[4]) * vec4(1.0, 0.5, 0.5, 1.0);
}

vec4 calcAmbientLight(AmbientLight light) {
	// Cálculo da contribuição da fonte de luz ambiente global, para a cor do objeto.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);
	return ambient;
}

vec4 calcDirectionalLight(DirectionalLight light) {
	// Cálculo da reflexão da componente da luz ambiente.
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	// Cálculo da reflexão da componente da luz difusa.
	vec3 lightDirectionEyeSpace = (View * vec4(light.direction, 0.0)).xyz;
	vec3 L = normalize(-lightDirectionEyeSpace); // Direção inversa à da direção luz.
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;
	
	// Cálculo da reflexão da componente da luz especular.
	// Como os cálculos estão a ser realizados nas coordenadas do olho, então a câmara está na posição (0,0,0).
	// Resulta então um vetor V entre os pontos (0,0,0) e vPositionEyeSpace:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// Que pode ser simplificado como:
	//		- vPositionEyeSpace
	vec3 V = normalize(-vPositionEyeSpace);
	//vec4 H = normalize(L + V);	// Modelo Blinn-Phong
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	//float NdotH = max(dot(N, H), 0.0);	// Modelo Blinn-Phong
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

	// Cálculo da contribuição da fonte de luz direcional para a cor final do fragmento.
	return (ambient + diffuse + specular);
}

vec4 calcPointLight(PointLight light) 
{
	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);

	vec3 lightPositionEye = (View * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionEye - vPositionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;
	vec3 V = normalize(-vPositionEyeSpace);

	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);

	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	// attenuation
	float dist = length(mat3(View) * light.position - vPositionEyeSpace);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	return (attenuation * (ambient + diffuse + specular));
}
vec4 calcSpotLight(SpotLight light) {

vec4 ambient = vec4(material.ambient * light.ambient, 1.0);
	vec3 lightDir = normalize(light.position - vPositionEyeSpace);
	float theta = dot(lightDir, normalize(-light.spotDirection));

	if(theta > light.angle)
	{
		vec3 L = normalize(-lightDir);
		vec3 N = normalize(vPositionEyeSpace);
		float NdotL = max(dot(N, L), 0.0);
		vec4 diffuse = vec4(material.diffuse * light.diffuse, 1.0) * NdotL;

		vec3 V = normalize(-vPositionEyeSpace);
		vec3 R = reflect(-L, N);
		float RdotV = max(dot(R, V), 0.0);
		vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

		return (ambient + diffuse + specular);
	}
	else
	{
		return ambient;
	}




}