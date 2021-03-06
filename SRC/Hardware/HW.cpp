// hardware init/update code
// IMPORTANT: whole HW should use physical CPU addressing, not effective!
#include "pch.h"

namespace Flipper
{
    Flipper* HW;

    // This thread acts as the HWUpdate of Dolwin 0.10.
    // Previously, an HWUpdate call occurred after each Gekko instruction (or so).
    // This was tied only to update VI, SI and AI.
    // After switching to multitasking, leave the old HWUpdate update mechanism, will be gradually replaced by other threads of different Flipper components.
    void Flipper::HwUpdateThread(void* Parameter)
    {
        Flipper* flipper = (Flipper*)Parameter;

        while (true)
        {
            int64_t ticks = Gekko::Gekko->GetTicks();
            if (ticks < flipper->hwUpdateTbrValue)
            {
                Gekko::Gekko->WakeMeUp(Gekko::GekkoWaiter::HwUpdate, Flipper::ticksToHwUpdate, flipper->hwUpdateThread);
                continue;
            }
            flipper->hwUpdateTbrValue = ticks + Flipper::ticksToHwUpdate;

            flipper->Update();
        }
    }

    Flipper::Flipper(HWConfig* config)
    {
        DBReport2(DbgChannel::Info,
            "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
            "Hardware Initialization.\n"
            "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n"
        );

        Mixer = new AudioMixer(config);
        assert(Mixer);

        MIOpen(config); // memory protection and 1T-SRAM interface
        VIOpen(config); // video (TV)
        CPOpen(config); // fifo
        AIOpen(config); // audio (AID and AIS)
        AROpen();       // aux. memory (ARAM)
        EIOpen(config); // expansion interface (EXI)
        DIOpen();       // disk
        SIOpen();       // GC controllers
        PIOpen(config); // interrupts, console regs

        DSP = new DSP::DspCore(config);
        assert(DSP);

        DBReport("\n");

        GXOpen(mi.ram, wnd.hMainWindow);
        PADOpen();

        Debug::Hub.AddNode(HW_JDI_JSON, hw_init_handlers);

        hwUpdateThread = new Thread(HwUpdateThread, false, this, "HW");
        assert(hwUpdateThread);
        Gekko::Gekko->WakeMeUp(Gekko::GekkoWaiter::HwUpdate, Flipper::ticksToHwUpdate, hwUpdateThread);
    }

    Flipper::~Flipper()
    {
        delete hwUpdateThread;

        Debug::Hub.RemoveNode(HW_JDI_JSON);

        delete DSP;

        CPClose();
        AIClose();
        ARClose();      // release ARAM
        EIClose();      // take care about closing of memcards and BBA
        VIClose();      // close GDI (if opened)
        DIClose();      // release streaming buffer
        MIClose();

        PADClose();
        delete Mixer;
        GXClose();
    }

    void Flipper::Update()
    {
        // update joypads and video
        VIUpdate();
        SIPoll();
    }

}
