#ifndef GL_SHADERS
#define GL_SHADERS

char BlendShader_VS[] =
"#version 400\n"

"layout(location = 0) in vec2 VertexPosition;"

"out vec2 RealTexCoord;"

"void main(){"
"gl_Position = vec4(VertexPosition, 1.0, 1.0);"
"RealTexCoord = 0.5*VertexPosition+0.5;"
"}";

char BlendShader_FS[] =
"#version 400\n"

"uniform sampler2D tex;"

"in vec2 RealTexCoord;"

"layout(location = 0) out vec4 BaseColor;"

"void main(){"
"BaseColor = texture(tex, RealTexCoord);"
"}";

char DepthBlendShader_VS[] =
"#version 400\n"

"layout(location = 0) in vec2 VertexPosition;"

"out vec2 RealTexCoord;"

"void main(){"
"gl_Position = vec4(VertexPosition, 1.0, 1.0);"
"RealTexCoord = 0.5*VertexPosition+0.5;"
"}";

char DepthBlendShader_FS[] =
"#version 400\n"

"uniform sampler2D tex0;"
"uniform sampler2D tex1;"

"in vec2 RealTexCoord;"

"layout(location = 0) out vec4 BaseColor;"
"out float gl_FragDepth;"

"void main(){"
"gl_FragDepth = texture(tex0, RealTexCoord).x;"
"BaseColor = texture(tex1, RealTexCoord);"
"}";

char WhiteShader_VS[] =
"#version 400\n"
"layout(std140) uniform GlobalData{"
"mat4 ObjMat;"
"mat4 ProjMat;"
"};"

"layout(location = 0) in vec3 VertexPosition;"
"layout(location = 1) in vec3 VertexNorm;"
"layout(location = 2) in vec2 TexCoord;"

"void main(){"
"gl_Position = ProjMat * ObjMat * vec4(VertexPosition, 1.0);"
"}";

char WhiteShader_FS[] =
"#version 400\n"

"layout(location = 0) out vec4 BaseColor;"

"void main(){"
"BaseColor = vec4(1.0f,1.0f,1.0f,1.0f);"
"}";

char DepthShader_VS[] =
"#version 400\n"
"layout(std140) uniform GlobalData{"
"mat4 ObjMat;"
"mat4 ProjMat;"
"};"

"uniform uint Status;"

"layout(location = 0) in vec3 VertexPosition;"
"layout(location = 1) in vec3 VertexNorm;"
"layout(location = 2) in vec2 TexCoord;"

"out vec2 RealTexCoord;"

"void main(){"
"gl_Position = ProjMat * ObjMat * vec4(VertexPosition, 1.0);"
"RealTexCoord = TexCoord;"
"}";

char DepthShader_FS[] =
"#version 400\n"

"const uint DIFFUSE_MAP_EXIST = 1 << 1;"
"const uint SPECULAR_MAP_EXIST = 1 << 2;"
"const uint SPECULAR_EXPONENT_MAP_EXIST = 1 << 3;"
"const uint OPACITY_MAP_EXIST = 1 << 4;"
"const uint NORMAL_MAP_EXIST = 1 << 5;"
"const uint NORMAL_MAP_DEPTH = 1 << 6;"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat;"
"mat4 ProjMat;"
"};"

"uniform uint Status;"

"uniform sampler2D tex;"
"in vec2 RealTexCoord;"

"out float gl_FragDepth;"

"void main(){"
"if((Status & OPACITY_MAP_EXIST)>0){"
"vec4 Opacity=texture(tex, RealTexCoord);"
"if(Opacity.a<0.5)discard;"
"}"
"gl_FragDepth=gl_FragCoord.z;"
"}";

char DepthShader_Sphere_VS[] =
"#version 400\n"
"layout(std140) uniform GlobalData{"
"mat4 ObjMat;"
"mat4 ProjMat;"
"mat4 ObjMat_Inv;"
"mat4 ProjMat_Inv;"
"uvec2 GBufferSize;"
"};"

"uniform uint Status;"

"layout(location = 0) in vec3 VertexPosition;"

"out vec3 InputPos;"

"void main(){"
"gl_Position = ObjMat * vec4(VertexPosition, 1.0);"
"InputPos = VertexPosition;"
"gl_Position = ProjMat * gl_Position;"
"}";

char DepthShader_Sphere_FS[] =
"#version 400\n"

"const uint DIFFUSE_MAP_EXIST = 1 << 1;"
"const uint SPECULAR_MAP_EXIST = 1 << 2;"
"const uint SPECULAR_EXPONENT_MAP_EXIST = 1 << 3;"
"const uint OPACITY_MAP_EXIST = 1 << 4;"
"const uint NORMAL_MAP_EXIST = 1 << 5;"
"const uint NORMAL_MAP_DEPTH = 1 << 6;"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat;"
"mat4 ProjMat;"
"mat4 ObjMat_Inv;"
"mat4 ProjMat_Inv;"
"uvec2 GBufferSize;"
"};"

"uniform uint Status;"

"uniform sampler2D tex;"

"in vec3 InputPos;"

"out float gl_FragDepth;"

"const float M_PI = 3.14159265358979323846;"

"void main(){"
"float Radius = max(abs(InputPos.x), max(abs(InputPos.y), abs(InputPos.z)));"

