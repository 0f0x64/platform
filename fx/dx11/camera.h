namespace Camera 
{
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

	void Set(camData cam)
	{
		XMVECTOR Eye = XMVectorSet(cam.eye.x, cam.eye.y, cam.eye.z, 0.0f);
		XMVECTOR At = XMVectorSet(cam.at.x, cam.at.y, cam.at.z, 0.0f);
		XMVECTOR Up = XMVectorSet(cam.up.x, cam.up.y, cam.up.z, 0.0f);

		ConstBuf::camera.world[0] = XMMatrixIdentity();
		ConstBuf::camera.view[0] = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));
		ConstBuf::camera.proj[0] = XMMatrixTranspose(XMMatrixPerspectiveFovLH(cam.angle, width / (FLOAT)height, 0.01f, 100.0f));

		ConstBuf::UpdateCamera();
		ConstBuf::SetToVertex(3);
		ConstBuf::SetToPixel(3);
	}
}