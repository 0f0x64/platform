#define setCamKey( camTime, camType, position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle, sType, position {slide_x, slide_y, slide_z }, axisType, rotation {fly_x, fly_y, fly_z }, jitter) setCamKey_ref (__FILE__, __LINE__ ,camTime, camType, position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle, sType, position {slide_x, slide_y, slide_z }, axisType, rotation {fly_x, fly_y, fly_z }, jitter)
void setCamKey_impl(  timestamp camTime, keyType camType, position eye, position at, position up, int angle, sliderType sType, position slide, camAxis axisType, rotation fly, int jitter);
void setCamKey_ref ( CALLER_INFO,  timestamp camTime, keyType camType, position eye, position at, position up, int angle, sliderType sType, position slide, camAxis axisType, rotation fly, int jitter)
{
setCamKey_impl (camTime, camType, position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle, sType, position {slide_x, slide_y, slide_z }, axisType, rotation {fly_x, fly_y, fly_z }, jitter);
cmdLevel--;
}

#define CalcCubemap( target) CalcCubemap_ref (__FILE__, __LINE__ ,target)
void CalcCubemap_impl(  texture target);
void CalcCubemap_ref ( CALLER_INFO,  texture target)
{
CalcCubemap_impl (target);
cmdLevel--;
}

#define ShowCubemap( envTexture) ShowCubemap_ref (__FILE__, __LINE__ ,envTexture)
void ShowCubemap_impl( texture envTexture);
void ShowCubemap_ref ( CALLER_INFO, texture envTexture)
{
ShowCubemap_impl (envTexture);
cmdLevel--;
}

#define SetInputAsm( topo) SetInputAsm_ref (__FILE__, __LINE__ ,topo)
void SetInputAsm_impl(  topology topo);
void SetInputAsm_ref ( CALLER_INFO,  topology topo)
{
SetInputAsm_impl (topo);
cmdLevel--;
}

#define SetRT( targetRT, level) SetRT_ref (__FILE__, __LINE__ ,targetRT, level)
void SetRT_impl(  texture targetRT, int level);
void SetRT_ref ( CALLER_INFO,  texture targetRT, int level)
{
SetRT_impl (targetRT, level);
cmdLevel--;
}

#define CreateMips( ) CreateMips_ref (__FILE__, __LINE__ ,)
void CreateMips_impl( );
void CreateMips_ref ( CALLER_INFO)
{
CreateMips_impl ();
cmdLevel--;
}

#define SetDepthMode( mode) SetDepthMode_ref (__FILE__, __LINE__ ,mode)
void SetDepthMode_impl(  depthmode mode);
void SetDepthMode_ref ( CALLER_INFO,  depthmode mode)
{
SetDepthMode_impl (mode);
cmdLevel--;
}

#define Draw( quadcount, instances) Draw_ref (__FILE__, __LINE__ ,quadcount, instances)
void Draw_impl(  int quadcount, int instances = 1);
void Draw_ref ( CALLER_INFO,  int quadcount, int instances = 1)
{
Draw_impl (quadcount, instances);
cmdLevel--;
}

#define SetCamera( position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle) SetCamera_ref (__FILE__, __LINE__ ,position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle)
void SetCamera_impl(  position eye, position at, position up, unsigned int angle);
void SetCamera_ref ( CALLER_INFO,  position eye, position at, position up, unsigned int angle)
{
SetCamera_impl (position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle);
cmdLevel--;
}

#define ClearRT( color4 {color_x, color_y, color_z, color_w }) ClearRT_ref (__FILE__, __LINE__ ,color4 {color_x, color_y, color_z, color_w })
void ClearRT_impl(  color4 color);
void ClearRT_ref ( CALLER_INFO,  color4 color)
{
ClearRT_impl (color4 {color_x, color_y, color_z, color_w });
cmdLevel--;
}

#define ClearRTDepth( ) ClearRTDepth_ref (__FILE__, __LINE__ ,)
void ClearRTDepth_impl( );
void ClearRTDepth_ref ( CALLER_INFO)
{
ClearRTDepth_impl ();
cmdLevel--;
}

#define SetBlendMode( mode, op) SetBlendMode_ref (__FILE__, __LINE__ ,mode, op)
void SetBlendMode_impl(  blendmode mode, blendop op = blendop::add);
void SetBlendMode_ref ( CALLER_INFO,  blendmode mode, blendop op = blendop::add)
{
SetBlendMode_impl (mode, op);
cmdLevel--;
}

#define SetCull( mode) SetCull_ref (__FILE__, __LINE__ ,mode)
void SetCull_impl(  cullmode mode);
void SetCull_ref ( CALLER_INFO,  cullmode mode)
{
SetCull_impl (mode);
cmdLevel--;
}

