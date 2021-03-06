#include <string>
struct	string_dref {
	std::string					string_data;
};
struct	int_dref {
	int					data;
};
static string_dref sayarray;
static string_dref acarsoutarray;
static string_dref cduoutarray;
static string_dref acarsinarray;
static int_dref sendAcars;
#define ARRAY_DIM 32
extern int g_my_idarray[ARRAY_DIM];
extern int g_my_afarray[ARRAY_DIM];
extern int g_my_xarray[ARRAY_DIM];
extern int g_my_yarray[ARRAY_DIM];
extern int g_my_zarray[ARRAY_DIM];
extern int g_my_damagearray[ARRAY_DIM];