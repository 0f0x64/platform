#if REFLECTION


AddToUI(__FUNCTION__);
cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
cmdLevel++;

#define CreateMips() CreateMips( __FILE__, __LINE__ )

#else

#define CreateMips() CreateMips( )

#endif

cmdCounter++;