"vec2 GBufferCoord=vec2(gl_FragCoord.x / GBufferSize.x, gl_FragCoord.y / GBufferSize.y);"
"vec4 TmpVec = ObjMat_Inv * ProjMat_Inv * vec4(GBufferCoord * 2.0 - 1.0, 0.0, 1.0);"
"vec3 EyePos = TmpVec.xyz / TmpVec.w;"
"TmpVec = ObjMat_Inv * ProjMat_Inv * vec4(GBufferCoord * 2.0 - 1.0, 0.5, 1.0);" // z = 0.5, in case of infinity frustum.
"vec3 EyeVec_Incident = normalize(TmpVec.xyz / TmpVec.w - EyePos);"
"TmpVec = ObjMat_Inv * ProjMat_Inv[3];"

//ray-sphere intersection
"float b = dot(EyePos, EyeVec_Incident);"
"float c = dot(EyePos, EyePos) - Radius * Radius;"
"float disc = b*b - c;"
"if (disc <= 0.0f)discard;" //discard points outside the sphere

"vec2 RealTexCoord;"
"vec3 SpherePos, ProjPos;"
"bool Inside = true;"

"float sdisc = sqrt(disc);"

"if(b + sdisc < 0.0f){"
"ProjPos = EyePos - (b + sdisc) * EyeVec_Incident;"

"SpherePos = normalize(ProjPos);"
"RealTexCoord.x = atan(SpherePos.y, SpherePos.x) / (2 * M_PI) + 0.5;"
"RealTexCoord.y = -0.5 - asin(SpherePos.z) / M_PI;"

"if((Status & OPACITY_MAP_EXIST)>0){"
"vec4 Opacity=texture(tex, RealTexCoord);"
"if(Opacity.a>=0.5)Inside = false;"
"}"
"else Inside = false;"
"}"

"if(Inside){"
"ProjPos = EyePos - (b - sdisc) * EyeVec_Incident;"

"SpherePos = normalize(ProjPos);"
"RealTexCoord.x = atan(SpherePos.y, SpherePos.x) / (2 * M_PI) + 0.5;"
"RealTexCoord.y = -0.5 - asin(SpherePos.z) / M_PI;"

"if((Status & OPACITY_MAP_EXIST)>0){"
"vec4 Opacity=texture(tex, RealTexCoord);"
"if(Opacity.a<0.5)discard;"
"}"
"}"

"TmpVec = ProjMat * ObjMat * vec4(ProjPos, 1.0);"
"ProjPos = TmpVec.xyz / TmpVec.w;"
"if (TmpVec.w <= 0.0f || abs(ProjPos.z) > 1.0f)discard;"
"ProjPos.z = (ProjPos.z + 1.0f) * 0.5f;"

"gl_FragDepth=ProjPos.z;"
"}";

char PhongShader_VS[] =
"#version 400\n"
"layout(std140) uniform GlobalData{"
"mat4 ObjMat;"
"mat4 ProjMat;"
"mat4 ObjMat_Inv;"
"mat4 ProjMat_Inv;"
"vec4 Diffuse_Info;" // the 4th component is undefined
"vec4 Specular_Info;" // the 4th component is glossiness
"};"

"uniform uint Status;"

"layout(location = 0) in vec3 VertexPosition;"
"layout(location = 1) in vec3 VertexNorm;"
"layout(location = 2) in vec2 TexCoord;"

"out vec3 ProjPos;"
"out vec3 RealNormal;"
"out vec2 RealTexCoord;"

"void main(){"
"gl_Position = ObjMat * vec4(VertexPosition, 1.0);"
"ProjPos = vec3(gl_Position)/gl_Position.w;"
"gl_Position = ProjMat * gl_Position;"
"RealNormal = vec3(transpose(ObjMat_Inv) * vec4(VertexNorm, 0.0));"
"RealTexCoord = TexCoord;"
"}";

char PhongShader_FS[] =
"#version 400\n"

"const uint DIFFUSE_MAP_EXIST = 1 << 1;"
"const uint SPECULAR_MAP_EXIST = 1 << 2;"
"const uint SPECULAR_EXPONENT_MAP_EXIST = 1 << 3;"
"const uint OPACITY_MAP_EXIST = 1 << 4;"
"const uint NORMAL_MAP_EXIST = 1 << 5;"
"const uint NORMAL_MAP_DEPTH = 1 << 6;"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat;"
"mat4 ProjMat;"
"mat4 ObjMat_Inv;"
"mat4 ProjMat_Inv;"
"vec4 Diffuse_Info;" // the 4th component is undefined
"vec4 Specular_Info;" // the 4th component is glossiness
"};"

"uniform uint Status;"

"uniform sampler2D tex0;"
"uniform sampler2D tex1;"
"uniform sampler2D tex2;"
"uniform sampler2D tex3;"

"in vec3 ProjPos;"
"in vec3 RealNormal;"
"in vec2 RealTexCoord;"

"layout(location = 0) out vec4 OutputColor;"

