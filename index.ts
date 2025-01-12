import runtime from "./bin/runtime.mjs"
export default (rom: Uint8Array, opt: {
    isSubframe?: boolean
})=>runtime().then(e=>{
    const ptr = e._Malloc(rom.length);
    e.HEAPU8.set(rom, ptr);
    e._Free(ptr);
    e._DoBreakBeforeDumpkey(opt.isSubframe);
    return {
        Load: a=>e._Load(a),
        Store: (a,v)=>e._Store(a,v),
        Emulate: e._Emulate()
    }
})