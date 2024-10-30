#pragma once

const char* vertex_rgb_shader_code = R"(#version 330 core
layout (location = 0) in float aCol;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

uniform float minColor;
uniform float maxColor;

uniform vec3 f[7];
uniform vec3 fcolors[7];

uniform vec3 voxelSize;
uniform vec3 imagePosition;

uniform int sizeX;
uniform int sizeY;

uniform int factor;

out VS_OUT{
	vec3 color;
	vec3 vPos;
	vec3 vScale;
	mat4 modelviewMatrix;
	mat4 projectionMatrix;
	bool isValid;
} vout;

vec3 get_filter(int i, float nCol)
{
	if (f[i][2] > f[i][1])
	{
		nCol = nCol - f[i][1];
		nCol = nCol / (f[i][2] - f[i][1]);
		return fcolors[i] * vec3(nCol);
	}
	else
		return fcolors[i];
}

void main()
{
	float nCol = aCol;

	if (nCol >= minColor && nCol <= maxColor)
	{
		int aPosY = int( float(gl_VertexID) / sizeX );
		int aPosX = gl_VertexID - ( aPosY * sizeX );

		vout.vScale = voxelSize * factor;
		vout.vPos = imagePosition + ( vout.vScale *  vec3(aPosX, aPosY, 0.0) );	
		vout.modelviewMatrix = modelviewMatrix;
		vout.projectionMatrix = projectionMatrix;
		
		for (int i=0; i<7; i++)
		{
			if (f[i][0] != 0.0 && nCol >= f[i][1] && nCol <= f[i][2])
			{
				vout.color = get_filter(i, nCol);

				vout.isValid = true;
				return;
			}
		}

		if (f[6][0] == 0.0)
		{
			if (maxColor > minColor)
			{
				nCol = nCol - minColor;
				nCol = nCol / (maxColor - minColor);
				vout.color = vec3(nCol);
			}
			else
				vout.color = vec3(1.0, 1.0, 1.0);
			vout.isValid = true;
		}
		else
		{
			vout.isValid = false;
		}
	}
	else
	{
		vout.isValid = false;
	}
}
)";


const char* vertex_shader_code = R"(#version 330 core
layout (location = 0) in float aCol;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

uniform float minColor;
uniform float maxColor;

uniform vec3 f[7];
uniform vec3 fcolors[7];

uniform vec3 voxelSize;
uniform vec3 imagePosition;

uniform int sizeX;
uniform int sizeY;

uniform int factor;

out VS_OUT{
	vec3 color;
	vec3 vPos;
	vec3 vScale;
	mat4 modelviewMatrix;
	mat4 projectionMatrix;
	bool isValid;
} vout;

vec3 get_filter(int i, float nCol)
{
	if (f[i][2] > f[i][1])
	{
		nCol = nCol - f[i][1];
		nCol = nCol / (f[i][2] - f[i][1]);
		return fcolors[i] * vec3(nCol);
	}
	else
		return fcolors[i];
}

void main()
{
	float nCol = aCol;

	if (nCol >= minColor && nCol <= maxColor)
	{
		int aPosY = int( float(gl_VertexID) / sizeX );
		int aPosX = gl_VertexID - ( aPosY * sizeX );

		vout.vScale = voxelSize * factor;
		vout.vPos = imagePosition + ( vout.vScale *  vec3(aPosX, aPosY, 0.0) );	
		vout.modelviewMatrix = modelviewMatrix;
		vout.projectionMatrix = projectionMatrix;
		
		for (int i=0; i<7; i++)
		{
			if (f[i][0] != 0.0 && nCol >= f[i][1] && nCol <= f[i][2])
			{
				vout.color = get_filter(i, nCol);

				vout.isValid = true;
				return;
			}
		}

		if (f[6][0] == 0.0)
		{
			if (maxColor > minColor)
			{
				nCol = nCol - minColor;
				nCol = nCol / (maxColor - minColor);
				vout.color = vec3(nCol);
			}
			else
				vout.color = vec3(1.0, 1.0, 1.0);
			vout.isValid = true;
		}
		else
		{
			vout.isValid = false;
		}
	}
	else
	{
		vout.isValid = false;
	}
}
)";


const char* geometry_shader_code = R"(#version 330 core
layout(points) in;
layout(points, max_vertices = 1) out;

in VS_OUT{
	vec3 color;
	vec3 vPos;
	vec3 vScale;
	mat4 modelviewMatrix;
	mat4 projectionMatrix;
	bool isValid;
} gs_in[];

out vec3 vertexColor;

