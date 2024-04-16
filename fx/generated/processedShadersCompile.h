void Init ()
{

CreateVS (0, shadersData::box);
CreateVS (1, shadersData::lineDrawer);
CreateVS (2, shadersData::objViewer);
CreateVS (3, shadersData::quad);
CreateVS (4, shadersData::simpleCube);
CreatePS (0, shadersData::basic);
CreatePS (1, shadersData::box_ps);
CreatePS (2, shadersData::colorFill);
CreatePS (3, shadersData::cubemapCreator);
CreatePS (4, shadersData::cubeMapViewer);
CreatePS (5, shadersData::genNormals);
CreatePS (6, shadersData::obj1);
CreatePS (7, shadersData::simpleFx);

};