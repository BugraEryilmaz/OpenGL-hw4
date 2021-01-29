#version 120

//layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix //
uniform mat4 MV; // ModelView idMVPMatrix //
uniform vec3 cameraPosition;    //
uniform vec3 lightPosition; //
uniform float heightFactor;//

// Texture-related data
uniform sampler2D hTexture; //
uniform int widthTexture;   //
uniform int heightTexture;  //


// Output to Fragment Shader
varying vec2 textureCoordinate; // For texture-color
varying vec3 vertexNormal; // For Lighting computation
varying vec3 ToLightVector; // Vector from Vertex to Light;
varying vec3 ToCameraVector; // Vector from Vertex to Camera;

float getHeight(vec2 xz) {
    vec2 normalizedXZ;
    normalizedXZ.x = 1-xz.x/widthTexture;
    normalizedXZ.y = 1-xz.y/heightTexture;
    return texture2D(hTexture, normalizedXZ).r*20;
}

float calcHeight(vec2 xz) {
    int total = 0;
    float height = 0;
    if (xz.x-1>=0 && xz.y-1>=0){
        vec2 botleftxz;
        botleftxz.x = xz.x-1;
        botleftxz.y = xz.y-1;
        height += getHeight(botleftxz)*2;
        total += 2;
    }
    
    if (xz.x<widthTexture && xz.y<heightTexture){
        vec2 toprightxz = xz;
        height += getHeight(toprightxz)*2;
        total += 2;
    }

    if (xz.x-1>=0 && xz.y<heightTexture){
        vec2 topleftxz = xz;
        topleftxz.x = xz.x-1;
        height += getHeight(topleftxz);
        total += 1;
    }

    if (xz.x<widthTexture && xz.y-1>=0){
        vec2 botrightxz = xz;
        botrightxz.y = xz.y-1;
        height += getHeight(botrightxz);
        total += 1;
    }

    return height/total;
}

vec3 calcNormal(vec2 xz) {
    int total = 0;
    vec3 ret = vec3(0,0,0);
    vec3 vertex0 = vec3(xz.x, calcHeight(vec2(xz.x,xz.y)), xz.y);
    vec3 vertex1;
    vec3 vertex2;
    vec3 vertex3;
    vec3 vertex4;
    vec3 vertex5;
    vec3 vertex6;
    if (xz.x-1>=0 && xz.y-1>=0){
        vertex4 = vec3(xz.x-1, calcHeight(vec2(xz.x-1,xz.y))  , xz.y);
        vertex5 = vec3(xz.x-1, calcHeight(vec2(xz.x-1,xz.y-1)), xz.y-1);
        vertex6 = vec3(xz.x  , calcHeight(vec2(xz.x  ,xz.y-1)), xz.y-1);

        vec3 vect56 = vertex6 - vertex5;
        vec3 vect50 = vertex0 - vertex5;
        vec3 vect54 = vertex4 - vertex5;

        ret += normalize(cross(vect50, vect56));
        ret += normalize(cross(vect54, vect50));
    }
    
    if (xz.x<widthTexture && xz.y<heightTexture){
        vertex1 = vec3(xz.x+1, calcHeight(vec2(xz.x+1,xz.y))  , xz.y);
        vertex2 = vec3(xz.x+1, calcHeight(vec2(xz.x+1,xz.y+1)), xz.y+1);
        vertex3 = vec3(xz.x  , calcHeight(vec2(xz.x  ,xz.y+1)), xz.y+1);

        vec3 vect01 = vertex1 - vertex0;
        vec3 vect02 = vertex2 - vertex0;
        vec3 vect03 = vertex3 - vertex0;

        ret += normalize(cross(vect02, vect01));
        ret += normalize(cross(vect03, vect02));
    }

    if (xz.x-1>=0 && xz.y<heightTexture){
        vertex3 = vec3(xz.x  , calcHeight(vec2(xz.x  ,xz.y+1)), xz.y+1);
        vertex4 = vec3(xz.x-1, calcHeight(vec2(xz.x-1,xz.y))  , xz.y);

        vec3 vect43 = vertex3 - vertex4;
        vec3 vect40 = vertex0 - vertex4;

        ret += normalize(cross(vect43, vect40));
    }

    if (xz.x<widthTexture && xz.y-1>=0){
        vertex1 = vec3(xz.x+1, calcHeight(vec2(xz.x+1,xz.y))  , xz.y);
        vertex6 = vec3(xz.x  , calcHeight(vec2(xz.x  ,xz.y-1)), xz.y-1);

        vec3 vect60 = vertex0 - vertex6;
        vec3 vect61 = vertex1 - vertex6;

        ret += normalize(cross(vect60, vect61));
    }
    ret = normalize(ret);
    return ret;
}

void main()
{

    // get texture value, compute height
    // compute normal vector using also the heights of neighbor vertices
    vec2 xz = gl_Vertex.xz;
    //vec4 pos = MVP*vec4(xz.x, calcHeight(xz), xz.y, 1);
    vec4 pos = MVP*vec4(gl_Vertex.x, calcHeight(xz), gl_Vertex.z, 1);
    vertexNormal = normalize(calcNormal(xz));
    // compute toLight vector vertex coordinate in VCS
    ToLightVector = normalize(lightPosition-pos.xyz);
    ToCameraVector = normalize(cameraPosition-pos.xyz);
    textureCoordinate.x = 1-xz.x/widthTexture;
    textureCoordinate.y = 1-xz.y/heightTexture;
   
    // set gl_Position variable correctly to give the transformed vertex position

    //gl_Position = vec4(0,0,0,0); // this is a placeholder. It does not correctly set the position 
    gl_Position = pos;
}
