#if REFLECTION


AddToUI(__FUNCTION__);
cmdParamDesc[cmdCounter].uiDraw = &editor::paramEdit::showStackItem;
cmdLevel++;

#define playTrack() playTrack( __FILE__, __LINE__ )

#else

#define playTrack() playTrack( )

#endif

cmdCounter++;

