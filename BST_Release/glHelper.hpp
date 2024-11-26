//OpenGL related functions. Make sure the calling thread can access OpenGL resources!
UBINT LoadVertexShader(UBCHAR *ShaderPath);
UBINT LoadPixelShader(UBCHAR *ShaderPath);
UBINT LinkShaderProgram(UBINT VS, UBINT PS);
/*-------------------------------- IMPLEMENTATION --------------------------------*/
UBINT LoadVertexShader(UBCHAR *ShaderPath){
	UBINT VS, ShaderLen, Result;
	char *ShaderSrc, *ShaderLog;
	ShaderSrc = (char *)nsFile::ReadFileToMem(ShaderPath, &ShaderLen);
	VS = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VS, 1, (const GLchar **)&ShaderSrc, NULL);
	nsBasic::MemFree( ShaderSrc, ShaderLen);
	glCompileShader(VS);
	glGetShaderiv(VS, GL_COMPILE_STATUS, (int *)&Result);
	if (!Result){
		//Add breakpoint here to debug your shader
		glGetShaderiv(VS, GL_INFO_LOG_LENGTH, (int *)&Result);
		ShaderLog = (char *)nsBasic::MemAlloc( Result);
		glGetShaderInfoLog(VS, Result, (GLsizei *)&Result, ShaderLog);
		nsBasic::MemFree( ShaderLog, Result);
		return 0;
	}
	else return VS;
}
UBINT LoadPixelShader(UBCHAR *ShaderPath){
	UBINT PS, ShaderLen, Result;
	char *ShaderSrc, *ShaderLog;
	ShaderSrc = (char *)nsFile::ReadFileToMem(ShaderPath, &ShaderLen);
	PS = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(PS, 1, (const GLchar **)&ShaderSrc, NULL);
	nsBasic::MemFree( ShaderSrc, ShaderLen);
	glCompileShader(PS);
	glGetShaderiv(PS, GL_COMPILE_STATUS, (int *)&Result);
	if (!Result){
		//Add breakpoint here to debug your shader
		glGetShaderiv(PS, GL_INFO_LOG_LENGTH, (int *)&Result);
		ShaderLog = (char *)nsBasic::MemAlloc( Result);
		glGetShaderInfoLog(PS, Result, (GLsizei *)&Result, ShaderLog);
		nsBasic::MemFree( ShaderLog, Result);
		return 0;
	}
	else return PS;
}
UBINT LinkShaderProgram(UBINT VS, UBINT PS){
	UBINT SP, Result;
	char *ProgramLog;
	SP = glCreateProgram();
	glAttachShader(SP, VS);
	glAttachShader(SP, PS);
	glLinkProgram(SP);
	glGetProgramiv(SP, GL_LINK_STATUS, (int *)&Result);
	if (!Result){
		//Add breakpoint here to debug your program
		glGetProgramiv(SP, GL_INFO_LOG_LENGTH, (int *)&Result);
		ProgramLog = (char *)nsBasic::MemAlloc( Result);
		glGetProgramInfoLog(SP, Result, (GLsizei *)&Result, ProgramLog);
		nsBasic::MemFree(ProgramLog, Result);
		return 0;
	}
	else{
		glDeleteShader(VS);
		glDeleteShader(PS);
		return SP;
	}
}