#include <switch.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

#define KIP_LIST_LENGTH 32 //Maximum number of kip files the program supports
#define BCT_LIST_LENGTH 2 //Amount of bct values currently editable

string kipName[KIP_LIST_LENGTH];
string bctName[BCT_LIST_LENGTH] = { "debugmode = ", "debugmode_user = " };
int bctValue[BCT_LIST_LENGTH] = { -1, -1 };
int menuSelected = 0;
int kipCount = 0;
bool kipValue[KIP_LIST_LENGTH];
bool run = true;
bool bctSelected = false;
u64 kDown;

void updateInputs() {
	hidScanInput();
	kDown = hidKeysDown(CONTROLLER_P1_AUTO);
	if (kDown & KEY_PLUS) run = false;
}

void printError(int errorCode, string extra) {
	printf(CONSOLE_ESC(2J) CONSOLE_RED "An Error Has Occured! (Error Code: %d) Press + To Exit Back To HB-Menu\n\n", errorCode);
	switch (errorCode) {
	case 0:
		printf("Unable To Make Dirrectory: %s", extra.c_str());
		break;
	case 1:
		printf("No Kips Found!\nDid You Install Any?");
		break;
	case 2:
		printf("More Than %d Kips Found!", KIP_LIST_LENGTH);
		break;
	case 3:
		printf("File Moved Failed!\nOffending Kip: '%s'\n\n\nThis Is Typicly Caused By A Kip Being Present In kips and kips_disabled\nAt The Same Time, Check These Folders First", extra.c_str());
		break;
	case 4:
		printf("Unabled To Open BCT.ini!");
		break;
	case 5:
		printf("Unabled To Find Value In BCT.ini!\nOffending Value: '%s'", extra.c_str());
		break;
	default:
		printf("UKNOWN ERROR\n(You're In DEEP Trouble If This Shows Up Lol :P)");
		break;
	}
	while (run) {
		consoleUpdate(NULL);
		updateInputs();
	}
}

void printWarning(int warningCode, string extra) {
	printf(CONSOLE_ESC(2J) CONSOLE_YELLOW "### WARNING (Warning Code: %d) ###\nPress + To Exit Back To HB-Menu, Press - To Continue\n\n", warningCode);
	switch (warningCode) {
	case 0:
		printf("REBOOTING VIA HOMEBREW WHILE USING EXFAT WILL CAUSE CORRUPTION!\n\n\nContinue at your own risk!");
		break;
	default:
		printf("UKNOWN WARNING\n\n\nProceed With Caution!");
		break;
	}
	while (run) {
		consoleUpdate(NULL);
		updateInputs();
		if (kDown & KEY_MINUS) {
			return;
		}
	}
}

void scanForKips() {
	kipCount = 0;
	DIR* enabledKipDir;
	struct dirent* enabledKipEnt;
	enabledKipDir = opendir("/atmosphere/kips/");
	if (enabledKipDir == NULL) {
		if (mkdir("/atmosphere/kips/", 0700) == -1) {
			printError(0, "/atmosphere/kips/");
		}
	}
	else {
		while ((enabledKipEnt = readdir(enabledKipDir))) {
			kipName[kipCount] = enabledKipEnt->d_name;
			kipValue[kipCount] = true;
			kipCount++;
			if (kipCount == KIP_LIST_LENGTH) {
				kipCount = 0; //Set the kip count back to 0 to prevent a crash screen from showing
				printError(2, "");
				return;
			}
		}
	}
	DIR* disabledKipDir;
	struct dirent* disabledKipEnt;
	disabledKipDir = opendir("/atmosphere/kips_disabled/");
	if (disabledKipDir == NULL) {
		if (mkdir("/atmosphere/kips_disabled/", 0700) == -1) {
			printError(0, "/atmosphere/kips_disabled/");
		}
	}
	else {
		while ((disabledKipEnt = readdir(disabledKipDir))) {
			kipName[kipCount] = disabledKipEnt->d_name;
			kipValue[kipCount] = false;
			kipCount++;
			if (kipCount == KIP_LIST_LENGTH) {
				kipCount = 0; //Set the kip count back to 0 to prevent a crash screen from showing
				printError(2, "");
				return;
			}
		}
	}
	if (kipCount == 0) {
		printError(1, "");
	}
}

void readBCT() {
	ifstream bctIfStream("/atmosphere/BCT.ini");
	string bctString((std::istreambuf_iterator<char>(bctIfStream)), (std::istreambuf_iterator<char>()));
	bctIfStream.close();
	size_t debugModeLocation = bctString.find("debugmode = ", 0) + 12;
	size_t debugModeUserLocation = bctString.find("debugmode_user = ", 0) + 17;
	if (debugModeLocation == string::npos) {
		bctValue[0] = -1;
	}
	else {
		bctValue[0] = bctString[debugModeLocation] - 48;
	}
	if (debugModeUserLocation == string::npos) {
		bctValue[1] = -1;
	}
	else {
		bctValue[1] = bctString[debugModeUserLocation] - 48;
	}
}

