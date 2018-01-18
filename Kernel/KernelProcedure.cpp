#include "KernelProcedure.h"
#include "ConsoleManager.h"
#include "SkyConsole.h"
#include "string.h"
#include "Hal.h"
#include "PIT.h"
#include "Process.h"
#include "ProcessManager.h"
#include "Scheduler.h"
#include "SkyAPI.h"

bool systemOn = false;
void NativeConsole();

void NativeConsole()
{
	systemOn = true;
	ConsoleManager manager;

	char	commandBuffer[MAXPATH];

	while (1)
	{
		SkyConsole::Print("Command> ");
		memset(commandBuffer, 0, MAXPATH);
		//SkyConsole::Print("commandBuffer Address : 0x%x\n", &commandBuffer);	

		SkyConsole::GetCommand(commandBuffer, MAXPATH - 2);
		SkyConsole::Print("\n");

		if (manager.RunCommand(commandBuffer) == true)
			break;

		/*int first = GetTickCount();
		int count = 4;
		while (count != 0)
		{

		int second = GetTickCount();
		if (second - first > 100)
		{
		SkyConsole::Print("%d\n", second);

		first = GetTickCount();
		}
		}*/
	}
}

DWORD WINAPI SystemConsoleProc(LPVOID parameter)
{	

	while (1) {
		NativeConsole();
	}

	for (;;);

	return 0;
}

DWORD WINAPI SampleLoop(LPVOID parameter)
{
	char* str = "Sample Loop!!\n";
	SkyConsole::Print("%s", str);
	int first = GetTickCount();
	bool bExit = false;
	SkyConsole::Print("%s", str);	

	Process* pProcess = (Process*)parameter;
	SkyConsole::Print("Thread Routine Process Address %x\n", pProcess);

	while (bExit == false)
	{
		static int count = 0;
		int second = GetTickCount();
		if (second - first > 100)
		{
			SkyConsole::Print("%s", str);

			first = GetTickCount();
			count++;
		}
	}

	return 0;
}

#define TS_WATCHDOG_CLOCK_POS		(0xb8000+(80-1)*2)
#define TIMEOUT_PER_SECOND		50				/* system timer (irq0) */
static bool m_bShowTSWatchdogClock = true;

DWORD WINAPI WatchDogProc(LPVOID parameter)
{		
	Process* pProcess = (Process*)parameter;
	int pos = 0;
	char *addr = (char *)TS_WATCHDOG_CLOCK_POS, status[] = { '-', '\\', '|', '/', '-', '\\', '|', '/' };
	int first = GetTickCount();

	while (1) 
	{
	
		int second = GetTickCount();
		if (second - first >= TIMEOUT_PER_SECOND)
		{
			if (++pos > 7) 
				pos = 0;
		
			if (m_bShowTSWatchdogClock)
				*addr = status[pos];

			first = GetTickCount();
		}
	}

	return 0;
}

DWORD WINAPI ProcessRemoverProc(LPVOID parameter)
{
	Process* pProcess = (Process*)parameter;
	
	while (1)
	{
		__asm cli

		ProcessManager::GetInstance()->RemoveTerminatedProcess();
		Scheduler::GetInstance()->Yield(kGetCurrentThreadId());

		__asm sti		
	}

	return 0;
}

/* render rectangle in 32 bpp modes. */
extern void rect32(int x, int y, int w, int h, int col);


DWORD WINAPI RectGenerate(LPVOID parameter)
{
	int col = 0;
	bool dir = true;
	SkyConsole::Print("RectGenerate\n");
	while (1) {
		rect32(200, 250, 100, 100, col << 16);
		if (dir) {
			if (col++ == 0xfe)
				dir = false;
		}
		else
			if (col-- == 1)
				dir = true;
	}

	return 0;
}