void main(void)
{
	if (gs_in[0].isValid)
	{
		vertexColor = gs_in[0].color;
		gl_Position = gs_in[0].projectionMatrix * gs_in[0].modelviewMatrix * vec4(gs_in[0].vPos, 1.0);

		EmitVertex();
		EndPrimitive();
	}
}
)";

const char* geometry_shader_code_boxes = R"(#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

in VS_OUT {
    vec3 color;
	vec3 vPos;
	vec3 vScale;
	mat4 modelviewMatrix;
	mat4 projectionMatrix;
    bool isValid;
} gs_in[];

out vec3 vertexColor;

void main() {
    if (gs_in[0].isValid) {
        vertexColor = gs_in[0].color;

        // Pozycja wierzcho³ka (punkt)
        vec4 pointPos = vec4(gs_in[0].vPos, 1.0);

        // Wierzcho³ki tworz¹ce œciany kostki
        vec4 vertices[8];

        // Obliczenia pozycji wierzcho³ków
        vec3 halfSize = 0.5 * gs_in[0].vScale; // Po³owa d³ugoœci boku kostki
		
		// Wierzcho³ki kostki
        vertices[0] = pointPos + vec4(-halfSize[0], -halfSize[1], -halfSize[2], 0.0); // Lewy dolny tylny
		vertices[1] = pointPos + vec4( halfSize[0], -halfSize[1], -halfSize[2], 0.0); // Prawy dolny tylny
		vertices[2] = pointPos + vec4(-halfSize[0],  halfSize[1], -halfSize[2], 0.0); // Lewy górny tylny
		vertices[3] = pointPos + vec4( halfSize[0],  halfSize[1], -halfSize[2], 0.0); // Prawy górny tylny
		vertices[4] = pointPos + vec4(-halfSize[0], -halfSize[1],  halfSize[2], 0.0); // Lewy dolny przedni
		vertices[5] = pointPos + vec4( halfSize[0], -halfSize[1],  halfSize[2], 0.0); // Prawy dolny przedni
		vertices[6] = pointPos + vec4(-halfSize[0],  halfSize[1],  halfSize[2], 0.0); // Lewy górny przedni
		vertices[7] = pointPos + vec4( halfSize[0],  halfSize[1],  halfSize[2], 0.0); // Prawy górny przedni

		// TO TRZEBA ROZRYSOWAC I SPRAWDZIC !!!:
		int indices[20];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 3;
		indices[4] = 6;
		indices[5] = 7;
		indices[6] = 4;
		indices[7] = 5;
		indices[8] = 0;
		indices[9] = 1;

		indices[10] = 1;
		indices[11] = 5;
		indices[12] = 3;
		indices[13] = 7;
		indices[14] = 2;
		indices[15] = 6;
		indices[16] = 0;
		indices[17] = 4;
		indices[18] = 1;
		indices[19] = 5;

        // Generowanie œcian kostki
        for (int i = 0; i < 10; ++i) {
			int idx = indices[i];
            gl_Position = gs_in[0].projectionMatrix * gs_in[0].modelviewMatrix * vertices[idx];
            EmitVertex();
        }
        EndPrimitive();
        for (int i = 10; i < 20; ++i) {
			int idx = indices[i];
            gl_Position = gs_in[0].projectionMatrix * gs_in[0].modelviewMatrix * vertices[idx];
            EmitVertex();
        }
        EndPrimitive();
    }
}
)";

const char* fragment_shader_code = R"(#version 330 core
in vec3 vertexColor;
in vec3 FragPos; // Pozycja fragmentu/prymitywu w przestrzeni œwiata

out vec4 FragColor;

void main()
{
    vec3 viewPos = vec3(0, 0, 200);  // Pozycja obserwatora/kamery
    vec3 lightPos = viewPos;  // Pozycja œwiat³a od strony ekranu
    //vec3 lightPos = vec3(0, 100, 600);  // Pozycja œwiat³a
    vec3 lightColor = vec3(1.0, 1.0, 1.0);  // Kolor œwiat³a
    float ambientStrength = 0.6;
    float specularStrength = 0.5;  // Si³a œwiat³a spekularnego
    float shininess = 32.0;  // Po³ysk (shininess)

    // Obliczenia oœwietlenia (tutaj mo¿esz dodaæ w³asn¹ logikê)
    vec3 norm = vec3(0.0,0.0,1.0); //kierunek "do ekranu"
    vec3 lightDir = normalize(lightPos - FragPos);  // Poprawione obliczanie kierunku œwiat³a
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambientStrength * lightColor + diffuse + specular) * vertexColor.rgb;

    FragColor = vec4(result, 1.0);
}
)";