#define SetScissors( rect {bbox_x, bbox_y, bbox_z, bbox_w }) SetScissors_ref (__FILE__, __LINE__ ,rect {bbox_x, bbox_y, bbox_z, bbox_w })
void SetScissors_impl(  rect bbox);
void SetScissors_ref ( CALLER_INFO,  rect bbox)
{
SetScissors_impl (rect {bbox_x, bbox_y, bbox_z, bbox_w });
cmdLevel--;
}

#define CopyRTColor( dst, src) CopyRTColor_ref (__FILE__, __LINE__ ,dst, src)
void CopyRTColor_impl(  texture dst, texture src);
void CopyRTColor_ref ( CALLER_INFO,  texture dst, texture src)
{
CopyRTColor_impl (dst, src);
cmdLevel--;
}

#define CopyRTDepth( dst, src) CopyRTDepth_ref (__FILE__, __LINE__ ,dst, src)
void CopyRTDepth_impl(  texture dst, texture src);
void CopyRTDepth_ref ( CALLER_INFO,  texture dst, texture src)
{
CopyRTDepth_impl (dst, src);
cmdLevel--;
}

#define cameraMan( ) cameraMan_ref (__FILE__, __LINE__ ,)
void cameraMan_impl( );
void cameraMan_ref ( CALLER_INFO)
{
cameraMan_impl ();
cmdLevel--;
}

#define ShowObject( geometry, normals, quality, position {pos_x, pos_y, pos_z }) ShowObject_ref (__FILE__, __LINE__ ,geometry, normals, quality, position {pos_x, pos_y, pos_z })
void ShowObject_impl( texture geometry, texture normals, unsigned int quality, position pos);
void ShowObject_ref ( CALLER_INFO, texture geometry, texture normals, unsigned int quality, position pos)
{
ShowObject_impl (geometry, normals, quality, position {pos_x, pos_y, pos_z });
cmdLevel--;
}

#define CalcObject( targetGeo, targetNrml) CalcObject_ref (__FILE__, __LINE__ ,targetGeo, targetNrml)
void CalcObject_impl( texture targetGeo, texture targetNrml);
void CalcObject_ref ( CALLER_INFO, texture targetGeo, texture targetNrml)
{
CalcObject_impl (targetGeo, targetNrml);
cmdLevel--;
}

#define oscillator( a, b) oscillator_ref (__FILE__, __LINE__ ,a, b)
void oscillator_impl(  int a, int b);
void oscillator_ref ( CALLER_INFO,  int a, int b)
{
oscillator_impl (a, b);
cmdLevel--;
}

#define eq( a) eq_ref (__FILE__, __LINE__ ,a)
void eq_impl(  int a);
void eq_ref ( CALLER_INFO,  int a)
{
eq_impl (a);
cmdLevel--;
}

#define Pitch( ) Pitch_ref (__FILE__, __LINE__ ,)
void Pitch_impl(  int count, unsigned char ...);
void Pitch_ref ( CALLER_INFO,  int count, unsigned char ...)
{
Pitch_impl ();
cmdLevel--;
}

#define Clip( pos, len, repeat, bpmScale, overDub, swing) Clip_ref (__FILE__, __LINE__ ,pos, len, repeat, bpmScale, overDub, swing)
void Clip_impl(  timestamp pos, int len, int repeat, int bpmScale, overdub overDub, int swing);
void Clip_ref ( CALLER_INFO,  timestamp pos, int len, int repeat, int bpmScale, overdub overDub, int swing)
{
Clip_impl (pos, len, repeat, bpmScale, overDub, swing);
cmdLevel--;
}

#define kick( vol, pan, send, solo, mute) kick_ref (__FILE__, __LINE__ ,vol, pan, send, solo, mute)
void kick_impl(  volume vol, panorama pan, volume send, switcher solo, switcher mute);
void kick_ref ( CALLER_INFO,  volume vol, panorama pan, volume send, switcher solo, switcher mute)
{
kick_impl (vol, pan, send, solo, mute);
cmdLevel--;
}

#define snare( vol, pan, send, solo, mute) snare_ref (__FILE__, __LINE__ ,vol, pan, send, solo, mute)
void snare_impl(  volume vol, panorama pan, volume send, switcher solo, switcher mute);
void snare_ref ( CALLER_INFO,  volume vol, panorama pan, volume send, switcher solo, switcher mute)
{
snare_impl (vol, pan, send, solo, mute);
cmdLevel--;
}

#define Track( masterBPM) Track_ref (__FILE__, __LINE__ ,masterBPM)
void Track_impl(  int masterBPM);
void Track_ref ( CALLER_INFO,  int masterBPM)
{
Track_impl (masterBPM);
cmdLevel--;
}

