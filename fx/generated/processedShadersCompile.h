void CompileAll ()
{
CreateVS (0, shadersData::meshOut);
CreateVS (1, shadersData::quad);
CreatePS (0, shadersData::simpleFx);
CreatePS (1, shadersData::simpleTex);


};