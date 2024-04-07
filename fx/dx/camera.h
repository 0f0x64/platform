namespace Camera
{

	void Set()
	{
		float t = (float)(timer::frameBeginTime * .001);
	//	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
		XMVECTOR Eye = XMVectorSet(sinf(t), 0.2f, cosf(t), 0.0f);
		XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		
		ConstBuf::camera[0][0] = XMMatrixIdentity();//world
		ConstBuf::camera[0][1] = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));
		ConstBuf::camera[0][2] = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f));

		//ConstBuf::camera[0][2] = XMMatrixIdentity();//world


		ConstBuf::Api.Update(2, ConstBuf::camera);
		ConstBuf::Api.Set(2);
	}
}