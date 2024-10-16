#pragma once

#define PACKET_SIZE (size_t)100000

#define POLYGON_GROUP_VERTEX_SHADER R"(#version 120
    attribute highp vec3 inPosition;
    attribute vec2 inTexCoord;
	attribute vec4 inVColors;
	attribute vec3 inNormals;
	attribute vec4 alternativeColor;
    varying vec2 TexCoord;
    varying vec4 currentColor;
    varying vec3 vNormal;
	uniform bool useTexture;
	uniform bool useVColors;
    uniform bool useVNormals;
	uniform bool useAlternativeColor;
    void main()
    {
        gl_Position = gl_ModelViewProjectionMatrix * vec4(inPosition,1.0);
        if (useTexture)
			TexCoord = inTexCoord;
		else
			TexCoord = vec2(-1.0, -1.0);
		
		if (!useAlternativeColor)
		{
			if (useVColors)
			{
				currentColor = inVColors;
			}
			else
			{
				currentColor = alternativeColor;
			}
		}
		else
		{
			currentColor = alternativeColor;
		}

		if (useVNormals)
            //vNormal = inNormals;
			vNormal = (gl_ModelViewMatrix * vec4(inNormals, 0.0)).xyz;
        else
            //vNormal = vec3(0.0, 0.0, 1.0);
			vNormal = (gl_ModelViewMatrix * vec4(0.0, 0.0, 1.0, 0.0)).xyz;


    }
)"

#define POLYGON_GROUP_FRAGMENT_SHADER R"(#version 120
    varying vec2 TexCoord;
    varying vec4 currentColor;
    varying vec3 vNormal;
    uniform sampler2D ourTexture;
    uniform bool useTexture;
    uniform bool useVColors;
    uniform bool useVNormals;

    uniform vec3 lightDir = vec3(0.0, 0.0, 1.0); // kierunek œwiat³a
    uniform vec3 lightColor = vec3(1.0, 1.0, 1.0); // kolor œwiat³a
    uniform vec3 viewDir = vec3(0.0, 0.0, 1.0); // kierunek widzenia
    uniform vec3 ambientColor = vec3(0.2, 0.2, 0.2); // kolor œwiat³a otoczenia

    void main()
    {
        vec4 surfaceColor;

        if (useTexture && TexCoord.x >= 0.0 && TexCoord.x <= 1.0 && TexCoord.y >= 0.0 && TexCoord.y <= 1.0)
            surfaceColor = texture2D(ourTexture, TexCoord);
        else
            surfaceColor = currentColor;

        if (useVNormals)
        {
            vec3 N = normalize(vNormal);
            vec3 L = normalize(lightDir);
            vec3 V = normalize(viewDir);
    
            // Sk³adowa dyfuzyjna
            float diff = max(dot(N, L), 0.0);
    
            // Sk³adowa spekularna (zredukowany wp³yw)
            vec3 H = normalize(L + V); 
            float spec = pow(max(dot(N, H), 0.0), 16.0); // Zmniejszony wyk³adnik blasku
    
            vec3 finalColor = ambientColor + (diff + 0.2 * spec) * lightColor * surfaceColor.rgb; // Dodajemy sk³adow¹ ambientaln¹ i redukujemy wp³yw sk³adowej spekularnej
    
            gl_FragColor = vec4(finalColor, surfaceColor.a);
        }
        else
        {
            gl_FragColor = surfaceColor;
        }
    }
)"


#define POLYGON_GROUP_FRAGMENT_SHADER_1 R"(#version 120
    varying vec2 TexCoord;
    varying vec4 currentColor;
    varying vec3 vNormal;
    uniform sampler2D ourTexture;
	uniform bool useTexture;
	uniform bool useVColors;
	uniform bool useVNormals;

	uniform vec3 lightDir = vec3(0.0, 0.0, 1.0); // kierunek œwiat³a
	uniform vec3 lightColor = vec3(1.0, 1.0, 1.0); // kolor œwiat³a
	uniform vec3 viewDir = vec3(0.0, 0.0, 1.0); // kierunek widzenia

    void main()
    {
		vec4 surfaceColor;

        if (useTexture && TexCoord.x >= 0.0 && TexCoord.x <= 1.0 && TexCoord.y >= 0.0 && TexCoord.y <= 1.0)
			surfaceColor = texture2D(ourTexture, TexCoord);
		else
			surfaceColor = currentColor;

		if (useVNormals)
		{
			vec3 N = normalize(vNormal);
			vec3 L = normalize(lightDir);
			vec3 V = normalize(viewDir);
    
			// Sk³adowa dyfuzyjna
			float diff = max(dot(N, L), 0.0);
    
			// Sk³adowa spekularna
			vec3 H = normalize(L + V); 
			float spec = pow(max(dot(N, H), 0.0), 32.0); // 32.0 to tzw. wyk³adnik blasku

			vec3 finalColor = (diff + spec) * lightColor * surfaceColor.rgb;
    
			gl_FragColor = vec4(finalColor, surfaceColor.a);
		}
		else
		{
			gl_FragColor = surfaceColor;
		}
    }
)"

