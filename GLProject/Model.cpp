#include "Model.h"
#include "LoadShaders.h"
#include <glm/gtc/matrix_transform.hpp> // translate, rotate, scale, perspective, ...
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;


namespace biblioteca {



	void Model::Draw(vec3 position, vec3 orientation) {
		Camera::GetInstance()->Update();
		mat4 tempmodel = model;
		tempmodel = translate(tempmodel, position);

		//Orientation é o pitch, yaw, roll em graus
		tempmodel = rotate(tempmodel, radians(orientation.x), vec3(1, 0, 0)); //pitch
		tempmodel = rotate(tempmodel, radians(orientation.y), vec3(0, 1, 0)); //yaw
		tempmodel = rotate(tempmodel, radians(orientation.z), vec3(0, 0, 1)); //roll

		//angle to deform
		if (deformAngle < 360) deformAngle += 1;
		if (deformAngle == 360) deformAngle = 0;

		GLint modelId = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "Model");
		glProgramUniformMatrix4fv(shaderProgram, modelId, 1, GL_FALSE, glm::value_ptr(tempmodel));

		mat4 modelView = Camera::GetInstance()->view * tempmodel;
		GLint modelViewId = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "ModelView");
		glProgramUniformMatrix4fv(shaderProgram, modelViewId, 1, GL_FALSE, glm::value_ptr(modelView));

		mat3 normalMatrix = glm::inverseTranspose(glm::mat3(modelView));
		GLint normalMatrixId = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "NormalMatrix");
		glProgramUniformMatrix4fv(shaderProgram, normalMatrixId, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		GLint viewID = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "View");
		glProgramUniformMatrix4fv(shaderProgram, viewID, 1, GL_FALSE, glm::value_ptr(Camera::GetInstance()->view));
		GLint projectionId = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "Projection");
		glProgramUniformMatrix4fv(shaderProgram, projectionId, 1, GL_FALSE, glm::value_ptr(Camera::GetInstance()->projection));

		GLint angleId = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "deformAngle");
		glProgramUniform1i(shaderProgram, angleId, deformAngle);


		glBindVertexArray(vertexArrayObject);

		// Envia comando para desenho de primitivas GL_TRIANGLES, que utilizará os dados do VAO vinculado.
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		// glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, (void*)0); // ebo
	}

	bool Model::ReadFiles(const char* filename) {
		FILE* file;
		errno_t err;
		err = fopen_s(&file, filename, "r");
		if (file == NULL) {
			throw("Impossible to open the file !\n");
			return false;
		}

		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		std::vector< glm::vec3 > temp_vertices;
		std::vector< glm::vec2 > temp_uvs;
		std::vector< glm::vec3 > temp_normals;

		while (1) {
			char lineHeader[128];
			int res = fscanf_s(file, "%s", lineHeader, (unsigned int)_countof(lineHeader));
			if (res == EOF)
				break;
			if (strcmp(lineHeader, "mtllib") == 0) {
				fscanf_s(file, "%s\n", materialsFilename, (unsigned int)_countof(materialsFilename));
				ReadMaterial(materialsFilename);
			}

			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}

			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}

			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}

			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					throw("Failed to read face information\n");
					return false;
				}

				for (int i = 0; i < 3; i++)
				{
					vertices.push_back(temp_vertices.at(vertexIndex[i] - 1));
					uvs.push_back(temp_uvs.at(uvIndex[i] - 1));
					normals.push_back(temp_normals.at(normalIndex[i] - 1));
				}

			};
		}
		fclose(file);
		return true;
	}

	bool Model::ReadMaterial(const char* filename) {
		FILE* file;
		errno_t err;
		err = fopen_s(&file, filename, "r");

		if (file == NULL) {

			throw("Num abriu !\n");
			return false;
		}
		while (1) {
			char lineHeader[128];
			int res = fscanf_s(file, "%s", lineHeader, (unsigned int)_countof(lineHeader));
			if (res == EOF)
				break;
			if (strcmp(lineHeader, "Ka") == 0) {
				fscanf_s(file, "%f %f %f\n", &ka.x, &ka.y, &ka.z);

			}
			if (strcmp(lineHeader, "Kd") == 0) {
				fscanf_s(file, "%f %f %f\n", &kd.x, &kd.y, &kd.z);

			}
			if (strcmp(lineHeader, "Ks") == 0) {
				fscanf_s(file, "%f %f %f\n", &ks.x, &ks.y, &ks.z);
			}
			if (strcmp(lineHeader, "Ns") == 0) {
				fscanf_s(file, "%f\n", &ns);
			}

			if (strcmp(lineHeader, "map_Kd") == 0) {
				char path_image[100];
				fscanf_s(file, "%s\n", path_image, (unsigned int)_countof(path_image));
				load_texture(path_image);
			}
		}

		fclose(file);
		return true;
	}

	void Model::load_texture(const char* filename) {
		GLuint textureName = 0;

		// Gera um nome de textura
		glGenTextures(1, &textureName);

		// Ativa a Unidade de Textura #0
		// A Unidade de Textura 0 já está ativa por defeito.
		// Só uma Unidade de Textura está ativa a cada momento.
		glActiveTexture(GL_TEXTURE0);

		// Vincula esse nome de textura ao target GL_TEXTURE_2D da Unidade de Textura ativa.
		glBindTexture(GL_TEXTURE_2D, textureName);

		// Define os parâmetros de filtragem (wrapping e ajuste de tamanho)
		// para a textura que está vinculada ao target GL_TEXTURE_2D da Unidade de Textura ativa.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Leitura/descompressão do ficheiro com imagem de textura
		int width, height, nChannels;
		// Ativa a inversão vertical da imagem, aquando da sua leitura para memória.
		stbi_set_flip_vertically_on_load(true);
		// Leitura da imagem para memória do CPU
		unsigned char* imageData = stbi_load(filename, &width, &height, &nChannels, 0);
		if (imageData) {
			// Carrega os dados da imagem para o Objeto de Textura vinculado ao target GL_TEXTURE_2D da Unidade de Textura ativa.
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);

			// Gera o Mipmap para essa textura
			glGenerateMipmap(GL_TEXTURE_2D);

			// Liberta a imagem da memória do CPU
			stbi_image_free(imageData);
		}
		else {
			std::cout << "Error loading texture!" << std::endl;
		}
	}


	GLuint Model::sendModelData() {

		GLfloat vertex[9264 * 3];
		GLfloat uvsArray[9264 * 2];
		GLfloat normais[9264 * 3];
		for (int i = 0; i < vertices.size(); i++) {
			// at = vertices[i]
			vertex[i * 3] = vertices.at(i).x;
			vertex[i * 3 + 1] = vertices.at(i).y;
			vertex[i * 3 + 2] = vertices.at(i).z;
			// normais
			normais[i * 3] = normals.at(i).x;
			normais[i * 3 + 1] = normals.at(i).y;
			normais[i * 3 + 2] = normals.at(i).z;

			// Uvs
			uvsArray[i * 2] = uvs.at(i).x;
			uvsArray[i * 2 + 1] = uvs.at(i).y;
		}

		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		glGenBuffers(3, bufferArrayObjects);

		// Send datas to buffers
		for (int i = 0; i < 3; i++) {
			glBindBuffer(GL_ARRAY_BUFFER, bufferArrayObjects[i]);

			if (i == 0) glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertex), vertex, 0);
			if (i == 1) glBufferStorage(GL_ARRAY_BUFFER, sizeof(uvsArray), uvsArray, 0);
			if (i == 2) glBufferStorage(GL_ARRAY_BUFFER, sizeof(normais), normais, 0);
		}

		ShaderInfo shaders[] = {
		{GL_VERTEX_SHADER,"VertexShader.vert"},
		{GL_FRAGMENT_SHADER,"FragmentShader.frag"},
		{GL_NONE,NULL},
		};

		GLuint shaderProgram = LoadShaders(shaders);
		this->shaderProgram = shaderProgram;
		glUseProgram(shaderProgram);


		//Posição no shader (ponteiro da variavel do shader)
		GLint vertexPositions = glGetProgramResourceLocation(shaderProgram, GL_PROGRAM_INPUT, "vPosition");
		GLint uvs = glGetProgramResourceLocation(shaderProgram, GL_PROGRAM_INPUT, "uvs");
		GLint normals = glGetProgramResourceLocation(shaderProgram, GL_PROGRAM_INPUT, "vNormal");

		// Ativa o VBO 'Buffers[0]'.
		glBindBuffer(GL_ARRAY_BUFFER, bufferArrayObjects[0]);
		glVertexAttribPointer(vertexPositions, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		// Ativa o VBO 'Buffers[1]'.
		glBindBuffer(GL_ARRAY_BUFFER, bufferArrayObjects[1]);
		glVertexAttribPointer(uvs, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, bufferArrayObjects[2]);
		glVertexAttribPointer(normals, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


		glEnableVertexAttribArray(vertexPositions);
		glEnableVertexAttribArray(uvs);
		glEnableVertexAttribArray(normals);

		//Textura

		GLint textura = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "textura");
		glProgramUniform1i(shaderProgram, textura, 0);

		return shaderProgram;
	}
}