"float PackVec4(vec4 Raw_Data){"
"uvec4 IntColor=uvec4(255.0*Raw_Data);"
"IntColor.w&=254;"
"return uintBitsToFloat(IntColor.x+256*IntColor.y+65536*IntColor.z+16777216*IntColor.w);"  //7-bit for w
"}"
"vec2 PackNorm(vec3 Norm){"
"if(length(Norm.xy) < 0.0000001)return vec2(1.0,0.0) * sqrt((Norm.z+1.0)*0.5);"
"else return normalize(Norm.xy) * sqrt((Norm.z+1.0)*0.5);"
"}"

"void main(){"
"vec4 BaseColor;"
"vec4 SpecularColor;"

"if((Status & OPACITY_MAP_EXIST)>0){"
"vec4 Opacity=texture(tex2, RealTexCoord);"
"if(Opacity.a<0.5)discard;"
"}"

"if((Status & DIFFUSE_MAP_EXIST)>0)BaseColor = texture(tex0, RealTexCoord);"
"else BaseColor=vec4(Diffuse_Info.xyz, 1.0);"

"SpecularColor=vec4(Specular_Info);"
"if((Status & SPECULAR_EXPONENT_MAP_EXIST)>0)SpecularColor.w *= texture(tex1, RealTexCoord).a;"
"SpecularColor.w=0.07238 * log(Specular_Info.w);" // the last step convert a number in range [0, 1000] to range [0, 0.5].

"vec3 NormalVec;"

"if((Status & NORMAL_MAP_EXIST)>0){"
//per pixel tangent frame calculation
"vec3 Pos_dX = dFdx(ProjPos);"
"vec3 Pos_dY = dFdy(ProjPos);"
"vec2 Tex_dX = dFdx(RealTexCoord);"
"vec2 Tex_dY = dFdy(RealTexCoord);"

"vec3 T = normalize(Pos_dY*Tex_dX.y-Pos_dX*Tex_dY.y);"
"vec3 B = normalize(Pos_dY*Tex_dX.x-Pos_dX*Tex_dY.x);"

"mat3 TBN = mat3(T, B, RealNormal);"

"if((Status & NORMAL_MAP_DEPTH)>0){"
"ivec2 Tex_Size=textureSize(tex3,0);"
"vec2 TexCoord_dX=vec2(1.0/float(Tex_Size.x),0);"
"vec2 TexCoord_dY=vec2(0,1.0/float(Tex_Size.y));"

"const float BumpStrength=1.0;"

"vec3 NormalMap;"
"NormalMap.z=texture(tex3, RealTexCoord).w;"
"NormalMap.x=BumpStrength*(NormalMap.z-texture(tex3, RealTexCoord-TexCoord_dX).w);"
"NormalMap.y=BumpStrength*(NormalMap.z-texture(tex3, RealTexCoord-TexCoord_dY).w);"
"NormalMap.z=sqrt(1-dot(NormalMap.xy,NormalMap.xy));"
"NormalVec = normalize(TBN*NormalMap);"
"}"
"else{"
"vec4 NormalMap=2.0*texture(tex3, RealTexCoord) - 1.0;"
"NormalMap.xy = -NormalMap.xy;"
"NormalVec=normalize(TBN*vec3(NormalMap));"
"}"
"}"
"else NormalVec = normalize(RealNormal);"

"if(false==gl_FrontFacing)NormalVec=-NormalVec;"

//Data packing
"OutputColor=vec4(PackVec4(BaseColor), PackVec4(SpecularColor), PackNorm(NormalVec));"
"}";

char PhongShader_Sphere_VS[] =
"#version 400\n"
"layout(std140) uniform GlobalData{"
"mat4 ObjMat;"
"mat4 ProjMat;"
"mat4 ObjMat_Inv;"
"mat4 ProjMat_Inv;"
"vec4 Diffuse_Info;" // the 4th component is undefined
"vec4 Specular_Info;" // the 4th component is glossiness
"uvec2 GBufferSize;"
"};"

"uniform uint Status;"

"layout(location = 0) in vec3 VertexPosition;"

"out vec3 InputPos;"

"void main(){"
"gl_Position = ObjMat * vec4(VertexPosition, 1.0);"
"InputPos = VertexPosition;"
"gl_Position = ProjMat * gl_Position;"
"}";

char PhongShader_Sphere_FS[] =
"#version 400\n"

"const uint DIFFUSE_MAP_EXIST = 1 << 1;"
"const uint SPECULAR_MAP_EXIST = 1 << 2;"
"const uint SPECULAR_EXPONENT_MAP_EXIST = 1 << 3;"
"const uint OPACITY_MAP_EXIST = 1 << 4;"
"const uint NORMAL_MAP_EXIST = 1 << 5;"
"const uint NORMAL_MAP_DEPTH = 1 << 6;"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat;"
"mat4 ProjMat;"
"mat4 ObjMat_Inv;"
"mat4 ProjMat_Inv;"
"vec4 Diffuse_Info;" // the 4th component is undefined
"vec4 Specular_Info;" // the 4th component is glossiness
"uvec2 GBufferSize;"
"};"

"uniform uint Status;"

"uniform sampler2D tex0;"
"uniform sampler2D tex1;"
"uniform sampler2D tex2;"
"uniform sampler2D tex3;"

"in vec3 InputPos;"

"layout(location = 0) out vec4 OutputColor;"
"out float gl_FragDepth;"

