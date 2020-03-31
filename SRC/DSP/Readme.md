# GAMECUBE DSP

Low-level DSP emulation module.

## DspCore

How does the DSP core work.

The Run method executes the Update method until it is stopped by the Suspend method or it encounters a breakpoint.

Suspend method stops DSP thread execution indifinitely (same as HALT instruction).

The Step debugging method is used to unconditionally execute next DSP instruction (by interpreter).

The Update method checks the value of Gekko TBR. If its value has exceeded the limit for the execution of one DSP instruction (or segment in case of Jitc),
interpreter/Jitc Execute method is called.

DspCore uses the interpreter and recompiler at the same time, of their own free will, depending on the situation.

## DSP analyzer

This component analyzes the DSP instructions and is used by all interested systems (disassembler, interpreter and recompiler). 
That is, in fact, this is a universal decoder.

I decided to divide all the DSP instructions into groups (by higher 4 bits). Decoder implemented as simple if-else.

Hybrid (packaged) instructions are stored into two different groups in **AnalyseInfo** struct.

The simplest example of **AnalyzeInfo** consumption can be found in the disassembler.

The DSP instruction format is so tightly packed and has a lot of entropy, so I could make a mistake in decoding somewhere. All this then has to appear.

## GameCube DSP interpreter

The development idea is as follows - to do at least something (critical mass of code), then do some reverse engineering
of the microcodes and IROM and bring the emulation to an adequate state.

### Interpreter architecture

The interpreter is not involved in instruction decoding. It receives ready-made information from the analyzer (**AnalyzeInfo** struct).

This is a new concept of emulation of processor systems, which I decided to try on the GameCube DSP.

### Various Notes from known Macronix datasheets

- Branch and call instructions are not allowed within program loop
- TRAP-- Always call to hex 000C address

## DSP JDI

The debugging interface specification provided by this component can be found in Data\DspJdi.json.