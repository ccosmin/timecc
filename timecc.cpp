// timecc.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

class TimeIt
{
public:
   TimeIt(_TCHAR* progName)
      : m_progName(progName)
   {
      LARGE_INTEGER li;
      if ( ! ::QueryPerformanceFrequency(&li) )
      {
         fprintf(stderr, "QueryPerformanceFrequency has failed, bailing out\n");
         exit(1);
      }
      m_pcFreq = li.QuadPart / 1000.;
      if ( ! ::QueryPerformanceCounter(&li) )
      {
         fprintf(stderr, "QueryPerformanceCounter has failed, bailing out\n");
         exit(2);
      }
      m_counterStart = li.QuadPart;
   }

   ~TimeIt()
   {
      LARGE_INTEGER li;
      if ( ! ::QueryPerformanceCounter(&li) )
      {
         fprintf(stderr, "QueryPerformanceCounter has failed, bailing out\n");
         exit(2);
      }
      // In ms.
      double count = (li.QuadPart - m_counterStart) / m_pcFreq;
      _ftprintf(stderr, TEXT("%s executed in: %.2fms\n"), m_progName, count);
   }
private:
   _TCHAR* m_progName;
   double m_pcFreq;
   __int64 m_counterStart;
};

int _tmain(int argc, _TCHAR* argv[])
{
   if ( argc < 2 )
   {
      fprintf(stderr, "timecc [program.exe arg1 arg2]\n");
      return 0;
   }
   TimeIt doTimeIt(argv[1]);

   STARTUPINFO si = {0};
   si.cb = sizeof(STARTUPINFO);
   PROCESS_INFORMATION pi = {0};
   
   _TCHAR* startExe = wcsstr(::GetCommandLine(), argv[1]);

   // Start the child process. 
   if ( ! CreateProcess(NULL,    // No module name (use command line)
                        startExe, // Command line
                        NULL,    // Process handle not inheritable
                        NULL,    // Thread handle not inheritable
                        FALSE,   // Set handle inheritance to FALSE
                        0,       // No creation flags
                        NULL,    // Use parent's environment block
                        NULL,    // Use parent's starting directory 
                        &si,     // Pointer to STARTUPINFO structure
                        &pi ))   // Pointer to PROCESS_INFORMATION structure
   {
      fprintf(stderr, "CreateProcess failed (%d)\n", GetLastError());
      return 1;
   }

   if ( ::WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED )
   {
      fprintf(stderr, "Waiting for process to end has failed (%d)\n", GetLastError());
      return 1;
   }

   return 0;
}
