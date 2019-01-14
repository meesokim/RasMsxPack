
;@ ------------------------------------------------------------------
;@ ------------------------------------------------------------------
.equ	SCTLR_ENABLE_DATA_CACHE,        0x4
.equ	SCTLR_ENABLE_BRANCH_PREDICTION, 0x800
.equ	SCTLR_ENABLE_INSTRUCTION_CACHE, 0x1000
.equ	SCTLR_ENABLE_MMU,				0x1

	.macro safe_svcmode_maskall reg:req
	
	mrs	\reg , cpsr
	eor	\reg, \reg, #0x1A		/* test for HYP mode */
	tst	\reg, #0x1F
	bic	\reg , \reg , #0x1F		/* clear mode bits */
	orr	\reg , \reg , #0xC0 | 0x13	/* mask IRQ/FIQ bits and set SVC mode */
	bne	1f				/* branch if not HYP mode */
	orr	\reg, \reg, #0x100		/* mask Abort bit */
	adr	lr, 2f
	msr	spsr_cxsf, \reg
	.word	0xE12EF30E			/* msr ELR_hyp, lr */
	.word	0xE160006E			/* eret */
1:	msr	cpsr_c, \reg
2:

	.endm

.globl _start
_start:
    ldr     sp, = (128 * 1024 * 1024)
    // R0 = System Control Register
    mrc p15,0,r0,c1,c0,0
	
    // Enable caches and branch prediction
    bic r0,#SCTLR_ENABLE_BRANCH_PREDICTION
    bic r0,#SCTLR_ENABLE_DATA_CACHE
    bic r0,#SCTLR_ENABLE_INSTRUCTION_CACHE
    bic r0,#SCTLR_ENABLE_MMU
	
    // System Control Register = R0
    mcr p15,0,r0,c1,c0,0
enter_critical_section:
	mrs r0, cpsr
	and r0, r0, #0xc0  // leave just the I and F flags	
	cpsid if
    bl main
hang: b hang

.globl dummy
dummy:
    bx lr
	
.globl start_mmu
start_mmu:
    mov r2,#0
    mcr p15,0,r2,c7,c7,0 ;@ invalidate caches
    mcr p15,0,r2,c8,c7,0 ;@ invalidate tlb
    mcr p15,0,r2,c7,c10,4 ;@ DSB ??

    mvn r2,#0
    bic r2,#0xC
    mcr p15,0,r2,c3,c0,0 ;@ domain

    mcr p15,0,r0,c2,c0,0 ;@ tlb base
    mcr p15,0,r0,c2,c0,1 ;@ tlb base

    mrc p15,0,r2,c1,c0,0
    orr r2,r2,r1
    mcr p15,0,r2,c1,c0,0

    bx lr
	
;@ performs a memory barrier
;@ http://infocenter.arm.com/help/topic/com.arm.doc.ddi0360f/I1014942.html
;@
.global membarrier
membarrier:
    push {r3}
    mov r3, #0                      ;@ The read register Should Be Zero before the call
    mcr p15, 0, r3, C7, C6, 0       ;@ Invalidate Entire Data Cache
    mcr p15, 0, r3, c7, c10, 0      ;@ Clean Entire Data Cache
    mcr p15, 0, r3, c7, c14, 0      ;@ Clean and Invalidate Entire Data Cache
    mcr p15, 0, r3, c7, c10, 4      ;@ Data Synchronization Barrier
    mcr p15, 0, r3, c7, c10, 5      ;@ Data Memory Barrier
    bx lr	
;@-------------------------------------------------------------------------
;@-------------------------------------------------------------------------


;@-------------------------------------------------------------------------
;@
;@ Copyright (c) 2012 David Welch dwelch@dwelch.com
;@
;@ Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
;@
;@ The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
;@
;@ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
;@
;@-------------------------------------------------------------------------
