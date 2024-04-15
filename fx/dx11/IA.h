namespace InputAssembler
{

	void Set(int topology)
	{
		context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)topology);
		context->IASetInputLayout(NULL);
		context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	}

}