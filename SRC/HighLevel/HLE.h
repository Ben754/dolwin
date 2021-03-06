void    os_ignore();
void    os_ret0();
void    os_ret1();
void    os_trap();

// HLE state variables
typedef struct HLEControl
{
    // current loaded map file
    TCHAR       mapfile[0x1000];
} HLEControl;

extern  HLEControl hle;

void    HLESetCall(const char *name, void (*call)());
void    HLEInit();
void    HLEShutdown();
void    HLEOpen();
void    HLEClose();
void    HLEExecuteCallback(uint32_t entryPoint);
