#if REFLECTION


AddToUI(__FUNCTION__);
cmdLevel++;

#define cameraMan() cameraMan( __FILE__, __LINE__ )

#else

#define cameraMan() cameraMan( )

#endif

cmdCounter++;

