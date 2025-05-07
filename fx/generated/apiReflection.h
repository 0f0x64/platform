namespace BasicCam {

#ifdef REFLECTION

void setCamKey_impl(  timestamp camTime, keyType camType, position eye, position at, position up, int angle, sliderType sType, position slide, camAxis axisType, rotation fly, int jitter);

void setCamKey_reflect (CALLER_INFO, timestamp &camTime, keyType &camType, position &eye, position &at, position &up, int &angle, sliderType &sType, position &slide, camAxis &axisType, rotation &fly, int &jitter)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		camTime = (timestamp)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass)  camType = (keyType)cmdParamDesc[cmdCounter].param[1].value[0];
		if (!cmdParamDesc[cmdCounter].param[2].bypass) 
		{
			eye.x = cmdParamDesc[cmdCounter].param[2].value[0];
			eye.y = cmdParamDesc[cmdCounter].param[2].value[1];
			eye.z = cmdParamDesc[cmdCounter].param[2].value[2];
		}
		if (!cmdParamDesc[cmdCounter].param[3].bypass) 
		{
			at.x = cmdParamDesc[cmdCounter].param[3].value[0];
			at.y = cmdParamDesc[cmdCounter].param[3].value[1];
			at.z = cmdParamDesc[cmdCounter].param[3].value[2];
		}
		if (!cmdParamDesc[cmdCounter].param[4].bypass) 
		{
			up.x = cmdParamDesc[cmdCounter].param[4].value[0];
			up.y = cmdParamDesc[cmdCounter].param[4].value[1];
			up.z = cmdParamDesc[cmdCounter].param[4].value[2];
		}
		if (!cmdParamDesc[cmdCounter].param[5].bypass) 		angle = (int)cmdParamDesc[cmdCounter].param[5].value[0];
		if (!cmdParamDesc[cmdCounter].param[6].bypass)  sType = (sliderType)cmdParamDesc[cmdCounter].param[6].value[0];
		if (!cmdParamDesc[cmdCounter].param[7].bypass) 
		{
			slide.x = cmdParamDesc[cmdCounter].param[7].value[0];
			slide.y = cmdParamDesc[cmdCounter].param[7].value[1];
			slide.z = cmdParamDesc[cmdCounter].param[7].value[2];
		}
		if (!cmdParamDesc[cmdCounter].param[8].bypass)  axisType = (camAxis)cmdParamDesc[cmdCounter].param[8].value[0];
		if (!cmdParamDesc[cmdCounter].param[9].bypass) 
		{
			fly.x = cmdParamDesc[cmdCounter].param[9].value[0];
			fly.y = cmdParamDesc[cmdCounter].param[9].value[1];
			fly.z = cmdParamDesc[cmdCounter].param[9].value[2];
		}
		if (!cmdParamDesc[cmdCounter].param[10].bypass) 		jitter = (int)cmdParamDesc[cmdCounter].param[10].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 11;
		cmdParamDesc[cmdCounter].param[0].value[0] = camTime;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "timestamp"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "camTime"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = (int)camType;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "keyType"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "camType"); 
		cmdParamDesc[cmdCounter].param[2].value[0] = eye.x;
		cmdParamDesc[cmdCounter].param[2].value[1] = eye.y;
		cmdParamDesc[cmdCounter].param[2].value[2] = eye.z;
		cmdParamDesc[cmdCounter].param[2].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[2].type, "position"); 
		strcpy(cmdParamDesc[cmdCounter].param[2].name, "eye"); 
		cmdParamDesc[cmdCounter].param[3].value[0] = at.x;
		cmdParamDesc[cmdCounter].param[3].value[1] = at.y;
		cmdParamDesc[cmdCounter].param[3].value[2] = at.z;
		cmdParamDesc[cmdCounter].param[3].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[3].type, "position"); 
		strcpy(cmdParamDesc[cmdCounter].param[3].name, "at"); 
		cmdParamDesc[cmdCounter].param[4].value[0] = up.x;
		cmdParamDesc[cmdCounter].param[4].value[1] = up.y;
		cmdParamDesc[cmdCounter].param[4].value[2] = up.z;
		cmdParamDesc[cmdCounter].param[4].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[4].type, "position"); 
		strcpy(cmdParamDesc[cmdCounter].param[4].name, "up"); 
		cmdParamDesc[cmdCounter].param[5].value[0] = angle;
		cmdParamDesc[cmdCounter].param[5].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[5].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[5].name, "angle"); 
		cmdParamDesc[cmdCounter].param[6].value[0] = (int)sType;
		cmdParamDesc[cmdCounter].param[6].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[6].type, "sliderType"); 
		strcpy(cmdParamDesc[cmdCounter].param[6].name, "sType"); 
		cmdParamDesc[cmdCounter].param[7].value[0] = slide.x;
		cmdParamDesc[cmdCounter].param[7].value[1] = slide.y;
		cmdParamDesc[cmdCounter].param[7].value[2] = slide.z;
		cmdParamDesc[cmdCounter].param[7].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[7].type, "position"); 
		strcpy(cmdParamDesc[cmdCounter].param[7].name, "slide"); 
		cmdParamDesc[cmdCounter].param[8].value[0] = (int)axisType;
		cmdParamDesc[cmdCounter].param[8].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[8].type, "camAxis"); 
		strcpy(cmdParamDesc[cmdCounter].param[8].name, "axisType"); 
		cmdParamDesc[cmdCounter].param[9].value[0] = fly.x;
		cmdParamDesc[cmdCounter].param[9].value[1] = fly.y;
		cmdParamDesc[cmdCounter].param[9].value[2] = fly.z;
		cmdParamDesc[cmdCounter].param[9].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[9].type, "rotation"); 
		strcpy(cmdParamDesc[cmdCounter].param[9].name, "fly"); 
		cmdParamDesc[cmdCounter].param[10].value[0] = jitter;
		cmdParamDesc[cmdCounter].param[10].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[10].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[10].name, "jitter"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("setCamKey"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define setCamKey(camTime, camType, eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z, angle, sType, slide_x, slide_y, slide_z, axisType, fly_x, fly_y, fly_z, jitter) setCamKey_ref (__FILE__, __LINE__, camTime, camType, position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle, sType, position {slide_x, slide_y, slide_z }, axisType, rotation {fly_x, fly_y, fly_z }, jitter)

void setCamKey_ref ( CALLER_INFO,  timestamp camTime, keyType camType, position eye, position at, position up, int angle, sliderType sType, position slide, camAxis axisType, rotation fly, int jitter)
{
	setCamKey_reflect (srcFileName, srcLine, camTime, camType, eye, at, up, angle, sType, slide, axisType, fly, jitter);
	setCamKey_impl (camTime, camType, eye, at, up, angle, sType, slide, axisType, fly, jitter);
	cmdLevel--;
}

#else
#undef setCamKey
#define setCamKey(camTime, camType, eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z, angle, sType, slide_x, slide_y, slide_z, axisType, fly_x, fly_y, fly_z, jitter) setCamKey_( camTime, camType, position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle, sType, position {slide_x, slide_y, slide_z }, axisType, rotation {fly_x, fly_y, fly_z }, jitter)
#endif
}
namespace Cubemap {

#ifdef REFLECTION

void CalcCubemap_impl(  texture target);

void CalcCubemap_reflect (CALLER_INFO, texture &target)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  target = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 1;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)target;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "target"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("CalcCubemap"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define CalcCubemap(target) CalcCubemap_ref (__FILE__, __LINE__, target)

void CalcCubemap_ref ( CALLER_INFO,  texture target)
{
	CalcCubemap_reflect (srcFileName, srcLine, target);
	CalcCubemap_impl (target);
	cmdLevel--;
}

#else
#undef CalcCubemap
#define CalcCubemap(target) CalcCubemap_( target)
#endif
}
namespace Cubemap {

#ifdef REFLECTION

void ShowCubemap_impl( texture envTexture);

void ShowCubemap_reflect (CALLER_INFO, texture &envTexture)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  envTexture = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 1;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)envTexture;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "envTexture"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("ShowCubemap"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define ShowCubemap(envTexture) ShowCubemap_ref (__FILE__, __LINE__, envTexture)

void ShowCubemap_ref ( CALLER_INFO, texture envTexture)
{
	ShowCubemap_reflect (srcFileName, srcLine, envTexture);
	ShowCubemap_impl (envTexture);
	cmdLevel--;
}

#else
#undef ShowCubemap
#define ShowCubemap(envTexture) ShowCubemap_( envTexture)
#endif
}
namespace gfx {

#ifdef REFLECTION

void SetRT_impl(  texture targetRT, int level);

void SetRT_reflect (CALLER_INFO, texture &targetRT, int &level)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  targetRT = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass) 		level = (int)cmdParamDesc[cmdCounter].param[1].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 2;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)targetRT;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "targetRT"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = level;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "level"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("SetRT"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define SetRT(targetRT, level) SetRT_ref (__FILE__, __LINE__, targetRT, level)

void SetRT_ref ( CALLER_INFO,  texture targetRT, int level)
{
	SetRT_reflect (srcFileName, srcLine, targetRT, level);
	SetRT_impl (targetRT, level);
	cmdLevel--;
}

#else
#undef SetRT
#define SetRT(targetRT, level) SetRT_( targetRT, level)
#endif
}
namespace gfx {

#ifdef REFLECTION

void CreateMips_impl( );

void CreateMips_reflect (CALLER_INFO)
{

	if (paramsAreLoaded) {
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 0;

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("CreateMips"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define CreateMips() CreateMips_ref (__FILE__, __LINE__)

void CreateMips_ref ( CALLER_INFO)
{
	CreateMips_reflect (srcFileName, srcLine);
	CreateMips_impl ();
	cmdLevel--;
}

#else
#undef CreateMips
#define CreateMips() CreateMips_( )
#endif
}
namespace gfx {

#ifdef REFLECTION

void SetDepthMode_impl(  depthmode mode);

void SetDepthMode_reflect (CALLER_INFO, depthmode &mode)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  mode = (depthmode)cmdParamDesc[cmdCounter].param[0].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 1;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)mode;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "depthmode"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "mode"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("SetDepthMode"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define SetDepthMode(mode) SetDepthMode_ref (__FILE__, __LINE__, mode)

void SetDepthMode_ref ( CALLER_INFO,  depthmode mode)
{
	SetDepthMode_reflect (srcFileName, srcLine, mode);
	SetDepthMode_impl (mode);
	cmdLevel--;
}

#else
#undef SetDepthMode
#define SetDepthMode(mode) SetDepthMode_( mode)
#endif
}
namespace gfx {

#ifdef REFLECTION

void Draw_impl(  int quadcount, int instances);

void Draw_reflect (CALLER_INFO, int &quadcount, int &instances)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		quadcount = (int)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass) 		instances = (int)cmdParamDesc[cmdCounter].param[1].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 2;
		cmdParamDesc[cmdCounter].param[0].value[0] = quadcount;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "quadcount"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = instances;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "instances"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("Draw"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define Draw(quadcount, instances) Draw_ref (__FILE__, __LINE__, quadcount, instances)

void Draw_ref ( CALLER_INFO,  int quadcount, int instances)
{
	Draw_reflect (srcFileName, srcLine, quadcount, instances);
	Draw_impl (quadcount, instances);
	cmdLevel--;
}

#else
#undef Draw
#define Draw(quadcount, instances) Draw_( quadcount, instances)
#endif
}
namespace gfx {

#ifdef REFLECTION

void SetCamera_impl(  position eye, position at, position up, unsigned int angle);

void SetCamera_reflect (CALLER_INFO, position &eye, position &at, position &up, unsigned int &angle)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 
		{
			eye.x = cmdParamDesc[cmdCounter].param[0].value[0];
			eye.y = cmdParamDesc[cmdCounter].param[0].value[1];
			eye.z = cmdParamDesc[cmdCounter].param[0].value[2];
		}
		if (!cmdParamDesc[cmdCounter].param[1].bypass) 
		{
			at.x = cmdParamDesc[cmdCounter].param[1].value[0];
			at.y = cmdParamDesc[cmdCounter].param[1].value[1];
			at.z = cmdParamDesc[cmdCounter].param[1].value[2];
		}
		if (!cmdParamDesc[cmdCounter].param[2].bypass) 
		{
			up.x = cmdParamDesc[cmdCounter].param[2].value[0];
			up.y = cmdParamDesc[cmdCounter].param[2].value[1];
			up.z = cmdParamDesc[cmdCounter].param[2].value[2];
		}
		if (!cmdParamDesc[cmdCounter].param[3].bypass) 		angle = (unsigned int)cmdParamDesc[cmdCounter].param[3].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 4;
		cmdParamDesc[cmdCounter].param[0].value[0] = eye.x;
		cmdParamDesc[cmdCounter].param[0].value[1] = eye.y;
		cmdParamDesc[cmdCounter].param[0].value[2] = eye.z;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "position"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "eye"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = at.x;
		cmdParamDesc[cmdCounter].param[1].value[1] = at.y;
		cmdParamDesc[cmdCounter].param[1].value[2] = at.z;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "position"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "at"); 
		cmdParamDesc[cmdCounter].param[2].value[0] = up.x;
		cmdParamDesc[cmdCounter].param[2].value[1] = up.y;
		cmdParamDesc[cmdCounter].param[2].value[2] = up.z;
		cmdParamDesc[cmdCounter].param[2].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[2].type, "position"); 
		strcpy(cmdParamDesc[cmdCounter].param[2].name, "up"); 
		cmdParamDesc[cmdCounter].param[3].value[0] = angle;
		cmdParamDesc[cmdCounter].param[3].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[3].type, "unsigned int"); 
		strcpy(cmdParamDesc[cmdCounter].param[3].name, "angle"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("SetCamera"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define SetCamera(eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z, angle) SetCamera_ref (__FILE__, __LINE__, position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle)

void SetCamera_ref ( CALLER_INFO,  position eye, position at, position up, unsigned int angle)
{
	SetCamera_reflect (srcFileName, srcLine, eye, at, up, angle);
	SetCamera_impl (eye, at, up, angle);
	cmdLevel--;
}

#else
#undef SetCamera
#define SetCamera(eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z, angle) SetCamera_( position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle)
#endif
}
namespace gfx {

#ifdef REFLECTION

void ClearRT_impl(  color4 color);

void ClearRT_reflect (CALLER_INFO, color4 &color)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		color.x = cmdParamDesc[cmdCounter].param[0].value[0];
		color.y = cmdParamDesc[cmdCounter].param[0].value[1];
		color.z = cmdParamDesc[cmdCounter].param[0].value[2];
		color.w = cmdParamDesc[cmdCounter].param[0].value[3];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 1;
		cmdParamDesc[cmdCounter].param[0].value[0] = color.x;
		cmdParamDesc[cmdCounter].param[0].value[1] = color.y;
		cmdParamDesc[cmdCounter].param[0].value[2] = color.z;
		cmdParamDesc[cmdCounter].param[0].value[3] = color.w;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "color4"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "color"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("ClearRT"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define ClearRT(color_x, color_y, color_z, color_w) ClearRT_ref (__FILE__, __LINE__, color4 {color_x, color_y, color_z, color_w })

void ClearRT_ref ( CALLER_INFO,  color4 color)
{
	ClearRT_reflect (srcFileName, srcLine, color);
	ClearRT_impl (color);
	cmdLevel--;
}

#else
#undef ClearRT
#define ClearRT(color_x, color_y, color_z, color_w) ClearRT_( color4 {color_x, color_y, color_z, color_w })
#endif
}
namespace gfx {

#ifdef REFLECTION

void ClearRTDepth_impl( );

void ClearRTDepth_reflect (CALLER_INFO)
{

	if (paramsAreLoaded) {
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 0;

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("ClearRTDepth"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define ClearRTDepth() ClearRTDepth_ref (__FILE__, __LINE__)

void ClearRTDepth_ref ( CALLER_INFO)
{
	ClearRTDepth_reflect (srcFileName, srcLine);
	ClearRTDepth_impl ();
	cmdLevel--;
}

#else
#undef ClearRTDepth
#define ClearRTDepth() ClearRTDepth_( )
#endif
}
namespace gfx {

#ifdef REFLECTION

void SetBlendMode_impl(  blendmode mode, blendop op);

void SetBlendMode_reflect (CALLER_INFO, blendmode &mode, blendop &op)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  mode = (blendmode)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass)  op = (blendop)cmdParamDesc[cmdCounter].param[1].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 2;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)mode;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "blendmode"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "mode"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = (int)op;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "blendop"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "op"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("SetBlendMode"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define SetBlendMode(mode, op) SetBlendMode_ref (__FILE__, __LINE__, mode, op)

void SetBlendMode_ref ( CALLER_INFO,  blendmode mode, blendop op)
{
	SetBlendMode_reflect (srcFileName, srcLine, mode, op);
	SetBlendMode_impl (mode, op);
	cmdLevel--;
}

#else
#undef SetBlendMode
#define SetBlendMode(mode, op) SetBlendMode_( mode, op)
#endif
}
namespace gfx {

#ifdef REFLECTION

void SetCull_impl(  cullmode mode);

void SetCull_reflect (CALLER_INFO, cullmode &mode)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  mode = (cullmode)cmdParamDesc[cmdCounter].param[0].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 1;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)mode;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "cullmode"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "mode"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("SetCull"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define SetCull(mode) SetCull_ref (__FILE__, __LINE__, mode)

void SetCull_ref ( CALLER_INFO,  cullmode mode)
{
	SetCull_reflect (srcFileName, srcLine, mode);
	SetCull_impl (mode);
	cmdLevel--;
}

#else
#undef SetCull
#define SetCull(mode) SetCull_( mode)
#endif
}
namespace gfx {

#ifdef REFLECTION

void SetScissors_impl(  rect bbox);

void SetScissors_reflect (CALLER_INFO, rect &bbox)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		bbox.x = cmdParamDesc[cmdCounter].param[0].value[0];
		bbox.y = cmdParamDesc[cmdCounter].param[0].value[1];
		bbox.z = cmdParamDesc[cmdCounter].param[0].value[2];
		bbox.w = cmdParamDesc[cmdCounter].param[0].value[3];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 1;
		cmdParamDesc[cmdCounter].param[0].value[0] = bbox.x;
		cmdParamDesc[cmdCounter].param[0].value[1] = bbox.y;
		cmdParamDesc[cmdCounter].param[0].value[2] = bbox.z;
		cmdParamDesc[cmdCounter].param[0].value[3] = bbox.w;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "rect"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "bbox"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("SetScissors"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define SetScissors(bbox_x, bbox_y, bbox_z, bbox_w) SetScissors_ref (__FILE__, __LINE__, rect {bbox_x, bbox_y, bbox_z, bbox_w })

void SetScissors_ref ( CALLER_INFO,  rect bbox)
{
	SetScissors_reflect (srcFileName, srcLine, bbox);
	SetScissors_impl (bbox);
	cmdLevel--;
}

#else
#undef SetScissors
#define SetScissors(bbox_x, bbox_y, bbox_z, bbox_w) SetScissors_( rect {bbox_x, bbox_y, bbox_z, bbox_w })
#endif
}
namespace gfx {

#ifdef REFLECTION

void CopyRTColor_impl(  texture dst, texture src);

void CopyRTColor_reflect (CALLER_INFO, texture &dst, texture &src)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  dst = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass)  src = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 2;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)dst;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "dst"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = (int)src;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "src"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("CopyRTColor"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define CopyRTColor(dst, src) CopyRTColor_ref (__FILE__, __LINE__, dst, src)

void CopyRTColor_ref ( CALLER_INFO,  texture dst, texture src)
{
	CopyRTColor_reflect (srcFileName, srcLine, dst, src);
	CopyRTColor_impl (dst, src);
	cmdLevel--;
}

#else
#undef CopyRTColor
#define CopyRTColor(dst, src) CopyRTColor_( dst, src)
#endif
}
namespace gfx {

#ifdef REFLECTION

void CopyRTDepth_impl(  texture dst, texture src);

void CopyRTDepth_reflect (CALLER_INFO, texture &dst, texture &src)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  dst = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass)  src = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 2;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)dst;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "dst"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = (int)src;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "src"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("CopyRTDepth"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define CopyRTDepth(dst, src) CopyRTDepth_ref (__FILE__, __LINE__, dst, src)

void CopyRTDepth_ref ( CALLER_INFO,  texture dst, texture src)
{
	CopyRTDepth_reflect (srcFileName, srcLine, dst, src);
	CopyRTDepth_impl (dst, src);
	cmdLevel--;
}

#else
#undef CopyRTDepth
#define CopyRTDepth(dst, src) CopyRTDepth_( dst, src)
#endif
}
namespace Loop {

#ifdef REFLECTION

void cameraMan_impl( );

void cameraMan_reflect (CALLER_INFO)
{

	if (paramsAreLoaded) {
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 0;

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("cameraMan"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define cameraMan() cameraMan_ref (__FILE__, __LINE__)

void cameraMan_ref ( CALLER_INFO)
{
	cameraMan_reflect (srcFileName, srcLine);
	cameraMan_impl ();
	cmdLevel--;
}

#else
#undef cameraMan
#define cameraMan() cameraMan_( )
#endif
}
namespace Object {

#ifdef REFLECTION

void ShowObject_impl( texture geometry, texture normals, unsigned int quality, position pos);

void ShowObject_reflect (CALLER_INFO, texture &geometry, texture &normals, unsigned int &quality, position &pos)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  geometry = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass)  normals = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
		if (!cmdParamDesc[cmdCounter].param[2].bypass) 		quality = (unsigned int)cmdParamDesc[cmdCounter].param[2].value[0];
		if (!cmdParamDesc[cmdCounter].param[3].bypass) 
		{
			pos.x = cmdParamDesc[cmdCounter].param[3].value[0];
			pos.y = cmdParamDesc[cmdCounter].param[3].value[1];
			pos.z = cmdParamDesc[cmdCounter].param[3].value[2];
		}
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 4;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)geometry;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "geometry"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = (int)normals;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "normals"); 
		cmdParamDesc[cmdCounter].param[2].value[0] = quality;
		cmdParamDesc[cmdCounter].param[2].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[2].type, "unsigned int"); 
		strcpy(cmdParamDesc[cmdCounter].param[2].name, "quality"); 
		cmdParamDesc[cmdCounter].param[3].value[0] = pos.x;
		cmdParamDesc[cmdCounter].param[3].value[1] = pos.y;
		cmdParamDesc[cmdCounter].param[3].value[2] = pos.z;
		cmdParamDesc[cmdCounter].param[3].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[3].type, "position"); 
		strcpy(cmdParamDesc[cmdCounter].param[3].name, "pos"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("ShowObject"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define ShowObject(geometry, normals, quality, pos_x, pos_y, pos_z) ShowObject_ref (__FILE__, __LINE__, geometry, normals, quality, position {pos_x, pos_y, pos_z })

void ShowObject_ref ( CALLER_INFO, texture geometry, texture normals, unsigned int quality, position pos)
{
	ShowObject_reflect (srcFileName, srcLine, geometry, normals, quality, pos);
	ShowObject_impl (geometry, normals, quality, pos);
	cmdLevel--;
}

#else
#undef ShowObject
#define ShowObject(geometry, normals, quality, pos_x, pos_y, pos_z) ShowObject_( geometry, normals, quality, position {pos_x, pos_y, pos_z })
#endif
}
namespace Object {

#ifdef REFLECTION

void CalcNormals_impl(  texture srcGeomerty, texture targetNrml);

void CalcNormals_reflect (CALLER_INFO, texture &srcGeomerty, texture &targetNrml)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  srcGeomerty = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass)  targetNrml = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 2;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)srcGeomerty;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "srcGeomerty"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = (int)targetNrml;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "targetNrml"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("CalcNormals"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define CalcNormals(srcGeomerty, targetNrml) CalcNormals_ref (__FILE__, __LINE__, srcGeomerty, targetNrml)

void CalcNormals_ref ( CALLER_INFO,  texture srcGeomerty, texture targetNrml)
{
	CalcNormals_reflect (srcFileName, srcLine, srcGeomerty, targetNrml);
	CalcNormals_impl (srcGeomerty, targetNrml);
	cmdLevel--;
}

#else
#undef CalcNormals
#define CalcNormals(srcGeomerty, targetNrml) CalcNormals_( srcGeomerty, targetNrml)
#endif
}
namespace Object {

#ifdef REFLECTION

void CalcObject_impl( texture targetGeo, texture targetNrml);

void CalcObject_reflect (CALLER_INFO, texture &targetGeo, texture &targetNrml)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass)  targetGeo = (texture)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass)  targetNrml = (texture)cmdParamDesc[cmdCounter].param[1].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 2;
		cmdParamDesc[cmdCounter].param[0].value[0] = (int)targetGeo;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "targetGeo"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = (int)targetNrml;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "texture"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "targetNrml"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("CalcObject"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define CalcObject(targetGeo, targetNrml) CalcObject_ref (__FILE__, __LINE__, targetGeo, targetNrml)

void CalcObject_ref ( CALLER_INFO, texture targetGeo, texture targetNrml)
{
	CalcObject_reflect (srcFileName, srcLine, targetGeo, targetNrml);
	CalcObject_impl (targetGeo, targetNrml);
	cmdLevel--;
}

#else
#undef CalcObject
#define CalcObject(targetGeo, targetNrml) CalcObject_( targetGeo, targetNrml)
#endif
}
namespace tracker {

#ifdef REFLECTION

void oscillator_impl(  int a, int b);

void oscillator_reflect (CALLER_INFO, int &a, int &b)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		a = (int)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass) 		b = (int)cmdParamDesc[cmdCounter].param[1].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 2;
		cmdParamDesc[cmdCounter].param[0].value[0] = a;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "a"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = b;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "b"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("oscillator"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define oscillator(a, b) oscillator_ref (__FILE__, __LINE__, a, b)

void oscillator_ref ( CALLER_INFO,  int a, int b)
{
	oscillator_reflect (srcFileName, srcLine, a, b);
	oscillator_impl (a, b);
	cmdLevel--;
}

#else
#undef oscillator
#define oscillator(a, b) oscillator_( a, b)
#endif
}
namespace tracker {

#ifdef REFLECTION

void eq_impl(  int a);

void eq_reflect (CALLER_INFO, int &a)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		a = (int)cmdParamDesc[cmdCounter].param[0].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 1;
		cmdParamDesc[cmdCounter].param[0].value[0] = a;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "a"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("eq"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define eq(a) eq_ref (__FILE__, __LINE__, a)

void eq_ref ( CALLER_INFO,  int a)
{
	eq_reflect (srcFileName, srcLine, a);
	eq_impl (a);
	cmdLevel--;
}

#else
#undef eq
#define eq(a) eq_( a)
#endif
}
namespace tracker {

#ifdef REFLECTION

void Pitch_impl(  int count, unsigned char ...);

void Pitch_reflect (CALLER_INFO, int &count)
{

	if (paramsAreLoaded) {
	count = cmdParamDesc[cmdCounter].param[0].value[0];
	cmdParamDesc[cmdCounter].pCount = count+1;
	for (int i = 1; i <= count; i++)
	{
		va_params [i]= (unsigned char) cmdParamDesc[cmdCounter].param[i].value[0];
	}
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].param[0].value[0] = count ;
		cmdParamDesc[cmdCounter].pCount = count+1;
	for (int i = 0; i < 256; i++)
	{
		cmdParamDesc[cmdCounter].param[i].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[i].type , "int");
 		cmdParamDesc[cmdCounter].param[i].typeIndex = -1;
 		cmdParamDesc[cmdCounter].param[i]._min = 0;
		cmdParamDesc[cmdCounter].param[i]._max = 255;
		cmdParamDesc[cmdCounter].param[i]._dim = 1;
	}
	for (int i = 1; i <= count; i++)
	{
		cmdParamDesc[cmdCounter].param[i].value[0]= (int)va_params[i];
	}
	}

	AddToUI("Pitch"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define Pitch(...) Pitch_ref (__FILE__, __LINE__, __VA_ARGS__)

void Pitch_ref ( CALLER_INFO,  int count, unsigned char ...)
{
	VA_READ

	Pitch_reflect (srcFileName, srcLine, count);
	Pitch_impl (count, 0 );
	cmdLevel--;
}

#else
#undef Pitch
#define Pitch(...) Pitch_( __VA_ARGS__)
#endif
}
namespace tracker {

#ifdef REFLECTION

void Clip_impl(  timestamp pos, int len, int repeat, int bpmScale, overdub overDub, int swing);

void Clip_reflect (CALLER_INFO, timestamp &pos, int &len, int &repeat, int &bpmScale, overdub &overDub, int &swing)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		pos = (timestamp)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass) 		len = (int)cmdParamDesc[cmdCounter].param[1].value[0];
		if (!cmdParamDesc[cmdCounter].param[2].bypass) 		repeat = (int)cmdParamDesc[cmdCounter].param[2].value[0];
		if (!cmdParamDesc[cmdCounter].param[3].bypass) 		bpmScale = (int)cmdParamDesc[cmdCounter].param[3].value[0];
		if (!cmdParamDesc[cmdCounter].param[4].bypass)  overDub = (overdub)cmdParamDesc[cmdCounter].param[4].value[0];
		if (!cmdParamDesc[cmdCounter].param[5].bypass) 		swing = (int)cmdParamDesc[cmdCounter].param[5].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 6;
		cmdParamDesc[cmdCounter].param[0].value[0] = pos;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "timestamp"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "pos"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = len;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "len"); 
		cmdParamDesc[cmdCounter].param[2].value[0] = repeat;
		cmdParamDesc[cmdCounter].param[2].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[2].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[2].name, "repeat"); 
		cmdParamDesc[cmdCounter].param[3].value[0] = bpmScale;
		cmdParamDesc[cmdCounter].param[3].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[3].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[3].name, "bpmScale"); 
		cmdParamDesc[cmdCounter].param[4].value[0] = (int)overDub;
		cmdParamDesc[cmdCounter].param[4].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[4].type, "overdub"); 
		strcpy(cmdParamDesc[cmdCounter].param[4].name, "overDub"); 
		cmdParamDesc[cmdCounter].param[5].value[0] = swing;
		cmdParamDesc[cmdCounter].param[5].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[5].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[5].name, "swing"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("Clip"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define Clip(pos, len, repeat, bpmScale, overDub, swing) Clip_ref (__FILE__, __LINE__, pos, len, repeat, bpmScale, overDub, swing)

void Clip_ref ( CALLER_INFO,  timestamp pos, int len, int repeat, int bpmScale, overdub overDub, int swing)
{
	Clip_reflect (srcFileName, srcLine, pos, len, repeat, bpmScale, overDub, swing);
	Clip_impl (pos, len, repeat, bpmScale, overDub, swing);
	cmdLevel--;
}

#else
#undef Clip
#define Clip(pos, len, repeat, bpmScale, overDub, swing) Clip_( pos, len, repeat, bpmScale, overDub, swing)
#endif
}
namespace tracker {

#ifdef REFLECTION

void kick_impl(  volume vol, panorama pan, volume send, switcher solo, switcher mute);

void kick_reflect (CALLER_INFO, volume &vol, panorama &pan, volume &send, switcher &solo, switcher &mute)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		vol = (volume)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass) 		pan = (panorama)cmdParamDesc[cmdCounter].param[1].value[0];
		if (!cmdParamDesc[cmdCounter].param[2].bypass) 		send = (volume)cmdParamDesc[cmdCounter].param[2].value[0];
		if (!cmdParamDesc[cmdCounter].param[3].bypass)  solo = (switcher)cmdParamDesc[cmdCounter].param[3].value[0];
		if (!cmdParamDesc[cmdCounter].param[4].bypass)  mute = (switcher)cmdParamDesc[cmdCounter].param[4].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 5;
		cmdParamDesc[cmdCounter].param[0].value[0] = vol;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "volume"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "vol"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = pan;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "panorama"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "pan"); 
		cmdParamDesc[cmdCounter].param[2].value[0] = send;
		cmdParamDesc[cmdCounter].param[2].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[2].type, "volume"); 
		strcpy(cmdParamDesc[cmdCounter].param[2].name, "send"); 
		cmdParamDesc[cmdCounter].param[3].value[0] = (int)solo;
		cmdParamDesc[cmdCounter].param[3].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[3].type, "switcher"); 
		strcpy(cmdParamDesc[cmdCounter].param[3].name, "solo"); 
		cmdParamDesc[cmdCounter].param[4].value[0] = (int)mute;
		cmdParamDesc[cmdCounter].param[4].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[4].type, "switcher"); 
		strcpy(cmdParamDesc[cmdCounter].param[4].name, "mute"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("kick"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define kick(vol, pan, send, solo, mute) kick_ref (__FILE__, __LINE__, vol, pan, send, solo, mute)

void kick_ref ( CALLER_INFO,  volume vol, panorama pan, volume send, switcher solo, switcher mute)
{
	kick_reflect (srcFileName, srcLine, vol, pan, send, solo, mute);
	kick_impl (vol, pan, send, solo, mute);
	cmdLevel--;
}

#else
#undef kick
#define kick(vol, pan, send, solo, mute) kick_( vol, pan, send, solo, mute)
#endif
}
namespace tracker {

#ifdef REFLECTION

void snare_impl(  volume vol, panorama pan, volume send, switcher solo, switcher mute);

void snare_reflect (CALLER_INFO, volume &vol, panorama &pan, volume &send, switcher &solo, switcher &mute)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		vol = (volume)cmdParamDesc[cmdCounter].param[0].value[0];
		if (!cmdParamDesc[cmdCounter].param[1].bypass) 		pan = (panorama)cmdParamDesc[cmdCounter].param[1].value[0];
		if (!cmdParamDesc[cmdCounter].param[2].bypass) 		send = (volume)cmdParamDesc[cmdCounter].param[2].value[0];
		if (!cmdParamDesc[cmdCounter].param[3].bypass)  solo = (switcher)cmdParamDesc[cmdCounter].param[3].value[0];
		if (!cmdParamDesc[cmdCounter].param[4].bypass)  mute = (switcher)cmdParamDesc[cmdCounter].param[4].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 5;
		cmdParamDesc[cmdCounter].param[0].value[0] = vol;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "volume"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "vol"); 
		cmdParamDesc[cmdCounter].param[1].value[0] = pan;
		cmdParamDesc[cmdCounter].param[1].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[1].type, "panorama"); 
		strcpy(cmdParamDesc[cmdCounter].param[1].name, "pan"); 
		cmdParamDesc[cmdCounter].param[2].value[0] = send;
		cmdParamDesc[cmdCounter].param[2].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[2].type, "volume"); 
		strcpy(cmdParamDesc[cmdCounter].param[2].name, "send"); 
		cmdParamDesc[cmdCounter].param[3].value[0] = (int)solo;
		cmdParamDesc[cmdCounter].param[3].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[3].type, "switcher"); 
		strcpy(cmdParamDesc[cmdCounter].param[3].name, "solo"); 
		cmdParamDesc[cmdCounter].param[4].value[0] = (int)mute;
		cmdParamDesc[cmdCounter].param[4].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[4].type, "switcher"); 
		strcpy(cmdParamDesc[cmdCounter].param[4].name, "mute"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("snare"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define snare(vol, pan, send, solo, mute) snare_ref (__FILE__, __LINE__, vol, pan, send, solo, mute)

void snare_ref ( CALLER_INFO,  volume vol, panorama pan, volume send, switcher solo, switcher mute)
{
	snare_reflect (srcFileName, srcLine, vol, pan, send, solo, mute);
	snare_impl (vol, pan, send, solo, mute);
	cmdLevel--;
}

#else
#undef snare
#define snare(vol, pan, send, solo, mute) snare_( vol, pan, send, solo, mute)
#endif
}
namespace tracker {

#ifdef REFLECTION

void Track_impl(  int masterBPM);

void Track_reflect (CALLER_INFO, int &masterBPM)
{

	if (paramsAreLoaded) {
		if (!cmdParamDesc[cmdCounter].param[0].bypass) 		masterBPM = (int)cmdParamDesc[cmdCounter].param[0].value[0];
	} else {
		strcpy(cmdParamDesc[cmdCounter].caller.fileName,srcFileName);
		cmdParamDesc[cmdCounter].caller.line = srcLine;
		cmdParamDesc[cmdCounter].pCount = 1;
		cmdParamDesc[cmdCounter].param[0].value[0] = masterBPM;
		cmdParamDesc[cmdCounter].param[0].bypass = false;
		strcpy(cmdParamDesc[cmdCounter].param[0].type, "int"); 
		strcpy(cmdParamDesc[cmdCounter].param[0].name, "masterBPM"); 

		editor::paramEdit::setParamsAttr();
		editor::paramEdit::setBypass();
	}

	AddToUI("Track"); 
	cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
	cmdLevel++;

	cmdCounter++;

}

#define Track(masterBPM) Track_ref (__FILE__, __LINE__, masterBPM)

void Track_ref ( CALLER_INFO,  int masterBPM)
{
	Track_reflect (srcFileName, srcLine, masterBPM);
	Track_impl (masterBPM);
	cmdLevel--;
}

#else
#undef Track
#define Track(masterBPM) Track_( masterBPM)
#endif
}