"float PackVec4(vec4 Raw_Data){"
"uvec4 IntColor=uvec4(255.0*Raw_Data);"
"IntColor.w&=254;"
"return uintBitsToFloat(IntColor.x+256*IntColor.y+65536*IntColor.z+16777216*IntColor.w);"  //7-bit for w
"}"
"vec2 PackNorm(vec3 Norm){"
"if(length(Norm.xy) < 0.0000001)return vec2(1.0,0.0) * sqrt((Norm.z+1.0)*0.5);"
"else return normalize(Norm.xy) * sqrt((Norm.z+1.0)*0.5);"
"}"

"const float M_PI = 3.14159265358979323846;"

"void main(){"

"vec4 BaseColor;"
"vec4 SpecularColor;"

"float Radius = max(abs(InputPos.x), max(abs(InputPos.y), abs(InputPos.z)));"

"vec2 GBufferCoord=vec2(gl_FragCoord.x / GBufferSize.x, gl_FragCoord.y / GBufferSize.y);"
"vec4 TmpVec = ObjMat_Inv * ProjMat_Inv * vec4(GBufferCoord * 2.0 - 1.0, 0.0, 1.0);"
"vec3 EyePos = TmpVec.xyz / TmpVec.w;"
"TmpVec = ObjMat_Inv * ProjMat_Inv * vec4(GBufferCoord * 2.0 - 1.0, 0.5, 1.0);" // z = 0.5, in case of infinity frustum.
"vec3 EyeVec_Incident = normalize(TmpVec.xyz / TmpVec.w - EyePos);"
"TmpVec = ObjMat_Inv * ProjMat_Inv[3];"

//ray-sphere intersection
"float b = dot(EyePos, EyeVec_Incident);"
"float c = dot(EyePos, EyePos) - Radius * Radius;"
"float disc = b*b - c;"
"if (disc <= 0.0f)discard;" //discard points outside the sphere

"vec2 RealTexCoord;"
"vec3 SpherePos, ProjPos, RealNormal;"
"bool Inside = true;"

"float sdisc = sqrt(disc);"

"if(b + sdisc < 0.0f){"
"ProjPos = EyePos - (b + sdisc) * EyeVec_Incident;"
"RealNormal = vec3(transpose(ObjMat_Inv) * vec4(ProjPos, 0.0));"

"SpherePos = normalize(ProjPos);"
"RealTexCoord.x = atan(SpherePos.y, SpherePos.x) / (2 * M_PI) + 0.5;"
"RealTexCoord.y = -0.5 - asin(SpherePos.z) / M_PI;"

"if((Status & OPACITY_MAP_EXIST)>0){"
"vec4 Opacity=texture(tex2, RealTexCoord);"
"if(Opacity.a>=0.5)Inside = false;"
"}"
"else Inside = false;"
"}"

"if(Inside){"
"ProjPos = EyePos - (b - sdisc) * EyeVec_Incident;"
"RealNormal = vec3(transpose(ObjMat_Inv) * vec4(-ProjPos, 0.0));"

"SpherePos = normalize(ProjPos);"
"RealTexCoord.x = atan(SpherePos.y, SpherePos.x) / (2 * M_PI) + 0.5;"
"RealTexCoord.y = -0.5 - asin(SpherePos.z) / M_PI;"

"if((Status & OPACITY_MAP_EXIST)>0){"
"vec4 Opacity=texture(tex2, RealTexCoord);"
"if(Opacity.a<0.5)discard;"
"}"
"}"

"TmpVec = ProjMat * ObjMat * vec4(ProjPos, 1.0);"
"ProjPos = TmpVec.xyz / TmpVec.w;"
"if (TmpVec.w <= 0.0f || abs(ProjPos.z) > 1.0f)discard;"
"ProjPos.z = (ProjPos.z + 1.0f) * 0.5f;"

"if((Status & DIFFUSE_MAP_EXIST)>0)BaseColor = texture(tex0, RealTexCoord);"
"else BaseColor=vec4(Diffuse_Info.xyz, 1.0);"

"SpecularColor=vec4(Specular_Info);"
"if((Status & SPECULAR_EXPONENT_MAP_EXIST)>0)SpecularColor.w *= texture(tex1, RealTexCoord).a;"
"SpecularColor.w=0.07238 * log(Specular_Info.w);" // the last step convert a number in range [0, 1000] to range [0, 0.5].

"vec3 NormalVec;"

"if((Status & NORMAL_MAP_EXIST)>0){"
//per pixel tangent frame calculation
"vec3 T = normalize(vec3(SpherePos.y, -SpherePos.x, 0.0));"
"vec3 B = cross(T, RealNormal);"
//the texture wrapping on the sphere is not uniform
"T *= length(SpherePos.xy) / (2 * M_PI);"
"B /= M_PI;"
"if(Inside)B = -B;"

"mat3 TBN = mat3(T, B, RealNormal);"

"if((Status & NORMAL_MAP_DEPTH)>0){"
"ivec2 Tex_Size=textureSize(tex3,0);"
"vec2 TexCoord_dX=vec2(1.0/float(Tex_Size.x),0);"
"vec2 TexCoord_dY=vec2(0,1.0/float(Tex_Size.y));"

"const float BumpStrength=1.0;"

