#include "SceneNode_SceneMgr.hpp"
#include "SceneNode_Prog_Raster.hpp"
#include "SceneNode_Prog_Light.hpp"
#include "SceneNode_VP.hpp"
#include "SceneNode_Light.hpp"
#include "RenderCore.hpp"

namespace nsScene{
	void RenderOnce(nsRender::RenderCore *lpCore, nsBasic::IBase *SceneRoot){
		nsScene::VPScheduler curVPS;
		nsScene::RenderList *VPList = nullptr;
		GLuint GLVAO;

		glEnable(GL_TEXTURE_2D);
		glGenVertexArrays(1, &GLVAO);
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindVertexArray(GLVAO);
		glEnableVertexAttribArray(0); //Pos
		glEnableVertexAttribArray(1); //Norm
		glEnableVertexAttribArray(2); //TexCoord

		curVPS.Initialize(SceneRoot);
		while (curVPS.NextVPQuery(SceneRoot));
		curVPS.Sort();
		curVPS.Execute(SceneRoot, &lpCore->Scheduler);
		lpCore->Scheduler.Compile();
		if (lpCore->Scheduler.CanExecute()){
			lpCore->Scheduler.Execute(lpCore->CurDevice);
		}
		curVPS.FrameEnd();
		lpCore->Scheduler.Clear();
		lpCore->CurDevice->FrameEnd();

		glDeleteVertexArrays(1, &GLVAO);
		glDisableClientState(GL_VERTEX_ARRAY);
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		glUseProgram(0);

		//End Render Loop
	}
}