#include <emscripten.h>

typedef unsigned short ushort;
typedef unsigned char uchar;

ushort pc;

uchar* stack;
uchar stack_i;

ushort* callstack;
uchar callstack_i;

uchar* rom;
uchar* ram;

bool _doBreakBeforeDumpkey;
uchar keyState;

uchar ioState;

int main(){
    pc = 0;

    stack = new uchar[256];
    stack_i = 0;

    callstack = new ushort[256];
    callstack_i = 0;

    rom = new uchar[0xa000];
    ram = new uchar[0x6000];

    _doBreakBeforeDumpkey = false;
    keyState = 0;

    ioState = 255;

    return 0;
}
extern "C" void EMSCRIPTEN_KEEPALIVE DoBreakBeforeDumpkey(bool value){
    _doBreakBeforeDumpkey = value;
}
extern "C" void EMSCRIPTEN_KEEPALIVE SetKeyState(uchar state){
    keyState = state;
}
extern "C" bool EMSCRIPTEN_KEEPALIVE IOState(){
    return ioState;
}
extern "C" uchar* EMSCRIPTEN_KEEPALIVE GetIOPtr(){
    return rom + 0x5000;
}
extern "C" uchar* EMSCRIPTEN_KEEPALIVE Malloc(int length){
    return new uchar[length];
}
extern "C" void EMSCRIPTEN_KEEPALIVE Free(uchar* addr){
    delete[] addr;
}
extern "C" void EMSCRIPTEN_KEEPALIVE InitROM(uchar* addr, int length){
    for(int i = 0;i < length;i++)
        rom[i] = addr[i];
}
extern "C" uchar EMSCRIPTEN_KEEPALIVE Load(ushort addr){
    if(addr >= 0xa000){
        return ram[addr - 0xa000];
    }else{
        return rom[addr];
    }
}
extern "C" void EMSCRIPTEN_KEEPALIVE Store(ushort addr, uchar value){
    if(addr >= 0xa000){
        ram[addr - 0xa000] = value;
    }
}
void Push(uchar value){
    if(stack_i == 255) throw "stack overflow";
    stack[stack_i++] = value;
}
uchar Pop(){
    if(stack_i == 0) throw "stack underflow";
    return stack[--stack_i];
}
ushort PopAddr(){
    return Pop() | (Pop() << 8);
}
extern "C" void EMSCRIPTEN_KEEPALIVE Emulate(){
    if(ioState != 255) ioState = 255;
    int emucount = 0;
    uchar tmp = 0;
    ushort addr = 0;
    while(emucount++ < 100000){
        switch(Load(pc)){
            case 0x00:
                break;
            case 0x01:
                Push(Load(++pc));
                break;
            case 0x02:
                Pop();
                break;
            case 0x03:
                stack_i = 0;
                break;
            case 0x04:
                Push(Pop() + Pop());
                break;
            case 0x05:
                tmp = Pop();
                Push(Pop() - tmp);
                break;
            case 0x06:
                Push(Pop() * Pop());
                break;
            case 0x07:
                tmp = Pop();
                Push(Pop() / tmp);
                break;
            case 0x08:
                tmp = Pop();
                Push(Pop() % tmp);
                break;
            case 0x09:
                Push(~(Pop() & Pop()));
                break;
            case 0x0a:
                Push(Pop() == Pop());
                break;
            case 0x0b:
                Push(Pop() < Pop()); //スタックの都合で逆
                break;
            case 0x0c:
                addr = PopAddr();
                if(Pop()){
                    pc = addr;
                    continue;
                }
                break;
            case 0x0d:
                pc = PopAddr();
                continue;
            case 0x0e:
                if(callstack_i == 255) throw "callstack overflow";
                callstack[callstack_i++] = pc;
                pc = PopAddr();
                continue;
            case 0x0f:
                if(callstack_i == 0){
                    throw "callstack underflow";
                }else{
                    pc = callstack[--callstack_i];
                }
                break;
            case 0x10:
                Push(Load(PopAddr()));
                break;
            case 0x11:
                Store(PopAddr(), Pop());
                break;
            case 0x12:
                Push(keyState);
                break;
            
            case 0x18:
                ioState = Pop();
                if(ioState == 4 || ioState == 3){
                    for(ushort i = 0x5000;i < 0x6000;i++)
                        ram[i] = 0;
                    ioState = 255;
                    break;
                }
                return;
        }
        pc++;
        if(_doBreakBeforeDumpkey && Load(pc) == 0x12) return;
    }
   return;
}
