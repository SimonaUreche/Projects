# 📌 MIPS32 & MIPS32 Pipeline Processor
# 🚀 Project Overview

This project implements a MIPS32 architecture with a 5-stage pipeline processor, designed to improve instruction execution efficiency. The design includes hazard detection and resolution mechanisms, modifications to key pipeline stages, and was tested on an FPGA board for validation.

# 🔍 What is MIPS32?
MIPS32 is a 32-bit Reduced Instruction Set Computer (RISC) architecture developed for high-performance computing. It is widely used in embedded systems, networking devices, and processor design research.

# ✅ Key Features of MIPS32

* Load/Store Architecture: All operations are performed on registers, and memory access happens through load/store instructions.
  
* Fixed-Length Instructions: 32-bit instructions ensure simplicity in decoding.
* 
# * Three Instruction Formats:

     * R-type (Register-based operations)
  
    * I-type (Immediate values, memory access)
  
    * J-type (Jump instructions)
  
# * General-Purpose Registers:
32 registers ($0 - $31), with $0 always storing the value 0.
  
# * Pipeline Execution:
* Supports pipelined processing to improve instruction throughput.

# 🏗️ MIPS32 Pipeline Implementation - 
A 5-stage pipeline was implemented for instruction execution:

# * Instruction Fetch (IF) 
– Fetches instructions from memory.

# * Instruction Decode (ID) – 
Decodes instructions and fetches register values.

# * Execution (EX) – 
Performs arithmetic/logic operations.

# * Memory Access (MEM) – 
Reads/writes data from/to memory.

# * Write Back (WB) – 
Writes results to registers.

# 🏆 Key Functionalities of the MIPS32 Pipeline Processor

✅ 5-Stage Pipeline	Parallel execution for better performance

✅ Register Forwarding	Reduces stalls by forwarding ALU results

✅ Hazard Detection	Identifies and resolves pipeline conflicts

✅ MIPS32 Instruction Set	Implements core MIPS32 functionality

✅ Tested on FPGA	Verified functionality with real hardware
