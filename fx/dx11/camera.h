typedef struct {
	float x;
	float y;
	float z;
} vec3;

typedef struct {
	XMVECTOR eye;
	XMVECTOR at;
	XMVECTOR up;
	float angle;
} camData;


namespace Camera
{

	#if EditMode

	struct {

		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;

		bool overRide = true;
		float angle = 60.f;

		void Init()
		{
			XMVECTOR Eye = XMVectorSet(0,111,113.f, 0.0f);
			XMVECTOR At = XMVectorSet( 0,0,0, 0.0f);
			XMVECTOR Up = XMVectorSet( 0,1,0, 0.0f);

			world = XMMatrixIdentity();
			view = XMMatrixLookAtLH(Eye, At, Up);
			proj = XMMatrixPerspectiveFovLH(DegreesToRadians(angle), width / (FLOAT)height, 0.01f, 100.0f);
		}

	} viewCam;

	#endif

	void Set(positionF eye, positionF at, positionF up, float angle)
	{

		XMVECTOR Eye = XMVectorSet(eye.x,eye.y,eye.z, 0.0f);
		XMVECTOR At =  XMVectorSet(at.x, at.y, at.z, 0.0f);
		XMVECTOR Up =  XMVectorSet(up.x, up.y, up.z, 0.0f);

		ConstBuf::camera.world[0] = XMMatrixIdentity();
		ConstBuf::camera.view[0] = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));
		ConstBuf::camera.proj[0] = XMMatrixTranspose(XMMatrixPerspectiveFovLH(DegreesToRadians(angle), dx11::iaspect, 0.01f, 100.0f));

		#if EditMode
		if (viewCam.overRide)
		{
			ConstBuf::camera.world[0] = viewCam.world;
			ConstBuf::camera.view[0] = XMMatrixTranspose(viewCam.view);
			ConstBuf::camera.proj[0] = XMMatrixTranspose(viewCam.proj);
		}
		#endif	

		ConstBuf::UpdateCamera();
		ConstBuf::ConstToVertex(3);
		ConstBuf::ConstToPixel(3);
	}
}