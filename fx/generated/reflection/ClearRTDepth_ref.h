#if REFLECTION


AddToUI(__FUNCTION__);
cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
cmdLevel++;

#define ClearRTDepth() ClearRTDepth( __FILE__, __LINE__ )

#else

#define ClearRTDepth() ClearRTDepth( )

#endif

cmdCounter++;

