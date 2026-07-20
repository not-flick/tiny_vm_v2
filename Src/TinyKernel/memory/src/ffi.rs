use crate::memory::Memory;

#[unsafe(no_mangle)]
pub extern "C" fn memory_create(size: usize) -> *mut Memory {
    Box::into_raw(Box::new(Memory::new(size)))
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_destroy(memory: *mut Memory) {
    if memory.is_null() {
        return;
    }

    drop(Box::from_raw(memory));
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_size(memory: *const Memory) -> usize {
    if memory.is_null() {
        return 0;
    }

    (*memory).size()
}

//
// READ FUNCTIONS
//

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_read8(
    memory: *const Memory,
    addr: usize,
) -> u8 {
    if memory.is_null() {
        return 0;
    }

    (*memory).read8(addr)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_read16(
    memory: *const Memory,
    addr: usize,
) -> u16 {
    if memory.is_null() {
        return 0;
    }

    (*memory).read16(addr)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_read32(
    memory: *const Memory,
    addr: usize,
) -> u32 {
    if memory.is_null() {
        return 0;
    }

    (*memory).read32(addr)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_read64(
    memory: *const Memory,
    addr: usize,
) -> u64 {
    if memory.is_null() {
        return 0;
    }

    (*memory).read64(addr)
}

//
// WRITE FUNCTIONS
//

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_write8(
    memory: *mut Memory,
    addr: usize,
    value: u8,
) {
    if memory.is_null() {
        return;
    }

    (*memory).write8(addr, value);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_write16(
    memory: *mut Memory,
    addr: usize,
    value: u16,
) {
    if memory.is_null() {
        return;
    }

    (*memory).write16(addr, value);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_write32(
    memory: *mut Memory,
    addr: usize,
    value: u32,
) {
    if memory.is_null() {
        return;
    }

    (*memory).write32(addr, value);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_write64(
    memory: *mut Memory,
    addr: usize,
    value: u64,
) {
    if memory.is_null() {
        return;
    }

    (*memory).write64(addr, value);
}

//
// UTILITY FUNCTIONS
//

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_reset(memory: *mut Memory) {
    if memory.is_null() {
        return;
    }

    (*memory).reset();
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_fill(
    memory: *mut Memory,
    value: u8,
) {
    if memory.is_null() {
        return;
    }

    (*memory).fill(value);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn memory_copy(
    memory: *mut Memory,
    dst: usize,
    src: usize,
    len: usize,
) {
    if memory.is_null() {
        return;
    }

    (*memory).copy(dst, src, len);
}