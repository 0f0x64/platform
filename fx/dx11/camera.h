typedef struct {
	float x;
	float y;
	float z;
} vec3;

typedef struct {
	vec3 eye;
	vec3 at;
	vec3 up;
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
		float angle = 120.f;

		void Init()
		{
			XMVECTOR Eye = XMVectorSet(0,0,3.f, 0.0f);
			XMVECTOR At = XMVectorSet( 0,0,0, 0.0f);
			XMVECTOR Up = XMVectorSet( 0,1,0, 0.0f);

			world = XMMatrixIdentity();
			view = XMMatrixLookAtLH(Eye, At, Up);
			proj = XMMatrixPerspectiveFovLH(DegreesToRadians(angle), width / (FLOAT)height, 0.01f, 100.0f);
		}

	} viewCam;

	#endif

	API Camera(camData* cam)
	{
		XMVECTOR Eye = XMVectorSet(cam->eye.x, cam->eye.y, cam->eye.z, 0.0f);
		XMVECTOR At = XMVectorSet(cam->at.x, cam->at.y, cam->at.z, 0.0f);
		XMVECTOR Up = XMVectorSet(cam->up.x, cam->up.y, cam->up.z, 0.0f);

		ConstBuf::camera.world[0] = XMMatrixIdentity();
		ConstBuf::camera.view[0] = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));
		ConstBuf::camera.proj[0] = XMMatrixTranspose(XMMatrixPerspectiveFovLH(cam->angle, width / (FLOAT)height, 0.01f, 100.0f));

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