void Init ()
{

CreateVS (0, shadersData::galaxy);
CreateVS (1, shadersData::neitron_star);
CreateVS (2, shadersData::objViewer);
CreateVS (3, shadersData::pillars);
CreateVS (4, shadersData::quad);
CreateVS (5, shadersData::simpleCube);
CreateVS (6, shadersData::space);
CreateVS (7, shadersData::box);
CreateVS (8, shadersData::letter);
CreateVS (9, shadersData::lineDrawer);
CreateVS (10, shadersData::lineDrawer3d);
CreatePS (0, shadersData::basic);
CreatePS (1, shadersData::basicLow);
CreatePS (2, shadersData::cat);
CreatePS (3, shadersData::cubemapCreator);
CreatePS (4, shadersData::cubeMapViewer);
CreatePS (5, shadersData::genNormals);
CreatePS (6, shadersData::obj1);
CreatePS (7, shadersData::output);
CreatePS (8, shadersData::box_ps);
CreatePS (9, shadersData::letter_ps);
CreatePS (10, shadersData::lineDrawerUV_ps);
CreatePS (11, shadersData::lineDrawer_ps);

};