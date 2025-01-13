import runtime from "./bin/runtime.mjs"
export default (rom: Uint8Array, opt: {
    isSubframe?: boolean,
    IO: {
        save: (data: Uint8Array) => void,
        load: () => Uint8Array,
        img: (data: Uint8Array) => void,
        snd: (data: Uint8Array) => void,

        key: () => number
    }
})=>runtime().then((e: {
    HEAPU8: Uint8Array,
    _DoBreakBeforeDumpkey: (val: boolean) => void,
    _SetKeyState: (state: number) => void,
    _IOState: () => number,
    _GetIOPtr: () => number,
    _Malloc: (len: number) => number,
    _Free: (ptr: number) => void,
    _Emulate: ()=>void,
    _InitROM: (ptr: number, len: number) => void,
    _Load: (ptr: number) => number,
    _Store: (ptr: number, val: number) => void,
})=>{
    const ptr = e._Malloc(rom.length);
    e.HEAPU8.set(rom, ptr);
    e._InitROM(ptr, rom.length);
    e._Free(ptr);

    e._DoBreakBeforeDumpkey(opt.isSubframe ?? false);
    return {
        Load: a=>e._Load(a),
        Store: (a,v)=>e._Store(a,v),
        Emulate: ()=>{
            e._SetKeyState(opt.IO.key());
            e._Emulate();
            const state = e._IOState();
            if(state != 255){
                switch(state){
                    case 0:
                        opt.IO.img(e.HEAPU8.subarray(e._GetIOPtr(), 0x1000))
                        break;
                    case 1:
                        opt.IO.snd(e.HEAPU8.subarray(e._GetIOPtr(), 0x1000))
                        break;
                    case 2:
                        e.HEAPU8.set(opt.IO.load(), e._GetIOPtr());
                        break;
                    case 3:
                        opt.IO.save(e.HEAPU8.subarray(e._GetIOPtr(), 0x1000))
                        break;
                }
            }
        }
    }
})