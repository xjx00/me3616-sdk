/*
** File   : app.ld
**
** Copyright (C) 2013-2018 Gosuncn. All rights reserved.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**      http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Author : lixingyuan@gosuncn.cn
**
**  $Date: 2018/02/08 08:45:36GMT+08:00 $
**
*/

/* Entry Point */
ENTRY(Main_Entry)

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)     : ORIGIN = 0x08277800, LENGTH = 0x20000 /* 128K */
    RAM (rw)       : ORIGIN = 0x10174000, LENGTH = 0x40000 /* 256K */
    RETSRAM (rw)   : ORIGIN = 0x04300700, LENGTH = 128     /* 128B  */
}

/* Generate a link error if heap  don't fit into RAM */
_main_stack_size = 0x1000;
_callback_stack_size = 0x1000;

_main_task_priority = 0;
_callback_task_priority = 0;

_api_version = 0x00000001;
_app_properties = 0xa5a5a5a5;
_app_version = 0x00000001;
_app_id = 0x5a5a5a5a;
_load_address = ORIGIN(FLASH);

/*
 * There will be a link error if there is not this amount of 
 * RAM free at the end. 
 */
_Minimum_Heap_Size = 0x1000;

/*
 * Default heap definitions.
 * The heap start immediately after the last statically allocated 
 * .sbss/.noinit section, and extends up to the main stack limit.
 */
PROVIDE ( _Heap_Begin = _end_noinit ) ;
PROVIDE ( _Heap_Limit = ORIGIN(RAM) + LENGTH(RAM) ) ;

