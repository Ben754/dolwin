// emulator is profiled every real-time second.
// status bar should present and contain at least two sections :
// first (largest) is used for frame timing percentage
// second (small) is used for FPS
#include "pch.h"

//
// local data
//

static  BOOL    Profiler;   // uservar

static  int64_t     startTime, stopTime;
static  int64_t     gfxStartTime, gfxStopTime;
static  int64_t     sfxStartTime, sfxStopTime;
static  int64_t     dvdStartTime, dvdStopTime;
static  int64_t     cpuTime, gfxTime, sfxTime, dvdTime, idleTime;
static  int64_t     ONE_SECOND;
static  int64_t     fpsTime, mipsTime;
static  DWORD       checkTime;

// ---------------------------------------------------------------------------

// precede timer utility

static void __fastcall MyReadTimeStampCounter(int64_t *ptr)
{
    *ptr = __rdtsc();
}

// get average CPU speed (in MHz)
static float GetClockSpeed()
{
    int i = 0;
    LARGE_INTEGER   t0, t1, perfFreq;
    int64_t stamp0 = 0, stamp1 = 0;
    int64_t diffStamp, diffTicks;

    if(QueryPerformanceFrequency(&perfFreq))
    {
        while(i < 3)
        {
            i++;

            QueryPerformanceCounter(&t0);
            t1.LowPart = t0.LowPart;
            t1.HighPart = t0.HighPart;

            while((t1.LowPart - t0.LowPart) < 50)
            {
                QueryPerformanceCounter(&t1);
                MyReadTimeStampCounter(&stamp0);
            }

            t0.LowPart = t1.LowPart;
            t0.HighPart = t1.HighPart;

            while((t1.LowPart - t0.LowPart) < 1000)
            {
                QueryPerformanceCounter(&t1);
                MyReadTimeStampCounter(&stamp1);
            }
        }

        diffStamp = stamp1 - stamp0;
        diffTicks = t1.LowPart - t0.LowPart;
        diffTicks *= 100000;
        diffTicks = diffTicks / (perfFreq.LowPart / 10);

        return (float)((float)diffStamp / (float)diffTicks);
    }
    else return 0.0f;
}

// ---------------------------------------------------------------------------

void OpenProfiler(bool enable)
{
    // load user variable
    Profiler = enable;

    // status window should be valid
    if(!IsWindow(wnd.hStatusWindow)) Profiler = FALSE;

    // high-precision timer should exist
    LARGE_INTEGER freq;
    if(!QueryPerformanceFrequency(&freq)) Profiler = FALSE;

    // reset counters
    if(Profiler)
    {
        cpuTime = gfxTime = sfxTime = dvdTime = idleTime = 0;
        ONE_SECOND = (int64_t)((double)GetClockSpeed() * (double)1000000.0);
        MyReadTimeStampCounter(&startTime);
        MyReadTimeStampCounter(&fpsTime);
        MyReadTimeStampCounter(&mipsTime);
    }

    checkTime = GetTickCount();
}

// update after every fifo call
void UpdateProfiler()
{
    if(Profiler)
    {
        TCHAR buf[128], mips[128];

        if((GetTickCount() - checkTime) < 1000) return;
        checkTime = GetTickCount();
    
        // measure time
        MyReadTimeStampCounter(&stopTime);
        int64_t total = stopTime - startTime;

        cpuTime = total - gfxTime - sfxTime - dvdTime;
        int64_t cur; MyReadTimeStampCounter(&cur);
        int64_t diff = cur - startTime;

        // calculate how long we can be in idle state (VSYNC, as in real)
        // current emulation speed is not allowing to waste any time
        idleTime = 0;
/*/
        if(diff < ONE_SECOND / 60)
        {
            idleTime = (ONE_SECOND / 60) - diff;
            total += idleTime;
        }
/*/

        // frames per second
        MyReadTimeStampCounter(&cur);
        diff = cur - fpsTime;
        if(diff >= ONE_SECOND)
        {
            _stprintf_s (buf, _countof(buf) - 1, _T("FPS:%u"), vi.frames);
            vi.frames = 0;
            SetStatusText(STATUS_ENUM::Fps, buf);
            MyReadTimeStampCounter(&fpsTime);
        }

        // calculate MIPS
        MyReadTimeStampCounter(&cur);
        diff = cur - mipsTime;
        if(cur >= ONE_SECOND)
        {
            _stprintf_s (mips, _countof(mips) - 1, _T("%.1f"), (float)Gekko::Gekko->GetOpcodeCount() / 1000000.0f);
            Gekko::Gekko->ResetOpcodeCount();
            MyReadTimeStampCounter(&mipsTime);
        }
        else
        {
            TCHAR * st = GetStatusText(STATUS_ENUM::Progress);
            _stscanf (st, L"mips:%s ", mips);
        }

        // update status bar
        {
            _stprintf_s (buf, _countof(buf) - 1, _T("mips:%s  core:%-2.1f  video:%-2.1f  sound:%-2.1f  dvd:%-2.1f  idle:%-2.1f"),
                mips,
                (double)cpuTime * 100 / (double)total,
                (double)gfxTime * 100 / (double)total,
                (double)sfxTime * 100 / (double)total,
                (double)dvdTime * 100 / (double)total,
                (double)idleTime* 100 / (double)total
            );
            SetStatusText(STATUS_ENUM::Progress, buf);
        }

        // reset counters
        cpuTime = gfxTime = sfxTime = dvdTime = idleTime = 0;
        MyReadTimeStampCounter(&startTime);
    }
}

// ---------------------------------------------------------------------------

// profilers set

void BeginProfileGfx() { if(Profiler) MyReadTimeStampCounter(&gfxStartTime); }
void EndProfileGfx()   { if(Profiler) { MyReadTimeStampCounter(&gfxStopTime);
                         gfxTime += gfxStopTime - gfxStartTime; } }

void BeginProfileSfx() { if(Profiler) MyReadTimeStampCounter(&sfxStartTime); }
void EndProfileSfx()   { if(Profiler) { MyReadTimeStampCounter(&sfxStopTime);
                         sfxTime += sfxStopTime - sfxStartTime; } }

void BeginProfileDVD() { if(Profiler) MyReadTimeStampCounter(&dvdStartTime); }
void EndProfileDVD()   { if(Profiler) { MyReadTimeStampCounter(&dvdStopTime);
                         dvdTime += dvdStopTime - dvdStartTime; } }
