#if REFLECTION


AddToUI(__FUNCTION__);
cmdLevel++;

#define CreateMips() CreateMips( __FILE__, __LINE__ )

#else

#define CreateMips() CreateMips( )

#endif

cmdCounter++;