/* Define output sections */
SECTIONS
{
    /* The preamble code goes first into FLASH */
    .preamble :
    {
        . = ALIGN(4);
        FILL(0xFF);
        
        KEEP(*(.preamble .preamble.*)) /* preamble data */
        
        . = ALIGN(0x100);
        
        _preamble_limit = .;
    } >FLASH

    .inits : ALIGN(4)
    {
        /* 
         * Memory regions initialisation arrays.
         *
         * Thee are two kinds of arrays for each RAM region, one for 
         * data and one for bss. Each is iterrated at startup and the   
         * region initialisation is performed.
         * 
         * The data array includes:
         * - from (LOADADDR())
         * - region_begin (ADDR())
         * - region_end (ADDR()+SIZEOF())
         *
         * The bss array includes:
         * - region_begin (ADDR())
         * - region_end (ADDR()+SIZEOF())
         *
         * WARNING: It is mandatory that the regions are word aligned, 
         * since the initialisation code works only on words.
         */
         
        __data_regions_array_start = .;
        
        LONG(LOADADDR(.data));
        LONG(ADDR(.data));
        LONG(ADDR(.data)+SIZEOF(.data));
        
        __data_regions_array_end = .;
        
        __bss_regions_array_start = .;
        
        LONG(ADDR(.bss));
        LONG(ADDR(.bss)+SIZEOF(.bss));
        
        __bss_regions_array_end = .;

        /* End of memory regions initialisation arrays. */
    
        /*
         * These are the old initialisation sections, intended to contain
         * naked code, with the prologue/epilogue added by crti.o/crtn.o
         * when linking with startup files. The standalone startup code
         * currently does not run these, better use the init arrays below.
         */
        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);

        /*
         * The preinit code, i.e. an array of pointers to initialisation 
         * functions to be performed before constructors.
         */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        
        /*
         * Used to run the SystemInit() before anything else.
         */
        KEEP(*(.preinit_array_sysinit .preinit_array_sysinit.*))
        
        /* 
         * Used for other platform inits.
         */
        KEEP(*(.preinit_array_platform .preinit_array_platform.*))
        
        /*
         * The application inits. If you need to enforce some order in 
         * execution, create new sections, as before.
         */
        KEEP(*(.preinit_array .preinit_array.*))

        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);

        /*
         * The init code, i.e. an array of pointers to static constructors.
         */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);

        /*
         * The fini code, i.e. an array of pointers to static destructors.
         */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

    } >FLASH

    /*
     * The program code is stored in the .text section, 
     * which goes to FLASH.
     */
    .text : ALIGN(4)
    {
        *(.text .text.*)            /* all remaining code */
 
        /* read-only data (constants) */
        *(.rodata .rodata.* .constdata .constdata.*)        

        *(vtable)                   /* C++ virtual tables */

        KEEP(*(.eh_frame*))

        /*
         * Stub sections generated by the linker, to glue together 
         * ARM and Thumb code. .glue_7 is used for ARM code calling 
         * Thumb code, and .glue_7t is used for Thumb code calling 
         * ARM code. Apparently always generated by the linker, for some
         * architectures, so better leave them here.
         */
        *(.glue_7)
        *(.glue_7t)

    } >FLASH

    /* ARM magic sections */
    .ARM.extab : ALIGN(4)
    {
       *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH
    
    . = ALIGN(4);
    __exidx_start = .;      
    .ARM.exidx : ALIGN(4)
    {
       *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    . = ALIGN(4);
    _etext = .;
    __etext = .;

    /* Stacks for main task and callback task. */
    .stacks :
    {
        . = ALIGN(8);
        _main_stack_base = .;
        . = . + _main_stack_size;
        . = ALIGN(8);
        _callback_stack_base = .;
        . = . + _callback_stack_size;
    } >RAM
    
    .init_data (NOLOAD) :
    {
        . = ALIGN(4);
        *(.init_data .init_data.*)        
    } > RAM
  
    /* 
     * This address is used by the startup code to 
     * initialise the .data section.
     */
    _sidata = LOADADDR(.data);

    /*
     * The initialised data section.
     *
     * The program executes knowing that the data is in the RAM
     * but the loader puts the initial values in the FLASH (inidata).
     * It is one task of the startup to copy the initial values from 
     * FLASH to RAM.
     */
    .data : ALIGN(4)
    {
        FILL(0xFF)
        /* This is used by the startup code to initialise the .data section */
        __data_start__ = . ;
        *(.data_begin .data_begin.*)

        *(.data .data.*)
        
        *(.data_end .data_end.*)
        . = ALIGN(4);

        /* This is used by the startup code to initialise the .data section */
        __data_end__ = . ;

    } >RAM AT>FLASH

    /* The primary uninitialised data section. */
    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_start__ = .;      /* standard newlib definition */
        *(.bss_begin .bss_begin.*)

        *(.bss .bss.*)
        *(COMMON)
        
        *(.bss_end .bss_end.*)
        . = ALIGN(4);
        __bss_end__ = .;        /* standard newlib definition */
    } >RAM

    .noinit (NOLOAD) : ALIGN(4)
    {
        _noinit = .;
        
        *(.noinit .noinit.*) 
        
         . = ALIGN(4) ;
        _end_noinit = .;   
    } > RAM

    /* Mandatory to be word aligned, _sbrk assumes this */
    PROVIDE ( end = _end_noinit ); /* was _ebss */
    PROVIDE ( _end = _end_noinit );
    PROVIDE ( __end = _end_noinit );
    PROVIDE ( __end__ = _end_noinit );

    /*
     * Used for validation only, do not allocate anything here!
     *
     * This is just to check that there is enough RAM left for the Main
     * stack. It should generate an error if it's full.
     */
    ASSERT(_Minimum_Heap_Size <= (_end_noinit - ORIGIN(RAM)), "RAM expired")

    /* Used for keeping data when resume from PSM */
    .retsram_data (NOLOAD) : 
    {
        . = ALIGN(4);
        _retsram_start = .;
        *(.retsram_data)
        . = ALIGN(4);
        _retsram_limit = .;
    } > RETSRAM

    /* Used for calculate app binary size */
    .end :
    {
        . = ALIGN(4);
        _app_limit = .;
    } >FLASH
    
    /* After that there are only debugging sections. */
    
    /* This can remove the debugging information from the standard libraries */    
    /* 
    DISCARD :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
     */
  
    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /*
     * DWARF debug sections.
     * Symbols in the DWARF debugging sections are relative to the beginning
     * of the section so we begin them at 0.  
     */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }    
}
