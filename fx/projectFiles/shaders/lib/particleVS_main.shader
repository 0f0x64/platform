//[
VS_OUTPUT_PARTICLE VS(uint vID : SV_VertexID,uint iID : SV_InstanceID)
{
    float4 grid = getGridInst(vID,iID,gX,gY); 
    pos_color p = CalcParticles(iID,grid);
    VS_OUTPUT_PARTICLE output = { p.pos,grid.zw, p.color, p.sz};
    return output;
}
//]


