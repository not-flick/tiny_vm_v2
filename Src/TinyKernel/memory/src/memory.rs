//! TinyVM Memory Manager
//!
//! Owns the VM's RAM and provides primitive read operations.
//! Higher level components such as the Bus, MMIO, ROM and Devices
//! will build on top of this module.

pub struct Memory {
    ram: Vec<u8>,
}

impl Memory {
    /// Create a new block of RAM.
    pub fn new(size: usize) -> Self {
        Self {
            ram: vec![0; size],
        }
    }

    /// Returns the total RAM size in bytes.
    pub fn size(&self) -> usize {
        self.ram.len()
    }

    /// Returns true if an address range is valid.
    fn check(&self, addr: usize, size: usize) -> bool {
        addr.checked_add(size)
            .map(|end| end <= self.ram.len())
            .unwrap_or(false)
    }

    /// Immutable pointer to RAM.
    pub fn as_ptr(&self) -> *const u8 {
        self.ram.as_ptr()
    }

    /// Mutable pointer to RAM.
    pub fn as_mut_ptr(&mut self) -> *mut u8 {
        self.ram.as_mut_ptr()
    }

    // ============================================================
    // READ OPERATIONS
    // ============================================================

    /// Read one byte.
    pub fn read8(&self, addr: usize) -> u8 {
        if !self.check(addr, 1) {
            return 0;
        }

        self.ram[addr]
    }

    /// Read two bytes.
    pub fn read16(&self, addr: usize) -> u16 {
        if !self.check(addr, 2) {
            return 0;
        }

        u16::from_le_bytes([
            self.ram[addr],
            self.ram[addr + 1],
        ])
    }

    /// Read four bytes.
    pub fn read32(&self, addr: usize) -> u32 {
        if !self.check(addr, 4) {
            return 0;
        }

        u32::from_le_bytes([
            self.ram[addr],
            self.ram[addr + 1],
            self.ram[addr + 2],
            self.ram[addr + 3],
        ])
    }

    /// Read eight bytes.
    pub fn read64(&self, addr: usize) -> u64 {
        if !self.check(addr, 8) {
            return 0;
        }

        u64::from_le_bytes([
            self.ram[addr],
            self.ram[addr + 1],
            self.ram[addr + 2],
            self.ram[addr + 3],
            self.ram[addr + 4],
            self.ram[addr + 5],
            self.ram[addr + 6],
            self.ram[addr + 7],
        ])
    }

    // ============================================================
    // WRITE OPERATIONS
    // ============================================================

    /// Write one byte.
    pub fn write8(&mut self, addr: usize, value: u8) {
        if !self.check(addr, 1) {
            return;
        }

        self.ram[addr] = value;
    }

    /// Write two bytes.
    pub fn write16(&mut self, addr: usize, value: u16) {
        if !self.check(addr, 2) {
            return;
        }

        let bytes = value.to_le_bytes();
        self.ram[addr..addr + 2].copy_from_slice(&bytes);
    }

    /// Write four bytes.
    pub fn write32(&mut self, addr: usize, value: u32) {
        if !self.check(addr, 4) {
            return;
        }

        let bytes = value.to_le_bytes();
        self.ram[addr..addr + 4].copy_from_slice(&bytes);
    }

    /// Write eight bytes.
    pub fn write64(&mut self, addr: usize, value: u64) {
        if !self.check(addr, 8) {
            return;
        }

        let bytes = value.to_le_bytes();
        self.ram[addr..addr + 8].copy_from_slice(&bytes);
    }

    // ============================================================
    // MEMORY UTILITIES
    // ============================================================

    /// Reset the entire RAM to zero.
    pub fn reset(&mut self) {
        self.ram.fill(0);
    }

    /// Fill the entire RAM with a byte.
    pub fn fill(&mut self, value: u8) {
        self.ram.fill(value);
    }

    /// Copy a block of memory.
    ///
    /// Behaves similarly to memmove().
    pub fn copy(&mut self, dst: usize, src: usize, len: usize) {
        if !self.check(src, len) || !self.check(dst, len) {
            return;
        }

        self.ram.copy_within(src..src + len, dst);
    }

    /// Returns a slice of RAM.
    pub fn data(&self) -> &[u8] {
        &self.ram
    }

    /// Returns a mutable slice of RAM.
    pub fn data_mut(&mut self) -> &mut [u8] {
        &mut self.ram
    }

    /// Resize the RAM.
    ///
    /// New bytes are initialized to zero.
    pub fn resize(&mut self, new_size: usize) {
        self.ram.resize(new_size, 0);
    }
}