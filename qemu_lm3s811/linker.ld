/******************************************************************************
 *
 * standalone.ld - Linker script for applications using startup.c and
 *                 DriverLib.
 *
 * Copyright (c) 2005-2007 Luminary Micro, Inc.  All rights reserved.
 *
 * Software License Agreement
 *
 * Luminary Micro, Inc. (LMI) is supplying this software for use solely and
 * exclusively on LMI's microcontroller products.
 *
 * The software is owned by LMI and/or its suppliers, and is protected under
 * applicable copyright laws.  All rights are reserved.  Any use in violation
 * of the foregoing restrictions may subject the user to criminal sanctions
 * under applicable laws, as well as to civil liability for the breach of the
 * terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * This is part of revision 1049 of the Stellaris Driver Library.
 *
 *****************************************************************************/


GROUP (
  "libstdc++_nano.a"
  "libsupc++_nano.a"
  "libc_nano.a"
  "libm.a"
  "libgcc.a"
  "crtbegin.o"
  "crtend.o" 
  "crti.o"
  "crtn.o"
)
 
 
/*
GROUP (
  "libstdc++_nano.a"
  "libsupc++_nano.a"
  "libc_nano.a"
) 
*/ 
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 64K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
}
ENTRY(ResetISR)
SECTIONS
{
    .text : ALIGN(8)
    {
		__vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        *(.after_vectors*)
		
		*(.text)
		*(.rodata* .rodata.* .constdata .constdata.*)
		    
		/* C++ constructors etc */
			. = ALIGN(8);
			KEEP(*(.init))
			
			. = ALIGN(8);
			__preinit_array_start = .;
			KEEP (*(.preinit_array))
			__preinit_array_end = .;
			
			. = ALIGN(8);
			__init_array_start = .;
			KEEP (*(SORT(.init_array.*)))
			KEEP (*(.init_array))
			__init_array_end = .;
			
			KEEP(*(.fini));
            . = ALIGN(8);
            KEEP (*crtbegin.o(.ctors))
            KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
            KEEP (*(SORT(.ctors.*)))
            KEEP (*crtend.o(.ctors))
            
            . = ALIGN(8);
            KEEP (*crtbegin.o(.dtors))
            KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
            KEEP (*(SORT(.dtors.*)))
            KEEP (*crtend.o(.dtors))
            . = ALIGN(8);
        /* End C++ */
			
        _etext = .;
    } > FLASH

    .ARM.extab : ALIGN(8)
	{
	    *(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH
	
	__exidx_start = .;
	.ARM.exidx : ALIGN(8)
	{
	    *(.ARM.exidx.* .gnu.linkonce.armexidx.*)
	} > FLASH
	__exidx_end = .;
	
	_etext = .;
	
    .data : AT(_etext) ALIGN(8)
    {
        _data = .;
        *(vtable)
        *(.data)
        _edata = .;
    } > SRAM

    .bss :
    {
        _bss = .;
        *(.bss)
        *(COMMON)
        _ebss = .;
    } > SRAM
	__bss_start__ = _bss;
	__bss_end__ = _ebss;
	
	
	_StackSize = 0x100;
    /* Reserve space in memory for Stack */
    .heap2stackfill  :
    {
        . += _StackSize;
    } > SRAM
    /* Locate actual Stack in memory map */
    .stack ORIGIN(SRAM) + LENGTH(SRAM) - _StackSize - 0:  ALIGN(8)
    {
        _vStackBase = .;
        . = ALIGN(8);
        _vStackTop = . + _StackSize;
    } > SRAM
}
