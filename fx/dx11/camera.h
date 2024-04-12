namespace Camera 
{
	void Set()
	{
		float t = (float)(timer::frameBeginTime * .001);
		XMVECTOR Eye = XMVectorSet(sinf(t)*2, .25f, cosf(t)*2, 0.0f);
		XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		ConstBuf::camera.world[0] = XMMatrixIdentity();//world
		ConstBuf::camera.view[0] = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));
		ConstBuf::camera.proj[0] = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f));

		ConstBuf::UpdateCamera();
		ConstBuf::SetV(3);
		ConstBuf::SetP(3);
	}
}