void setKip(int kipId, bool enabled) {
	string name = kipName[kipId];
	string start = "/atmosphere/";
	string end = "/atmosphere/";
	if (enabled) {
		start.append("kips_disabled/");
		end.append("kips/");
	}
	else {
		start.append("kips/");
		end.append("kips_disabled/");
	}
	start.append(name);
	end.append(name);
	if (rename(start.c_str(), end.c_str()) != 0) {
		printError(3, name);
	}
}

void setBCT(int bctId, bool enabled) {
	size_t location = string::npos;
	ifstream bctIfStream("/atmosphere/BCT.ini");
	string bctString((std::istreambuf_iterator<char>(bctIfStream)), (std::istreambuf_iterator<char>()));
	bctIfStream.close();
	switch (bctId) {
	case 0:
		location = bctString.find("debugmode = ", 0);
		if (location != string::npos) {
			location = location + 12;
		}
		break;
	case 1:
		location = bctString.find("debugmode_user = ", 0);
		if (location != string::npos) {
			location = location + 17;
		}
		break;
	default:
		break;
	}
	if (location != string::npos) {
		bctString[location] = 48 + enabled;
		ofstream bctOfStream("/atmosphere/BCT.ini");
		if (bctOfStream.is_open()) {
			bctOfStream << bctString;
			bctOfStream.close();
		}
		else {
			printError(4, "");
		}
	}
	else {
		printError(5, bctName[bctId]);
	}
}

void updateScreen() {
	printf(CONSOLE_ESC(2J));
	printf(CONSOLE_MAGENTA CONSOLE_ESC(4m) "Kip Select V%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
	printf(CONSOLE_RESET " (+ To Exit Back To HB-Menu, - To Refresh, Y To Restart)");
	printf(CONSOLE_ESC(45;68H) CONSOLE_MAGENTA "-By Sciguy429");
	printf(CONSOLE_ESC(4;1H) CONSOLE_RESET CONSOLE_ESC(4m) "Installed Kips:");
	printf(CONSOLE_ESC(4;59H) "BCT.ini:\n");
	printf(CONSOLE_RESET);
	for (int i = 0; i < kipCount; i++) {
		if (kipValue[i]) {
			printf(CONSOLE_GREEN);
		}
		else {
			printf(CONSOLE_RED);
		}
		if (menuSelected == i && !bctSelected) {
			printf(CONSOLE_ESC(7m));
		}
		printf(" * %s\n", kipName[i].c_str());
		printf(CONSOLE_RESET);
	}
	printf(CONSOLE_ESC(5;1H));
	for (int i = 0; i < BCT_LIST_LENGTH; i++) {
		printf(CONSOLE_ESC(58C));
		string bctStatus;
		switch (bctValue[i]) {
		case 0:
			bctStatus = "0";
			printf(CONSOLE_RED);
			break;
		case 1:
			bctStatus = "1";
			printf(CONSOLE_GREEN);
			break;
		default:
			bctStatus = "?";
			printf(CONSOLE_YELLOW);
			break;
		}
		if (menuSelected == i && bctSelected) {
			printf(CONSOLE_ESC(7m));
		}
		printf(" * %s%s\n", bctName[i].c_str(), bctStatus.c_str());
		printf(CONSOLE_RESET);
	}
}

int main(int argc, char **argv)
{
	consoleInit(NULL);
	scanForKips();
	readBCT();
	updateScreen();
	while (appletMainLoop() && run)
	{
		updateInputs();
		if (kDown & KEY_MINUS) {
			menuSelected = 0;
			bctSelected = false;
			scanForKips();
			readBCT();
			updateScreen();
		}
		else if (kDown & KEY_LEFT) {
			if (bctSelected == true) {
				bctSelected = false;
				if (menuSelected > kipCount) {
					menuSelected = kipCount;
				}
				updateScreen();
			}
		}
		else if (kDown & KEY_RIGHT) {
			if (bctSelected == false) {
				bctSelected = true;
				if (menuSelected > BCT_LIST_LENGTH - 1) {
					menuSelected = BCT_LIST_LENGTH - 1;
				}
				updateScreen();
			}
		}
		else if (kDown & KEY_DOWN) {
			menuSelected++;
			if (menuSelected >= (kipCount * !bctSelected) + (BCT_LIST_LENGTH * bctSelected)) {
				menuSelected = (kipCount * !bctSelected) + (BCT_LIST_LENGTH * bctSelected) - 1;
			}
			else {
				updateScreen();
			}
		}
		else if (kDown & KEY_UP) {
			menuSelected--;
			if (menuSelected < 0) {
				menuSelected = 0;
			}
			else {
				updateScreen();
			}
		}
		else if (kDown & KEY_A) {
			if (bctSelected) {
				if (bctValue[menuSelected] == 0) {
					bctValue[menuSelected] = 1;
					setBCT(menuSelected, true);
					updateScreen();
				}
				else if (bctValue[menuSelected] == 1) {
					bctValue[menuSelected] = 0;
					setBCT(menuSelected, false);
					updateScreen();
				}
			}
			else {
				kipValue[menuSelected] = !kipValue[menuSelected];
				setKip(menuSelected, kipValue[menuSelected]);
				updateScreen();
			}
		}
		else if (kDown & KEY_Y) {
			printWarning(0, "");
			if (run) {
				bpcInitialize();
				bpcRebootSystem();
			}
		}
		consoleUpdate(NULL);
	}
	consoleExit(NULL);
	return 0;
}
