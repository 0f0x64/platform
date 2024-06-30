#define CalcCubemap(target) CalcCubemap( __FILE__, __LINE__ , target)

#define ShowCubemap(envTexture) ShowCubemap( __FILE__, __LINE__ , envTexture)

#define ShowObject(geometry, normals, quality, pos_x, pos_y, pos_z) ShowObject( __FILE__, __LINE__ , geometry, normals, quality, position {pos_x, pos_y, pos_z })

#define CalcObject(targetGeo, targetNrml) CalcObject( __FILE__, __LINE__ , targetGeo, targetNrml)

#define Blending(mode, operation) Blending( __FILE__, __LINE__ , mode, operation)

#define Camera(eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z, angle) Camera( __FILE__, __LINE__ , position {eye_x, eye_y, eye_z }, position {at_x, at_y, at_z }, position {up_x, up_y, up_z }, angle)

#define Depth(mode) Depth( __FILE__, __LINE__ , mode)

#define Clear(color_x, color_y, color_z, color_w) Clear( __FILE__, __LINE__ , color4 {color_x, color_y, color_z, color_w })

#define ClearDepth() ClearDepth( __FILE__, __LINE__ )

#define NullDrawer(quadCount, instances) NullDrawer( __FILE__, __LINE__ , quadCount, instances)

#define IA(topoType) IA( __FILE__, __LINE__ , topoType)

#define Cull(mode) Cull( __FILE__, __LINE__ , mode)

#define Scissors(r_x, r_y, r_z, r_w) Scissors( __FILE__, __LINE__ , rect {r_x, r_y, r_z, r_w })

#define Sampler(shader, slot, filterType, addressU, addressV) Sampler( __FILE__, __LINE__ , shader, slot, filterType, addressU, addressV)

#define SamplerComp(slot) SamplerComp( __FILE__, __LINE__ , slot)

#define vShader(n) vShader( __FILE__, __LINE__ , n)

#define pShader(n) pShader( __FILE__, __LINE__ , n)

#define CopyColor(dst, src) CopyColor( __FILE__, __LINE__ , dst, src)

#define CopyDepth(dst, src) CopyDepth( __FILE__, __LINE__ , dst, src)

#define TextureToShader(tex, slot, tA) TextureToShader( __FILE__, __LINE__ , tex, slot, tA)

#define CreateMipMap() CreateMipMap( __FILE__, __LINE__ )

#define RenderTarget(target, level) RenderTarget( __FILE__, __LINE__ , target, level)

#define oscillator(a, b) oscillator( __FILE__, __LINE__ , a, b)

#define eq(a) eq( __FILE__, __LINE__ , a)

#define channel_01_bass(volume, pan, ms) channel_01_bass( __FILE__, __LINE__ , volume, pan, ms)

#define channel_02_solo(volume, pan, ms) channel_02_solo( __FILE__, __LINE__ , volume, pan, ms)

#define mix(level) mix( __FILE__, __LINE__ , level)

#define playTrack() playTrack( __FILE__, __LINE__ )

