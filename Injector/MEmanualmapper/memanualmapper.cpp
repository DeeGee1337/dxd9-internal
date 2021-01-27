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
printf("[F1] Aimbot (Left Mousebutton)\n");
printf("[F2] Recoil Control System\n");
printf("[F3] Triggerbot (Mousebutton 5)\n");
printf("[F4] 2D Player ESP\n");
printf("[F5] Snapline ESP\n");
printf("[F6] Glow\n");
printf("[F7] Bunnyhop\n");
printf("[F8] Recoil Crosshair\n");
printf("\n");
printf("Credits GH\n");
printf("\n");

	mapper->setup("csgo.exe", "csgo.exe");
	mapper->load_dll("C:\\fickloch\\dxd9 internal.dll");
	mapper->inject_dll();
	system("pause");
}