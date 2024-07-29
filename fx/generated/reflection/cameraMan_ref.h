#if REFLECTION


AddToUI(__FUNCTION__);
cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
cmdLevel++;

#define cameraMan() cameraMan( __FILE__, __LINE__ )

#else

#define cameraMan() cameraMan( )

#endif

cmdCounter++;