"vec3 NormalMap;"
"NormalMap.z=texture(tex3, RealTexCoord).w;"
"NormalMap.x=BumpStrength*(NormalMap.z-texture(tex3, RealTexCoord-TexCoord_dX).w);"
"NormalMap.y=BumpStrength*(NormalMap.z-texture(tex3, RealTexCoord-TexCoord_dY).w);"
"NormalMap.z=sqrt(1-dot(NormalMap.xy,NormalMap.xy));"
"NormalVec = normalize(TBN*NormalMap);"
"}"
"else{"
"vec4 NormalMap=2.0*texture(tex3, RealTexCoord) - 1.0;"
"NormalMap.xy = -NormalMap.xy;"
"NormalVec=normalize(TBN*vec3(NormalMap));"
"}"
"}"
"else NormalVec = normalize(RealNormal);"

//Data packing
"OutputColor=vec4(PackVec4(BaseColor), PackVec4(SpecularColor), PackNorm(NormalVec));"
"gl_FragDepth = ProjPos.z;"
"}";

char LightStencil_VS[] =
"#version 400\n"

"layout(std140) uniform GlobalData{"
"mat4 ViewMat;"
"mat4 ObjMat;"
"uvec2 GBufferSize;"
"};"

"layout(location = 0) in vec3 VertexPosition;"

"void main(){"
"gl_Position = ViewMat * ObjMat * vec4(VertexPosition, 1.0);"
"}";

char LightStencil_FS[] =
"#version 400\n"

"layout(std140) uniform GlobalData{"
"mat4 ViewMat;"
"mat4 ObjMat;"
"uvec2 GBufferSize;"
"};"

"uniform sampler2D tex;"

"layout(location = 0) out vec4 BaseColor;"

"void main(){"
"vec2 GBufferCoord=vec2(gl_FragCoord.x/GBufferSize.x,gl_FragCoord.y/GBufferSize.y);"
"BaseColor = vec4(gl_FragCoord.z-texture(tex, GBufferCoord).x,1.0,1.0,1.0);"
"}";

char BoxFilter_VS[] =
"#version 400\n"

"layout(location = 0) in vec2 VertexPosition;"

"out vec2 RealTexCoord;"

"void main(){"
"gl_Position = vec4(VertexPosition, 1.0, 1.0);"
"RealTexCoord = 0.5*VertexPosition+0.5;"
"}";

char BoxFilter_FS[] =
"#version 400\n"

"uniform sampler2D tex;"

"in vec2 RealTexCoord;"

"layout(location = 0) out vec4 BaseColor;"

"void main(){"
"ivec2 Tex_Size=textureSize(tex2,0);"
"vec2 TexCoord_Diff=vec2(0.5/float(Tex_Size.x),0.5/float(Tex_Size.y));"
"BaseColor = 0.25*texture(tex, vec2(RealTexCoord.x-TexCoord_Diff.x,RealTexCoord.y-TexCoord_Diff.y));"
"BaseColor += 0.25*texture(tex, vec2(RealTexCoord.x-TexCoord_Diff.x,RealTexCoord.y+TexCoord_Diff.y));"
"BaseColor += 0.25*texture(tex, vec2(RealTexCoord.x+TexCoord_Diff.x,RealTexCoord.y-TexCoord_Diff.y));"
"BaseColor += 0.25*texture(tex, vec2(RealTexCoord.x+TexCoord_Diff.x,RealTexCoord.y+TexCoord_Diff.y));"
"}";

char ESMShadowMask_VS[] =
"#version 400\n"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat_Cam;"
"mat4 ProjMat_Cam;"
"mat4 ObjMat_Light;"
"mat4 ProjMat_Light;"
"mat4 ObjMat_Cam_Inv;"
"mat4 ProjMat_Cam_Inv;"
"mat4 ObjMat_Light_Inv;"
"uvec2 GBufferSize;"
"};"

"layout(location = 0) in vec3 VertexPosition;"

"void main(){"
"gl_Position = ProjMat_Cam * ObjMat_Cam_Inv * ObjMat_Light * vec4(VertexPosition, 1.0);"
"if(gl_Position.z / gl_Position.w > 1.0f)gl_Position.z = gl_Position.w;" //in case that the view frustum is finite.
"}";

char ESMShadowMask_FS[] =
"#version 400\n"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat_Cam;"
"mat4 ProjMat_Cam;"
"mat4 ObjMat_Light;"
"mat4 ProjMat_Light;"
"mat4 ObjMat_Cam_Inv;"
"mat4 ProjMat_Cam_Inv;"
"mat4 ObjMat_Light_Inv;"
"uvec2 GBufferSize;"
"};"

"uniform sampler2D tex0, tex1;"

"layout(location = 0) out vec4 OutputColor;"

//Poisson samples are sorted by the distance
"float Poisson_X[16] = float[](0.0, -0.0185813775163211, 0.00987838877025593, 0.0158161369046160, 0.0393407798961443, -0.00194108741438204, -0.0434518425579765, -0.0224856458106236, -0.0636835347435893, 0.0472574019044190, 0.0690963384796983, 0.0675211647441149, 0.0280335509940221, -0.0659947525630424, -0.0788755644190642, 0.0610086817407035);"
"float Poisson_Y[16] = float[](0.0, 0.00564195011333912, 0.0196368924367792, -0.0261038765958646, 0.00108969185269281, -0.0576244003151479, -0.0397377177566476, 0.0558176093492525, 0.00355066286071482, -0.0465579983854927, -0.0184262330816445, 0.0261101636636018, 0.0673435256351129, 0.0351113356882897, -0.0283602627109986, 0.0589140423604903);"
"const float M_PI = 3.14159265358979323846;"

