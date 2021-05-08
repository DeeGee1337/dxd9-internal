#include "globals.h"

int main() {
	ManualMapper* mapper = new ManualMapper();

printf("\n");
printf("     //    ) )                   //   ) )\n");
printf("    //    / /  ___      ___     //\n");
printf("   //    / / //___) ) //___) ) //  ____  //___) ) //___) )\n");
printf("  //    / / //       //       //    / / //       //\n");
printf(" //____/ / ((____   ((____   ((____/ / ((____   ((____\n\n");

printf(".......................................................\n\n");

printf("Injector with Manualmapping Beta\n");
printf("CSGO Internal\n");
printf("Credits GH\n");
printf("\n");

	mapper->setup("csgo.exe", "csgo.exe");
	mapper->load_dll("C:\\Users\\Anon\\Documents\\GitHub\\dxd9-internal\\Release\\dxd9 internal.dll");
	mapper->inject_dll();
	system("pause");
}