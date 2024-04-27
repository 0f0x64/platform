void Init ()
{

CreateVS (0, shadersData::box);
CreateVS (1, shadersData::letter);
CreateVS (2, shadersData::lineDrawer);
CreateVS (3, shadersData::lineDrawer3d);
CreateVS (4, shadersData::objViewer);
CreateVS (5, shadersData::quad);
CreateVS (6, shadersData::simpleCube);
CreatePS (0, shadersData::basic);
CreatePS (1, shadersData::box_ps);
CreatePS (2, shadersData::cubemapCreator);
CreatePS (3, shadersData::cubeMapViewer);
CreatePS (4, shadersData::genNormals);
CreatePS (5, shadersData::letter_ps);
CreatePS (6, shadersData::lineDrawer_ps);
CreatePS (7, shadersData::obj1);
CreatePS (8, shadersData::simpleFx);

};