"void main(){"
"vec2 GBufferCoord=vec2(gl_FragCoord.x / GBufferSize.x, gl_FragCoord.y / GBufferSize.y);"

"vec4 GBufferPos=vec4(GBufferCoord.x*2.0f-1.0f,GBufferCoord.y*2.0f-1.0f,texture(tex0, GBufferCoord).x*2.0f-1.0f, 1.0f);"
"mat4 MatInv=ObjMat_Light_Inv * ObjMat_Cam * ProjMat_Cam_Inv;"

"GBufferPos=MatInv*GBufferPos;"

"vec2 SMSizeCoeff = 1.0f / textureSize(tex1, 0);"

"vec4 TmpVec = GBufferPos;"
"vec3 LightVec_Incident=vec3(TmpVec) / TmpVec.w;"

"TmpVec=ProjMat_Light*vec4(LightVec_Incident,1.0);"
"vec3 GBufferProj=vec3(TmpVec) / TmpVec.w;"

"if (abs(GBufferProj.x) > 1.0)discard;"
"if (abs(GBufferProj.y) > 1.0)discard;"
"if (abs(GBufferProj.z) > 1.0)discard;"

"int i, SampleUsed = 16;"
"float SampleRadius = sqrt(M_PI * (Poisson_X[SampleUsed - 1] * Poisson_X[SampleUsed - 1] + Poisson_Y[SampleUsed - 1] * Poisson_Y[SampleUsed - 1]) / (float(SampleUsed)));"
"float ESMValue = 0.0f, WeightSum = 0.0f;"

"GBufferProj.xy = (GBufferProj.xy + 1.0) * 0.5;"

"float Dir = 2 * M_PI * fract(cos(gl_FragCoord.x * (12.9898) + gl_FragCoord.y * (4.1414)) * 43758.5453);" // a random number generator (unknown source from the Internet. Good enough for our purpose.
"vec2 dX = vec2(cos(Dir) * SMSizeCoeff[0], sin(Dir) * SMSizeCoeff[1]);"
"vec2 dY = vec2(-sin(Dir) * SMSizeCoeff[0], cos(Dir) * SMSizeCoeff[1]);"

"for(i = 0;i<SampleUsed;i++){"
"float BaseDepth=2.0*texture(tex1, vec2(GBufferProj.xy + (Poisson_X[i] * dX + Poisson_Y[i] * dY) / SampleRadius)).x-1.0;"
"float Weight = exp(-(Poisson_X[i] * Poisson_X[i] + Poisson_Y[i] * Poisson_Y[i]) / (32 * SampleRadius * SampleRadius));"
"ESMValue += Weight * exp(10000.0f * (BaseDepth - GBufferProj.z));"
"WeightSum += Weight;"
"}"
"OutputColor=vec4(clamp(ESMValue / WeightSum, 0.0, 1.0), 0.0, 0.0, 0.0);"
"}";

char PhongLight_VS[] =
"#version 400\n"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat_Cam;"
"mat4 ProjMat_Cam;"
"mat4 ObjMat_Light;"
"mat4 ProjMat_Light;"
"mat4 ObjMat_Cam_Inv;"
"mat4 ProjMat_Cam_Inv;"
"mat4 ObjMat_Light_Inv;"
"vec3 Intensity;"
"uvec2 GBufferSize;"
"};"

"layout(location = 0) in vec3 VertexPosition;"

"void main(){"
"gl_Position = ProjMat_Cam * ObjMat_Cam_Inv * ObjMat_Light * vec4(VertexPosition, 1.0);"
"if(gl_Position.z / gl_Position.w > 1.0f)gl_Position.z = gl_Position.w;" //in case that the view frustum is finite.
"}";

char PhongLight_FS[] =
"#version 400\n"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat_Cam;"
"mat4 ProjMat_Cam;"
"mat4 ObjMat_Light;"
"mat4 ProjMat_Light;"
"mat4 ObjMat_Cam_Inv;"
"mat4 ProjMat_Cam_Inv;"
"mat4 ObjMat_Light_Inv;"
"vec3 Intensity;"
"uvec2 GBufferSize;"
"};"

"uniform uint Status;"

"uniform sampler2D tex0,tex1,tex2;" //tex0 for mask, tex1 for depth, tex2 for g-buffer

"layout(location = 0) out vec4 BaseColor;"
"out float gl_FragDepth;"

"vec4 UnPackVec4(float Packed_Data){"
"uint IntColor_Packed=floatBitsToUint(Packed_Data);"
"vec4 RetValue;"
"RetValue.x=float(IntColor_Packed&255)/255.0;"
"RetValue.y=float((IntColor_Packed/256)&255)/255.0;"
"RetValue.z=float((IntColor_Packed/65536)&255)/255.0;"
"RetValue.w=float((IntColor_Packed/16777216)&255)/255.0;" //7-bit for w
"return RetValue;"
"}"
"vec3 UnPackNorm(vec2 Norm_Packed){"
"vec3 RetValue;"
"RetValue.z = dot(Norm_Packed, Norm_Packed) * 2.0 - 1.0;"
"if(RetValue.z > -0.999999)Norm_Packed = normalize(Norm_Packed);"
"RetValue.xy = Norm_Packed * sqrt(1 - RetValue.z * RetValue.z);"
"return RetValue;"
"}"

