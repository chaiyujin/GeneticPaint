#include "const.h"
using namespace Yuki;

int Tools::random_int(int l, int r) {
	static Random rand;
	int ret = l + rand.random<int>(r - l);
	return ret;
}

bool Tools::will_mutate(int rate) {
	if (rate < 1) rate = 1;
	if (random_int(0, rate) == 0)
		return true;
	else
		return false;
}

void Tools::config_from(const char *file_path) {
	Config cfg(file_path);
#define CFG(x) cfg.get(#x, x)

	CFG(Max_Width);
	CFG(Max_Height);

	CFG(Vertex_Max_Move_Rate);
	CFG(Vertex_Mid_Move_Rate);
	CFG(Vertex_Min_Move_Rate);
	CFG(Vertex_Mid_Move_Range);
	CFG(Vertex_Min_Move_Range);
}

// image
int Tools::Max_Width = 256;
int Tools::Max_Height = 256;
// vertex
int Tools::Vertex_Max_Move_Rate = 1500;
int Tools::Vertex_Mid_Move_Rate = 100;
int Tools::Vertex_Min_Move_Rate = 20;
int Tools::Vertex_Mid_Move_Range = 10;
int Tools::Vertex_Min_Move_Range = 1;
// color
int Tools::Color_Mutate_Rate = 1500;
int Tools::Color_Min = 0;
int Tools::Color_Max = 256;
// triangle
int Tools::Add_Triangle_Rate = 500;
int Tools::Add_New_Triangle_Rate = 2;
int Tools::Remove_Triangle_Rate = 1200;
int Tools::Move_Triangle_Rate = 500;
// Polygon
int Tools::Max_Vertices = 50;
int Tools::Add_Vertex_Rate = 200;
int Tools::Remove_Vertex_Rate = 800;
int Tools::Move_Vertex_Rate = 800;
// Polygons
int Tools::Add_Polygon_Rate = 200;
int Tools::Add_New_Polygon_Rate = 2;
int Tools::Remove_Polygon_Rate = 800;
int Tools::Move_Polygon_Rate = 500;