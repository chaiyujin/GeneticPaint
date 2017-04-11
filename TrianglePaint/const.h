#pragma once
#include "utils/yuki.h"

class Tools {
public:
	template <class T, class U>
	static T clamp(T x, U l, U r) {
		if (x < l) return (T)l;
		if (x > r) return (T)r;
		return x;
	}
	static int random_int(int l, int r);
	static bool will_mutate(int rate);
	static void config_from(const char *file_path);
	// image
	static int Max_Width;
	static int Max_Height;
	// vertex
	static int Vertex_Max_Move_Rate;
	static int Vertex_Mid_Move_Rate;
	static int Vertex_Min_Move_Rate;
	static int Vertex_Mid_Move_Range;
	static int Vertex_Min_Move_Range;
	// color
	static int Color_Mutate_Rate;
	static int Color_Min, Color_Max;
	// triangle
	static int Add_Triangle_Rate;
	static int Add_New_Triangle_Rate;
	static int Remove_Triangle_Rate;
	static int Move_Triangle_Rate;
};