"void main(){"
"vec2 GBufferCoord=vec2(gl_FragCoord.x/GBufferSize.x,gl_FragCoord.y/GBufferSize.y);"
"vec3 DestPos=vec3(GBufferCoord.x*2.0-1.0,GBufferCoord.y*2.0-1.0,texture(tex1, GBufferCoord).x*2.0-1.0);"

"vec4 TmpVec=ObjMat_Light_Inv * ObjMat_Cam * ProjMat_Cam_Inv * vec4(GBufferCoord * 2.0 - 1.0, 0.0, 1.0);"
"vec3 EyePos = vec3(TmpVec) / TmpVec.w;"

"if (DestPos.z >= 1.0)discard;"

"TmpVec=ObjMat_Light_Inv * ObjMat_Cam * ProjMat_Cam_Inv * vec4(DestPos, 1.0);"
"vec3 LightVec_Incident = vec3(TmpVec) / TmpVec.w;"
"vec3 EyeVec_Incident = LightVec_Incident - EyePos;"

"TmpVec=ProjMat_Light * TmpVec;"
"vec3 GBufferProj=vec3(TmpVec) / TmpVec.w;"

"if (abs(GBufferProj.x) > 1.0)discard;"
"if (abs(GBufferProj.y) > 1.0)discard;"
"if (abs(GBufferProj.z) > 1.0)discard;"

"float mask=texture(tex0, vec2(GBufferCoord.x, GBufferCoord.y)).x;"

"vec4 GBufferData=texture(tex2,GBufferCoord);"
"vec3 Normal=vec3(transpose(ObjMat_Light)*transpose(ObjMat_Cam_Inv)*vec4(UnPackNorm(GBufferData.zw),0.0));"

"BaseColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);"

"if (mask > 0.0f){"
//diffuse color
"float LightVec_Len = length(LightVec_Incident);"
"float I_Diff=-dot(LightVec_Incident, Normal) / (LightVec_Len * LightVec_Len * LightVec_Len);"
"if (I_Diff > 0.0)BaseColor.xyz += I_Diff * UnPackVec4(GBufferData.x).xyz;"

//specular color
"vec4 Specular_Info=UnPackVec4(GBufferData.y);"
"float EyeVec_Len = length(EyeVec_Incident);"
"Specular_Info.w = exp(Specular_Info.w / 0.07238);" // glossiness value. this step convert a number in range [0, 0.5] to range [0, 1000].
"vec3 ReflectVec=normalize(LightVec_Incident - (2 * dot(LightVec_Incident, Normal)) * Normal);"
"float I_Spec=-dot(EyeVec_Incident, ReflectVec) / EyeVec_Len;"
"if (I_Spec > 0.0){"
"I_Spec = pow(I_Spec, Specular_Info.w) / (LightVec_Len * LightVec_Len);"
"BaseColor.xyz += I_Spec * Specular_Info.xyz;"
"}"

"BaseColor *= vec4(mask * Intensity, 1.0);"
"}"

"gl_FragDepth=texture(tex1, GBufferCoord).x;"
"}";

char SSAOShadowMask_VS[] =
"#version 400\n"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat_Cam;"
"mat4 ProjMat_Cam;"
"mat4 ObjMat_Cam_Inv;"
"mat4 ProjMat_Cam_Inv;"
"uvec2 GBufferSize;"
"};"

"layout(location = 0) in vec2 VertexPosition;"

"void main(){"
"gl_Position = vec4(VertexPosition, 0.0, 1.0);"
"}";

char SSAOShadowMask_FS[] =
"#version 400\n"

"layout(std140) uniform GlobalData{"
"mat4 ObjMat_Cam;"
"mat4 ProjMat_Cam;"
"mat4 ObjMat_Cam_Inv;"
"mat4 ProjMat_Cam_Inv;"
"uvec2 GBufferSize;"
"};"

"uniform sampler2D tex0, tex1;"

"layout(location = 0) out vec4 OutputColor;"

//Poisson samples are sorted by the distance
"float Poisson_X[16] = float[](-0.0185813775163211, 0.00987838877025593, 0.0158161369046160, 0.0393407798961443, -0.00194108741438204, -0.0434518425579765, -0.0224856458106236, -0.0636835347435893, 0.0472574019044190, 0.0690963384796983, 0.0675211647441149, 0.0280335509940221, -0.0659947525630424, -0.0788755644190642, 0.0610086817407035, -0.0485022634239897);"
"float Poisson_Y[16] = float[](0.00564195011333912, 0.0196368924367792, -0.0261038765958646, 0.00108969185269281, -0.0576244003151479, -0.0397377177566476, 0.0558176093492525, 0.00355066286071482, -0.0465579983854927, -0.0184262330816445, 0.0261101636636018, 0.0673435256351129, 0.0351113356882897, -0.0283602627109986, 0.0589140423604903, 0.0747343562445035);"
"const float M_PI = 3.14159265358979323846;"

"vec4 UnPackVec4(float Packed_Data){"
"uint IntColor_Packed=floatBitsToUint(Packed_Data);"
"vec4 RetValue;"
"RetValue.x=float(IntColor_Packed&255)/255.0;"
"RetValue.y=float((IntColor_Packed/256)&255)/255.0;"
"RetValue.z=float((IntColor_Packed/65536)&255)/255.0;"
"RetValue.w=float((IntColor_Packed/16777216)&255)/255.0;" //7-bit for w
"return RetValue;"
"}"
"vec3 UnPackNorm(vec2 Norm_Packed){"
"vec3 RetValue;"
"RetValue.z = dot(Norm_Packed, Norm_Packed) * 2.0 - 1.0;"
"if(RetValue.z > -0.999999)Norm_Packed = normalize(Norm_Packed);"
"RetValue.xy = Norm_Packed * sqrt(1 - RetValue.z * RetValue.z);"
"return RetValue;"
"}"

"vec3 GetPos(vec2 TexCoord){"
"vec3 DestPos=vec3(TexCoord*2.0-1.0,texture(tex0, TexCoord).x*2.0-1.0);"
"vec4 TmpVec=ProjMat_Cam_Inv * vec4(DestPos, 1.0);"
"return vec3(TmpVec) / TmpVec.w;"
"}"
"vec3 GetNorm(vec2 TexCoord){"
"return vec3(vec4(UnPackNorm(texture(tex1, TexCoord).zw),0.0));"
"}"

"void main(){"
"vec2 GBufferCoord=vec2(gl_FragCoord.x / GBufferSize.x, gl_FragCoord.y / GBufferSize.y);"

"int i, SampleUsed = 16;"
"float SampleRadius = sqrt(M_PI * (Poisson_X[SampleUsed - 1] * Poisson_X[SampleUsed - 1] + Poisson_Y[SampleUsed - 1] * Poisson_Y[SampleUsed - 1]) / (float(SampleUsed)));"
"float AOValue = 0.0f;"

"float Dir = 2 * M_PI * fract(cos(gl_FragCoord.x * (12.9898) + gl_FragCoord.y * (4.1414)) * 43758.5453);" // a random number generator (unknown source from the Internet. Good enough for our purpose.
"vec2 dX = vec2(cos(Dir), sin(Dir)) / 16.0f;"
"vec2 dY = vec2(-sin(Dir), cos(Dir)) / 16.0f;"

"vec3 CenterPos = GetPos(GBufferCoord);"
"vec3 CenterNorm = GetNorm(GBufferCoord);"
"SampleRadius *= length(CenterPos);"

"for(i = 0;i<SampleUsed;i++){"
"vec3 SampleVector = GetPos(vec2(GBufferCoord + (Poisson_X[i] * dX + Poisson_Y[i] * dY) / SampleRadius)) - CenterPos;"
"AOValue += max(0.0, dot(normalize(SampleVector), CenterNorm)) / (1.0 + 32.0f * dot(SampleVector, SampleVector));"
"}"
"OutputColor=vec4(1.0 - AOValue / SampleUsed, 0.0, 0.0, 0.0);"
"}";

char AmbientLight_VS[] =
"#version 400\n"

"layout(std140) uniform GlobalData{"
"vec3 Intensity;"
"uvec2 GBufferSize;"
"};"

"layout(location = 0) in vec2 VertexPosition;"

"void main(){"
"gl_Position = vec4(VertexPosition, 0.0, 1.0);"
"}";

char AmbientLight_FS[] =
"#version 400\n"

"const uint AO_MASK_EXIST = 1 << 0;"
"const uint INTENSITY_MAP_EXIST = 1 << 1;"

"layout(std140) uniform GlobalData{"
"vec3 Intensity;"
"uvec2 GBufferSize;"
"};"

"uniform uint Status;"

"uniform sampler2D tex0,tex1,tex2;" //tex0 for shadow mask, tex1 for depth, tex2 for g-buffer

"layout(location = 0) out vec4 BaseColor;"
"out float gl_FragDepth;"

"vec4 UnPackVec4(float Packed_Data){"
"uint IntColor_Packed=floatBitsToUint(Packed_Data);"
"vec4 RetValue;"
"RetValue.x=float(IntColor_Packed&255)/255.0;"
"RetValue.y=float((IntColor_Packed/256)&255)/255.0;"
"RetValue.z=float((IntColor_Packed/65536)&255)/255.0;"
"RetValue.w=float((IntColor_Packed/16777216)&255)/255.0;" //7-bit for w
"return RetValue;"
"}"

"void main(){"
"vec2 GBufferCoord=vec2(gl_FragCoord.x/GBufferSize.x,gl_FragCoord.y/GBufferSize.y);"

"vec4 GBufferData=texture(tex2,GBufferCoord);"

"float mask;"
"if((Status & AO_MASK_EXIST) > 0)mask=texture(tex0, vec2(GBufferCoord.x, GBufferCoord.y)).x;"
"else mask = 1.0f;"

"BaseColor=vec4(Intensity * mask * UnPackVec4(GBufferData.x).xyz,1.0);"

"gl_FragDepth=texture(tex1, GBufferCoord).x;"
"}";

#endif