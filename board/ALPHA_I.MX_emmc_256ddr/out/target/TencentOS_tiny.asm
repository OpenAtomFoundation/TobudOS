
out/target/TencentOS_tiny.elf：     文件格式 elf32-littlearm


Disassembly of section .text:

87800000 <__text_start>:
87800000:	e59ff018 	ldr	pc, [pc, #24]	; 87800020 <vector_reset>
87800004:	e59ff018 	ldr	pc, [pc, #24]	; 87800024 <vector_undefined>
87800008:	e59ff018 	ldr	pc, [pc, #24]	; 87800028 <vector_swi>
8780000c:	e59ff018 	ldr	pc, [pc, #24]	; 8780002c <vector_prefetch_abort>
87800010:	e59ff018 	ldr	pc, [pc, #24]	; 87800030 <vector_data_abort>
87800014:	e59ff018 	ldr	pc, [pc, #24]	; 87800034 <vector_reserved>
87800018:	e59ff018 	ldr	pc, [pc, #24]	; 87800038 <vector_irq>
8780001c:	e59ff018 	ldr	pc, [pc, #24]	; 8780003c <vector_fiq>

87800020 <vector_reset>:
87800020:	87800040 	strhi	r0, [r0, r0, asr #32]

87800024 <vector_undefined>:
87800024:	878000dc 			; <UNDEFINED> instruction: 0x878000dc

87800028 <vector_swi>:
87800028:	8780010c 	strhi	r0, [r0, ip, lsl #2]

8780002c <vector_prefetch_abort>:
8780002c:	87800110 	usada8hi	r0, r0, r1, r0

87800030 <vector_data_abort>:
87800030:	87800144 	strhi	r0, [r0, r4, asr #2]

87800034 <vector_reserved>:
87800034:	87800178 			; <UNDEFINED> instruction: 0x87800178

87800038 <vector_irq>:
87800038:	8780017c 			; <UNDEFINED> instruction: 0x8780017c

8780003c <vector_fiq>:
8780003c:	878001b8 			; <UNDEFINED> instruction: 0x878001b8

87800040 <arm_reset>:
87800040:	ee11cf10 	mrc	15, 0, ip, cr1, cr0, {0}
87800044:	e3ccca01 	bic	ip, ip, #4096	; 0x1000
87800048:	e3ccc005 	bic	ip, ip, #5
8780004c:	ee01cf10 	mcr	15, 0, ip, cr1, cr0, {0}
87800050:	f10e0092 	cpsid	i,#18
87800054:	e59fd05c 	ldr	sp, [pc, #92]	; 878000b8 <arm_reset+0x78>
87800058:	f10e0091 	cpsid	i,#17
8780005c:	e59fd058 	ldr	sp, [pc, #88]	; 878000bc <arm_reset+0x7c>
87800060:	f10e0097 	cpsid	i,#23
87800064:	e59fd054 	ldr	sp, [pc, #84]	; 878000c0 <arm_reset+0x80>
87800068:	f10e009b 	cpsid	i,#27
8780006c:	e59fd050 	ldr	sp, [pc, #80]	; 878000c4 <arm_reset+0x84>
87800070:	f10e009f 	cpsid	i,#31
87800074:	e59fd04c 	ldr	sp, [pc, #76]	; 878000c8 <arm_reset+0x88>
87800078:	f10e0093 	cpsid	i,#19
8780007c:	e59fd048 	ldr	sp, [pc, #72]	; 878000cc <arm_reset+0x8c>
87800080:	f57ff04f 	dsb	sy
87800084:	f57ff06f 	isb	sy
87800088:	e59f0040 	ldr	r0, [pc, #64]	; 878000d0 <arm_reset+0x90>
8780008c:	ee0c0f10 	mcr	15, 0, r0, cr12, cr0, {0}
87800090:	f57ff04f 	dsb	sy
87800094:	f57ff06f 	isb	sy
87800098:	e59f0034 	ldr	r0, [pc, #52]	; 878000d4 <arm_reset+0x94>
8780009c:	e59f1034 	ldr	r1, [pc, #52]	; 878000d8 <arm_reset+0x98>
878000a0:	e3a02000 	mov	r2, #0
878000a4:	e1500001 	cmp	r0, r1
878000a8:	b4802004 	strlt	r2, [r0], #4
878000ac:	bafffffc 	blt	878000a4 <arm_reset+0x64>
878000b0:	fa0024a9 	blx	8780935c <main>
878000b4:	eafffffe 	b	878000b4 <arm_reset+0x74>
878000b8:	8780f198 			; <UNDEFINED> instruction: 0x8780f198
878000bc:	8780e198 			; <UNDEFINED> instruction: 0x8780e198
878000c0:	8780f298 			; <UNDEFINED> instruction: 0x8780f298
878000c4:	8780f398 			; <UNDEFINED> instruction: 0x8780f398
878000c8:	8780fb98 			; <UNDEFINED> instruction: 0x8780fb98
878000cc:	87810b98 			; <UNDEFINED> instruction: 0x87810b98
878000d0:	87800000 	strhi	r0, [r0, r0]
878000d4:	8780df98 			; <UNDEFINED> instruction: 0x8780df98
878000d8:	8781b1a8 	strhi	fp, [r1, r8, lsr #3]

878000dc <arm_undefined>:
878000dc:	f96d0513 	srsdb	sp!, #19
878000e0:	f10e0093 	cpsid	i,#19
878000e4:	e92d500f 	push	{r0, r1, r2, r3, ip, lr}
878000e8:	e1a0000d 	mov	r0, sp
878000ec:	e31d0004 	tst	sp, #4
878000f0:	024dd004 	subeq	sp, sp, #4
878000f4:	e52d0004 	push	{r0}		; (str r0, [sp, #-4]!)
878000f8:	fa00129a 	blx	87804b68 <arm_undefined_handler>
878000fc:	e49d0004 	pop	{r0}		; (ldr r0, [sp], #4)
87800100:	e1a0d000 	mov	sp, r0
87800104:	e8bd500f 	pop	{r0, r1, r2, r3, ip, lr}
87800108:	f8bd0a00 	rfeia	sp!

8780010c <arm_syscall>:
8780010c:	eafffffe 	b	8780010c <arm_syscall>

87800110 <arm_prefetch_abort>:
87800110:	e24ee004 	sub	lr, lr, #4
87800114:	f96d0513 	srsdb	sp!, #19
87800118:	f10e0093 	cpsid	i,#19
8780011c:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
87800120:	e1a0000d 	mov	r0, sp
87800124:	e31d0004 	tst	sp, #4
87800128:	024dd004 	subeq	sp, sp, #4
8780012c:	e52d0004 	push	{r0}		; (str r0, [sp, #-4]!)
87800130:	fa0012b1 	blx	87804bfc <arm_prefetch_abort_handler>
87800134:	e49d0004 	pop	{r0}		; (ldr r0, [sp], #4)
87800138:	e1a0d000 	mov	sp, r0
8780013c:	e8bd5fff 	pop	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
87800140:	f8bd0a00 	rfeia	sp!

87800144 <arm_data_abort>:
87800144:	e24ee008 	sub	lr, lr, #8
87800148:	f96d0513 	srsdb	sp!, #19
8780014c:	f10e0093 	cpsid	i,#19
87800150:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
87800154:	e1a0000d 	mov	r0, sp
87800158:	e31d0004 	tst	sp, #4
8780015c:	024dd004 	subeq	sp, sp, #4
87800160:	e52d0004 	push	{r0}		; (str r0, [sp, #-4]!)
87800164:	fa00129e 	blx	87804be4 <arm_data_abort_handler>
87800168:	e49d0004 	pop	{r0}		; (ldr r0, [sp], #4)
8780016c:	e1a0d000 	mov	sp, r0
87800170:	e8bd5fff 	pop	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
87800174:	f8bd0a00 	rfeia	sp!

87800178 <arm_reserved>:
87800178:	eafffffe 	b	87800178 <arm_reserved>

8780017c <arm_irq>:
8780017c:	e24ee004 	sub	lr, lr, #4
87800180:	f96d0513 	srsdb	sp!, #19
87800184:	f10e0093 	cpsid	i,#19
87800188:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
8780018c:	e1a0000d 	mov	r0, sp
87800190:	e31d0004 	tst	sp, #4
87800194:	024dd004 	subeq	sp, sp, #4
87800198:	e52d0004 	push	{r0}		; (str r0, [sp, #-4]!)
8780019c:	fa000f94 	blx	87803ff4 <tos_knl_irq_enter>
878001a0:	fa001238 	blx	87804a88 <interrupt_irq>
878001a4:	fa000f9b 	blx	87804018 <tos_knl_irq_leave>
878001a8:	e49d0004 	pop	{r0}		; (ldr r0, [sp], #4)
878001ac:	e1a0d000 	mov	sp, r0
878001b0:	e8bd5fff 	pop	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
878001b4:	f8bd0a00 	rfeia	sp!

878001b8 <arm_fiq>:
878001b8:	eafffffe 	b	878001b8 <arm_fiq>

878001bc <port_int_disable>:
878001bc:	f10c0080 	cpsid	i

878001c0 <port_int_enable>:
878001c0:	f1080080 	cpsie	i

878001c4 <port_cpsr_save>:
878001c4:	e10f0000 	mrs	r0, CPSR
878001c8:	f10c0080 	cpsid	i
878001cc:	e12fff1e 	bx	lr

878001d0 <port_cpsr_restore>:
878001d0:	e129f000 	msr	CPSR_fc, r0
878001d4:	e12fff1e 	bx	lr

878001d8 <port_sched_start>:
878001d8:	ea00000a 	b	87800208 <port_context_switch+0x2c>

878001dc <port_context_switch>:
878001dc:	e50d000c 	str	r0, [sp, #-12]
878001e0:	e10f0000 	mrs	r0, CPSR
878001e4:	e31e0001 	tst	lr, #1
878001e8:	13800020 	orrne	r0, r0, #32
878001ec:	e92d0001 	stmfd	sp!, {r0}
878001f0:	e92d4000 	stmfd	sp!, {lr}
878001f4:	e51d0004 	ldr	r0, [sp, #-4]
878001f8:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
878001fc:	e59f0024 	ldr	r0, [pc, #36]	; 87800228 <port_irq_context_switch+0x4>
87800200:	e5900000 	ldr	r0, [r0]
87800204:	e580d000 	str	sp, [r0]
87800208:	e59f001c 	ldr	r0, [pc, #28]	; 8780022c <port_irq_context_switch+0x8>
8780020c:	e5900000 	ldr	r0, [r0]
87800210:	e59f1010 	ldr	r1, [pc, #16]	; 87800228 <port_irq_context_switch+0x4>
87800214:	e5810000 	str	r0, [r1]
87800218:	e590d000 	ldr	sp, [r0]
8780021c:	e8bd5fff 	pop	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
87800220:	f8bd0a00 	rfeia	sp!

87800224 <port_irq_context_switch>:
87800224:	eafffff7 	b	87800208 <port_context_switch+0x2c>
87800228:	87810ba0 	strhi	r0, [r1, r0, lsr #23]
8780022c:	87810b9c 			; <UNDEFINED> instruction: 0x87810b9c

87800230 <__umodsi3>:
87800230:	e2512001 	subs	r2, r1, #1
87800234:	3a000062 	bcc	878003c4 <Ldiv0>
87800238:	11500001 	cmpne	r0, r1
8780023c:	03a00000 	moveq	r0, #0
87800240:	81110002 	tsthi	r1, r2
87800244:	00000002 	andeq	r0, r0, r2
87800248:	91a0f00e 	movls	pc, lr
8780024c:	e3a02000 	mov	r2, #0
87800250:	e3510201 	cmp	r1, #268435456	; 0x10000000
87800254:	31510000 	cmpcc	r1, r0
87800258:	31a01201 	lslcc	r1, r1, #4
8780025c:	32822004 	addcc	r2, r2, #4
87800260:	3afffffa 	bcc	87800250 <__umodsi3+0x20>
87800264:	e3510102 	cmp	r1, #-2147483648	; 0x80000000
87800268:	31510000 	cmpcc	r1, r0
8780026c:	31a01081 	lslcc	r1, r1, #1
87800270:	32822001 	addcc	r2, r2, #1
87800274:	3afffffa 	bcc	87800264 <__umodsi3+0x34>
87800278:	e2522003 	subs	r2, r2, #3
8780027c:	ba00000e 	blt	878002bc <__umodsi3+0x8c>
87800280:	e1500001 	cmp	r0, r1
87800284:	20400001 	subcs	r0, r0, r1
87800288:	e15000a1 	cmp	r0, r1, lsr #1
8780028c:	204000a1 	subcs	r0, r0, r1, lsr #1
87800290:	e1500121 	cmp	r0, r1, lsr #2
87800294:	20400121 	subcs	r0, r0, r1, lsr #2
87800298:	e15001a1 	cmp	r0, r1, lsr #3
8780029c:	204001a1 	subcs	r0, r0, r1, lsr #3
878002a0:	e3500001 	cmp	r0, #1
878002a4:	e1a01221 	lsr	r1, r1, #4
878002a8:	a2522004 	subsge	r2, r2, #4
878002ac:	aafffff3 	bge	87800280 <__umodsi3+0x50>
878002b0:	e3120003 	tst	r2, #3
878002b4:	13300000 	teqne	r0, #0
878002b8:	0a00000a 	beq	878002e8 <__umodsi3+0xb8>
878002bc:	e3720002 	cmn	r2, #2
878002c0:	ba000006 	blt	878002e0 <__umodsi3+0xb0>
878002c4:	0a000002 	beq	878002d4 <__umodsi3+0xa4>
878002c8:	e1500001 	cmp	r0, r1
878002cc:	20400001 	subcs	r0, r0, r1
878002d0:	e1a010a1 	lsr	r1, r1, #1
878002d4:	e1500001 	cmp	r0, r1
878002d8:	20400001 	subcs	r0, r0, r1
878002dc:	e1a010a1 	lsr	r1, r1, #1
878002e0:	e1500001 	cmp	r0, r1
878002e4:	20400001 	subcs	r0, r0, r1
878002e8:	e1a0f00e 	mov	pc, lr
878002ec:	90909090 	umullsls	r9, r0, r0, r0

878002f0 <__modsi3>:
878002f0:	e3510000 	cmp	r1, #0
878002f4:	0a000032 	beq	878003c4 <Ldiv0>
878002f8:	42611000 	rsbmi	r1, r1, #0
878002fc:	e1b0c000 	movs	ip, r0
87800300:	42600000 	rsbmi	r0, r0, #0
87800304:	e2512001 	subs	r2, r1, #1
87800308:	11500001 	cmpne	r0, r1
8780030c:	03a00000 	moveq	r0, #0
87800310:	81110002 	tsthi	r1, r2
87800314:	00000002 	andeq	r0, r0, r2
87800318:	9a000026 	bls	878003b8 <__modsi3+0xc8>
8780031c:	e3a02000 	mov	r2, #0
87800320:	e3510201 	cmp	r1, #268435456	; 0x10000000
87800324:	31510000 	cmpcc	r1, r0
87800328:	31a01201 	lslcc	r1, r1, #4
8780032c:	32822004 	addcc	r2, r2, #4
87800330:	3afffffa 	bcc	87800320 <__modsi3+0x30>
87800334:	e3510102 	cmp	r1, #-2147483648	; 0x80000000
87800338:	31510000 	cmpcc	r1, r0
8780033c:	31a01081 	lslcc	r1, r1, #1
87800340:	32822001 	addcc	r2, r2, #1
87800344:	3afffffa 	bcc	87800334 <__modsi3+0x44>
87800348:	e2522003 	subs	r2, r2, #3
8780034c:	ba00000e 	blt	8780038c <__modsi3+0x9c>
87800350:	e1500001 	cmp	r0, r1
87800354:	20400001 	subcs	r0, r0, r1
87800358:	e15000a1 	cmp	r0, r1, lsr #1
8780035c:	204000a1 	subcs	r0, r0, r1, lsr #1
87800360:	e1500121 	cmp	r0, r1, lsr #2
87800364:	20400121 	subcs	r0, r0, r1, lsr #2
87800368:	e15001a1 	cmp	r0, r1, lsr #3
8780036c:	204001a1 	subcs	r0, r0, r1, lsr #3
87800370:	e3500001 	cmp	r0, #1
87800374:	e1a01221 	lsr	r1, r1, #4
87800378:	a2522004 	subsge	r2, r2, #4
8780037c:	aafffff3 	bge	87800350 <__modsi3+0x60>
87800380:	e3120003 	tst	r2, #3
87800384:	13300000 	teqne	r0, #0
87800388:	0a00000a 	beq	878003b8 <__modsi3+0xc8>
8780038c:	e3720002 	cmn	r2, #2
87800390:	ba000006 	blt	878003b0 <__modsi3+0xc0>
87800394:	0a000002 	beq	878003a4 <__modsi3+0xb4>
87800398:	e1500001 	cmp	r0, r1
8780039c:	20400001 	subcs	r0, r0, r1
878003a0:	e1a010a1 	lsr	r1, r1, #1
878003a4:	e1500001 	cmp	r0, r1
878003a8:	20400001 	subcs	r0, r0, r1
878003ac:	e1a010a1 	lsr	r1, r1, #1
878003b0:	e1500001 	cmp	r0, r1
878003b4:	20400001 	subcs	r0, r0, r1
878003b8:	e35c0000 	cmp	ip, #0
878003bc:	42600000 	rsbmi	r0, r0, #0
878003c0:	e1a0f00e 	mov	pc, lr

878003c4 <Ldiv0>:
878003c4:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
878003c8:	e3a00000 	mov	r0, #0
878003cc:	e49df004 	pop	{pc}		; (ldr pc, [sp], #4)

878003d0 <task1_entry>:
878003d0:	b508      	push	{r3, lr}
878003d2:	f64d 4010 	movw	r0, #56336	; 0xdc10
878003d6:	f2c8 7080 	movt	r0, #34688	; 0x8780
878003da:	f004 fe37 	bl	8780504c <printf>
878003de:	2100      	movs	r1, #0
878003e0:	f44f 60fa 	mov.w	r0, #2000	; 0x7d0
878003e4:	f003 f826 	bl	87803434 <tos_task_delay>
878003e8:	e7f3      	b.n	878003d2 <task1_entry+0x2>
878003ea:	bf00      	nop

878003ec <task2_entry>:
878003ec:	b508      	push	{r3, lr}
878003ee:	f64d 4018 	movw	r0, #56344	; 0xdc18
878003f2:	f2c8 7080 	movt	r0, #34688	; 0x8780
878003f6:	f004 fe29 	bl	8780504c <printf>
878003fa:	2100      	movs	r1, #0
878003fc:	f640 30b8 	movw	r0, #3000	; 0xbb8
87800400:	f003 f818 	bl	87803434 <tos_task_delay>
87800404:	e7f3      	b.n	878003ee <task2_entry+0x2>
87800406:	bf00      	nop

87800408 <tos_mmblk_pool_create>:
87800408:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
8780040a:	4605      	mov	r5, r0
8780040c:	460c      	mov	r4, r1
8780040e:	4616      	mov	r6, r2
87800410:	461f      	mov	r7, r3
87800412:	f003 fee5 	bl	878041e0 <knl_is_inirq>
87800416:	bb40      	cbnz	r0, 8780046a <tos_mmblk_pool_create+0x62>
87800418:	b354      	cbz	r4, 87800470 <tos_mmblk_pool_create+0x68>
8780041a:	07a3      	lsls	r3, r4, #30
8780041c:	d002      	beq.n	87800424 <tos_mmblk_pool_create+0x1c>
8780041e:	f240 20bf 	movw	r0, #703	; 0x2bf
87800422:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800424:	f017 0003 	ands.w	r0, r7, #3
87800428:	d002      	beq.n	87800430 <tos_mmblk_pool_create+0x28>
8780042a:	f240 20be 	movw	r0, #702	; 0x2be
8780042e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800430:	f1b6 0e01 	subs.w	lr, r6, #1
87800434:	eb04 0c07 	add.w	ip, r4, r7
87800438:	4662      	mov	r2, ip
8780043a:	d00b      	beq.n	87800454 <tos_mmblk_pool_create+0x4c>
8780043c:	4623      	mov	r3, r4
8780043e:	3001      	adds	r0, #1
87800440:	19d1      	adds	r1, r2, r7
87800442:	4586      	cmp	lr, r0
87800444:	601a      	str	r2, [r3, #0]
87800446:	4613      	mov	r3, r2
87800448:	460a      	mov	r2, r1
8780044a:	d1f8      	bne.n	8780043e <tos_mmblk_pool_create+0x36>
8780044c:	1eb2      	subs	r2, r6, #2
8780044e:	fb02 7207 	mla	r2, r2, r7, r7
87800452:	4462      	add	r2, ip
87800454:	2000      	movs	r0, #0
87800456:	f64d 23d3 	movw	r3, #56019	; 0xdad3
8780045a:	6010      	str	r0, [r2, #0]
8780045c:	60ac      	str	r4, [r5, #8]
8780045e:	60ec      	str	r4, [r5, #12]
87800460:	61ae      	str	r6, [r5, #24]
87800462:	616e      	str	r6, [r5, #20]
87800464:	612f      	str	r7, [r5, #16]
87800466:	606b      	str	r3, [r5, #4]
87800468:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
8780046a:	f44f 70c8 	mov.w	r0, #400	; 0x190
8780046e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800470:	f240 404c 	movw	r0, #1100	; 0x44c
87800474:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800476:	bf00      	nop

87800478 <tos_mmblk_pool_destroy>:
87800478:	4603      	mov	r3, r0
8780047a:	b180      	cbz	r0, 8780049e <tos_mmblk_pool_destroy+0x26>
8780047c:	6841      	ldr	r1, [r0, #4]
8780047e:	f64d 22d3 	movw	r2, #56019	; 0xdad3
87800482:	4291      	cmp	r1, r2
87800484:	d002      	beq.n	8780048c <tos_mmblk_pool_destroy+0x14>
87800486:	f240 404d 	movw	r0, #1101	; 0x44d
8780048a:	4770      	bx	lr
8780048c:	2200      	movs	r2, #0
8780048e:	6082      	str	r2, [r0, #8]
87800490:	4610      	mov	r0, r2
87800492:	60da      	str	r2, [r3, #12]
87800494:	619a      	str	r2, [r3, #24]
87800496:	615a      	str	r2, [r3, #20]
87800498:	611a      	str	r2, [r3, #16]
8780049a:	605a      	str	r2, [r3, #4]
8780049c:	4770      	bx	lr
8780049e:	f240 404c 	movw	r0, #1100	; 0x44c
878004a2:	4770      	bx	lr

878004a4 <tos_mmblk_alloc>:
878004a4:	b138      	cbz	r0, 878004b6 <tos_mmblk_alloc+0x12>
878004a6:	6842      	ldr	r2, [r0, #4]
878004a8:	f64d 23d3 	movw	r3, #56019	; 0xdad3
878004ac:	429a      	cmp	r2, r3
878004ae:	d005      	beq.n	878004bc <tos_mmblk_alloc+0x18>
878004b0:	f240 404d 	movw	r0, #1101	; 0x44d
878004b4:	4770      	bx	lr
878004b6:	f240 404c 	movw	r0, #1100	; 0x44c
878004ba:	4770      	bx	lr
878004bc:	b570      	push	{r4, r5, r6, lr}
878004be:	4604      	mov	r4, r0
878004c0:	460d      	mov	r5, r1
878004c2:	f004 fbcd 	bl	87804c60 <tos_cpu_cpsr_save>
878004c6:	69a6      	ldr	r6, [r4, #24]
878004c8:	b92e      	cbnz	r6, 878004d6 <tos_mmblk_alloc+0x32>
878004ca:	f004 fbcb 	bl	87804c64 <tos_cpu_cpsr_restore>
878004ce:	602e      	str	r6, [r5, #0]
878004d0:	f240 20bd 	movw	r0, #701	; 0x2bd
878004d4:	bd70      	pop	{r4, r5, r6, pc}
878004d6:	68e3      	ldr	r3, [r4, #12]
878004d8:	3e01      	subs	r6, #1
878004da:	602b      	str	r3, [r5, #0]
878004dc:	68e3      	ldr	r3, [r4, #12]
878004de:	681b      	ldr	r3, [r3, #0]
878004e0:	61a6      	str	r6, [r4, #24]
878004e2:	60e3      	str	r3, [r4, #12]
878004e4:	f004 fbbe 	bl	87804c64 <tos_cpu_cpsr_restore>
878004e8:	2000      	movs	r0, #0
878004ea:	bd70      	pop	{r4, r5, r6, pc}

878004ec <tos_mmblk_free>:
878004ec:	2900      	cmp	r1, #0
878004ee:	bf18      	it	ne
878004f0:	2800      	cmpne	r0, #0
878004f2:	b570      	push	{r4, r5, r6, lr}
878004f4:	bf0c      	ite	eq
878004f6:	2601      	moveq	r6, #1
878004f8:	2600      	movne	r6, #0
878004fa:	d007      	beq.n	8780050c <tos_mmblk_free+0x20>
878004fc:	6842      	ldr	r2, [r0, #4]
878004fe:	f64d 23d3 	movw	r3, #56019	; 0xdad3
87800502:	429a      	cmp	r2, r3
87800504:	d005      	beq.n	87800512 <tos_mmblk_free+0x26>
87800506:	f240 404d 	movw	r0, #1101	; 0x44d
8780050a:	bd70      	pop	{r4, r5, r6, pc}
8780050c:	f240 404c 	movw	r0, #1100	; 0x44c
87800510:	bd70      	pop	{r4, r5, r6, pc}
87800512:	4604      	mov	r4, r0
87800514:	460d      	mov	r5, r1
87800516:	f004 fba3 	bl	87804c60 <tos_cpu_cpsr_save>
8780051a:	69a3      	ldr	r3, [r4, #24]
8780051c:	6962      	ldr	r2, [r4, #20]
8780051e:	4293      	cmp	r3, r2
87800520:	d304      	bcc.n	8780052c <tos_mmblk_free+0x40>
87800522:	f004 fb9f 	bl	87804c64 <tos_cpu_cpsr_restore>
87800526:	f44f 702f 	mov.w	r0, #700	; 0x2bc
8780052a:	bd70      	pop	{r4, r5, r6, pc}
8780052c:	68e2      	ldr	r2, [r4, #12]
8780052e:	3301      	adds	r3, #1
87800530:	602a      	str	r2, [r5, #0]
87800532:	61a3      	str	r3, [r4, #24]
87800534:	60e5      	str	r5, [r4, #12]
87800536:	f004 fb95 	bl	87804c64 <tos_cpu_cpsr_restore>
8780053a:	4630      	mov	r0, r6
8780053c:	bd70      	pop	{r4, r5, r6, pc}
8780053e:	bf00      	nop

87800540 <tos_barrier_create>:
87800540:	b510      	push	{r4, lr}
87800542:	4604      	mov	r4, r0
87800544:	b158      	cbz	r0, 8780055e <tos_barrier_create+0x1e>
87800546:	b909      	cbnz	r1, 8780054c <tos_barrier_create+0xc>
87800548:	2005      	movs	r0, #5
8780054a:	bd10      	pop	{r4, pc}
8780054c:	8201      	strh	r1, [r0, #16]
8780054e:	3008      	adds	r0, #8
87800550:	f003 fee2 	bl	87804318 <pend_object_init>
87800554:	f640 33ee 	movw	r3, #3054	; 0xbee
87800558:	2000      	movs	r0, #0
8780055a:	6063      	str	r3, [r4, #4]
8780055c:	bd10      	pop	{r4, pc}
8780055e:	f240 404c 	movw	r0, #1100	; 0x44c
87800562:	bd10      	pop	{r4, pc}

87800564 <tos_barrier_destroy>:
87800564:	b140      	cbz	r0, 87800578 <tos_barrier_destroy+0x14>
87800566:	6842      	ldr	r2, [r0, #4]
87800568:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
8780056a:	f640 33ee 	movw	r3, #3054	; 0xbee
8780056e:	429a      	cmp	r2, r3
87800570:	d005      	beq.n	8780057e <tos_barrier_destroy+0x1a>
87800572:	f240 404d 	movw	r0, #1101	; 0x44d
87800576:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800578:	f240 404c 	movw	r0, #1100	; 0x44c
8780057c:	4770      	bx	lr
8780057e:	f100 0608 	add.w	r6, r0, #8
87800582:	4604      	mov	r4, r0
87800584:	f004 fb6c 	bl	87804c60 <tos_cpu_cpsr_save>
87800588:	2103      	movs	r1, #3
8780058a:	4607      	mov	r7, r0
8780058c:	4630      	mov	r0, r6
8780058e:	2500      	movs	r5, #0
87800590:	f003 ff36 	bl	87804400 <pend_wakeup_all>
87800594:	4630      	mov	r0, r6
87800596:	f003 fec3 	bl	87804320 <pend_object_deinit>
8780059a:	4638      	mov	r0, r7
8780059c:	6065      	str	r5, [r4, #4]
8780059e:	f004 fb61 	bl	87804c64 <tos_cpu_cpsr_restore>
878005a2:	f003 fdbd 	bl	87804120 <knl_sched>
878005a6:	4628      	mov	r0, r5
878005a8:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878005aa:	bf00      	nop

878005ac <tos_barrier_pend>:
878005ac:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
878005ae:	4605      	mov	r5, r0
878005b0:	f003 fe16 	bl	878041e0 <knl_is_inirq>
878005b4:	4604      	mov	r4, r0
878005b6:	b940      	cbnz	r0, 878005ca <tos_barrier_pend+0x1e>
878005b8:	b155      	cbz	r5, 878005d0 <tos_barrier_pend+0x24>
878005ba:	686a      	ldr	r2, [r5, #4]
878005bc:	f640 33ee 	movw	r3, #3054	; 0xbee
878005c0:	429a      	cmp	r2, r3
878005c2:	d008      	beq.n	878005d6 <tos_barrier_pend+0x2a>
878005c4:	f240 404d 	movw	r0, #1101	; 0x44d
878005c8:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878005ca:	f44f 70c8 	mov.w	r0, #400	; 0x190
878005ce:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878005d0:	f240 404c 	movw	r0, #1100	; 0x44c
878005d4:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878005d6:	f004 fb43 	bl	87804c60 <tos_cpu_cpsr_save>
878005da:	8a29      	ldrh	r1, [r5, #16]
878005dc:	4606      	mov	r6, r0
878005de:	b151      	cbz	r1, 878005f6 <tos_barrier_pend+0x4a>
878005e0:	2901      	cmp	r1, #1
878005e2:	d027      	beq.n	87800634 <tos_barrier_pend+0x88>
878005e4:	f003 fdf2 	bl	878041cc <knl_is_sched_locked>
878005e8:	b148      	cbz	r0, 878005fe <tos_barrier_pend+0x52>
878005ea:	4630      	mov	r0, r6
878005ec:	f004 fb3a 	bl	87804c64 <tos_cpu_cpsr_restore>
878005f0:	f240 40b1 	movw	r0, #1201	; 0x4b1
878005f4:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878005f6:	f004 fb35 	bl	87804c64 <tos_cpu_cpsr_restore>
878005fa:	2006      	movs	r0, #6
878005fc:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878005fe:	8a2f      	ldrh	r7, [r5, #16]
87800600:	f640 34a0 	movw	r4, #2976	; 0xba0
87800604:	f2c8 7481 	movt	r4, #34689	; 0x8781
87800608:	f105 0108 	add.w	r1, r5, #8
8780060c:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
87800610:	6820      	ldr	r0, [r4, #0]
87800612:	3f01      	subs	r7, #1
87800614:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
87800618:	822f      	strh	r7, [r5, #16]
8780061a:	f003 fed1 	bl	878043c0 <pend_task_block>
8780061e:	4630      	mov	r0, r6
87800620:	f004 fb20 	bl	87804c64 <tos_cpu_cpsr_restore>
87800624:	f003 fd7c 	bl	87804120 <knl_sched>
87800628:	6823      	ldr	r3, [r4, #0]
8780062a:	6e18      	ldr	r0, [r3, #96]	; 0x60
8780062c:	e8bd 40f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, lr}
87800630:	f003 be8a 	b.w	87804348 <pend_state2errno>
87800634:	f105 0008 	add.w	r0, r5, #8
87800638:	822c      	strh	r4, [r5, #16]
8780063a:	f003 fee1 	bl	87804400 <pend_wakeup_all>
8780063e:	4630      	mov	r0, r6
87800640:	f004 fb10 	bl	87804c64 <tos_cpu_cpsr_restore>
87800644:	4620      	mov	r0, r4
87800646:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87800648 <tos_barrier_reset>:
87800648:	b510      	push	{r4, lr}
8780064a:	4604      	mov	r4, r0
8780064c:	b082      	sub	sp, #8
8780064e:	b148      	cbz	r0, 87800664 <tos_barrier_reset+0x1c>
87800650:	f640 33ee 	movw	r3, #3054	; 0xbee
87800654:	6842      	ldr	r2, [r0, #4]
87800656:	429a      	cmp	r2, r3
87800658:	bf18      	it	ne
8780065a:	f240 404d 	movwne	r0, #1101	; 0x44d
8780065e:	d005      	beq.n	8780066c <tos_barrier_reset+0x24>
87800660:	b002      	add	sp, #8
87800662:	bd10      	pop	{r4, pc}
87800664:	f240 404c 	movw	r0, #1100	; 0x44c
87800668:	b002      	add	sp, #8
8780066a:	bd10      	pop	{r4, pc}
8780066c:	9101      	str	r1, [sp, #4]
8780066e:	f004 faf7 	bl	87804c60 <tos_cpu_cpsr_save>
87800672:	9901      	ldr	r1, [sp, #4]
87800674:	8221      	strh	r1, [r4, #16]
87800676:	f004 faf5 	bl	87804c64 <tos_cpu_cpsr_restore>
8780067a:	2000      	movs	r0, #0
8780067c:	e7f0      	b.n	87800660 <tos_barrier_reset+0x18>
8780067e:	bf00      	nop

87800680 <blk_split>:
87800680:	1d0b      	adds	r3, r1, #4
87800682:	6842      	ldr	r2, [r0, #4]
87800684:	4403      	add	r3, r0
87800686:	b4f0      	push	{r4, r5, r6, r7}
87800688:	685e      	ldr	r6, [r3, #4]
8780068a:	f06f 0403 	mvn.w	r4, #3
8780068e:	1a64      	subs	r4, r4, r1
87800690:	f022 0203 	bic.w	r2, r2, #3
87800694:	4414      	add	r4, r2
87800696:	461d      	mov	r5, r3
87800698:	f006 0203 	and.w	r2, r6, #3
8780069c:	4607      	mov	r7, r0
8780069e:	4322      	orrs	r2, r4
878006a0:	4618      	mov	r0, r3
878006a2:	f845 2f04 	str.w	r2, [r5, #4]!
878006a6:	687a      	ldr	r2, [r7, #4]
878006a8:	f002 0203 	and.w	r2, r2, #3
878006ac:	4311      	orrs	r1, r2
878006ae:	6079      	str	r1, [r7, #4]
878006b0:	685a      	ldr	r2, [r3, #4]
878006b2:	f022 0203 	bic.w	r2, r2, #3
878006b6:	18a9      	adds	r1, r5, r2
878006b8:	50ab      	str	r3, [r5, r2]
878006ba:	684a      	ldr	r2, [r1, #4]
878006bc:	bcf0      	pop	{r4, r5, r6, r7}
878006be:	f042 0202 	orr.w	r2, r2, #2
878006c2:	604a      	str	r2, [r1, #4]
878006c4:	685a      	ldr	r2, [r3, #4]
878006c6:	f042 0201 	orr.w	r2, r2, #1
878006ca:	605a      	str	r2, [r3, #4]
878006cc:	4770      	bx	lr
878006ce:	bf00      	nop

878006d0 <mmheap_ctl_init>:
878006d0:	f242 4228 	movw	r2, #9256	; 0x2428
878006d4:	b470      	push	{r4, r5, r6}
878006d6:	f2c8 7281 	movt	r2, #34689	; 0x8781
878006da:	2600      	movs	r6, #0
878006dc:	4635      	mov	r5, r6
878006de:	f102 0380 	add.w	r3, r2, #128	; 0x80
878006e2:	f502 6448 	add.w	r4, r2, #3200	; 0xc80
878006e6:	f102 0110 	add.w	r1, r2, #16
878006ea:	4610      	mov	r0, r2
878006ec:	6191      	str	r1, [r2, #24]
878006ee:	61d1      	str	r1, [r2, #28]
878006f0:	f840 6f20 	str.w	r6, [r0, #32]!
878006f4:	6016      	str	r6, [r2, #0]
878006f6:	6056      	str	r6, [r2, #4]
878006f8:	6096      	str	r6, [r2, #8]
878006fa:	60d6      	str	r6, [r2, #12]
878006fc:	f103 0280 	add.w	r2, r3, #128	; 0x80
87800700:	f840 5f04 	str.w	r5, [r0, #4]!
87800704:	f843 1f04 	str.w	r1, [r3, #4]!
87800708:	4293      	cmp	r3, r2
8780070a:	d1fb      	bne.n	87800704 <mmheap_ctl_init+0x34>
8780070c:	429c      	cmp	r4, r3
8780070e:	d1f5      	bne.n	878006fc <mmheap_ctl_init+0x2c>
87800710:	bc70      	pop	{r4, r5, r6}
87800712:	4770      	bx	lr

87800714 <generic_fls.part.0>:
87800714:	0c02      	lsrs	r2, r0, #16
87800716:	4603      	mov	r3, r0
87800718:	2020      	movs	r0, #32
8780071a:	0412      	lsls	r2, r2, #16
8780071c:	b90a      	cbnz	r2, 87800722 <generic_fls.part.0+0xe>
8780071e:	041b      	lsls	r3, r3, #16
87800720:	2010      	movs	r0, #16
87800722:	f013 4f7f 	tst.w	r3, #4278190080	; 0xff000000
87800726:	bf04      	itt	eq
87800728:	021b      	lsleq	r3, r3, #8
8780072a:	3808      	subeq	r0, #8
8780072c:	f013 4f70 	tst.w	r3, #4026531840	; 0xf0000000
87800730:	bf04      	itt	eq
87800732:	011b      	lsleq	r3, r3, #4
87800734:	3804      	subeq	r0, #4
87800736:	f013 4f40 	tst.w	r3, #3221225472	; 0xc0000000
8780073a:	bf04      	itt	eq
8780073c:	009b      	lsleq	r3, r3, #2
8780073e:	3802      	subeq	r0, #2
87800740:	2b00      	cmp	r3, #0
87800742:	bfa8      	it	ge
87800744:	f100 30ff 	addge.w	r0, r0, #4294967295	; 0xffffffff
87800748:	4770      	bx	lr
8780074a:	bf00      	nop

8780074c <mapping_insert>:
8780074c:	287f      	cmp	r0, #127	; 0x7f
8780074e:	b570      	push	{r4, r5, r6, lr}
87800750:	4604      	mov	r4, r0
87800752:	460e      	mov	r6, r1
87800754:	bf98      	it	ls
87800756:	1084      	asrls	r4, r0, #2
87800758:	4615      	mov	r5, r2
8780075a:	bf98      	it	ls
8780075c:	2000      	movls	r0, #0
8780075e:	d906      	bls.n	8780076e <mapping_insert+0x22>
87800760:	f7ff ffd8 	bl	87800714 <generic_fls.part.0>
87800764:	1f83      	subs	r3, r0, #6
87800766:	3807      	subs	r0, #7
87800768:	411c      	asrs	r4, r3
8780076a:	f084 0420 	eor.w	r4, r4, #32
8780076e:	6030      	str	r0, [r6, #0]
87800770:	602c      	str	r4, [r5, #0]
87800772:	bd70      	pop	{r4, r5, r6, pc}

87800774 <blk_insert>:
87800774:	b5f0      	push	{r4, r5, r6, r7, lr}
87800776:	4604      	mov	r4, r0
87800778:	6840      	ldr	r0, [r0, #4]
8780077a:	b083      	sub	sp, #12
8780077c:	aa01      	add	r2, sp, #4
8780077e:	4669      	mov	r1, sp
87800780:	f020 0003 	bic.w	r0, r0, #3
87800784:	f7ff ffe2 	bl	8780074c <mapping_insert>
87800788:	9d00      	ldr	r5, [sp, #0]
8780078a:	f242 4328 	movw	r3, #9256	; 0x2428
8780078e:	9801      	ldr	r0, [sp, #4]
87800790:	f2c8 7381 	movt	r3, #34689	; 0x8781
87800794:	2101      	movs	r1, #1
87800796:	f103 0610 	add.w	r6, r3, #16
8780079a:	f105 0708 	add.w	r7, r5, #8
8780079e:	eb03 0787 	add.w	r7, r3, r7, lsl #2
878007a2:	eb00 1245 	add.w	r2, r0, r5, lsl #5
878007a6:	fa01 fc00 	lsl.w	ip, r1, r0
878007aa:	6a18      	ldr	r0, [r3, #32]
878007ac:	40a9      	lsls	r1, r5
878007ae:	3220      	adds	r2, #32
878007b0:	eb03 0282 	add.w	r2, r3, r2, lsl #2
878007b4:	4301      	orrs	r1, r0
878007b6:	6878      	ldr	r0, [r7, #4]
878007b8:	6855      	ldr	r5, [r2, #4]
878007ba:	60e6      	str	r6, [r4, #12]
878007bc:	ea4c 0000 	orr.w	r0, ip, r0
878007c0:	60a5      	str	r5, [r4, #8]
878007c2:	60ec      	str	r4, [r5, #12]
878007c4:	6054      	str	r4, [r2, #4]
878007c6:	6219      	str	r1, [r3, #32]
878007c8:	6078      	str	r0, [r7, #4]
878007ca:	b003      	add	sp, #12
878007cc:	bdf0      	pop	{r4, r5, r6, r7, pc}
878007ce:	bf00      	nop

878007d0 <blk_prepare_used>:
878007d0:	b510      	push	{r4, lr}
878007d2:	4604      	mov	r4, r0
878007d4:	b318      	cbz	r0, 8780081e <blk_prepare_used+0x4e>
878007d6:	460a      	mov	r2, r1
878007d8:	6843      	ldr	r3, [r0, #4]
878007da:	3210      	adds	r2, #16
878007dc:	f023 0303 	bic.w	r3, r3, #3
878007e0:	4293      	cmp	r3, r2
878007e2:	d30f      	bcc.n	87800804 <blk_prepare_used+0x34>
878007e4:	f7ff ff4c 	bl	87800680 <blk_split>
878007e8:	6863      	ldr	r3, [r4, #4]
878007ea:	6842      	ldr	r2, [r0, #4]
878007ec:	f023 0303 	bic.w	r3, r3, #3
878007f0:	4423      	add	r3, r4
878007f2:	f042 0202 	orr.w	r2, r2, #2
878007f6:	605c      	str	r4, [r3, #4]
878007f8:	6042      	str	r2, [r0, #4]
878007fa:	f7ff ffbb 	bl	87800774 <blk_insert>
878007fe:	6863      	ldr	r3, [r4, #4]
87800800:	f023 0303 	bic.w	r3, r3, #3
87800804:	f104 0008 	add.w	r0, r4, #8
87800808:	3b04      	subs	r3, #4
8780080a:	4403      	add	r3, r0
8780080c:	685a      	ldr	r2, [r3, #4]
8780080e:	f022 0202 	bic.w	r2, r2, #2
87800812:	605a      	str	r2, [r3, #4]
87800814:	6863      	ldr	r3, [r4, #4]
87800816:	f023 0301 	bic.w	r3, r3, #1
8780081a:	6063      	str	r3, [r4, #4]
8780081c:	bd10      	pop	{r4, pc}
8780081e:	bd10      	pop	{r4, pc}

87800820 <remove_free_block>:
87800820:	eb02 1341 	add.w	r3, r2, r1, lsl #5
87800824:	b4f0      	push	{r4, r5, r6, r7}
87800826:	f242 4428 	movw	r4, #9256	; 0x2428
8780082a:	68c6      	ldr	r6, [r0, #12]
8780082c:	f2c8 7481 	movt	r4, #34689	; 0x8781
87800830:	3320      	adds	r3, #32
87800832:	eb04 0383 	add.w	r3, r4, r3, lsl #2
87800836:	6885      	ldr	r5, [r0, #8]
87800838:	685f      	ldr	r7, [r3, #4]
8780083a:	60ee      	str	r6, [r5, #12]
8780083c:	60b5      	str	r5, [r6, #8]
8780083e:	4287      	cmp	r7, r0
87800840:	d001      	beq.n	87800846 <remove_free_block+0x26>
87800842:	bcf0      	pop	{r4, r5, r6, r7}
87800844:	4770      	bx	lr
87800846:	f104 0010 	add.w	r0, r4, #16
8780084a:	605d      	str	r5, [r3, #4]
8780084c:	4285      	cmp	r5, r0
8780084e:	d1f8      	bne.n	87800842 <remove_free_block+0x22>
87800850:	f101 0308 	add.w	r3, r1, #8
87800854:	2001      	movs	r0, #1
87800856:	eb04 0383 	add.w	r3, r4, r3, lsl #2
8780085a:	fa00 f502 	lsl.w	r5, r0, r2
8780085e:	685a      	ldr	r2, [r3, #4]
87800860:	ea22 0205 	bic.w	r2, r2, r5
87800864:	605a      	str	r2, [r3, #4]
87800866:	2a00      	cmp	r2, #0
87800868:	d1eb      	bne.n	87800842 <remove_free_block+0x22>
8780086a:	6a23      	ldr	r3, [r4, #32]
8780086c:	4088      	lsls	r0, r1
8780086e:	ea23 0000 	bic.w	r0, r3, r0
87800872:	6220      	str	r0, [r4, #32]
87800874:	e7e5      	b.n	87800842 <remove_free_block+0x22>
87800876:	bf00      	nop

87800878 <blk_remove>:
87800878:	b510      	push	{r4, lr}
8780087a:	4604      	mov	r4, r0
8780087c:	6840      	ldr	r0, [r0, #4]
8780087e:	b082      	sub	sp, #8
87800880:	aa01      	add	r2, sp, #4
87800882:	4669      	mov	r1, sp
87800884:	f020 0003 	bic.w	r0, r0, #3
87800888:	f7ff ff60 	bl	8780074c <mapping_insert>
8780088c:	4620      	mov	r0, r4
8780088e:	e89d 0006 	ldmia.w	sp, {r1, r2}
87800892:	f7ff ffc5 	bl	87800820 <remove_free_block>
87800896:	b002      	add	sp, #8
87800898:	bd10      	pop	{r4, pc}
8780089a:	bf00      	nop

8780089c <blk_merge_next>:
8780089c:	b570      	push	{r4, r5, r6, lr}
8780089e:	1d06      	adds	r6, r0, #4
878008a0:	6844      	ldr	r4, [r0, #4]
878008a2:	4605      	mov	r5, r0
878008a4:	f024 0403 	bic.w	r4, r4, #3
878008a8:	4434      	add	r4, r6
878008aa:	6863      	ldr	r3, [r4, #4]
878008ac:	07db      	lsls	r3, r3, #31
878008ae:	d50c      	bpl.n	878008ca <blk_merge_next+0x2e>
878008b0:	4620      	mov	r0, r4
878008b2:	f7ff ffe1 	bl	87800878 <blk_remove>
878008b6:	6862      	ldr	r2, [r4, #4]
878008b8:	686b      	ldr	r3, [r5, #4]
878008ba:	f022 0203 	bic.w	r2, r2, #3
878008be:	3304      	adds	r3, #4
878008c0:	4413      	add	r3, r2
878008c2:	f023 0203 	bic.w	r2, r3, #3
878008c6:	606b      	str	r3, [r5, #4]
878008c8:	50b5      	str	r5, [r6, r2]
878008ca:	4628      	mov	r0, r5
878008cc:	bd70      	pop	{r4, r5, r6, pc}
878008ce:	bf00      	nop

878008d0 <blk_locate_free>:
878008d0:	b530      	push	{r4, r5, lr}
878008d2:	2300      	movs	r3, #0
878008d4:	b083      	sub	sp, #12
878008d6:	9300      	str	r3, [sp, #0]
878008d8:	9301      	str	r3, [sp, #4]
878008da:	b188      	cbz	r0, 87800900 <blk_locate_free+0x30>
878008dc:	287f      	cmp	r0, #127	; 0x7f
878008de:	4604      	mov	r4, r0
878008e0:	d906      	bls.n	878008f0 <blk_locate_free+0x20>
878008e2:	f7ff ff17 	bl	87800714 <generic_fls.part.0>
878008e6:	2301      	movs	r3, #1
878008e8:	3806      	subs	r0, #6
878008ea:	4083      	lsls	r3, r0
878008ec:	3b01      	subs	r3, #1
878008ee:	441c      	add	r4, r3
878008f0:	4620      	mov	r0, r4
878008f2:	aa01      	add	r2, sp, #4
878008f4:	4669      	mov	r1, sp
878008f6:	f7ff ff29 	bl	8780074c <mapping_insert>
878008fa:	9c00      	ldr	r4, [sp, #0]
878008fc:	2c17      	cmp	r4, #23
878008fe:	dd03      	ble.n	87800908 <blk_locate_free+0x38>
87800900:	2400      	movs	r4, #0
87800902:	4620      	mov	r0, r4
87800904:	b003      	add	sp, #12
87800906:	bd30      	pop	{r4, r5, pc}
87800908:	f242 4528 	movw	r5, #9256	; 0x2428
8780090c:	9b01      	ldr	r3, [sp, #4]
8780090e:	f2c8 7581 	movt	r5, #34689	; 0x8781
87800912:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
87800916:	eb05 0284 	add.w	r2, r5, r4, lsl #2
8780091a:	6a50      	ldr	r0, [r2, #36]	; 0x24
8780091c:	fa01 f303 	lsl.w	r3, r1, r3
87800920:	4003      	ands	r3, r0
87800922:	d111      	bne.n	87800948 <blk_locate_free+0x78>
87800924:	3401      	adds	r4, #1
87800926:	6a2b      	ldr	r3, [r5, #32]
87800928:	fa01 f204 	lsl.w	r2, r1, r4
8780092c:	401a      	ands	r2, r3
8780092e:	d0e7      	beq.n	87800900 <blk_locate_free+0x30>
87800930:	4250      	negs	r0, r2
87800932:	4010      	ands	r0, r2
87800934:	bf08      	it	eq
87800936:	460c      	moveq	r4, r1
87800938:	d005      	beq.n	87800946 <blk_locate_free+0x76>
8780093a:	f7ff feeb 	bl	87800714 <generic_fls.part.0>
8780093e:	eb05 0380 	add.w	r3, r5, r0, lsl #2
87800942:	1e44      	subs	r4, r0, #1
87800944:	6a1b      	ldr	r3, [r3, #32]
87800946:	9400      	str	r4, [sp, #0]
87800948:	4258      	negs	r0, r3
8780094a:	4018      	ands	r0, r3
8780094c:	bf08      	it	eq
8780094e:	f04f 32ff 	moveq.w	r2, #4294967295	; 0xffffffff
87800952:	d10f      	bne.n	87800974 <blk_locate_free+0xa4>
87800954:	eb02 1444 	add.w	r4, r2, r4, lsl #5
87800958:	9201      	str	r2, [sp, #4]
8780095a:	eb05 0484 	add.w	r4, r5, r4, lsl #2
8780095e:	f8d4 4084 	ldr.w	r4, [r4, #132]	; 0x84
87800962:	2c00      	cmp	r4, #0
87800964:	d0cd      	beq.n	87800902 <blk_locate_free+0x32>
87800966:	4620      	mov	r0, r4
87800968:	9900      	ldr	r1, [sp, #0]
8780096a:	f7ff ff59 	bl	87800820 <remove_free_block>
8780096e:	4620      	mov	r0, r4
87800970:	b003      	add	sp, #12
87800972:	bd30      	pop	{r4, r5, pc}
87800974:	f7ff fece 	bl	87800714 <generic_fls.part.0>
87800978:	1e42      	subs	r2, r0, #1
8780097a:	e7eb      	b.n	87800954 <blk_locate_free+0x84>

8780097c <mmheap_init>:
8780097c:	b508      	push	{r3, lr}
8780097e:	f7ff fea7 	bl	878006d0 <mmheap_ctl_init>
87800982:	2000      	movs	r0, #0
87800984:	bd08      	pop	{r3, pc}
87800986:	bf00      	nop

87800988 <tos_mmheap_alloc>:
87800988:	b510      	push	{r4, lr}
8780098a:	4604      	mov	r4, r0
8780098c:	b158      	cbz	r0, 878009a6 <tos_mmheap_alloc+0x1e>
8780098e:	3003      	adds	r0, #3
87800990:	f020 0003 	bic.w	r0, r0, #3
87800994:	f1b0 4f80 	cmp.w	r0, #1073741824	; 0x40000000
87800998:	bf88      	it	hi
8780099a:	2400      	movhi	r4, #0
8780099c:	d803      	bhi.n	878009a6 <tos_mmheap_alloc+0x1e>
8780099e:	280c      	cmp	r0, #12
878009a0:	bf38      	it	cc
878009a2:	200c      	movcc	r0, #12
878009a4:	4604      	mov	r4, r0
878009a6:	4620      	mov	r0, r4
878009a8:	f7ff ff92 	bl	878008d0 <blk_locate_free>
878009ac:	b118      	cbz	r0, 878009b6 <tos_mmheap_alloc+0x2e>
878009ae:	4621      	mov	r1, r4
878009b0:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
878009b4:	e70c      	b.n	878007d0 <blk_prepare_used>
878009b6:	bd10      	pop	{r4, pc}

878009b8 <tos_mmheap_calloc>:
878009b8:	b538      	push	{r3, r4, r5, lr}
878009ba:	fb01 f400 	mul.w	r4, r1, r0
878009be:	4620      	mov	r0, r4
878009c0:	f7ff ffe2 	bl	87800988 <tos_mmheap_alloc>
878009c4:	4605      	mov	r5, r0
878009c6:	b118      	cbz	r0, 878009d0 <tos_mmheap_calloc+0x18>
878009c8:	4622      	mov	r2, r4
878009ca:	2100      	movs	r1, #0
878009cc:	f004 fd14 	bl	878053f8 <memset>
878009d0:	4628      	mov	r0, r5
878009d2:	bd38      	pop	{r3, r4, r5, pc}

878009d4 <tos_mmheap_aligned_alloc>:
878009d4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
878009d6:	460d      	mov	r5, r1
878009d8:	b190      	cbz	r0, 87800a00 <tos_mmheap_aligned_alloc+0x2c>
878009da:	1cc4      	adds	r4, r0, #3
878009dc:	f024 0403 	bic.w	r4, r4, #3
878009e0:	f1b4 4f80 	cmp.w	r4, #1073741824	; 0x40000000
878009e4:	d80c      	bhi.n	87800a00 <tos_mmheap_aligned_alloc+0x2c>
878009e6:	2c0c      	cmp	r4, #12
878009e8:	f101 0010 	add.w	r0, r1, #16
878009ec:	bf38      	it	cc
878009ee:	240c      	movcc	r4, #12
878009f0:	1902      	adds	r2, r0, r4
878009f2:	d13a      	bne.n	87800a6a <tos_mmheap_aligned_alloc+0x96>
878009f4:	2904      	cmp	r1, #4
878009f6:	4610      	mov	r0, r2
878009f8:	bf94      	ite	ls
878009fa:	2300      	movls	r3, #0
878009fc:	2301      	movhi	r3, #1
878009fe:	e030      	b.n	87800a62 <tos_mmheap_aligned_alloc+0x8e>
87800a00:	f115 0410 	adds.w	r4, r5, #16
87800a04:	d120      	bne.n	87800a48 <tos_mmheap_aligned_alloc+0x74>
87800a06:	4620      	mov	r0, r4
87800a08:	f7ff ff62 	bl	878008d0 <blk_locate_free>
87800a0c:	4606      	mov	r6, r0
87800a0e:	2800      	cmp	r0, #0
87800a10:	d05b      	beq.n	87800aca <tos_mmheap_aligned_alloc+0xf6>
87800a12:	f100 0708 	add.w	r7, r0, #8
87800a16:	1e6b      	subs	r3, r5, #1
87800a18:	18fa      	adds	r2, r7, r3
87800a1a:	4268      	negs	r0, r5
87800a1c:	4002      	ands	r2, r0
87800a1e:	1bd1      	subs	r1, r2, r7
87800a20:	f101 3eff 	add.w	lr, r1, #4294967295	; 0xffffffff
87800a24:	f1be 0f0e 	cmp.w	lr, #14
87800a28:	d808      	bhi.n	87800a3c <tos_mmheap_aligned_alloc+0x68>
87800a2a:	4413      	add	r3, r2
87800a2c:	f1c1 0110 	rsb	r1, r1, #16
87800a30:	42a9      	cmp	r1, r5
87800a32:	bf2c      	ite	cs
87800a34:	1859      	addcs	r1, r3, r1
87800a36:	1959      	addcc	r1, r3, r5
87800a38:	4001      	ands	r1, r0
87800a3a:	1bc9      	subs	r1, r1, r7
87800a3c:	bb21      	cbnz	r1, 87800a88 <tos_mmheap_aligned_alloc+0xb4>
87800a3e:	4621      	mov	r1, r4
87800a40:	4630      	mov	r0, r6
87800a42:	e8bd 40f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, lr}
87800a46:	e6c3      	b.n	878007d0 <blk_prepare_used>
87800a48:	1e6b      	subs	r3, r5, #1
87800a4a:	4268      	negs	r0, r5
87800a4c:	441c      	add	r4, r3
87800a4e:	4020      	ands	r0, r4
87800a50:	f1b0 4f80 	cmp.w	r0, #1073741824	; 0x40000000
87800a54:	bf9c      	itt	ls
87800a56:	2300      	movls	r3, #0
87800a58:	461c      	movls	r4, r3
87800a5a:	d834      	bhi.n	87800ac6 <tos_mmheap_aligned_alloc+0xf2>
87800a5c:	280c      	cmp	r0, #12
87800a5e:	bf38      	it	cc
87800a60:	200c      	movcc	r0, #12
87800a62:	2b00      	cmp	r3, #0
87800a64:	bf08      	it	eq
87800a66:	4620      	moveq	r0, r4
87800a68:	e7ce      	b.n	87800a08 <tos_mmheap_aligned_alloc+0x34>
87800a6a:	1e4b      	subs	r3, r1, #1
87800a6c:	4248      	negs	r0, r1
87800a6e:	4413      	add	r3, r2
87800a70:	2904      	cmp	r1, #4
87800a72:	ea00 0003 	and.w	r0, r0, r3
87800a76:	bf94      	ite	ls
87800a78:	2300      	movls	r3, #0
87800a7a:	2301      	movhi	r3, #1
87800a7c:	f1b0 4f80 	cmp.w	r0, #1073741824	; 0x40000000
87800a80:	bf88      	it	hi
87800a82:	2000      	movhi	r0, #0
87800a84:	d9ea      	bls.n	87800a5c <tos_mmheap_aligned_alloc+0x88>
87800a86:	e7ec      	b.n	87800a62 <tos_mmheap_aligned_alloc+0x8e>
87800a88:	6872      	ldr	r2, [r6, #4]
87800a8a:	f101 0310 	add.w	r3, r1, #16
87800a8e:	f022 0203 	bic.w	r2, r2, #3
87800a92:	429a      	cmp	r2, r3
87800a94:	d3d3      	bcc.n	87800a3e <tos_mmheap_aligned_alloc+0x6a>
87800a96:	3904      	subs	r1, #4
87800a98:	4630      	mov	r0, r6
87800a9a:	f7ff fdf1 	bl	87800680 <blk_split>
87800a9e:	4602      	mov	r2, r0
87800aa0:	4630      	mov	r0, r6
87800aa2:	6853      	ldr	r3, [r2, #4]
87800aa4:	f043 0302 	orr.w	r3, r3, #2
87800aa8:	6053      	str	r3, [r2, #4]
87800aaa:	6873      	ldr	r3, [r6, #4]
87800aac:	f023 0303 	bic.w	r3, r3, #3
87800ab0:	441f      	add	r7, r3
87800ab2:	f847 6c04 	str.w	r6, [r7, #-4]
87800ab6:	4616      	mov	r6, r2
87800ab8:	f7ff fe5c 	bl	87800774 <blk_insert>
87800abc:	4621      	mov	r1, r4
87800abe:	4630      	mov	r0, r6
87800ac0:	e8bd 40f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, lr}
87800ac4:	e684      	b.n	878007d0 <blk_prepare_used>
87800ac6:	2400      	movs	r4, #0
87800ac8:	e79d      	b.n	87800a06 <tos_mmheap_aligned_alloc+0x32>
87800aca:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87800acc <tos_mmheap_free>:
87800acc:	b378      	cbz	r0, 87800b2e <tos_mmheap_free+0x62>
87800ace:	b538      	push	{r3, r4, r5, lr}
87800ad0:	f1a0 0408 	sub.w	r4, r0, #8
87800ad4:	6863      	ldr	r3, [r4, #4]
87800ad6:	1f01      	subs	r1, r0, #4
87800ad8:	f023 0303 	bic.w	r3, r3, #3
87800adc:	18ca      	adds	r2, r1, r3
87800ade:	50cc      	str	r4, [r1, r3]
87800ae0:	6853      	ldr	r3, [r2, #4]
87800ae2:	f043 0302 	orr.w	r3, r3, #2
87800ae6:	6053      	str	r3, [r2, #4]
87800ae8:	6863      	ldr	r3, [r4, #4]
87800aea:	f043 0301 	orr.w	r3, r3, #1
87800aee:	6063      	str	r3, [r4, #4]
87800af0:	079b      	lsls	r3, r3, #30
87800af2:	d405      	bmi.n	87800b00 <tos_mmheap_free+0x34>
87800af4:	4620      	mov	r0, r4
87800af6:	f7ff fed1 	bl	8780089c <blk_merge_next>
87800afa:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
87800afe:	e639      	b.n	87800774 <blk_insert>
87800b00:	f850 5c08 	ldr.w	r5, [r0, #-8]
87800b04:	4628      	mov	r0, r5
87800b06:	f7ff feb7 	bl	87800878 <blk_remove>
87800b0a:	6862      	ldr	r2, [r4, #4]
87800b0c:	462c      	mov	r4, r5
87800b0e:	686b      	ldr	r3, [r5, #4]
87800b10:	4620      	mov	r0, r4
87800b12:	f022 0203 	bic.w	r2, r2, #3
87800b16:	3304      	adds	r3, #4
87800b18:	4413      	add	r3, r2
87800b1a:	f023 0203 	bic.w	r2, r3, #3
87800b1e:	606b      	str	r3, [r5, #4]
87800b20:	18ab      	adds	r3, r5, r2
87800b22:	605d      	str	r5, [r3, #4]
87800b24:	f7ff feba 	bl	8780089c <blk_merge_next>
87800b28:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
87800b2c:	e622      	b.n	87800774 <blk_insert>
87800b2e:	4770      	bx	lr

87800b30 <tos_mmheap_realloc>:
87800b30:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
87800b34:	fab1 f381 	clz	r3, r1
87800b38:	095b      	lsrs	r3, r3, #5
87800b3a:	2800      	cmp	r0, #0
87800b3c:	bf0c      	ite	eq
87800b3e:	2500      	moveq	r5, #0
87800b40:	f003 0501 	andne.w	r5, r3, #1
87800b44:	2d00      	cmp	r5, #0
87800b46:	d167      	bne.n	87800c18 <tos_mmheap_realloc+0xe8>
87800b48:	4688      	mov	r8, r1
87800b4a:	4604      	mov	r4, r0
87800b4c:	2800      	cmp	r0, #0
87800b4e:	d068      	beq.n	87800c22 <tos_mmheap_realloc+0xf2>
87800b50:	f1a0 0708 	sub.w	r7, r0, #8
87800b54:	687e      	ldr	r6, [r7, #4]
87800b56:	f1a0 0904 	sub.w	r9, r0, #4
87800b5a:	f026 0603 	bic.w	r6, r6, #3
87800b5e:	5980      	ldr	r0, [r0, r6]
87800b60:	bbb3      	cbnz	r3, 87800bd0 <tos_mmheap_realloc+0xa0>
87800b62:	1ccd      	adds	r5, r1, #3
87800b64:	f025 0503 	bic.w	r5, r5, #3
87800b68:	f1b5 4f80 	cmp.w	r5, #1073741824	; 0x40000000
87800b6c:	d82f      	bhi.n	87800bce <tos_mmheap_realloc+0x9e>
87800b6e:	2d0c      	cmp	r5, #12
87800b70:	bf38      	it	cc
87800b72:	250c      	movcc	r5, #12
87800b74:	42ae      	cmp	r6, r5
87800b76:	bf28      	it	cs
87800b78:	f105 0310 	addcs.w	r3, r5, #16
87800b7c:	d229      	bcs.n	87800bd2 <tos_mmheap_realloc+0xa2>
87800b7e:	f000 0201 	and.w	r2, r0, #1
87800b82:	f020 0303 	bic.w	r3, r0, #3
87800b86:	1d30      	adds	r0, r6, #4
87800b88:	f082 0201 	eor.w	r2, r2, #1
87800b8c:	4418      	add	r0, r3
87800b8e:	42a8      	cmp	r0, r5
87800b90:	bf2c      	ite	cs
87800b92:	4610      	movcs	r0, r2
87800b94:	f042 0001 	orrcc.w	r0, r2, #1
87800b98:	bb60      	cbnz	r0, 87800bf4 <tos_mmheap_realloc+0xc4>
87800b9a:	4638      	mov	r0, r7
87800b9c:	f7ff fe7e 	bl	8780089c <blk_merge_next>
87800ba0:	687a      	ldr	r2, [r7, #4]
87800ba2:	f105 0310 	add.w	r3, r5, #16
87800ba6:	f022 0203 	bic.w	r2, r2, #3
87800baa:	4491      	add	r9, r2
87800bac:	f8d9 2004 	ldr.w	r2, [r9, #4]
87800bb0:	f022 0202 	bic.w	r2, r2, #2
87800bb4:	f8c9 2004 	str.w	r2, [r9, #4]
87800bb8:	687e      	ldr	r6, [r7, #4]
87800bba:	f026 0201 	bic.w	r2, r6, #1
87800bbe:	f026 0603 	bic.w	r6, r6, #3
87800bc2:	429e      	cmp	r6, r3
87800bc4:	607a      	str	r2, [r7, #4]
87800bc6:	d206      	bcs.n	87800bd6 <tos_mmheap_realloc+0xa6>
87800bc8:	4620      	mov	r0, r4
87800bca:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87800bce:	461d      	mov	r5, r3
87800bd0:	2310      	movs	r3, #16
87800bd2:	429e      	cmp	r6, r3
87800bd4:	d3f8      	bcc.n	87800bc8 <tos_mmheap_realloc+0x98>
87800bd6:	4629      	mov	r1, r5
87800bd8:	4638      	mov	r0, r7
87800bda:	f7ff fd51 	bl	87800680 <blk_split>
87800bde:	6843      	ldr	r3, [r0, #4]
87800be0:	f023 0302 	bic.w	r3, r3, #2
87800be4:	6043      	str	r3, [r0, #4]
87800be6:	f7ff fe59 	bl	8780089c <blk_merge_next>
87800bea:	f7ff fdc3 	bl	87800774 <blk_insert>
87800bee:	4620      	mov	r0, r4
87800bf0:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87800bf4:	4608      	mov	r0, r1
87800bf6:	f7ff fec7 	bl	87800988 <tos_mmheap_alloc>
87800bfa:	4605      	mov	r5, r0
87800bfc:	b170      	cbz	r0, 87800c1c <tos_mmheap_realloc+0xec>
87800bfe:	4546      	cmp	r6, r8
87800c00:	4632      	mov	r2, r6
87800c02:	4621      	mov	r1, r4
87800c04:	bf28      	it	cs
87800c06:	4642      	movcs	r2, r8
87800c08:	f004 fc0a 	bl	87805420 <memcpy>
87800c0c:	4620      	mov	r0, r4
87800c0e:	f7ff ff5d 	bl	87800acc <tos_mmheap_free>
87800c12:	4628      	mov	r0, r5
87800c14:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87800c18:	f7ff ff58 	bl	87800acc <tos_mmheap_free>
87800c1c:	2000      	movs	r0, #0
87800c1e:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87800c22:	4608      	mov	r0, r1
87800c24:	e8bd 43f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
87800c28:	f7ff beae 	b.w	87800988 <tos_mmheap_alloc>

87800c2c <tos_mmheap_pool_add>:
87800c2c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87800c2e:	f242 4628 	movw	r6, #9256	; 0x2428
87800c32:	f2c8 7681 	movt	r6, #34689	; 0x8781
87800c36:	6835      	ldr	r5, [r6, #0]
87800c38:	2d03      	cmp	r5, #3
87800c3a:	d014      	beq.n	87800c66 <tos_mmheap_pool_add+0x3a>
87800c3c:	2d00      	cmp	r5, #0
87800c3e:	dd0c      	ble.n	87800c5a <tos_mmheap_pool_add+0x2e>
87800c40:	6873      	ldr	r3, [r6, #4]
87800c42:	4298      	cmp	r0, r3
87800c44:	d012      	beq.n	87800c6c <tos_mmheap_pool_add+0x40>
87800c46:	1d32      	adds	r2, r6, #4
87800c48:	2300      	movs	r3, #0
87800c4a:	e003      	b.n	87800c54 <tos_mmheap_pool_add+0x28>
87800c4c:	f852 4f04 	ldr.w	r4, [r2, #4]!
87800c50:	42a0      	cmp	r0, r4
87800c52:	d00b      	beq.n	87800c6c <tos_mmheap_pool_add+0x40>
87800c54:	3301      	adds	r3, #1
87800c56:	42ab      	cmp	r3, r5
87800c58:	d1f8      	bne.n	87800c4c <tos_mmheap_pool_add+0x20>
87800c5a:	f010 0503 	ands.w	r5, r0, #3
87800c5e:	d008      	beq.n	87800c72 <tos_mmheap_pool_add+0x46>
87800c60:	f44f 7048 	mov.w	r0, #800	; 0x320
87800c64:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800c66:	f240 3022 	movw	r0, #802	; 0x322
87800c6a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800c6c:	f240 3023 	movw	r0, #803	; 0x323
87800c70:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800c72:	f1a1 0308 	sub.w	r3, r1, #8
87800c76:	f64f 72f4 	movw	r2, #65524	; 0xfff4
87800c7a:	f023 0303 	bic.w	r3, r3, #3
87800c7e:	f6c3 72ff 	movt	r2, #16383	; 0x3fff
87800c82:	f1a3 010c 	sub.w	r1, r3, #12
87800c86:	4291      	cmp	r1, r2
87800c88:	d902      	bls.n	87800c90 <tos_mmheap_pool_add+0x64>
87800c8a:	f240 3021 	movw	r0, #801	; 0x321
87800c8e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800c90:	4607      	mov	r7, r0
87800c92:	f043 0301 	orr.w	r3, r3, #1
87800c96:	f857 2904 	ldr.w	r2, [r7], #-4
87800c9a:	4604      	mov	r4, r0
87800c9c:	607b      	str	r3, [r7, #4]
87800c9e:	4638      	mov	r0, r7
87800ca0:	f7ff fd68 	bl	87800774 <blk_insert>
87800ca4:	6832      	ldr	r2, [r6, #0]
87800ca6:	f04f 0e02 	mov.w	lr, #2
87800caa:	687b      	ldr	r3, [r7, #4]
87800cac:	4628      	mov	r0, r5
87800cae:	1c51      	adds	r1, r2, #1
87800cb0:	f023 0303 	bic.w	r3, r3, #3
87800cb4:	eb06 0282 	add.w	r2, r6, r2, lsl #2
87800cb8:	511f      	str	r7, [r3, r4]
87800cba:	4423      	add	r3, r4
87800cbc:	f8c3 e004 	str.w	lr, [r3, #4]
87800cc0:	6031      	str	r1, [r6, #0]
87800cc2:	6054      	str	r4, [r2, #4]
87800cc4:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87800cc6:	bf00      	nop

87800cc8 <mmheap_init_with_pool>:
87800cc8:	b538      	push	{r3, r4, r5, lr}
87800cca:	4604      	mov	r4, r0
87800ccc:	460d      	mov	r5, r1
87800cce:	f7ff fcff 	bl	878006d0 <mmheap_ctl_init>
87800cd2:	4629      	mov	r1, r5
87800cd4:	4620      	mov	r0, r4
87800cd6:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
87800cda:	f7ff bfa7 	b.w	87800c2c <tos_mmheap_pool_add>
87800cde:	bf00      	nop

87800ce0 <tos_mmheap_pool_rmv>:
87800ce0:	b570      	push	{r4, r5, r6, lr}
87800ce2:	2300      	movs	r3, #0
87800ce4:	b082      	sub	sp, #8
87800ce6:	9300      	str	r3, [sp, #0]
87800ce8:	9301      	str	r3, [sp, #4]
87800cea:	2800      	cmp	r0, #0
87800cec:	d045      	beq.n	87800d7a <tos_mmheap_pool_rmv+0x9a>
87800cee:	f242 4528 	movw	r5, #9256	; 0x2428
87800cf2:	f2c8 7581 	movt	r5, #34689	; 0x8781
87800cf6:	682c      	ldr	r4, [r5, #0]
87800cf8:	429c      	cmp	r4, r3
87800cfa:	dd0b      	ble.n	87800d14 <tos_mmheap_pool_rmv+0x34>
87800cfc:	686a      	ldr	r2, [r5, #4]
87800cfe:	4290      	cmp	r0, r2
87800d00:	d00c      	beq.n	87800d1c <tos_mmheap_pool_rmv+0x3c>
87800d02:	1d2a      	adds	r2, r5, #4
87800d04:	e003      	b.n	87800d0e <tos_mmheap_pool_rmv+0x2e>
87800d06:	f852 1f04 	ldr.w	r1, [r2, #4]!
87800d0a:	4288      	cmp	r0, r1
87800d0c:	d006      	beq.n	87800d1c <tos_mmheap_pool_rmv+0x3c>
87800d0e:	3301      	adds	r3, #1
87800d10:	42a3      	cmp	r3, r4
87800d12:	d1f8      	bne.n	87800d06 <tos_mmheap_pool_rmv+0x26>
87800d14:	f44f 7049 	mov.w	r0, #804	; 0x324
87800d18:	b002      	add	sp, #8
87800d1a:	bd70      	pop	{r4, r5, r6, pc}
87800d1c:	4606      	mov	r6, r0
87800d1e:	4604      	mov	r4, r0
87800d20:	f856 0904 	ldr.w	r0, [r6], #-4
87800d24:	aa01      	add	r2, sp, #4
87800d26:	4669      	mov	r1, sp
87800d28:	f020 0003 	bic.w	r0, r0, #3
87800d2c:	f7ff fd0e 	bl	8780074c <mapping_insert>
87800d30:	e89d 0006 	ldmia.w	sp, {r1, r2}
87800d34:	4630      	mov	r0, r6
87800d36:	f7ff fd73 	bl	87800820 <remove_free_block>
87800d3a:	6829      	ldr	r1, [r5, #0]
87800d3c:	f242 4228 	movw	r2, #9256	; 0x2428
87800d40:	f2c8 7281 	movt	r2, #34689	; 0x8781
87800d44:	2900      	cmp	r1, #0
87800d46:	dd1c      	ble.n	87800d82 <tos_mmheap_pool_rmv+0xa2>
87800d48:	6853      	ldr	r3, [r2, #4]
87800d4a:	429c      	cmp	r4, r3
87800d4c:	d019      	beq.n	87800d82 <tos_mmheap_pool_rmv+0xa2>
87800d4e:	3204      	adds	r2, #4
87800d50:	2300      	movs	r3, #0
87800d52:	3301      	adds	r3, #1
87800d54:	428b      	cmp	r3, r1
87800d56:	d003      	beq.n	87800d60 <tos_mmheap_pool_rmv+0x80>
87800d58:	f852 0f04 	ldr.w	r0, [r2, #4]!
87800d5c:	4284      	cmp	r4, r0
87800d5e:	d1f8      	bne.n	87800d52 <tos_mmheap_pool_rmv+0x72>
87800d60:	3901      	subs	r1, #1
87800d62:	2000      	movs	r0, #0
87800d64:	428b      	cmp	r3, r1
87800d66:	6029      	str	r1, [r5, #0]
87800d68:	bf1f      	itttt	ne
87800d6a:	eb05 0281 	addne.w	r2, r5, r1, lsl #2
87800d6e:	eb05 0383 	addne.w	r3, r5, r3, lsl #2
87800d72:	6852      	ldrne	r2, [r2, #4]
87800d74:	605a      	strne	r2, [r3, #4]
87800d76:	b002      	add	sp, #8
87800d78:	bd70      	pop	{r4, r5, r6, pc}
87800d7a:	f240 404c 	movw	r0, #1100	; 0x44c
87800d7e:	b002      	add	sp, #8
87800d80:	bd70      	pop	{r4, r5, r6, pc}
87800d82:	2300      	movs	r3, #0
87800d84:	e7ec      	b.n	87800d60 <tos_mmheap_pool_rmv+0x80>
87800d86:	bf00      	nop

87800d88 <tos_mmheap_pool_check>:
87800d88:	2900      	cmp	r1, #0
87800d8a:	bf18      	it	ne
87800d8c:	2800      	cmpne	r0, #0
87800d8e:	b538      	push	{r3, r4, r5, lr}
87800d90:	460d      	mov	r5, r1
87800d92:	bf0c      	ite	eq
87800d94:	2101      	moveq	r1, #1
87800d96:	2100      	movne	r1, #0
87800d98:	d01d      	beq.n	87800dd6 <tos_mmheap_pool_check+0x4e>
87800d9a:	4604      	mov	r4, r0
87800d9c:	2208      	movs	r2, #8
87800d9e:	4628      	mov	r0, r5
87800da0:	f004 fb2a 	bl	878053f8 <memset>
87800da4:	1f20      	subs	r0, r4, #4
87800da6:	d110      	bne.n	87800dca <tos_mmheap_pool_check+0x42>
87800da8:	e013      	b.n	87800dd2 <tos_mmheap_pool_check+0x4a>
87800daa:	07db      	lsls	r3, r3, #31
87800dac:	f100 0004 	add.w	r0, r0, #4
87800db0:	bf4b      	itete	mi
87800db2:	686b      	ldrmi	r3, [r5, #4]
87800db4:	682b      	ldrpl	r3, [r5, #0]
87800db6:	18d2      	addmi	r2, r2, r3
87800db8:	18d2      	addpl	r2, r2, r3
87800dba:	bf4c      	ite	mi
87800dbc:	606a      	strmi	r2, [r5, #4]
87800dbe:	602a      	strpl	r2, [r5, #0]
87800dc0:	6803      	ldr	r3, [r0, #0]
87800dc2:	f023 0303 	bic.w	r3, r3, #3
87800dc6:	18c0      	adds	r0, r0, r3
87800dc8:	d003      	beq.n	87800dd2 <tos_mmheap_pool_check+0x4a>
87800dca:	6843      	ldr	r3, [r0, #4]
87800dcc:	f033 0203 	bics.w	r2, r3, #3
87800dd0:	d1eb      	bne.n	87800daa <tos_mmheap_pool_check+0x22>
87800dd2:	2000      	movs	r0, #0
87800dd4:	bd38      	pop	{r3, r4, r5, pc}
87800dd6:	f240 404c 	movw	r0, #1100	; 0x44c
87800dda:	bd38      	pop	{r3, r4, r5, pc}

87800ddc <tos_mmheap_check>:
87800ddc:	b348      	cbz	r0, 87800e32 <tos_mmheap_check+0x56>
87800dde:	b5f0      	push	{r4, r5, r6, r7, lr}
87800de0:	f242 4628 	movw	r6, #9256	; 0x2428
87800de4:	f2c8 7681 	movt	r6, #34689	; 0x8781
87800de8:	b083      	sub	sp, #12
87800dea:	2208      	movs	r2, #8
87800dec:	2100      	movs	r1, #0
87800dee:	4604      	mov	r4, r0
87800df0:	f004 fb02 	bl	878053f8 <memset>
87800df4:	6833      	ldr	r3, [r6, #0]
87800df6:	2b00      	cmp	r3, #0
87800df8:	bfc4      	itt	gt
87800dfa:	4637      	movgt	r7, r6
87800dfc:	2500      	movgt	r5, #0
87800dfe:	dc0b      	bgt.n	87800e18 <tos_mmheap_check+0x3c>
87800e00:	e014      	b.n	87800e2c <tos_mmheap_check+0x50>
87800e02:	6860      	ldr	r0, [r4, #4]
87800e04:	9a01      	ldr	r2, [sp, #4]
87800e06:	6831      	ldr	r1, [r6, #0]
87800e08:	9b00      	ldr	r3, [sp, #0]
87800e0a:	4402      	add	r2, r0
87800e0c:	6062      	str	r2, [r4, #4]
87800e0e:	42a9      	cmp	r1, r5
87800e10:	6822      	ldr	r2, [r4, #0]
87800e12:	4413      	add	r3, r2
87800e14:	6023      	str	r3, [r4, #0]
87800e16:	dd09      	ble.n	87800e2c <tos_mmheap_check+0x50>
87800e18:	4669      	mov	r1, sp
87800e1a:	f857 0f04 	ldr.w	r0, [r7, #4]!
87800e1e:	3501      	adds	r5, #1
87800e20:	f7ff ffb2 	bl	87800d88 <tos_mmheap_pool_check>
87800e24:	2800      	cmp	r0, #0
87800e26:	d0ec      	beq.n	87800e02 <tos_mmheap_check+0x26>
87800e28:	b003      	add	sp, #12
87800e2a:	bdf0      	pop	{r4, r5, r6, r7, pc}
87800e2c:	2000      	movs	r0, #0
87800e2e:	b003      	add	sp, #12
87800e30:	bdf0      	pop	{r4, r5, r6, r7, pc}
87800e32:	f240 404c 	movw	r0, #1100	; 0x44c
87800e36:	4770      	bx	lr

87800e38 <prio_q_mgr_entry_cmp>:
87800e38:	7800      	ldrb	r0, [r0, #0]
87800e3a:	780b      	ldrb	r3, [r1, #0]
87800e3c:	4298      	cmp	r0, r3
87800e3e:	bf2c      	ite	cs
87800e40:	2000      	movcs	r0, #0
87800e42:	2001      	movcc	r0, #1
87800e44:	4770      	bx	lr
87800e46:	bf00      	nop

87800e48 <tos_prio_q_create>:
87800e48:	2a00      	cmp	r2, #0
87800e4a:	bf18      	it	ne
87800e4c:	2900      	cmpne	r1, #0
87800e4e:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87800e52:	bf0c      	ite	eq
87800e54:	2401      	moveq	r4, #1
87800e56:	2400      	movne	r4, #0
87800e58:	2800      	cmp	r0, #0
87800e5a:	bf08      	it	eq
87800e5c:	f044 0401 	orreq.w	r4, r4, #1
87800e60:	b082      	sub	sp, #8
87800e62:	bb8c      	cbnz	r4, 87800ec8 <tos_prio_q_create+0x80>
87800e64:	8104      	strh	r4, [r0, #8]
87800e66:	eb01 0743 	add.w	r7, r1, r3, lsl #1
87800e6a:	60c1      	str	r1, [r0, #12]
87800e6c:	4625      	mov	r5, r4
87800e6e:	b133      	cbz	r3, 87800e7e <tos_prio_q_create+0x36>
87800e70:	3401      	adds	r4, #1
87800e72:	b2a4      	uxth	r4, r4
87800e74:	42a3      	cmp	r3, r4
87800e76:	f821 4015 	strh.w	r4, [r1, r5, lsl #1]
87800e7a:	4625      	mov	r5, r4
87800e7c:	d8f8      	bhi.n	87800e70 <tos_prio_q_create+0x28>
87800e7e:	4616      	mov	r6, r2
87800e80:	f640 6239 	movw	r2, #3641	; 0xe39
87800e84:	4698      	mov	r8, r3
87800e86:	f2c8 7280 	movt	r2, #34688	; 0x8780
87800e8a:	f64f 73ff 	movw	r3, #65535	; 0xffff
87800e8e:	4605      	mov	r5, r0
87800e90:	f827 3c02 	strh.w	r3, [r7, #-2]
87800e94:	460c      	mov	r4, r1
87800e96:	62c7      	str	r7, [r0, #44]	; 0x2c
87800e98:	4639      	mov	r1, r7
87800e9a:	9200      	str	r2, [sp, #0]
87800e9c:	2304      	movs	r3, #4
87800e9e:	4642      	mov	r2, r8
87800ea0:	3010      	adds	r0, #16
87800ea2:	f000 fd4b 	bl	8780193c <tos_bin_heap_create>
87800ea6:	9808      	ldr	r0, [sp, #32]
87800ea8:	2300      	movs	r3, #0
87800eaa:	f64d 21d5 	movw	r1, #56021	; 0xdad5
87800eae:	2201      	movs	r2, #1
87800eb0:	f8c5 8038 	str.w	r8, [r5, #56]	; 0x38
87800eb4:	6368      	str	r0, [r5, #52]	; 0x34
87800eb6:	4618      	mov	r0, r3
87800eb8:	63ec      	str	r4, [r5, #60]	; 0x3c
87800eba:	642e      	str	r6, [r5, #64]	; 0x40
87800ebc:	632b      	str	r3, [r5, #48]	; 0x30
87800ebe:	6069      	str	r1, [r5, #4]
87800ec0:	602a      	str	r2, [r5, #0]
87800ec2:	b002      	add	sp, #8
87800ec4:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87800ec8:	f240 404c 	movw	r0, #1100	; 0x44c
87800ecc:	b002      	add	sp, #8
87800ece:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87800ed2:	bf00      	nop

87800ed4 <tos_prio_q_destroy>:
87800ed4:	b170      	cbz	r0, 87800ef4 <tos_prio_q_destroy+0x20>
87800ed6:	6842      	ldr	r2, [r0, #4]
87800ed8:	b538      	push	{r3, r4, r5, lr}
87800eda:	f64d 23d5 	movw	r3, #56021	; 0xdad5
87800ede:	429a      	cmp	r2, r3
87800ee0:	d002      	beq.n	87800ee8 <tos_prio_q_destroy+0x14>
87800ee2:	f240 404d 	movw	r0, #1101	; 0x44d
87800ee6:	bd38      	pop	{r3, r4, r5, pc}
87800ee8:	6803      	ldr	r3, [r0, #0]
87800eea:	2b01      	cmp	r3, #1
87800eec:	d005      	beq.n	87800efa <tos_prio_q_destroy+0x26>
87800eee:	f240 404e 	movw	r0, #1102	; 0x44e
87800ef2:	bd38      	pop	{r3, r4, r5, pc}
87800ef4:	f240 404c 	movw	r0, #1100	; 0x44c
87800ef8:	4770      	bx	lr
87800efa:	4604      	mov	r4, r0
87800efc:	2500      	movs	r5, #0
87800efe:	8125      	strh	r5, [r4, #8]
87800f00:	3010      	adds	r0, #16
87800f02:	60e5      	str	r5, [r4, #12]
87800f04:	62e5      	str	r5, [r4, #44]	; 0x2c
87800f06:	f000 fd39 	bl	8780197c <tos_bin_heap_destroy>
87800f0a:	6325      	str	r5, [r4, #48]	; 0x30
87800f0c:	4628      	mov	r0, r5
87800f0e:	6365      	str	r5, [r4, #52]	; 0x34
87800f10:	63a5      	str	r5, [r4, #56]	; 0x38
87800f12:	63e5      	str	r5, [r4, #60]	; 0x3c
87800f14:	6425      	str	r5, [r4, #64]	; 0x40
87800f16:	6065      	str	r5, [r4, #4]
87800f18:	6025      	str	r5, [r4, #0]
87800f1a:	bd38      	pop	{r3, r4, r5, pc}

87800f1c <tos_prio_q_create_dyn>:
87800f1c:	b388      	cbz	r0, 87800f82 <tos_prio_q_create_dyn+0x66>
87800f1e:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87800f22:	4606      	mov	r6, r0
87800f24:	00c8      	lsls	r0, r1, #3
87800f26:	b082      	sub	sp, #8
87800f28:	4615      	mov	r5, r2
87800f2a:	eba0 0041 	sub.w	r0, r0, r1, lsl #1
87800f2e:	460c      	mov	r4, r1
87800f30:	f7ff fd2a 	bl	87800988 <tos_mmheap_alloc>
87800f34:	4607      	mov	r7, r0
87800f36:	f240 407e 	movw	r0, #1150	; 0x47e
87800f3a:	b187      	cbz	r7, 87800f5e <tos_prio_q_create_dyn+0x42>
87800f3c:	fb05 f004 	mul.w	r0, r5, r4
87800f40:	f7ff fd22 	bl	87800988 <tos_mmheap_alloc>
87800f44:	4680      	mov	r8, r0
87800f46:	b168      	cbz	r0, 87800f64 <tos_prio_q_create_dyn+0x48>
87800f48:	4602      	mov	r2, r0
87800f4a:	9500      	str	r5, [sp, #0]
87800f4c:	4623      	mov	r3, r4
87800f4e:	4639      	mov	r1, r7
87800f50:	4630      	mov	r0, r6
87800f52:	f7ff ff79 	bl	87800e48 <tos_prio_q_create>
87800f56:	b968      	cbnz	r0, 87800f74 <tos_prio_q_create_dyn+0x58>
87800f58:	2000      	movs	r0, #0
87800f5a:	2302      	movs	r3, #2
87800f5c:	6033      	str	r3, [r6, #0]
87800f5e:	b002      	add	sp, #8
87800f60:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87800f64:	4638      	mov	r0, r7
87800f66:	f7ff fdb1 	bl	87800acc <tos_mmheap_free>
87800f6a:	f240 407e 	movw	r0, #1150	; 0x47e
87800f6e:	b002      	add	sp, #8
87800f70:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87800f74:	4640      	mov	r0, r8
87800f76:	f7ff fda9 	bl	87800acc <tos_mmheap_free>
87800f7a:	4638      	mov	r0, r7
87800f7c:	f7ff fda6 	bl	87800acc <tos_mmheap_free>
87800f80:	e7ea      	b.n	87800f58 <tos_prio_q_create_dyn+0x3c>
87800f82:	f240 404c 	movw	r0, #1100	; 0x44c
87800f86:	4770      	bx	lr

87800f88 <tos_prio_q_destroy_dyn>:
87800f88:	b170      	cbz	r0, 87800fa8 <tos_prio_q_destroy_dyn+0x20>
87800f8a:	6842      	ldr	r2, [r0, #4]
87800f8c:	b538      	push	{r3, r4, r5, lr}
87800f8e:	f64d 23d5 	movw	r3, #56021	; 0xdad5
87800f92:	429a      	cmp	r2, r3
87800f94:	d002      	beq.n	87800f9c <tos_prio_q_destroy_dyn+0x14>
87800f96:	f240 404d 	movw	r0, #1101	; 0x44d
87800f9a:	bd38      	pop	{r3, r4, r5, pc}
87800f9c:	6803      	ldr	r3, [r0, #0]
87800f9e:	2b02      	cmp	r3, #2
87800fa0:	d005      	beq.n	87800fae <tos_prio_q_destroy_dyn+0x26>
87800fa2:	f240 404e 	movw	r0, #1102	; 0x44e
87800fa6:	bd38      	pop	{r3, r4, r5, pc}
87800fa8:	f240 404c 	movw	r0, #1100	; 0x44c
87800fac:	4770      	bx	lr
87800fae:	4604      	mov	r4, r0
87800fb0:	2500      	movs	r5, #0
87800fb2:	8125      	strh	r5, [r4, #8]
87800fb4:	3010      	adds	r0, #16
87800fb6:	60e5      	str	r5, [r4, #12]
87800fb8:	62e5      	str	r5, [r4, #44]	; 0x2c
87800fba:	f000 fcdf 	bl	8780197c <tos_bin_heap_destroy>
87800fbe:	6be0      	ldr	r0, [r4, #60]	; 0x3c
87800fc0:	f7ff fd84 	bl	87800acc <tos_mmheap_free>
87800fc4:	6c20      	ldr	r0, [r4, #64]	; 0x40
87800fc6:	f7ff fd81 	bl	87800acc <tos_mmheap_free>
87800fca:	6325      	str	r5, [r4, #48]	; 0x30
87800fcc:	4628      	mov	r0, r5
87800fce:	6365      	str	r5, [r4, #52]	; 0x34
87800fd0:	63a5      	str	r5, [r4, #56]	; 0x38
87800fd2:	63e5      	str	r5, [r4, #60]	; 0x3c
87800fd4:	6425      	str	r5, [r4, #64]	; 0x40
87800fd6:	6065      	str	r5, [r4, #4]
87800fd8:	6025      	str	r5, [r4, #0]
87800fda:	bd38      	pop	{r3, r4, r5, pc}

87800fdc <tos_prio_q_enqueue>:
87800fdc:	2900      	cmp	r1, #0
87800fde:	bf18      	it	ne
87800fe0:	2800      	cmpne	r0, #0
87800fe2:	b5f0      	push	{r4, r5, r6, r7, lr}
87800fe4:	4604      	mov	r4, r0
87800fe6:	b085      	sub	sp, #20
87800fe8:	d044      	beq.n	87801074 <tos_prio_q_enqueue+0x98>
87800fea:	6845      	ldr	r5, [r0, #4]
87800fec:	f64d 20d5 	movw	r0, #56021	; 0xdad5
87800ff0:	4285      	cmp	r5, r0
87800ff2:	bf18      	it	ne
87800ff4:	f240 404d 	movwne	r0, #1101	; 0x44d
87800ff8:	d001      	beq.n	87800ffe <tos_prio_q_enqueue+0x22>
87800ffa:	b005      	add	sp, #20
87800ffc:	bdf0      	pop	{r4, r5, r6, r7, pc}
87800ffe:	6b60      	ldr	r0, [r4, #52]	; 0x34
87801000:	4290      	cmp	r0, r2
87801002:	bf18      	it	ne
87801004:	f240 507b 	movwne	r0, #1403	; 0x57b
87801008:	d1f7      	bne.n	87800ffa <tos_prio_q_enqueue+0x1e>
8780100a:	e88d 000a 	stmia.w	sp, {r1, r3}
8780100e:	f003 fe27 	bl	87804c60 <tos_cpu_cpsr_save>
87801012:	6b26      	ldr	r6, [r4, #48]	; 0x30
87801014:	6ba5      	ldr	r5, [r4, #56]	; 0x38
87801016:	f003 fe25 	bl	87804c64 <tos_cpu_cpsr_restore>
8780101a:	42ae      	cmp	r6, r5
8780101c:	bf08      	it	eq
8780101e:	f240 5079 	movweq	r0, #1401	; 0x579
87801022:	d0ea      	beq.n	87800ffa <tos_prio_q_enqueue+0x1e>
87801024:	f003 fe1c 	bl	87804c60 <tos_cpu_cpsr_save>
87801028:	8925      	ldrh	r5, [r4, #8]
8780102a:	f64f 72ff 	movw	r2, #65535	; 0xffff
8780102e:	4606      	mov	r6, r0
87801030:	4295      	cmp	r5, r2
87801032:	d023      	beq.n	8780107c <tos_prio_q_enqueue+0xa0>
87801034:	68e7      	ldr	r7, [r4, #12]
87801036:	6b62      	ldr	r2, [r4, #52]	; 0x34
87801038:	6c20      	ldr	r0, [r4, #64]	; 0x40
8780103a:	f837 7015 	ldrh.w	r7, [r7, r5, lsl #1]
8780103e:	9900      	ldr	r1, [sp, #0]
87801040:	fb05 0002 	mla	r0, r5, r2, r0
87801044:	8127      	strh	r7, [r4, #8]
87801046:	f004 f9eb 	bl	87805420 <memcpy>
8780104a:	9b01      	ldr	r3, [sp, #4]
8780104c:	2204      	movs	r2, #4
8780104e:	a903      	add	r1, sp, #12
87801050:	f104 0010 	add.w	r0, r4, #16
87801054:	f8ad 500e 	strh.w	r5, [sp, #14]
87801058:	f88d 300c 	strb.w	r3, [sp, #12]
8780105c:	f000 fcf8 	bl	87801a50 <tos_bin_heap_push>
87801060:	4605      	mov	r5, r0
87801062:	b9f8      	cbnz	r0, 878010a4 <tos_prio_q_enqueue+0xc8>
87801064:	6b23      	ldr	r3, [r4, #48]	; 0x30
87801066:	4630      	mov	r0, r6
87801068:	3301      	adds	r3, #1
8780106a:	6323      	str	r3, [r4, #48]	; 0x30
8780106c:	f003 fdfa 	bl	87804c64 <tos_cpu_cpsr_restore>
87801070:	4628      	mov	r0, r5
87801072:	e7c2      	b.n	87800ffa <tos_prio_q_enqueue+0x1e>
87801074:	f240 404c 	movw	r0, #1100	; 0x44c
87801078:	b005      	add	sp, #20
8780107a:	bdf0      	pop	{r4, r5, r6, r7, pc}
8780107c:	f64d 4030 	movw	r0, #56368	; 0xdc30
87801080:	f240 1227 	movw	r2, #295	; 0x127
87801084:	4912      	ldr	r1, [pc, #72]	; (878010d0 <tos_prio_q_enqueue+0xf4>)
87801086:	f2c8 7080 	movt	r0, #34688	; 0x8780
8780108a:	f003 ffdf 	bl	8780504c <printf>
8780108e:	f64d 4044 	movw	r0, #56388	; 0xdc44
87801092:	f2c8 7080 	movt	r0, #34688	; 0x8780
87801096:	f003 ffd9 	bl	8780504c <printf>
8780109a:	f002 ffef 	bl	8780407c <tos_knl_sched_lock>
8780109e:	f003 fddb 	bl	87804c58 <tos_cpu_int_disable>
878010a2:	e7fe      	b.n	878010a2 <tos_prio_q_enqueue+0xc6>
878010a4:	f249 4120 	movw	r1, #37920	; 0x9420
878010a8:	f64d 4030 	movw	r0, #56368	; 0xdc30
878010ac:	f2c8 7180 	movt	r1, #34688	; 0x8780
878010b0:	228d      	movs	r2, #141	; 0x8d
878010b2:	f2c8 7080 	movt	r0, #34688	; 0x8780
878010b6:	f003 ffc9 	bl	8780504c <printf>
878010ba:	f64d 4044 	movw	r0, #56388	; 0xdc44
878010be:	f2c8 7080 	movt	r0, #34688	; 0x8780
878010c2:	f003 ffc3 	bl	8780504c <printf>
878010c6:	f002 ffd9 	bl	8780407c <tos_knl_sched_lock>
878010ca:	f003 fdc5 	bl	87804c58 <tos_cpu_int_disable>
878010ce:	e7fe      	b.n	878010ce <tos_prio_q_enqueue+0xf2>
878010d0:	87809440 	strhi	r9, [r0, r0, asr #8]

878010d4 <tos_prio_q_dequeue>:
878010d4:	2900      	cmp	r1, #0
878010d6:	bf18      	it	ne
878010d8:	2800      	cmpne	r0, #0
878010da:	d00d      	beq.n	878010f8 <tos_prio_q_dequeue+0x24>
878010dc:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
878010e0:	f64d 24d5 	movw	r4, #56021	; 0xdad5
878010e4:	6845      	ldr	r5, [r0, #4]
878010e6:	b082      	sub	sp, #8
878010e8:	42a5      	cmp	r5, r4
878010ea:	bf18      	it	ne
878010ec:	f240 404d 	movwne	r0, #1101	; 0x44d
878010f0:	d005      	beq.n	878010fe <tos_prio_q_dequeue+0x2a>
878010f2:	b002      	add	sp, #8
878010f4:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878010f8:	f240 404c 	movw	r0, #1100	; 0x44c
878010fc:	4770      	bx	lr
878010fe:	4604      	mov	r4, r0
87801100:	460d      	mov	r5, r1
87801102:	461f      	mov	r7, r3
87801104:	4616      	mov	r6, r2
87801106:	f003 fdab 	bl	87804c60 <tos_cpu_cpsr_save>
8780110a:	f8d4 8030 	ldr.w	r8, [r4, #48]	; 0x30
8780110e:	f003 fda9 	bl	87804c64 <tos_cpu_cpsr_restore>
87801112:	f44f 60af 	mov.w	r0, #1400	; 0x578
87801116:	f1b8 0f00 	cmp.w	r8, #0
8780111a:	d0ea      	beq.n	878010f2 <tos_prio_q_dequeue+0x1e>
8780111c:	f003 fda0 	bl	87804c60 <tos_cpu_cpsr_save>
87801120:	466a      	mov	r2, sp
87801122:	4680      	mov	r8, r0
87801124:	a901      	add	r1, sp, #4
87801126:	f104 0010 	add.w	r0, r4, #16
8780112a:	f000 fcd5 	bl	87801ad8 <tos_bin_heap_pop>
8780112e:	bb90      	cbnz	r0, 87801196 <tos_prio_q_dequeue+0xc2>
87801130:	9b00      	ldr	r3, [sp, #0]
87801132:	2b04      	cmp	r3, #4
87801134:	d012      	beq.n	8780115c <tos_prio_q_dequeue+0x88>
87801136:	f64d 4030 	movw	r0, #56368	; 0xdc30
8780113a:	2298      	movs	r2, #152	; 0x98
8780113c:	491f      	ldr	r1, [pc, #124]	; (878011bc <tos_prio_q_dequeue+0xe8>)
8780113e:	f2c8 7080 	movt	r0, #34688	; 0x8780
87801142:	f003 ff83 	bl	8780504c <printf>
87801146:	f64d 4044 	movw	r0, #56388	; 0xdc44
8780114a:	f2c8 7080 	movt	r0, #34688	; 0x8780
8780114e:	f003 ff7d 	bl	8780504c <printf>
87801152:	f002 ff93 	bl	8780407c <tos_knl_sched_lock>
87801156:	f003 fd7f 	bl	87804c58 <tos_cpu_int_disable>
8780115a:	e7fe      	b.n	8780115a <tos_prio_q_dequeue+0x86>
8780115c:	b117      	cbz	r7, 87801164 <tos_prio_q_dequeue+0x90>
8780115e:	f89d 3004 	ldrb.w	r3, [sp, #4]
87801162:	703b      	strb	r3, [r7, #0]
87801164:	f8bd 3006 	ldrh.w	r3, [sp, #6]
87801168:	4628      	mov	r0, r5
8780116a:	6b62      	ldr	r2, [r4, #52]	; 0x34
8780116c:	6c21      	ldr	r1, [r4, #64]	; 0x40
8780116e:	8927      	ldrh	r7, [r4, #8]
87801170:	68e5      	ldr	r5, [r4, #12]
87801172:	fb03 1102 	mla	r1, r3, r2, r1
87801176:	f825 7013 	strh.w	r7, [r5, r3, lsl #1]
8780117a:	8123      	strh	r3, [r4, #8]
8780117c:	f004 f950 	bl	87805420 <memcpy>
87801180:	b10e      	cbz	r6, 87801186 <tos_prio_q_dequeue+0xb2>
87801182:	6b63      	ldr	r3, [r4, #52]	; 0x34
87801184:	6033      	str	r3, [r6, #0]
87801186:	6b23      	ldr	r3, [r4, #48]	; 0x30
87801188:	4640      	mov	r0, r8
8780118a:	3b01      	subs	r3, #1
8780118c:	6323      	str	r3, [r4, #48]	; 0x30
8780118e:	f003 fd69 	bl	87804c64 <tos_cpu_cpsr_restore>
87801192:	2000      	movs	r0, #0
87801194:	e7ad      	b.n	878010f2 <tos_prio_q_dequeue+0x1e>
87801196:	f64d 4030 	movw	r0, #56368	; 0xdc30
8780119a:	2297      	movs	r2, #151	; 0x97
8780119c:	4907      	ldr	r1, [pc, #28]	; (878011bc <tos_prio_q_dequeue+0xe8>)
8780119e:	f2c8 7080 	movt	r0, #34688	; 0x8780
878011a2:	f003 ff53 	bl	8780504c <printf>
878011a6:	f64d 4044 	movw	r0, #56388	; 0xdc44
878011aa:	f2c8 7080 	movt	r0, #34688	; 0x8780
878011ae:	f003 ff4d 	bl	8780504c <printf>
878011b2:	f002 ff63 	bl	8780407c <tos_knl_sched_lock>
878011b6:	f003 fd4f 	bl	87804c58 <tos_cpu_int_disable>
878011ba:	e7fe      	b.n	878011ba <tos_prio_q_dequeue+0xe6>
878011bc:	87809454 			; <UNDEFINED> instruction: 0x87809454

878011c0 <tos_prio_q_flush>:
878011c0:	b138      	cbz	r0, 878011d2 <tos_prio_q_flush+0x12>
878011c2:	6842      	ldr	r2, [r0, #4]
878011c4:	f64d 23d5 	movw	r3, #56021	; 0xdad5
878011c8:	429a      	cmp	r2, r3
878011ca:	d005      	beq.n	878011d8 <tos_prio_q_flush+0x18>
878011cc:	f240 404d 	movw	r0, #1101	; 0x44d
878011d0:	4770      	bx	lr
878011d2:	f240 404c 	movw	r0, #1100	; 0x44c
878011d6:	4770      	bx	lr
878011d8:	b570      	push	{r4, r5, r6, lr}
878011da:	4604      	mov	r4, r0
878011dc:	f003 fd40 	bl	87804c60 <tos_cpu_cpsr_save>
878011e0:	2300      	movs	r3, #0
878011e2:	4606      	mov	r6, r0
878011e4:	6ba1      	ldr	r1, [r4, #56]	; 0x38
878011e6:	461a      	mov	r2, r3
878011e8:	8123      	strh	r3, [r4, #8]
878011ea:	68e5      	ldr	r5, [r4, #12]
878011ec:	b131      	cbz	r1, 878011fc <tos_prio_q_flush+0x3c>
878011ee:	3301      	adds	r3, #1
878011f0:	b29b      	uxth	r3, r3
878011f2:	4299      	cmp	r1, r3
878011f4:	f825 3012 	strh.w	r3, [r5, r2, lsl #1]
878011f8:	461a      	mov	r2, r3
878011fa:	d8f8      	bhi.n	878011ee <tos_prio_q_flush+0x2e>
878011fc:	f101 4100 	add.w	r1, r1, #2147483648	; 0x80000000
87801200:	f64f 73ff 	movw	r3, #65535	; 0xffff
87801204:	3901      	subs	r1, #1
87801206:	f104 0010 	add.w	r0, r4, #16
8780120a:	f825 3011 	strh.w	r3, [r5, r1, lsl #1]
8780120e:	2500      	movs	r5, #0
87801210:	f000 fcea 	bl	87801be8 <tos_bin_heap_flush>
87801214:	4630      	mov	r0, r6
87801216:	6325      	str	r5, [r4, #48]	; 0x30
87801218:	f003 fd24 	bl	87804c64 <tos_cpu_cpsr_restore>
8780121c:	4628      	mov	r0, r5
8780121e:	bd70      	pop	{r4, r5, r6, pc}

87801220 <tos_prio_q_is_empty>:
87801220:	b130      	cbz	r0, 87801230 <tos_prio_q_is_empty+0x10>
87801222:	6842      	ldr	r2, [r0, #4]
87801224:	f64d 23d5 	movw	r3, #56021	; 0xdad5
87801228:	429a      	cmp	r2, r3
8780122a:	d002      	beq.n	87801232 <tos_prio_q_is_empty+0x12>
8780122c:	2000      	movs	r0, #0
8780122e:	4770      	bx	lr
87801230:	4770      	bx	lr
87801232:	b510      	push	{r4, lr}
87801234:	4604      	mov	r4, r0
87801236:	f003 fd13 	bl	87804c60 <tos_cpu_cpsr_save>
8780123a:	6b24      	ldr	r4, [r4, #48]	; 0x30
8780123c:	f003 fd12 	bl	87804c64 <tos_cpu_cpsr_restore>
87801240:	fab4 f484 	clz	r4, r4
87801244:	0964      	lsrs	r4, r4, #5
87801246:	4620      	mov	r0, r4
87801248:	bd10      	pop	{r4, pc}
8780124a:	bf00      	nop

8780124c <tos_prio_q_is_full>:
8780124c:	b130      	cbz	r0, 8780125c <tos_prio_q_is_full+0x10>
8780124e:	6842      	ldr	r2, [r0, #4]
87801250:	f64d 23d5 	movw	r3, #56021	; 0xdad5
87801254:	429a      	cmp	r2, r3
87801256:	d002      	beq.n	8780125e <tos_prio_q_is_full+0x12>
87801258:	2000      	movs	r0, #0
8780125a:	4770      	bx	lr
8780125c:	4770      	bx	lr
8780125e:	b510      	push	{r4, lr}
87801260:	4604      	mov	r4, r0
87801262:	f003 fcfd 	bl	87804c60 <tos_cpu_cpsr_save>
87801266:	6b22      	ldr	r2, [r4, #48]	; 0x30
87801268:	6ba3      	ldr	r3, [r4, #56]	; 0x38
8780126a:	1ad4      	subs	r4, r2, r3
8780126c:	fab4 f484 	clz	r4, r4
87801270:	0964      	lsrs	r4, r4, #5
87801272:	f003 fcf7 	bl	87804c64 <tos_cpu_cpsr_restore>
87801276:	4620      	mov	r0, r4
87801278:	bd10      	pop	{r4, pc}
8780127a:	bf00      	nop

8780127c <mail_q_do_post.part.8>:
8780127c:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
87801280:	f100 0708 	add.w	r7, r0, #8
87801284:	4604      	mov	r4, r0
87801286:	4688      	mov	r8, r1
87801288:	4616      	mov	r6, r2
8780128a:	461d      	mov	r5, r3
8780128c:	f003 fce8 	bl	87804c60 <tos_cpu_cpsr_save>
87801290:	4681      	mov	r9, r0
87801292:	4638      	mov	r0, r7
87801294:	f003 f848 	bl	87804328 <pend_is_nopending>
87801298:	bb10      	cbnz	r0, 878012e0 <mail_q_do_post.part.8+0x64>
8780129a:	b375      	cbz	r5, 878012fa <mail_q_do_post.part.8+0x7e>
8780129c:	68a3      	ldr	r3, [r4, #8]
8780129e:	681c      	ldr	r4, [r3, #0]
878012a0:	429f      	cmp	r7, r3
878012a2:	f1a3 0548 	sub.w	r5, r3, #72	; 0x48
878012a6:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
878012aa:	d011      	beq.n	878012d0 <mail_q_do_post.part.8+0x54>
878012ac:	4632      	mov	r2, r6
878012ae:	4641      	mov	r1, r8
878012b0:	6ea8      	ldr	r0, [r5, #104]	; 0x68
878012b2:	f004 f8b5 	bl	87805420 <memcpy>
878012b6:	66ee      	str	r6, [r5, #108]	; 0x6c
878012b8:	4628      	mov	r0, r5
878012ba:	2101      	movs	r1, #1
878012bc:	4625      	mov	r5, r4
878012be:	f003 f85b 	bl	87804378 <pend_task_wakeup>
878012c2:	4623      	mov	r3, r4
878012c4:	f853 4f48 	ldr.w	r4, [r3, #72]!
878012c8:	429f      	cmp	r7, r3
878012ca:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
878012ce:	d1ed      	bne.n	878012ac <mail_q_do_post.part.8+0x30>
878012d0:	4648      	mov	r0, r9
878012d2:	f003 fcc7 	bl	87804c64 <tos_cpu_cpsr_restore>
878012d6:	f002 ff23 	bl	87804120 <knl_sched>
878012da:	2000      	movs	r0, #0
878012dc:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
878012e0:	4632      	mov	r2, r6
878012e2:	4641      	mov	r1, r8
878012e4:	f104 0010 	add.w	r0, r4, #16
878012e8:	f003 fa1e 	bl	87804728 <tos_ring_q_enqueue>
878012ec:	4604      	mov	r4, r0
878012ee:	4648      	mov	r0, r9
878012f0:	f003 fcb8 	bl	87804c64 <tos_cpu_cpsr_restore>
878012f4:	4620      	mov	r0, r4
878012f6:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
878012fa:	68a4      	ldr	r4, [r4, #8]
878012fc:	4641      	mov	r1, r8
878012fe:	4632      	mov	r2, r6
87801300:	6a20      	ldr	r0, [r4, #32]
87801302:	f004 f88d 	bl	87805420 <memcpy>
87801306:	6266      	str	r6, [r4, #36]	; 0x24
87801308:	f1a4 0048 	sub.w	r0, r4, #72	; 0x48
8780130c:	2101      	movs	r1, #1
8780130e:	f003 f833 	bl	87804378 <pend_task_wakeup>
87801312:	e7dd      	b.n	878012d0 <mail_q_do_post.part.8+0x54>

87801314 <tos_mail_q_create>:
87801314:	b190      	cbz	r0, 8780133c <tos_mail_q_create+0x28>
87801316:	b538      	push	{r3, r4, r5, lr}
87801318:	4604      	mov	r4, r0
8780131a:	3010      	adds	r0, #16
8780131c:	f003 f98a 	bl	87804634 <tos_ring_q_create>
87801320:	4605      	mov	r5, r0
87801322:	b100      	cbz	r0, 87801326 <tos_mail_q_create+0x12>
87801324:	bd38      	pop	{r3, r4, r5, pc}
87801326:	f104 0008 	add.w	r0, r4, #8
8780132a:	f002 fff5 	bl	87804318 <pend_object_init>
8780132e:	f644 32ee 	movw	r2, #19438	; 0x4bee
87801332:	2301      	movs	r3, #1
87801334:	4628      	mov	r0, r5
87801336:	6062      	str	r2, [r4, #4]
87801338:	6023      	str	r3, [r4, #0]
8780133a:	bd38      	pop	{r3, r4, r5, pc}
8780133c:	f240 404c 	movw	r0, #1100	; 0x44c
87801340:	4770      	bx	lr
87801342:	bf00      	nop

87801344 <tos_mail_q_destroy>:
87801344:	b170      	cbz	r0, 87801364 <tos_mail_q_destroy+0x20>
87801346:	6842      	ldr	r2, [r0, #4]
87801348:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
8780134a:	f644 33ee 	movw	r3, #19438	; 0x4bee
8780134e:	429a      	cmp	r2, r3
87801350:	d002      	beq.n	87801358 <tos_mail_q_destroy+0x14>
87801352:	f240 404d 	movw	r0, #1101	; 0x44d
87801356:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87801358:	6803      	ldr	r3, [r0, #0]
8780135a:	2b01      	cmp	r3, #1
8780135c:	d005      	beq.n	8780136a <tos_mail_q_destroy+0x26>
8780135e:	f240 404e 	movw	r0, #1102	; 0x44e
87801362:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87801364:	f240 404c 	movw	r0, #1100	; 0x44c
87801368:	4770      	bx	lr
8780136a:	4604      	mov	r4, r0
8780136c:	f003 fc78 	bl	87804c60 <tos_cpu_cpsr_save>
87801370:	4606      	mov	r6, r0
87801372:	f104 0010 	add.w	r0, r4, #16
87801376:	f003 f979 	bl	8780466c <tos_ring_q_destroy>
8780137a:	4605      	mov	r5, r0
8780137c:	b988      	cbnz	r0, 878013a2 <tos_mail_q_destroy+0x5e>
8780137e:	f104 0708 	add.w	r7, r4, #8
87801382:	2103      	movs	r1, #3
87801384:	4638      	mov	r0, r7
87801386:	f003 f83b 	bl	87804400 <pend_wakeup_all>
8780138a:	4638      	mov	r0, r7
8780138c:	f002 ffc8 	bl	87804320 <pend_object_deinit>
87801390:	4630      	mov	r0, r6
87801392:	6065      	str	r5, [r4, #4]
87801394:	6025      	str	r5, [r4, #0]
87801396:	f003 fc65 	bl	87804c64 <tos_cpu_cpsr_restore>
8780139a:	f002 fec1 	bl	87804120 <knl_sched>
8780139e:	4628      	mov	r0, r5
878013a0:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878013a2:	4630      	mov	r0, r6
878013a4:	f003 fc5e 	bl	87804c64 <tos_cpu_cpsr_restore>
878013a8:	4628      	mov	r0, r5
878013aa:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

878013ac <tos_mail_q_create_dyn>:
878013ac:	b190      	cbz	r0, 878013d4 <tos_mail_q_create_dyn+0x28>
878013ae:	b538      	push	{r3, r4, r5, lr}
878013b0:	4604      	mov	r4, r0
878013b2:	3010      	adds	r0, #16
878013b4:	f003 f978 	bl	878046a8 <tos_ring_q_create_dyn>
878013b8:	4605      	mov	r5, r0
878013ba:	b100      	cbz	r0, 878013be <tos_mail_q_create_dyn+0x12>
878013bc:	bd38      	pop	{r3, r4, r5, pc}
878013be:	f104 0008 	add.w	r0, r4, #8
878013c2:	f002 ffa9 	bl	87804318 <pend_object_init>
878013c6:	f644 32ee 	movw	r2, #19438	; 0x4bee
878013ca:	2302      	movs	r3, #2
878013cc:	4628      	mov	r0, r5
878013ce:	6062      	str	r2, [r4, #4]
878013d0:	6023      	str	r3, [r4, #0]
878013d2:	bd38      	pop	{r3, r4, r5, pc}
878013d4:	f240 404c 	movw	r0, #1100	; 0x44c
878013d8:	4770      	bx	lr
878013da:	bf00      	nop

878013dc <tos_mail_q_destroy_dyn>:
878013dc:	b170      	cbz	r0, 878013fc <tos_mail_q_destroy_dyn+0x20>
878013de:	6842      	ldr	r2, [r0, #4]
878013e0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
878013e2:	f644 33ee 	movw	r3, #19438	; 0x4bee
878013e6:	429a      	cmp	r2, r3
878013e8:	d002      	beq.n	878013f0 <tos_mail_q_destroy_dyn+0x14>
878013ea:	f240 404d 	movw	r0, #1101	; 0x44d
878013ee:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878013f0:	6803      	ldr	r3, [r0, #0]
878013f2:	2b02      	cmp	r3, #2
878013f4:	d005      	beq.n	87801402 <tos_mail_q_destroy_dyn+0x26>
878013f6:	f240 404e 	movw	r0, #1102	; 0x44e
878013fa:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878013fc:	f240 404c 	movw	r0, #1100	; 0x44c
87801400:	4770      	bx	lr
87801402:	4604      	mov	r4, r0
87801404:	f003 fc2c 	bl	87804c60 <tos_cpu_cpsr_save>
87801408:	4606      	mov	r6, r0
8780140a:	f104 0010 	add.w	r0, r4, #16
8780140e:	f003 f969 	bl	878046e4 <tos_ring_q_destroy_dyn>
87801412:	4605      	mov	r5, r0
87801414:	b988      	cbnz	r0, 8780143a <tos_mail_q_destroy_dyn+0x5e>
87801416:	f104 0708 	add.w	r7, r4, #8
8780141a:	2103      	movs	r1, #3
8780141c:	4638      	mov	r0, r7
8780141e:	f002 ffef 	bl	87804400 <pend_wakeup_all>
87801422:	4638      	mov	r0, r7
87801424:	f002 ff7c 	bl	87804320 <pend_object_deinit>
87801428:	4630      	mov	r0, r6
8780142a:	6065      	str	r5, [r4, #4]
8780142c:	6025      	str	r5, [r4, #0]
8780142e:	f003 fc19 	bl	87804c64 <tos_cpu_cpsr_restore>
87801432:	f002 fe75 	bl	87804120 <knl_sched>
87801436:	4628      	mov	r0, r5
87801438:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
8780143a:	4630      	mov	r0, r6
8780143c:	f003 fc12 	bl	87804c64 <tos_cpu_cpsr_restore>
87801440:	4628      	mov	r0, r5
87801442:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87801444 <tos_mail_q_flush>:
87801444:	b138      	cbz	r0, 87801456 <tos_mail_q_flush+0x12>
87801446:	6842      	ldr	r2, [r0, #4]
87801448:	f644 33ee 	movw	r3, #19438	; 0x4bee
8780144c:	429a      	cmp	r2, r3
8780144e:	d005      	beq.n	8780145c <tos_mail_q_flush+0x18>
87801450:	f240 404d 	movw	r0, #1101	; 0x44d
87801454:	4770      	bx	lr
87801456:	f240 404c 	movw	r0, #1100	; 0x44c
8780145a:	4770      	bx	lr
8780145c:	3010      	adds	r0, #16
8780145e:	f003 b9ef 	b.w	87804840 <tos_ring_q_flush>
87801462:	bf00      	nop

87801464 <tos_mail_q_pend>:
87801464:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
87801468:	4604      	mov	r4, r0
8780146a:	460d      	mov	r5, r1
8780146c:	4617      	mov	r7, r2
8780146e:	f002 feb7 	bl	878041e0 <knl_is_inirq>
87801472:	b998      	cbnz	r0, 8780149c <tos_mail_q_pend+0x38>
87801474:	2d00      	cmp	r5, #0
87801476:	bf18      	it	ne
87801478:	2c00      	cmpne	r4, #0
8780147a:	bf0c      	ite	eq
8780147c:	f04f 0801 	moveq.w	r8, #1
87801480:	f04f 0800 	movne.w	r8, #0
87801484:	d00f      	beq.n	878014a6 <tos_mail_q_pend+0x42>
87801486:	6862      	ldr	r2, [r4, #4]
87801488:	f644 33ee 	movw	r3, #19438	; 0x4bee
8780148c:	429a      	cmp	r2, r3
8780148e:	bf18      	it	ne
87801490:	f240 464d 	movwne	r6, #1101	; 0x44d
87801494:	d00a      	beq.n	878014ac <tos_mail_q_pend+0x48>
87801496:	4630      	mov	r0, r6
87801498:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
8780149c:	f44f 76c8 	mov.w	r6, #400	; 0x190
878014a0:	4630      	mov	r0, r6
878014a2:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
878014a6:	f240 464c 	movw	r6, #1100	; 0x44c
878014aa:	e7f4      	b.n	87801496 <tos_mail_q_pend+0x32>
878014ac:	f003 fbd8 	bl	87804c60 <tos_cpu_cpsr_save>
878014b0:	463a      	mov	r2, r7
878014b2:	4681      	mov	r9, r0
878014b4:	4629      	mov	r1, r5
878014b6:	f104 0010 	add.w	r0, r4, #16
878014ba:	f003 f97d 	bl	878047b8 <tos_ring_q_dequeue>
878014be:	4606      	mov	r6, r0
878014c0:	b158      	cbz	r0, 878014da <tos_mail_q_pend+0x76>
878014c2:	e9dd 2308 	ldrd	r2, r3, [sp, #32]
878014c6:	4313      	orrs	r3, r2
878014c8:	d10b      	bne.n	878014e2 <tos_mail_q_pend+0x7e>
878014ca:	f8c7 8000 	str.w	r8, [r7]
878014ce:	4648      	mov	r0, r9
878014d0:	f003 fbc8 	bl	87804c64 <tos_cpu_cpsr_restore>
878014d4:	f44f 6696 	mov.w	r6, #1200	; 0x4b0
878014d8:	e7dd      	b.n	87801496 <tos_mail_q_pend+0x32>
878014da:	4648      	mov	r0, r9
878014dc:	f003 fbc2 	bl	87804c64 <tos_cpu_cpsr_restore>
878014e0:	e7d9      	b.n	87801496 <tos_mail_q_pend+0x32>
878014e2:	f002 fe73 	bl	878041cc <knl_is_sched_locked>
878014e6:	b128      	cbz	r0, 878014f4 <tos_mail_q_pend+0x90>
878014e8:	4648      	mov	r0, r9
878014ea:	f240 46b1 	movw	r6, #1201	; 0x4b1
878014ee:	f003 fbb9 	bl	87804c64 <tos_cpu_cpsr_restore>
878014f2:	e7d0      	b.n	87801496 <tos_mail_q_pend+0x32>
878014f4:	f640 36a0 	movw	r6, #2976	; 0xba0
878014f8:	e9dd 2308 	ldrd	r2, r3, [sp, #32]
878014fc:	f2c8 7681 	movt	r6, #34689	; 0x8781
87801500:	f104 0108 	add.w	r1, r4, #8
87801504:	6830      	ldr	r0, [r6, #0]
87801506:	6685      	str	r5, [r0, #104]	; 0x68
87801508:	f002 ff5a 	bl	878043c0 <pend_task_block>
8780150c:	4648      	mov	r0, r9
8780150e:	f003 fba9 	bl	87804c64 <tos_cpu_cpsr_restore>
87801512:	f002 fe05 	bl	87804120 <knl_sched>
87801516:	6833      	ldr	r3, [r6, #0]
87801518:	6e18      	ldr	r0, [r3, #96]	; 0x60
8780151a:	f002 ff15 	bl	87804348 <pend_state2errno>
8780151e:	b930      	cbnz	r0, 8780152e <tos_mail_q_pend+0xca>
87801520:	6833      	ldr	r3, [r6, #0]
87801522:	4606      	mov	r6, r0
87801524:	6eda      	ldr	r2, [r3, #108]	; 0x6c
87801526:	603a      	str	r2, [r7, #0]
87801528:	6698      	str	r0, [r3, #104]	; 0x68
8780152a:	66d8      	str	r0, [r3, #108]	; 0x6c
8780152c:	e7b3      	b.n	87801496 <tos_mail_q_pend+0x32>
8780152e:	4606      	mov	r6, r0
87801530:	e7b1      	b.n	87801496 <tos_mail_q_pend+0x32>
87801532:	bf00      	nop

87801534 <tos_mail_q_post>:
87801534:	2900      	cmp	r1, #0
87801536:	bf18      	it	ne
87801538:	2800      	cmpne	r0, #0
8780153a:	bf0c      	ite	eq
8780153c:	2301      	moveq	r3, #1
8780153e:	2300      	movne	r3, #0
87801540:	d009      	beq.n	87801556 <tos_mail_q_post+0x22>
87801542:	b430      	push	{r4, r5}
87801544:	f644 34ee 	movw	r4, #19438	; 0x4bee
87801548:	6845      	ldr	r5, [r0, #4]
8780154a:	42a5      	cmp	r5, r4
8780154c:	d006      	beq.n	8780155c <tos_mail_q_post+0x28>
8780154e:	f240 404d 	movw	r0, #1101	; 0x44d
87801552:	bc30      	pop	{r4, r5}
87801554:	4770      	bx	lr
87801556:	f240 404c 	movw	r0, #1100	; 0x44c
8780155a:	4770      	bx	lr
8780155c:	bc30      	pop	{r4, r5}
8780155e:	e68d      	b.n	8780127c <mail_q_do_post.part.8>

87801560 <tos_mail_q_post_all>:
87801560:	2900      	cmp	r1, #0
87801562:	bf18      	it	ne
87801564:	2800      	cmpne	r0, #0
87801566:	d00a      	beq.n	8780157e <tos_mail_q_post_all+0x1e>
87801568:	b410      	push	{r4}
8780156a:	f644 33ee 	movw	r3, #19438	; 0x4bee
8780156e:	6844      	ldr	r4, [r0, #4]
87801570:	429c      	cmp	r4, r3
87801572:	d007      	beq.n	87801584 <tos_mail_q_post_all+0x24>
87801574:	f240 404d 	movw	r0, #1101	; 0x44d
87801578:	f85d 4b04 	ldr.w	r4, [sp], #4
8780157c:	4770      	bx	lr
8780157e:	f240 404c 	movw	r0, #1100	; 0x44c
87801582:	4770      	bx	lr
87801584:	2301      	movs	r3, #1
87801586:	f85d 4b04 	ldr.w	r4, [sp], #4
8780158a:	e677      	b.n	8780127c <mail_q_do_post.part.8>

8780158c <tos_systick_get>:
8780158c:	b538      	push	{r3, r4, r5, lr}
8780158e:	f003 fb67 	bl	87804c60 <tos_cpu_cpsr_save>
87801592:	f640 33a8 	movw	r3, #2984	; 0xba8
87801596:	f2c8 7381 	movt	r3, #34689	; 0x8781
8780159a:	e9d3 4500 	ldrd	r4, r5, [r3]
8780159e:	f003 fb61 	bl	87804c64 <tos_cpu_cpsr_restore>
878015a2:	4620      	mov	r0, r4
878015a4:	4629      	mov	r1, r5
878015a6:	bd38      	pop	{r3, r4, r5, pc}

878015a8 <tos_systick_set>:
878015a8:	b538      	push	{r3, r4, r5, lr}
878015aa:	4604      	mov	r4, r0
878015ac:	460d      	mov	r5, r1
878015ae:	f003 fb57 	bl	87804c60 <tos_cpu_cpsr_save>
878015b2:	f640 33a8 	movw	r3, #2984	; 0xba8
878015b6:	f2c8 7381 	movt	r3, #34689	; 0x8781
878015ba:	e9c3 4500 	strd	r4, r5, [r3]
878015be:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
878015c2:	f003 bb4f 	b.w	87804c64 <tos_cpu_cpsr_restore>
878015c6:	bf00      	nop

878015c8 <tos_tick2millisec>:
878015c8:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
878015ca:	008b      	lsls	r3, r1, #2
878015cc:	0086      	lsls	r6, r0, #2
878015ce:	f44f 727a 	mov.w	r2, #1000	; 0x3e8
878015d2:	ea43 7790 	orr.w	r7, r3, r0, lsr #30
878015d6:	2300      	movs	r3, #0
878015d8:	0174      	lsls	r4, r6, #5
878015da:	017d      	lsls	r5, r7, #5
878015dc:	1ba4      	subs	r4, r4, r6
878015de:	ea45 65d6 	orr.w	r5, r5, r6, lsr #27
878015e2:	eb65 0507 	sbc.w	r5, r5, r7
878015e6:	1900      	adds	r0, r0, r4
878015e8:	4169      	adcs	r1, r5
878015ea:	00c5      	lsls	r5, r0, #3
878015ec:	00cc      	lsls	r4, r1, #3
878015ee:	ea44 7450 	orr.w	r4, r4, r0, lsr #29
878015f2:	4628      	mov	r0, r5
878015f4:	4621      	mov	r1, r4
878015f6:	f007 fe07 	bl	87809208 <__aeabi_uldivmod>
878015fa:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

878015fc <tos_millisec2tick>:
878015fc:	f44f 717a 	mov.w	r1, #1000	; 0x3e8
87801600:	b508      	push	{r3, lr}
87801602:	f44f 727a 	mov.w	r2, #1000	; 0x3e8
87801606:	2300      	movs	r3, #0
87801608:	fba0 0101 	umull	r0, r1, r0, r1
8780160c:	f007 fdfc 	bl	87809208 <__aeabi_uldivmod>
87801610:	bd08      	pop	{r3, pc}
87801612:	bf00      	nop

87801614 <tos_sleep_ms>:
87801614:	f44f 717a 	mov.w	r1, #1000	; 0x3e8
87801618:	b508      	push	{r3, lr}
8780161a:	f44f 727a 	mov.w	r2, #1000	; 0x3e8
8780161e:	2300      	movs	r3, #0
87801620:	fba0 0101 	umull	r0, r1, r0, r1
87801624:	f007 fdf0 	bl	87809208 <__aeabi_uldivmod>
87801628:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
8780162c:	f001 bf02 	b.w	87803434 <tos_task_delay>

87801630 <tos_sleep_hmsm>:
87801630:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
87801634:	4614      	mov	r4, r2
87801636:	2500      	movs	r5, #0
87801638:	223c      	movs	r2, #60	; 0x3c
8780163a:	fbe2 4501 	umlal	r4, r5, r2, r1
8780163e:	f44f 6261 	mov.w	r2, #3600	; 0xe10
87801642:	f44f 7a7a 	mov.w	sl, #1000	; 0x3e8
87801646:	fba3 ab0a 	umull	sl, fp, r3, sl
8780164a:	fbe2 4500 	umlal	r4, r5, r2, r0
8780164e:	f44f 727a 	mov.w	r2, #1000	; 0x3e8
87801652:	4650      	mov	r0, sl
87801654:	4659      	mov	r1, fp
87801656:	462f      	mov	r7, r5
87801658:	4626      	mov	r6, r4
8780165a:	00bb      	lsls	r3, r7, #2
8780165c:	00b4      	lsls	r4, r6, #2
8780165e:	ea43 7596 	orr.w	r5, r3, r6, lsr #30
87801662:	ea4f 1844 	mov.w	r8, r4, lsl #5
87801666:	ea4f 1945 	mov.w	r9, r5, lsl #5
8780166a:	2300      	movs	r3, #0
8780166c:	ea49 69d4 	orr.w	r9, r9, r4, lsr #27
87801670:	ebb8 0404 	subs.w	r4, r8, r4
87801674:	eb69 0505 	sbc.w	r5, r9, r5
87801678:	19a4      	adds	r4, r4, r6
8780167a:	417d      	adcs	r5, r7
8780167c:	00e7      	lsls	r7, r4, #3
8780167e:	00ee      	lsls	r6, r5, #3
87801680:	ea46 7654 	orr.w	r6, r6, r4, lsr #29
87801684:	f007 fdc0 	bl	87809208 <__aeabi_uldivmod>
87801688:	19c0      	adds	r0, r0, r7
8780168a:	4171      	adcs	r1, r6
8780168c:	e8bd 4ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
87801690:	f001 bed0 	b.w	87803434 <tos_task_delay>

87801694 <event_do_post.part.4>:
87801694:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87801698:	4607      	mov	r7, r0
8780169a:	2a00      	cmp	r2, #0
8780169c:	d13a      	bne.n	87801714 <event_do_post.part.4+0x80>
8780169e:	6903      	ldr	r3, [r0, #16]
878016a0:	4319      	orrs	r1, r3
878016a2:	6101      	str	r1, [r0, #16]
878016a4:	46b8      	mov	r8, r7
878016a6:	f003 fadb 	bl	87804c60 <tos_cpu_cpsr_save>
878016aa:	f858 3f08 	ldr.w	r3, [r8, #8]!
878016ae:	4606      	mov	r6, r0
878016b0:	681d      	ldr	r5, [r3, #0]
878016b2:	4598      	cmp	r8, r3
878016b4:	f1a3 0448 	sub.w	r4, r3, #72	; 0x48
878016b8:	f1a5 0548 	sub.w	r5, r5, #72	; 0x48
878016bc:	d10b      	bne.n	878016d6 <event_do_post.part.4+0x42>
878016be:	e021      	b.n	87801704 <event_do_post.part.4+0x70>
878016c0:	ea33 0201 	bics.w	r2, r3, r1
878016c4:	d013      	beq.n	878016ee <event_do_post.part.4+0x5a>
878016c6:	f105 0248 	add.w	r2, r5, #72	; 0x48
878016ca:	6cab      	ldr	r3, [r5, #72]	; 0x48
878016cc:	4590      	cmp	r8, r2
878016ce:	462c      	mov	r4, r5
878016d0:	f1a3 0548 	sub.w	r5, r3, #72	; 0x48
878016d4:	d016      	beq.n	87801704 <event_do_post.part.4+0x70>
878016d6:	f8b4 2070 	ldrh.w	r2, [r4, #112]	; 0x70
878016da:	6939      	ldr	r1, [r7, #16]
878016dc:	6f63      	ldr	r3, [r4, #116]	; 0x74
878016de:	f012 0f02 	tst.w	r2, #2
878016e2:	6fa0      	ldr	r0, [r4, #120]	; 0x78
878016e4:	d1ec      	bne.n	878016c0 <event_do_post.part.4+0x2c>
878016e6:	07d2      	lsls	r2, r2, #31
878016e8:	d5ed      	bpl.n	878016c6 <event_do_post.part.4+0x32>
878016ea:	400b      	ands	r3, r1
878016ec:	d0eb      	beq.n	878016c6 <event_do_post.part.4+0x32>
878016ee:	6003      	str	r3, [r0, #0]
878016f0:	2101      	movs	r1, #1
878016f2:	4620      	mov	r0, r4
878016f4:	f002 fe40 	bl	87804378 <pend_task_wakeup>
878016f8:	f8b4 3070 	ldrh.w	r3, [r4, #112]	; 0x70
878016fc:	075b      	lsls	r3, r3, #29
878016fe:	d5e2      	bpl.n	878016c6 <event_do_post.part.4+0x32>
87801700:	2300      	movs	r3, #0
87801702:	613b      	str	r3, [r7, #16]
87801704:	4630      	mov	r0, r6
87801706:	f003 faad 	bl	87804c64 <tos_cpu_cpsr_restore>
8780170a:	f002 fd09 	bl	87804120 <knl_sched>
8780170e:	2000      	movs	r0, #0
87801710:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87801714:	6101      	str	r1, [r0, #16]
87801716:	e7c5      	b.n	878016a4 <event_do_post.part.4+0x10>

87801718 <tos_event_create>:
87801718:	b150      	cbz	r0, 87801730 <tos_event_create+0x18>
8780171a:	b510      	push	{r4, lr}
8780171c:	4604      	mov	r4, r0
8780171e:	6101      	str	r1, [r0, #16]
87801720:	3008      	adds	r0, #8
87801722:	f002 fdf9 	bl	87804318 <pend_object_init>
87801726:	f643 33ee 	movw	r3, #15342	; 0x3bee
8780172a:	2000      	movs	r0, #0
8780172c:	6063      	str	r3, [r4, #4]
8780172e:	bd10      	pop	{r4, pc}
87801730:	f240 404c 	movw	r0, #1100	; 0x44c
87801734:	4770      	bx	lr
87801736:	bf00      	nop

87801738 <tos_event_destroy>:
87801738:	b140      	cbz	r0, 8780174c <tos_event_destroy+0x14>
8780173a:	6842      	ldr	r2, [r0, #4]
8780173c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
8780173e:	f643 33ee 	movw	r3, #15342	; 0x3bee
87801742:	429a      	cmp	r2, r3
87801744:	d005      	beq.n	87801752 <tos_event_destroy+0x1a>
87801746:	f240 404d 	movw	r0, #1101	; 0x44d
8780174a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
8780174c:	f240 404c 	movw	r0, #1100	; 0x44c
87801750:	4770      	bx	lr
87801752:	f100 0608 	add.w	r6, r0, #8
87801756:	4604      	mov	r4, r0
87801758:	2500      	movs	r5, #0
8780175a:	f003 fa81 	bl	87804c60 <tos_cpu_cpsr_save>
8780175e:	2103      	movs	r1, #3
87801760:	4607      	mov	r7, r0
87801762:	4630      	mov	r0, r6
87801764:	f002 fe4c 	bl	87804400 <pend_wakeup_all>
87801768:	4630      	mov	r0, r6
8780176a:	6125      	str	r5, [r4, #16]
8780176c:	f002 fdd8 	bl	87804320 <pend_object_deinit>
87801770:	4638      	mov	r0, r7
87801772:	6065      	str	r5, [r4, #4]
87801774:	f003 fa76 	bl	87804c64 <tos_cpu_cpsr_restore>
87801778:	f002 fcd2 	bl	87804120 <knl_sched>
8780177c:	4628      	mov	r0, r5
8780177e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87801780 <tos_event_pend>:
87801780:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87801784:	b083      	sub	sp, #12
87801786:	4604      	mov	r4, r0
87801788:	460f      	mov	r7, r1
8780178a:	4615      	mov	r5, r2
8780178c:	e9dd 890c 	ldrd	r8, r9, [sp, #48]	; 0x30
87801790:	f8bd 6038 	ldrh.w	r6, [sp, #56]	; 0x38
87801794:	f002 fd24 	bl	878041e0 <knl_is_inirq>
87801798:	bb78      	cbnz	r0, 878017fa <tos_event_pend+0x7a>
8780179a:	2d00      	cmp	r5, #0
8780179c:	bf18      	it	ne
8780179e:	2c00      	cmpne	r4, #0
878017a0:	d030      	beq.n	87801804 <tos_event_pend+0x84>
878017a2:	6862      	ldr	r2, [r4, #4]
878017a4:	f643 33ee 	movw	r3, #15342	; 0x3bee
878017a8:	429a      	cmp	r2, r3
878017aa:	bf18      	it	ne
878017ac:	f240 404d 	movwne	r0, #1101	; 0x44d
878017b0:	d002      	beq.n	878017b8 <tos_event_pend+0x38>
878017b2:	b003      	add	sp, #12
878017b4:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
878017b8:	f016 0203 	ands.w	r2, r6, #3
878017bc:	f04f 00c8 	mov.w	r0, #200	; 0xc8
878017c0:	bf0c      	ite	eq
878017c2:	2301      	moveq	r3, #1
878017c4:	2300      	movne	r3, #0
878017c6:	2a03      	cmp	r2, #3
878017c8:	bf08      	it	eq
878017ca:	f043 0301 	orreq.w	r3, r3, #1
878017ce:	2b00      	cmp	r3, #0
878017d0:	d1ef      	bne.n	878017b2 <tos_event_pend+0x32>
878017d2:	f003 fa45 	bl	87804c60 <tos_cpu_cpsr_save>
878017d6:	07b1      	lsls	r1, r6, #30
878017d8:	4682      	mov	sl, r0
878017da:	6923      	ldr	r3, [r4, #16]
878017dc:	d515      	bpl.n	8780180a <tos_event_pend+0x8a>
878017de:	ea37 0303 	bics.w	r3, r7, r3
878017e2:	bf08      	it	eq
878017e4:	602f      	streq	r7, [r5, #0]
878017e6:	d016      	beq.n	87801816 <tos_event_pend+0x96>
878017e8:	ea58 0309 	orrs.w	r3, r8, r9
878017ec:	d11c      	bne.n	87801828 <tos_event_pend+0xa8>
878017ee:	4650      	mov	r0, sl
878017f0:	f003 fa38 	bl	87804c64 <tos_cpu_cpsr_restore>
878017f4:	f44f 6096 	mov.w	r0, #1200	; 0x4b0
878017f8:	e7db      	b.n	878017b2 <tos_event_pend+0x32>
878017fa:	f44f 70c8 	mov.w	r0, #400	; 0x190
878017fe:	b003      	add	sp, #12
87801800:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87801804:	f240 404c 	movw	r0, #1100	; 0x44c
87801808:	e7d3      	b.n	878017b2 <tos_event_pend+0x32>
8780180a:	07f2      	lsls	r2, r6, #31
8780180c:	d5ec      	bpl.n	878017e8 <tos_event_pend+0x68>
8780180e:	403b      	ands	r3, r7
87801810:	bf18      	it	ne
87801812:	602b      	strne	r3, [r5, #0]
87801814:	d0e8      	beq.n	878017e8 <tos_event_pend+0x68>
87801816:	0773      	lsls	r3, r6, #29
87801818:	4650      	mov	r0, sl
8780181a:	bf44      	itt	mi
8780181c:	2300      	movmi	r3, #0
8780181e:	6123      	strmi	r3, [r4, #16]
87801820:	f003 fa20 	bl	87804c64 <tos_cpu_cpsr_restore>
87801824:	2000      	movs	r0, #0
87801826:	e7c4      	b.n	878017b2 <tos_event_pend+0x32>
87801828:	f002 fcd0 	bl	878041cc <knl_is_sched_locked>
8780182c:	4683      	mov	fp, r0
8780182e:	b128      	cbz	r0, 8780183c <tos_event_pend+0xbc>
87801830:	4650      	mov	r0, sl
87801832:	f003 fa17 	bl	87804c64 <tos_cpu_cpsr_restore>
87801836:	f240 40b1 	movw	r0, #1201	; 0x4b1
8780183a:	e7ba      	b.n	878017b2 <tos_event_pend+0x32>
8780183c:	f640 31a0 	movw	r1, #2976	; 0xba0
87801840:	464b      	mov	r3, r9
87801842:	f2c8 7181 	movt	r1, #34689	; 0x8781
87801846:	4642      	mov	r2, r8
87801848:	4608      	mov	r0, r1
8780184a:	f104 0108 	add.w	r1, r4, #8
8780184e:	6804      	ldr	r4, [r0, #0]
87801850:	9001      	str	r0, [sp, #4]
87801852:	6767      	str	r7, [r4, #116]	; 0x74
87801854:	4620      	mov	r0, r4
87801856:	67a5      	str	r5, [r4, #120]	; 0x78
87801858:	f8a4 6070 	strh.w	r6, [r4, #112]	; 0x70
8780185c:	f002 fdb0 	bl	878043c0 <pend_task_block>
87801860:	4650      	mov	r0, sl
87801862:	f003 f9ff 	bl	87804c64 <tos_cpu_cpsr_restore>
87801866:	f002 fc5b 	bl	87804120 <knl_sched>
8780186a:	9901      	ldr	r1, [sp, #4]
8780186c:	680b      	ldr	r3, [r1, #0]
8780186e:	f8c3 b074 	str.w	fp, [r3, #116]	; 0x74
87801872:	f8c3 b078 	str.w	fp, [r3, #120]	; 0x78
87801876:	f8a3 b070 	strh.w	fp, [r3, #112]	; 0x70
8780187a:	6e18      	ldr	r0, [r3, #96]	; 0x60
8780187c:	b003      	add	sp, #12
8780187e:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87801882:	f002 bd61 	b.w	87804348 <pend_state2errno>
87801886:	bf00      	nop

87801888 <tos_event_post>:
87801888:	b138      	cbz	r0, 8780189a <tos_event_post+0x12>
8780188a:	6842      	ldr	r2, [r0, #4]
8780188c:	f643 33ee 	movw	r3, #15342	; 0x3bee
87801890:	429a      	cmp	r2, r3
87801892:	d005      	beq.n	878018a0 <tos_event_post+0x18>
87801894:	f240 404d 	movw	r0, #1101	; 0x44d
87801898:	4770      	bx	lr
8780189a:	f240 404c 	movw	r0, #1100	; 0x44c
8780189e:	4770      	bx	lr
878018a0:	2201      	movs	r2, #1
878018a2:	e6f7      	b.n	87801694 <event_do_post.part.4>

878018a4 <tos_event_post_keep>:
878018a4:	b138      	cbz	r0, 878018b6 <tos_event_post_keep+0x12>
878018a6:	6842      	ldr	r2, [r0, #4]
878018a8:	f643 33ee 	movw	r3, #15342	; 0x3bee
878018ac:	429a      	cmp	r2, r3
878018ae:	d005      	beq.n	878018bc <tos_event_post_keep+0x18>
878018b0:	f240 404d 	movw	r0, #1101	; 0x44d
878018b4:	4770      	bx	lr
878018b6:	f240 404c 	movw	r0, #1100	; 0x44c
878018ba:	4770      	bx	lr
878018bc:	2200      	movs	r2, #0
878018be:	e6e9      	b.n	87801694 <event_do_post.part.4>

878018c0 <bin_heap_do_percolate_up>:
878018c0:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
878018c4:	1e4c      	subs	r4, r1, #1
878018c6:	6907      	ldr	r7, [r0, #16]
878018c8:	4605      	mov	r5, r0
878018ca:	eb04 74d4 	add.w	r4, r4, r4, lsr #31
878018ce:	460e      	mov	r6, r1
878018d0:	6980      	ldr	r0, [r0, #24]
878018d2:	4690      	mov	r8, r2
878018d4:	1064      	asrs	r4, r4, #1
878018d6:	f8d5 900c 	ldr.w	r9, [r5, #12]
878018da:	fa1f fa84 	uxth.w	sl, r4
878018de:	fb07 0b01 	mla	fp, r7, r1, r0
878018e2:	fb07 0404 	mla	r4, r7, r4, r0
878018e6:	b9b1      	cbnz	r1, 87801916 <bin_heap_do_percolate_up+0x56>
878018e8:	e025      	b.n	87801936 <bin_heap_do_percolate_up+0x76>
878018ea:	4621      	mov	r1, r4
878018ec:	4658      	mov	r0, fp
878018ee:	463a      	mov	r2, r7
878018f0:	f10a 34ff 	add.w	r4, sl, #4294967295	; 0xffffffff
878018f4:	f003 fd94 	bl	87805420 <memcpy>
878018f8:	eb04 74d4 	add.w	r4, r4, r4, lsr #31
878018fc:	69a8      	ldr	r0, [r5, #24]
878018fe:	692a      	ldr	r2, [r5, #16]
87801900:	4656      	mov	r6, sl
87801902:	1063      	asrs	r3, r4, #1
87801904:	b299      	uxth	r1, r3
87801906:	fb02 0b0a 	mla	fp, r2, sl, r0
8780190a:	fb02 0403 	mla	r4, r2, r3, r0
8780190e:	f1ba 0f00 	cmp.w	sl, #0
87801912:	d009      	beq.n	87801928 <bin_heap_do_percolate_up+0x68>
87801914:	468a      	mov	sl, r1
87801916:	4621      	mov	r1, r4
87801918:	4640      	mov	r0, r8
8780191a:	47c8      	blx	r9
8780191c:	2800      	cmp	r0, #0
8780191e:	d1e4      	bne.n	878018ea <bin_heap_do_percolate_up+0x2a>
87801920:	692b      	ldr	r3, [r5, #16]
87801922:	69a8      	ldr	r0, [r5, #24]
87801924:	fb03 fa06 	mul.w	sl, r3, r6
87801928:	463a      	mov	r2, r7
8780192a:	4641      	mov	r1, r8
8780192c:	4450      	add	r0, sl
8780192e:	e8bd 4ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
87801932:	f003 bd75 	b.w	87805420 <memcpy>
87801936:	468a      	mov	sl, r1
87801938:	e7f6      	b.n	87801928 <bin_heap_do_percolate_up+0x68>
8780193a:	bf00      	nop

8780193c <tos_bin_heap_create>:
8780193c:	b4f0      	push	{r4, r5, r6, r7}
8780193e:	9d04      	ldr	r5, [sp, #16]
87801940:	2d00      	cmp	r5, #0
87801942:	bf18      	it	ne
87801944:	2900      	cmpne	r1, #0
87801946:	bf0c      	ite	eq
87801948:	2401      	moveq	r4, #1
8780194a:	2400      	movne	r4, #0
8780194c:	2800      	cmp	r0, #0
8780194e:	bf08      	it	eq
87801950:	f044 0401 	orreq.w	r4, r4, #1
87801954:	b964      	cbnz	r4, 87801970 <tos_bin_heap_create+0x34>
87801956:	f64d 27d0 	movw	r7, #56016	; 0xdad0
8780195a:	2601      	movs	r6, #1
8780195c:	6084      	str	r4, [r0, #8]
8780195e:	60c5      	str	r5, [r0, #12]
87801960:	e880 00c0 	stmia.w	r0, {r6, r7}
87801964:	6103      	str	r3, [r0, #16]
87801966:	6142      	str	r2, [r0, #20]
87801968:	6181      	str	r1, [r0, #24]
8780196a:	4620      	mov	r0, r4
8780196c:	bcf0      	pop	{r4, r5, r6, r7}
8780196e:	4770      	bx	lr
87801970:	f240 444c 	movw	r4, #1100	; 0x44c
87801974:	4620      	mov	r0, r4
87801976:	bcf0      	pop	{r4, r5, r6, r7}
87801978:	4770      	bx	lr
8780197a:	bf00      	nop

8780197c <tos_bin_heap_destroy>:
8780197c:	4603      	mov	r3, r0
8780197e:	b168      	cbz	r0, 8780199c <tos_bin_heap_destroy+0x20>
87801980:	6841      	ldr	r1, [r0, #4]
87801982:	f64d 22d0 	movw	r2, #56016	; 0xdad0
87801986:	4291      	cmp	r1, r2
87801988:	d002      	beq.n	87801990 <tos_bin_heap_destroy+0x14>
8780198a:	f240 404d 	movw	r0, #1101	; 0x44d
8780198e:	4770      	bx	lr
87801990:	6802      	ldr	r2, [r0, #0]
87801992:	2a01      	cmp	r2, #1
87801994:	d005      	beq.n	878019a2 <tos_bin_heap_destroy+0x26>
87801996:	f240 404e 	movw	r0, #1102	; 0x44e
8780199a:	4770      	bx	lr
8780199c:	f240 404c 	movw	r0, #1100	; 0x44c
878019a0:	4770      	bx	lr
878019a2:	2200      	movs	r2, #0
878019a4:	6082      	str	r2, [r0, #8]
878019a6:	4610      	mov	r0, r2
878019a8:	60da      	str	r2, [r3, #12]
878019aa:	611a      	str	r2, [r3, #16]
878019ac:	615a      	str	r2, [r3, #20]
878019ae:	619a      	str	r2, [r3, #24]
878019b0:	605a      	str	r2, [r3, #4]
878019b2:	601a      	str	r2, [r3, #0]
878019b4:	4770      	bx	lr
878019b6:	bf00      	nop

878019b8 <tos_bin_heap_create_dyn>:
878019b8:	2b00      	cmp	r3, #0
878019ba:	bf18      	it	ne
878019bc:	2800      	cmpne	r0, #0
878019be:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
878019c2:	bf0c      	ite	eq
878019c4:	2601      	moveq	r6, #1
878019c6:	2600      	movne	r6, #0
878019c8:	d01b      	beq.n	87801a02 <tos_bin_heap_create_dyn+0x4a>
878019ca:	4604      	mov	r4, r0
878019cc:	fb02 f001 	mul.w	r0, r2, r1
878019d0:	461d      	mov	r5, r3
878019d2:	4690      	mov	r8, r2
878019d4:	460f      	mov	r7, r1
878019d6:	f7fe ffd7 	bl	87800988 <tos_mmheap_alloc>
878019da:	b168      	cbz	r0, 878019f8 <tos_bin_heap_create_dyn+0x40>
878019dc:	f64d 22d0 	movw	r2, #56016	; 0xdad0
878019e0:	2302      	movs	r3, #2
878019e2:	61a0      	str	r0, [r4, #24]
878019e4:	4630      	mov	r0, r6
878019e6:	60a6      	str	r6, [r4, #8]
878019e8:	60e5      	str	r5, [r4, #12]
878019ea:	f8c4 8010 	str.w	r8, [r4, #16]
878019ee:	6167      	str	r7, [r4, #20]
878019f0:	6062      	str	r2, [r4, #4]
878019f2:	6023      	str	r3, [r4, #0]
878019f4:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878019f8:	f240 467e 	movw	r6, #1150	; 0x47e
878019fc:	4630      	mov	r0, r6
878019fe:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87801a02:	f240 464c 	movw	r6, #1100	; 0x44c
87801a06:	4630      	mov	r0, r6
87801a08:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}

87801a0c <tos_bin_heap_destroy_dyn>:
87801a0c:	b168      	cbz	r0, 87801a2a <tos_bin_heap_destroy_dyn+0x1e>
87801a0e:	6842      	ldr	r2, [r0, #4]
87801a10:	f64d 23d0 	movw	r3, #56016	; 0xdad0
87801a14:	429a      	cmp	r2, r3
87801a16:	d002      	beq.n	87801a1e <tos_bin_heap_destroy_dyn+0x12>
87801a18:	f240 404d 	movw	r0, #1101	; 0x44d
87801a1c:	4770      	bx	lr
87801a1e:	6803      	ldr	r3, [r0, #0]
87801a20:	2b02      	cmp	r3, #2
87801a22:	d005      	beq.n	87801a30 <tos_bin_heap_destroy_dyn+0x24>
87801a24:	f240 404e 	movw	r0, #1102	; 0x44e
87801a28:	4770      	bx	lr
87801a2a:	f240 404c 	movw	r0, #1100	; 0x44c
87801a2e:	4770      	bx	lr
87801a30:	b510      	push	{r4, lr}
87801a32:	4604      	mov	r4, r0
87801a34:	6980      	ldr	r0, [r0, #24]
87801a36:	f7ff f849 	bl	87800acc <tos_mmheap_free>
87801a3a:	2300      	movs	r3, #0
87801a3c:	60a3      	str	r3, [r4, #8]
87801a3e:	4618      	mov	r0, r3
87801a40:	60e3      	str	r3, [r4, #12]
87801a42:	6123      	str	r3, [r4, #16]
87801a44:	6163      	str	r3, [r4, #20]
87801a46:	61a3      	str	r3, [r4, #24]
87801a48:	6063      	str	r3, [r4, #4]
87801a4a:	6023      	str	r3, [r4, #0]
87801a4c:	bd10      	pop	{r4, pc}
87801a4e:	bf00      	nop

87801a50 <tos_bin_heap_push>:
87801a50:	2900      	cmp	r1, #0
87801a52:	bf18      	it	ne
87801a54:	2800      	cmpne	r0, #0
87801a56:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87801a5a:	bf0c      	ite	eq
87801a5c:	2601      	moveq	r6, #1
87801a5e:	2600      	movne	r6, #0
87801a60:	d00e      	beq.n	87801a80 <tos_bin_heap_push+0x30>
87801a62:	6844      	ldr	r4, [r0, #4]
87801a64:	f64d 23d0 	movw	r3, #56016	; 0xdad0
87801a68:	429c      	cmp	r4, r3
87801a6a:	d003      	beq.n	87801a74 <tos_bin_heap_push+0x24>
87801a6c:	f240 404d 	movw	r0, #1101	; 0x44d
87801a70:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87801a74:	6903      	ldr	r3, [r0, #16]
87801a76:	4293      	cmp	r3, r2
87801a78:	d006      	beq.n	87801a88 <tos_bin_heap_push+0x38>
87801a7a:	2011      	movs	r0, #17
87801a7c:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87801a80:	f240 404c 	movw	r0, #1100	; 0x44c
87801a84:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87801a88:	4604      	mov	r4, r0
87801a8a:	460d      	mov	r5, r1
87801a8c:	f003 f8e8 	bl	87804c60 <tos_cpu_cpsr_save>
87801a90:	f8d4 8008 	ldr.w	r8, [r4, #8]
87801a94:	6967      	ldr	r7, [r4, #20]
87801a96:	f003 f8e5 	bl	87804c64 <tos_cpu_cpsr_restore>
87801a9a:	45b8      	cmp	r8, r7
87801a9c:	d102      	bne.n	87801aa4 <tos_bin_heap_push+0x54>
87801a9e:	200f      	movs	r0, #15
87801aa0:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87801aa4:	f003 f8dc 	bl	87804c60 <tos_cpu_cpsr_save>
87801aa8:	69a3      	ldr	r3, [r4, #24]
87801aaa:	4607      	mov	r7, r0
87801aac:	6922      	ldr	r2, [r4, #16]
87801aae:	4629      	mov	r1, r5
87801ab0:	68a0      	ldr	r0, [r4, #8]
87801ab2:	fb00 3002 	mla	r0, r0, r2, r3
87801ab6:	f003 fcb3 	bl	87805420 <memcpy>
87801aba:	462a      	mov	r2, r5
87801abc:	8921      	ldrh	r1, [r4, #8]
87801abe:	4620      	mov	r0, r4
87801ac0:	f7ff fefe 	bl	878018c0 <bin_heap_do_percolate_up>
87801ac4:	68a3      	ldr	r3, [r4, #8]
87801ac6:	4638      	mov	r0, r7
87801ac8:	3301      	adds	r3, #1
87801aca:	60a3      	str	r3, [r4, #8]
87801acc:	f003 f8ca 	bl	87804c64 <tos_cpu_cpsr_restore>
87801ad0:	4630      	mov	r0, r6
87801ad2:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87801ad6:	bf00      	nop

87801ad8 <tos_bin_heap_pop>:
87801ad8:	2900      	cmp	r1, #0
87801ada:	bf18      	it	ne
87801adc:	2800      	cmpne	r0, #0
87801ade:	d00a      	beq.n	87801af6 <tos_bin_heap_pop+0x1e>
87801ae0:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
87801ae4:	f64d 23d0 	movw	r3, #56016	; 0xdad0
87801ae8:	6846      	ldr	r6, [r0, #4]
87801aea:	429e      	cmp	r6, r3
87801aec:	d006      	beq.n	87801afc <tos_bin_heap_pop+0x24>
87801aee:	f240 404d 	movw	r0, #1101	; 0x44d
87801af2:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
87801af6:	f240 404c 	movw	r0, #1100	; 0x44c
87801afa:	4770      	bx	lr
87801afc:	4604      	mov	r4, r0
87801afe:	460d      	mov	r5, r1
87801b00:	4617      	mov	r7, r2
87801b02:	f003 f8ad 	bl	87804c60 <tos_cpu_cpsr_save>
87801b06:	6863      	ldr	r3, [r4, #4]
87801b08:	4680      	mov	r8, r0
87801b0a:	42b3      	cmp	r3, r6
87801b0c:	d04f      	beq.n	87801bae <tos_bin_heap_pop+0xd6>
87801b0e:	4628      	mov	r0, r5
87801b10:	6922      	ldr	r2, [r4, #16]
87801b12:	69a1      	ldr	r1, [r4, #24]
87801b14:	f003 fc84 	bl	87805420 <memcpy>
87801b18:	b10f      	cbz	r7, 87801b1e <tos_bin_heap_pop+0x46>
87801b1a:	6923      	ldr	r3, [r4, #16]
87801b1c:	603b      	str	r3, [r7, #0]
87801b1e:	68a3      	ldr	r3, [r4, #8]
87801b20:	6927      	ldr	r7, [r4, #16]
87801b22:	f8d4 a018 	ldr.w	sl, [r4, #24]
87801b26:	f103 3eff 	add.w	lr, r3, #4294967295	; 0xffffffff
87801b2a:	f8d4 900c 	ldr.w	r9, [r4, #12]
87801b2e:	f1be 0f02 	cmp.w	lr, #2
87801b32:	f8c4 e008 	str.w	lr, [r4, #8]
87801b36:	463a      	mov	r2, r7
87801b38:	4653      	mov	r3, sl
87801b3a:	eb0a 0007 	add.w	r0, sl, r7
87801b3e:	eb0a 0147 	add.w	r1, sl, r7, lsl #1
87801b42:	d94a      	bls.n	87801bda <tos_bin_heap_pop+0x102>
87801b44:	f04f 0b01 	mov.w	fp, #1
87801b48:	2502      	movs	r5, #2
87801b4a:	47c8      	blx	r9
87801b4c:	69a3      	ldr	r3, [r4, #24]
87801b4e:	2800      	cmp	r0, #0
87801b50:	bf0c      	ite	eq
87801b52:	462e      	moveq	r6, r5
87801b54:	465e      	movne	r6, fp
87801b56:	6921      	ldr	r1, [r4, #16]
87801b58:	4650      	mov	r0, sl
87801b5a:	463a      	mov	r2, r7
87801b5c:	f64f 75fe 	movw	r5, #65534	; 0xfffe
87801b60:	fb01 3106 	mla	r1, r1, r6, r3
87801b64:	f003 fc5c 	bl	87805420 <memcpy>
87801b68:	1c71      	adds	r1, r6, #1
87801b6a:	f8d4 e008 	ldr.w	lr, [r4, #8]
87801b6e:	0049      	lsls	r1, r1, #1
87801b70:	69a3      	ldr	r3, [r4, #24]
87801b72:	6922      	ldr	r2, [r4, #16]
87801b74:	400d      	ands	r5, r1
87801b76:	f105 3bff 	add.w	fp, r5, #4294967295	; 0xffffffff
87801b7a:	4575      	cmp	r5, lr
87801b7c:	fa1f fb8b 	uxth.w	fp, fp
87801b80:	46ac      	mov	ip, r5
87801b82:	fb02 3105 	mla	r1, r2, r5, r3
87801b86:	fb02 300b 	mla	r0, r2, fp, r3
87801b8a:	fb02 3a06 	mla	sl, r2, r6, r3
87801b8e:	d3dc      	bcc.n	87801b4a <tos_bin_heap_pop+0x72>
87801b90:	45f4      	cmp	ip, lr
87801b92:	d019      	beq.n	87801bc8 <tos_bin_heap_pop+0xf0>
87801b94:	68a5      	ldr	r5, [r4, #8]
87801b96:	4631      	mov	r1, r6
87801b98:	4620      	mov	r0, r4
87801b9a:	fb05 3202 	mla	r2, r5, r2, r3
87801b9e:	f7ff fe8f 	bl	878018c0 <bin_heap_do_percolate_up>
87801ba2:	4640      	mov	r0, r8
87801ba4:	f003 f85e 	bl	87804c64 <tos_cpu_cpsr_restore>
87801ba8:	2000      	movs	r0, #0
87801baa:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
87801bae:	f003 f857 	bl	87804c60 <tos_cpu_cpsr_save>
87801bb2:	68a6      	ldr	r6, [r4, #8]
87801bb4:	f003 f856 	bl	87804c64 <tos_cpu_cpsr_restore>
87801bb8:	2e00      	cmp	r6, #0
87801bba:	d1a8      	bne.n	87801b0e <tos_bin_heap_pop+0x36>
87801bbc:	4640      	mov	r0, r8
87801bbe:	f003 f851 	bl	87804c64 <tos_cpu_cpsr_restore>
87801bc2:	2010      	movs	r0, #16
87801bc4:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
87801bc8:	463a      	mov	r2, r7
87801bca:	4601      	mov	r1, r0
87801bcc:	4650      	mov	r0, sl
87801bce:	465e      	mov	r6, fp
87801bd0:	f003 fc26 	bl	87805420 <memcpy>
87801bd4:	6922      	ldr	r2, [r4, #16]
87801bd6:	69a3      	ldr	r3, [r4, #24]
87801bd8:	e7dc      	b.n	87801b94 <tos_bin_heap_pop+0xbc>
87801bda:	2600      	movs	r6, #0
87801bdc:	f04f 0b01 	mov.w	fp, #1
87801be0:	f04f 0c02 	mov.w	ip, #2
87801be4:	e7d4      	b.n	87801b90 <tos_bin_heap_pop+0xb8>
87801be6:	bf00      	nop

87801be8 <tos_bin_heap_flush>:
87801be8:	b140      	cbz	r0, 87801bfc <tos_bin_heap_flush+0x14>
87801bea:	6842      	ldr	r2, [r0, #4]
87801bec:	b538      	push	{r3, r4, r5, lr}
87801bee:	f64d 23d0 	movw	r3, #56016	; 0xdad0
87801bf2:	429a      	cmp	r2, r3
87801bf4:	d005      	beq.n	87801c02 <tos_bin_heap_flush+0x1a>
87801bf6:	f240 404d 	movw	r0, #1101	; 0x44d
87801bfa:	bd38      	pop	{r3, r4, r5, pc}
87801bfc:	f240 404c 	movw	r0, #1100	; 0x44c
87801c00:	4770      	bx	lr
87801c02:	4604      	mov	r4, r0
87801c04:	2500      	movs	r5, #0
87801c06:	f003 f82b 	bl	87804c60 <tos_cpu_cpsr_save>
87801c0a:	60a5      	str	r5, [r4, #8]
87801c0c:	f003 f82a 	bl	87804c64 <tos_cpu_cpsr_restore>
87801c10:	4628      	mov	r0, r5
87801c12:	bd38      	pop	{r3, r4, r5, pc}

87801c14 <tos_bin_heap_is_empty>:
87801c14:	b130      	cbz	r0, 87801c24 <tos_bin_heap_is_empty+0x10>
87801c16:	6842      	ldr	r2, [r0, #4]
87801c18:	f64d 23d0 	movw	r3, #56016	; 0xdad0
87801c1c:	429a      	cmp	r2, r3
87801c1e:	d002      	beq.n	87801c26 <tos_bin_heap_is_empty+0x12>
87801c20:	2000      	movs	r0, #0
87801c22:	4770      	bx	lr
87801c24:	4770      	bx	lr
87801c26:	b510      	push	{r4, lr}
87801c28:	4604      	mov	r4, r0
87801c2a:	f003 f819 	bl	87804c60 <tos_cpu_cpsr_save>
87801c2e:	68a4      	ldr	r4, [r4, #8]
87801c30:	f003 f818 	bl	87804c64 <tos_cpu_cpsr_restore>
87801c34:	fab4 f484 	clz	r4, r4
87801c38:	0964      	lsrs	r4, r4, #5
87801c3a:	4620      	mov	r0, r4
87801c3c:	bd10      	pop	{r4, pc}
87801c3e:	bf00      	nop

87801c40 <tos_bin_heap_is_full>:
87801c40:	b130      	cbz	r0, 87801c50 <tos_bin_heap_is_full+0x10>
87801c42:	6842      	ldr	r2, [r0, #4]
87801c44:	f64d 23d0 	movw	r3, #56016	; 0xdad0
87801c48:	429a      	cmp	r2, r3
87801c4a:	d002      	beq.n	87801c52 <tos_bin_heap_is_full+0x12>
87801c4c:	2000      	movs	r0, #0
87801c4e:	4770      	bx	lr
87801c50:	4770      	bx	lr
87801c52:	b510      	push	{r4, lr}
87801c54:	4604      	mov	r4, r0
87801c56:	f003 f803 	bl	87804c60 <tos_cpu_cpsr_save>
87801c5a:	68a2      	ldr	r2, [r4, #8]
87801c5c:	6963      	ldr	r3, [r4, #20]
87801c5e:	1ad4      	subs	r4, r2, r3
87801c60:	fab4 f484 	clz	r4, r4
87801c64:	0964      	lsrs	r4, r4, #5
87801c66:	f002 fffd 	bl	87804c64 <tos_cpu_cpsr_restore>
87801c6a:	4620      	mov	r0, r4
87801c6c:	bd10      	pop	{r4, pc}
87801c6e:	bf00      	nop

87801c70 <prio_mail_q_do_post.part.8>:
87801c70:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
87801c74:	f100 0708 	add.w	r7, r0, #8
87801c78:	4604      	mov	r4, r0
87801c7a:	4688      	mov	r8, r1
87801c7c:	4616      	mov	r6, r2
87801c7e:	461d      	mov	r5, r3
87801c80:	f002 ffee 	bl	87804c60 <tos_cpu_cpsr_save>
87801c84:	4681      	mov	r9, r0
87801c86:	4638      	mov	r0, r7
87801c88:	f002 fb4e 	bl	87804328 <pend_is_nopending>
87801c8c:	bb18      	cbnz	r0, 87801cd6 <prio_mail_q_do_post.part.8+0x66>
87801c8e:	9b08      	ldr	r3, [sp, #32]
87801c90:	b37b      	cbz	r3, 87801cf2 <prio_mail_q_do_post.part.8+0x82>
87801c92:	68a3      	ldr	r3, [r4, #8]
87801c94:	681c      	ldr	r4, [r3, #0]
87801c96:	429f      	cmp	r7, r3
87801c98:	f1a3 0548 	sub.w	r5, r3, #72	; 0x48
87801c9c:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
87801ca0:	d011      	beq.n	87801cc6 <prio_mail_q_do_post.part.8+0x56>
87801ca2:	4632      	mov	r2, r6
87801ca4:	4641      	mov	r1, r8
87801ca6:	6ea8      	ldr	r0, [r5, #104]	; 0x68
87801ca8:	f003 fbba 	bl	87805420 <memcpy>
87801cac:	66ee      	str	r6, [r5, #108]	; 0x6c
87801cae:	4628      	mov	r0, r5
87801cb0:	2101      	movs	r1, #1
87801cb2:	4625      	mov	r5, r4
87801cb4:	f002 fb60 	bl	87804378 <pend_task_wakeup>
87801cb8:	4623      	mov	r3, r4
87801cba:	f853 4f48 	ldr.w	r4, [r3, #72]!
87801cbe:	429f      	cmp	r7, r3
87801cc0:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
87801cc4:	d1ed      	bne.n	87801ca2 <prio_mail_q_do_post.part.8+0x32>
87801cc6:	4648      	mov	r0, r9
87801cc8:	f002 ffcc 	bl	87804c64 <tos_cpu_cpsr_restore>
87801ccc:	f002 fa28 	bl	87804120 <knl_sched>
87801cd0:	2000      	movs	r0, #0
87801cd2:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87801cd6:	462b      	mov	r3, r5
87801cd8:	4632      	mov	r2, r6
87801cda:	4641      	mov	r1, r8
87801cdc:	f104 0014 	add.w	r0, r4, #20
87801ce0:	f7ff f97c 	bl	87800fdc <tos_prio_q_enqueue>
87801ce4:	4604      	mov	r4, r0
87801ce6:	4648      	mov	r0, r9
87801ce8:	f002 ffbc 	bl	87804c64 <tos_cpu_cpsr_restore>
87801cec:	4620      	mov	r0, r4
87801cee:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87801cf2:	68a4      	ldr	r4, [r4, #8]
87801cf4:	4641      	mov	r1, r8
87801cf6:	4632      	mov	r2, r6
87801cf8:	6a20      	ldr	r0, [r4, #32]
87801cfa:	f003 fb91 	bl	87805420 <memcpy>
87801cfe:	6266      	str	r6, [r4, #36]	; 0x24
87801d00:	f1a4 0048 	sub.w	r0, r4, #72	; 0x48
87801d04:	2101      	movs	r1, #1
87801d06:	f002 fb37 	bl	87804378 <pend_task_wakeup>
87801d0a:	e7dc      	b.n	87801cc6 <prio_mail_q_do_post.part.8+0x56>

87801d0c <tos_prio_mail_q_create>:
87801d0c:	b388      	cbz	r0, 87801d72 <tos_prio_mail_q_create+0x66>
87801d0e:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87801d12:	4604      	mov	r4, r0
87801d14:	00d0      	lsls	r0, r2, #3
87801d16:	b082      	sub	sp, #8
87801d18:	461f      	mov	r7, r3
87801d1a:	eba0 0042 	sub.w	r0, r0, r2, lsl #1
87801d1e:	4615      	mov	r5, r2
87801d20:	460e      	mov	r6, r1
87801d22:	f7fe fe31 	bl	87800988 <tos_mmheap_alloc>
87801d26:	4680      	mov	r8, r0
87801d28:	f240 407e 	movw	r0, #1150	; 0x47e
87801d2c:	f1b8 0f00 	cmp.w	r8, #0
87801d30:	d015      	beq.n	87801d5e <tos_prio_mail_q_create+0x52>
87801d32:	462b      	mov	r3, r5
87801d34:	9700      	str	r7, [sp, #0]
87801d36:	4632      	mov	r2, r6
87801d38:	4641      	mov	r1, r8
87801d3a:	f104 0014 	add.w	r0, r4, #20
87801d3e:	f7ff f883 	bl	87800e48 <tos_prio_q_create>
87801d42:	4605      	mov	r5, r0
87801d44:	b970      	cbnz	r0, 87801d64 <tos_prio_mail_q_create+0x58>
87801d46:	f104 0008 	add.w	r0, r4, #8
87801d4a:	f8c4 8010 	str.w	r8, [r4, #16]
87801d4e:	f002 fae3 	bl	87804318 <pend_object_init>
87801d52:	4628      	mov	r0, r5
87801d54:	f647 32ee 	movw	r2, #31726	; 0x7bee
87801d58:	2301      	movs	r3, #1
87801d5a:	6062      	str	r2, [r4, #4]
87801d5c:	6023      	str	r3, [r4, #0]
87801d5e:	b002      	add	sp, #8
87801d60:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87801d64:	4640      	mov	r0, r8
87801d66:	f7fe feb1 	bl	87800acc <tos_mmheap_free>
87801d6a:	4628      	mov	r0, r5
87801d6c:	b002      	add	sp, #8
87801d6e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87801d72:	f240 404c 	movw	r0, #1100	; 0x44c
87801d76:	4770      	bx	lr

87801d78 <tos_prio_mail_q_destroy>:
87801d78:	b170      	cbz	r0, 87801d98 <tos_prio_mail_q_destroy+0x20>
87801d7a:	6842      	ldr	r2, [r0, #4]
87801d7c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87801d7e:	f647 33ee 	movw	r3, #31726	; 0x7bee
87801d82:	429a      	cmp	r2, r3
87801d84:	d002      	beq.n	87801d8c <tos_prio_mail_q_destroy+0x14>
87801d86:	f240 404d 	movw	r0, #1101	; 0x44d
87801d8a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87801d8c:	6803      	ldr	r3, [r0, #0]
87801d8e:	2b01      	cmp	r3, #1
87801d90:	d005      	beq.n	87801d9e <tos_prio_mail_q_destroy+0x26>
87801d92:	f240 404e 	movw	r0, #1102	; 0x44e
87801d96:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87801d98:	f240 404c 	movw	r0, #1100	; 0x44c
87801d9c:	4770      	bx	lr
87801d9e:	4604      	mov	r4, r0
87801da0:	f002 ff5e 	bl	87804c60 <tos_cpu_cpsr_save>
87801da4:	4606      	mov	r6, r0
87801da6:	f104 0014 	add.w	r0, r4, #20
87801daa:	f7ff f893 	bl	87800ed4 <tos_prio_q_destroy>
87801dae:	4605      	mov	r5, r0
87801db0:	b9a8      	cbnz	r0, 87801dde <tos_prio_mail_q_destroy+0x66>
87801db2:	f104 0708 	add.w	r7, r4, #8
87801db6:	2103      	movs	r1, #3
87801db8:	4638      	mov	r0, r7
87801dba:	f002 fb21 	bl	87804400 <pend_wakeup_all>
87801dbe:	6920      	ldr	r0, [r4, #16]
87801dc0:	f7fe fe84 	bl	87800acc <tos_mmheap_free>
87801dc4:	4638      	mov	r0, r7
87801dc6:	6125      	str	r5, [r4, #16]
87801dc8:	f002 faaa 	bl	87804320 <pend_object_deinit>
87801dcc:	4630      	mov	r0, r6
87801dce:	6065      	str	r5, [r4, #4]
87801dd0:	6025      	str	r5, [r4, #0]
87801dd2:	f002 ff47 	bl	87804c64 <tos_cpu_cpsr_restore>
87801dd6:	f002 f9a3 	bl	87804120 <knl_sched>
87801dda:	4628      	mov	r0, r5
87801ddc:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87801dde:	4630      	mov	r0, r6
87801de0:	f002 ff40 	bl	87804c64 <tos_cpu_cpsr_restore>
87801de4:	4628      	mov	r0, r5
87801de6:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87801de8 <tos_prio_mail_q_create_dyn>:
87801de8:	b190      	cbz	r0, 87801e10 <tos_prio_mail_q_create_dyn+0x28>
87801dea:	b538      	push	{r3, r4, r5, lr}
87801dec:	4604      	mov	r4, r0
87801dee:	3014      	adds	r0, #20
87801df0:	f7ff f894 	bl	87800f1c <tos_prio_q_create_dyn>
87801df4:	4605      	mov	r5, r0
87801df6:	b100      	cbz	r0, 87801dfa <tos_prio_mail_q_create_dyn+0x12>
87801df8:	bd38      	pop	{r3, r4, r5, pc}
87801dfa:	f104 0008 	add.w	r0, r4, #8
87801dfe:	f002 fa8b 	bl	87804318 <pend_object_init>
87801e02:	f647 32ee 	movw	r2, #31726	; 0x7bee
87801e06:	2302      	movs	r3, #2
87801e08:	4628      	mov	r0, r5
87801e0a:	6062      	str	r2, [r4, #4]
87801e0c:	6023      	str	r3, [r4, #0]
87801e0e:	bd38      	pop	{r3, r4, r5, pc}
87801e10:	f240 404c 	movw	r0, #1100	; 0x44c
87801e14:	4770      	bx	lr
87801e16:	bf00      	nop

87801e18 <tos_prio_mail_q_destroy_dyn>:
87801e18:	b170      	cbz	r0, 87801e38 <tos_prio_mail_q_destroy_dyn+0x20>
87801e1a:	6842      	ldr	r2, [r0, #4]
87801e1c:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87801e1e:	f647 33ee 	movw	r3, #31726	; 0x7bee
87801e22:	429a      	cmp	r2, r3
87801e24:	d002      	beq.n	87801e2c <tos_prio_mail_q_destroy_dyn+0x14>
87801e26:	f240 404d 	movw	r0, #1101	; 0x44d
87801e2a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87801e2c:	6803      	ldr	r3, [r0, #0]
87801e2e:	2b02      	cmp	r3, #2
87801e30:	d005      	beq.n	87801e3e <tos_prio_mail_q_destroy_dyn+0x26>
87801e32:	f240 404e 	movw	r0, #1102	; 0x44e
87801e36:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87801e38:	f240 404c 	movw	r0, #1100	; 0x44c
87801e3c:	4770      	bx	lr
87801e3e:	4604      	mov	r4, r0
87801e40:	f002 ff0e 	bl	87804c60 <tos_cpu_cpsr_save>
87801e44:	4606      	mov	r6, r0
87801e46:	f104 0014 	add.w	r0, r4, #20
87801e4a:	f7ff f89d 	bl	87800f88 <tos_prio_q_destroy_dyn>
87801e4e:	4605      	mov	r5, r0
87801e50:	b988      	cbnz	r0, 87801e76 <tos_prio_mail_q_destroy_dyn+0x5e>
87801e52:	f104 0708 	add.w	r7, r4, #8
87801e56:	2103      	movs	r1, #3
87801e58:	4638      	mov	r0, r7
87801e5a:	f002 fad1 	bl	87804400 <pend_wakeup_all>
87801e5e:	4638      	mov	r0, r7
87801e60:	f002 fa5e 	bl	87804320 <pend_object_deinit>
87801e64:	4630      	mov	r0, r6
87801e66:	6065      	str	r5, [r4, #4]
87801e68:	6025      	str	r5, [r4, #0]
87801e6a:	f002 fefb 	bl	87804c64 <tos_cpu_cpsr_restore>
87801e6e:	f002 f957 	bl	87804120 <knl_sched>
87801e72:	4628      	mov	r0, r5
87801e74:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87801e76:	4630      	mov	r0, r6
87801e78:	f002 fef4 	bl	87804c64 <tos_cpu_cpsr_restore>
87801e7c:	4628      	mov	r0, r5
87801e7e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87801e80 <tos_prio_mail_q_flush>:
87801e80:	b138      	cbz	r0, 87801e92 <tos_prio_mail_q_flush+0x12>
87801e82:	6842      	ldr	r2, [r0, #4]
87801e84:	f647 33ee 	movw	r3, #31726	; 0x7bee
87801e88:	429a      	cmp	r2, r3
87801e8a:	d005      	beq.n	87801e98 <tos_prio_mail_q_flush+0x18>
87801e8c:	f240 404d 	movw	r0, #1101	; 0x44d
87801e90:	4770      	bx	lr
87801e92:	f240 404c 	movw	r0, #1100	; 0x44c
87801e96:	4770      	bx	lr
87801e98:	3014      	adds	r0, #20
87801e9a:	f7ff b991 	b.w	878011c0 <tos_prio_q_flush>
87801e9e:	bf00      	nop

87801ea0 <tos_prio_mail_q_pend>:
87801ea0:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
87801ea4:	4604      	mov	r4, r0
87801ea6:	460d      	mov	r5, r1
87801ea8:	4617      	mov	r7, r2
87801eaa:	f002 f999 	bl	878041e0 <knl_is_inirq>
87801eae:	b998      	cbnz	r0, 87801ed8 <tos_prio_mail_q_pend+0x38>
87801eb0:	2d00      	cmp	r5, #0
87801eb2:	bf18      	it	ne
87801eb4:	2c00      	cmpne	r4, #0
87801eb6:	bf0c      	ite	eq
87801eb8:	f04f 0801 	moveq.w	r8, #1
87801ebc:	f04f 0800 	movne.w	r8, #0
87801ec0:	d00f      	beq.n	87801ee2 <tos_prio_mail_q_pend+0x42>
87801ec2:	6862      	ldr	r2, [r4, #4]
87801ec4:	f647 33ee 	movw	r3, #31726	; 0x7bee
87801ec8:	429a      	cmp	r2, r3
87801eca:	bf18      	it	ne
87801ecc:	f240 464d 	movwne	r6, #1101	; 0x44d
87801ed0:	d00a      	beq.n	87801ee8 <tos_prio_mail_q_pend+0x48>
87801ed2:	4630      	mov	r0, r6
87801ed4:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87801ed8:	f44f 76c8 	mov.w	r6, #400	; 0x190
87801edc:	4630      	mov	r0, r6
87801ede:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87801ee2:	f240 464c 	movw	r6, #1100	; 0x44c
87801ee6:	e7f4      	b.n	87801ed2 <tos_prio_mail_q_pend+0x32>
87801ee8:	f002 feba 	bl	87804c60 <tos_cpu_cpsr_save>
87801eec:	4643      	mov	r3, r8
87801eee:	4681      	mov	r9, r0
87801ef0:	463a      	mov	r2, r7
87801ef2:	4629      	mov	r1, r5
87801ef4:	f104 0014 	add.w	r0, r4, #20
87801ef8:	f7ff f8ec 	bl	878010d4 <tos_prio_q_dequeue>
87801efc:	4606      	mov	r6, r0
87801efe:	b158      	cbz	r0, 87801f18 <tos_prio_mail_q_pend+0x78>
87801f00:	e9dd 2308 	ldrd	r2, r3, [sp, #32]
87801f04:	4313      	orrs	r3, r2
87801f06:	d10b      	bne.n	87801f20 <tos_prio_mail_q_pend+0x80>
87801f08:	f8c7 8000 	str.w	r8, [r7]
87801f0c:	4648      	mov	r0, r9
87801f0e:	f002 fea9 	bl	87804c64 <tos_cpu_cpsr_restore>
87801f12:	f44f 6696 	mov.w	r6, #1200	; 0x4b0
87801f16:	e7dc      	b.n	87801ed2 <tos_prio_mail_q_pend+0x32>
87801f18:	4648      	mov	r0, r9
87801f1a:	f002 fea3 	bl	87804c64 <tos_cpu_cpsr_restore>
87801f1e:	e7d8      	b.n	87801ed2 <tos_prio_mail_q_pend+0x32>
87801f20:	f002 f954 	bl	878041cc <knl_is_sched_locked>
87801f24:	b128      	cbz	r0, 87801f32 <tos_prio_mail_q_pend+0x92>
87801f26:	4648      	mov	r0, r9
87801f28:	f240 46b1 	movw	r6, #1201	; 0x4b1
87801f2c:	f002 fe9a 	bl	87804c64 <tos_cpu_cpsr_restore>
87801f30:	e7cf      	b.n	87801ed2 <tos_prio_mail_q_pend+0x32>
87801f32:	f640 36a0 	movw	r6, #2976	; 0xba0
87801f36:	e9dd 2308 	ldrd	r2, r3, [sp, #32]
87801f3a:	f2c8 7681 	movt	r6, #34689	; 0x8781
87801f3e:	f104 0108 	add.w	r1, r4, #8
87801f42:	6830      	ldr	r0, [r6, #0]
87801f44:	6685      	str	r5, [r0, #104]	; 0x68
87801f46:	f002 fa3b 	bl	878043c0 <pend_task_block>
87801f4a:	4648      	mov	r0, r9
87801f4c:	f002 fe8a 	bl	87804c64 <tos_cpu_cpsr_restore>
87801f50:	f002 f8e6 	bl	87804120 <knl_sched>
87801f54:	6833      	ldr	r3, [r6, #0]
87801f56:	6e18      	ldr	r0, [r3, #96]	; 0x60
87801f58:	f002 f9f6 	bl	87804348 <pend_state2errno>
87801f5c:	b930      	cbnz	r0, 87801f6c <tos_prio_mail_q_pend+0xcc>
87801f5e:	6833      	ldr	r3, [r6, #0]
87801f60:	4606      	mov	r6, r0
87801f62:	6eda      	ldr	r2, [r3, #108]	; 0x6c
87801f64:	603a      	str	r2, [r7, #0]
87801f66:	6698      	str	r0, [r3, #104]	; 0x68
87801f68:	66d8      	str	r0, [r3, #108]	; 0x6c
87801f6a:	e7b2      	b.n	87801ed2 <tos_prio_mail_q_pend+0x32>
87801f6c:	4606      	mov	r6, r0
87801f6e:	e7b0      	b.n	87801ed2 <tos_prio_mail_q_pend+0x32>

87801f70 <tos_prio_mail_q_post>:
87801f70:	2900      	cmp	r1, #0
87801f72:	bf18      	it	ne
87801f74:	2800      	cmpne	r0, #0
87801f76:	b570      	push	{r4, r5, r6, lr}
87801f78:	bf08      	it	eq
87801f7a:	2601      	moveq	r6, #1
87801f7c:	b082      	sub	sp, #8
87801f7e:	bf18      	it	ne
87801f80:	2600      	movne	r6, #0
87801f82:	d009      	beq.n	87801f98 <tos_prio_mail_q_post+0x28>
87801f84:	6845      	ldr	r5, [r0, #4]
87801f86:	f647 34ee 	movw	r4, #31726	; 0x7bee
87801f8a:	42a5      	cmp	r5, r4
87801f8c:	bf18      	it	ne
87801f8e:	f240 404d 	movwne	r0, #1101	; 0x44d
87801f92:	d005      	beq.n	87801fa0 <tos_prio_mail_q_post+0x30>
87801f94:	b002      	add	sp, #8
87801f96:	bd70      	pop	{r4, r5, r6, pc}
87801f98:	f240 404c 	movw	r0, #1100	; 0x44c
87801f9c:	b002      	add	sp, #8
87801f9e:	bd70      	pop	{r4, r5, r6, pc}
87801fa0:	9600      	str	r6, [sp, #0]
87801fa2:	f7ff fe65 	bl	87801c70 <prio_mail_q_do_post.part.8>
87801fa6:	e7f5      	b.n	87801f94 <tos_prio_mail_q_post+0x24>

87801fa8 <tos_prio_mail_q_post_all>:
87801fa8:	2900      	cmp	r1, #0
87801faa:	bf18      	it	ne
87801fac:	2800      	cmpne	r0, #0
87801fae:	d00b      	beq.n	87801fc8 <tos_prio_mail_q_post_all+0x20>
87801fb0:	b530      	push	{r4, r5, lr}
87801fb2:	f647 34ee 	movw	r4, #31726	; 0x7bee
87801fb6:	6845      	ldr	r5, [r0, #4]
87801fb8:	b083      	sub	sp, #12
87801fba:	42a5      	cmp	r5, r4
87801fbc:	bf18      	it	ne
87801fbe:	f240 404d 	movwne	r0, #1101	; 0x44d
87801fc2:	d004      	beq.n	87801fce <tos_prio_mail_q_post_all+0x26>
87801fc4:	b003      	add	sp, #12
87801fc6:	bd30      	pop	{r4, r5, pc}
87801fc8:	f240 404c 	movw	r0, #1100	; 0x44c
87801fcc:	4770      	bx	lr
87801fce:	2401      	movs	r4, #1
87801fd0:	9400      	str	r4, [sp, #0]
87801fd2:	f7ff fe4d 	bl	87801c70 <prio_mail_q_do_post.part.8>
87801fd6:	e7f5      	b.n	87801fc4 <tos_prio_mail_q_post_all+0x1c>

87801fd8 <tos_countdownlatch_create>:
87801fd8:	b510      	push	{r4, lr}
87801fda:	4604      	mov	r4, r0
87801fdc:	b140      	cbz	r0, 87801ff0 <tos_countdownlatch_create+0x18>
87801fde:	3008      	adds	r0, #8
87801fe0:	8101      	strh	r1, [r0, #8]
87801fe2:	f002 f999 	bl	87804318 <pend_object_init>
87801fe6:	f642 33ee 	movw	r3, #11246	; 0x2bee
87801fea:	2000      	movs	r0, #0
87801fec:	6063      	str	r3, [r4, #4]
87801fee:	bd10      	pop	{r4, pc}
87801ff0:	f240 404c 	movw	r0, #1100	; 0x44c
87801ff4:	bd10      	pop	{r4, pc}
87801ff6:	bf00      	nop

87801ff8 <tos_countdownlatch_destroy>:
87801ff8:	b140      	cbz	r0, 8780200c <tos_countdownlatch_destroy+0x14>
87801ffa:	6842      	ldr	r2, [r0, #4]
87801ffc:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87801ffe:	f642 33ee 	movw	r3, #11246	; 0x2bee
87802002:	429a      	cmp	r2, r3
87802004:	d005      	beq.n	87802012 <tos_countdownlatch_destroy+0x1a>
87802006:	f240 404d 	movw	r0, #1101	; 0x44d
8780200a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
8780200c:	f240 404c 	movw	r0, #1100	; 0x44c
87802010:	4770      	bx	lr
87802012:	f100 0608 	add.w	r6, r0, #8
87802016:	4604      	mov	r4, r0
87802018:	f002 fe22 	bl	87804c60 <tos_cpu_cpsr_save>
8780201c:	2103      	movs	r1, #3
8780201e:	4607      	mov	r7, r0
87802020:	4630      	mov	r0, r6
87802022:	2500      	movs	r5, #0
87802024:	f002 f9ec 	bl	87804400 <pend_wakeup_all>
87802028:	4630      	mov	r0, r6
8780202a:	f002 f979 	bl	87804320 <pend_object_deinit>
8780202e:	4638      	mov	r0, r7
87802030:	6065      	str	r5, [r4, #4]
87802032:	f002 fe17 	bl	87804c64 <tos_cpu_cpsr_restore>
87802036:	f002 f873 	bl	87804120 <knl_sched>
8780203a:	4628      	mov	r0, r5
8780203c:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
8780203e:	bf00      	nop

87802040 <tos_countdownlatch_pend_timed>:
87802040:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87802044:	4604      	mov	r4, r0
87802046:	4616      	mov	r6, r2
87802048:	461f      	mov	r7, r3
8780204a:	f002 f8c9 	bl	878041e0 <knl_is_inirq>
8780204e:	b948      	cbnz	r0, 87802064 <tos_countdownlatch_pend_timed+0x24>
87802050:	b164      	cbz	r4, 8780206c <tos_countdownlatch_pend_timed+0x2c>
87802052:	6862      	ldr	r2, [r4, #4]
87802054:	f642 33ee 	movw	r3, #11246	; 0x2bee
87802058:	429a      	cmp	r2, r3
8780205a:	d00b      	beq.n	87802074 <tos_countdownlatch_pend_timed+0x34>
8780205c:	f240 404d 	movw	r0, #1101	; 0x44d
87802060:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87802064:	f44f 70c8 	mov.w	r0, #400	; 0x190
87802068:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780206c:	f240 404c 	movw	r0, #1100	; 0x44c
87802070:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87802074:	f002 fdf4 	bl	87804c60 <tos_cpu_cpsr_save>
87802078:	8a25      	ldrh	r5, [r4, #16]
8780207a:	4680      	mov	r8, r0
8780207c:	b145      	cbz	r5, 87802090 <tos_countdownlatch_pend_timed+0x50>
8780207e:	ea56 0307 	orrs.w	r3, r6, r7
87802082:	d10a      	bne.n	8780209a <tos_countdownlatch_pend_timed+0x5a>
87802084:	f002 fdee 	bl	87804c64 <tos_cpu_cpsr_restore>
87802088:	f44f 6096 	mov.w	r0, #1200	; 0x4b0
8780208c:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87802090:	f002 fde8 	bl	87804c64 <tos_cpu_cpsr_restore>
87802094:	4628      	mov	r0, r5
87802096:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780209a:	f002 f897 	bl	878041cc <knl_is_sched_locked>
8780209e:	b130      	cbz	r0, 878020ae <tos_countdownlatch_pend_timed+0x6e>
878020a0:	4640      	mov	r0, r8
878020a2:	f002 fddf 	bl	87804c64 <tos_cpu_cpsr_restore>
878020a6:	f240 40b1 	movw	r0, #1201	; 0x4b1
878020aa:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878020ae:	f640 35a0 	movw	r5, #2976	; 0xba0
878020b2:	463b      	mov	r3, r7
878020b4:	f2c8 7581 	movt	r5, #34689	; 0x8781
878020b8:	4632      	mov	r2, r6
878020ba:	f104 0108 	add.w	r1, r4, #8
878020be:	6828      	ldr	r0, [r5, #0]
878020c0:	f002 f97e 	bl	878043c0 <pend_task_block>
878020c4:	4640      	mov	r0, r8
878020c6:	f002 fdcd 	bl	87804c64 <tos_cpu_cpsr_restore>
878020ca:	f002 f829 	bl	87804120 <knl_sched>
878020ce:	682b      	ldr	r3, [r5, #0]
878020d0:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
878020d4:	6e18      	ldr	r0, [r3, #96]	; 0x60
878020d6:	f002 b937 	b.w	87804348 <pend_state2errno>
878020da:	bf00      	nop

878020dc <tos_countdownlatch_pend>:
878020dc:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
878020e0:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
878020e4:	f7ff bfac 	b.w	87802040 <tos_countdownlatch_pend_timed>

878020e8 <tos_countdownlatch_post>:
878020e8:	b138      	cbz	r0, 878020fa <tos_countdownlatch_post+0x12>
878020ea:	6842      	ldr	r2, [r0, #4]
878020ec:	f642 33ee 	movw	r3, #11246	; 0x2bee
878020f0:	429a      	cmp	r2, r3
878020f2:	d005      	beq.n	87802100 <tos_countdownlatch_post+0x18>
878020f4:	f240 404d 	movw	r0, #1101	; 0x44d
878020f8:	4770      	bx	lr
878020fa:	f240 404c 	movw	r0, #1100	; 0x44c
878020fe:	4770      	bx	lr
87802100:	b570      	push	{r4, r5, r6, lr}
87802102:	4604      	mov	r4, r0
87802104:	f002 fdac 	bl	87804c60 <tos_cpu_cpsr_save>
87802108:	4606      	mov	r6, r0
8780210a:	8a23      	ldrh	r3, [r4, #16]
8780210c:	b17b      	cbz	r3, 8780212e <tos_countdownlatch_post+0x46>
8780210e:	3b01      	subs	r3, #1
87802110:	b29d      	uxth	r5, r3
87802112:	8225      	strh	r5, [r4, #16]
87802114:	b97d      	cbnz	r5, 87802136 <tos_countdownlatch_post+0x4e>
87802116:	2101      	movs	r1, #1
87802118:	f104 0008 	add.w	r0, r4, #8
8780211c:	f002 f96c 	bl	878043f8 <pend_wakeup_one>
87802120:	4630      	mov	r0, r6
87802122:	f002 fd9f 	bl	87804c64 <tos_cpu_cpsr_restore>
87802126:	f001 fffb 	bl	87804120 <knl_sched>
8780212a:	4628      	mov	r0, r5
8780212c:	bd70      	pop	{r4, r5, r6, pc}
8780212e:	f002 fd99 	bl	87804c64 <tos_cpu_cpsr_restore>
87802132:	2032      	movs	r0, #50	; 0x32
87802134:	bd70      	pop	{r4, r5, r6, pc}
87802136:	f002 fd95 	bl	87804c64 <tos_cpu_cpsr_restore>
8780213a:	2000      	movs	r0, #0
8780213c:	bd70      	pop	{r4, r5, r6, pc}
8780213e:	bf00      	nop

87802140 <tos_countdownlatch_reset>:
87802140:	b510      	push	{r4, lr}
87802142:	4604      	mov	r4, r0
87802144:	b082      	sub	sp, #8
87802146:	b148      	cbz	r0, 8780215c <tos_countdownlatch_reset+0x1c>
87802148:	f642 33ee 	movw	r3, #11246	; 0x2bee
8780214c:	6842      	ldr	r2, [r0, #4]
8780214e:	429a      	cmp	r2, r3
87802150:	bf18      	it	ne
87802152:	f240 404d 	movwne	r0, #1101	; 0x44d
87802156:	d005      	beq.n	87802164 <tos_countdownlatch_reset+0x24>
87802158:	b002      	add	sp, #8
8780215a:	bd10      	pop	{r4, pc}
8780215c:	f240 404c 	movw	r0, #1100	; 0x44c
87802160:	b002      	add	sp, #8
87802162:	bd10      	pop	{r4, pc}
87802164:	9101      	str	r1, [sp, #4]
87802166:	f002 fd7b 	bl	87804c60 <tos_cpu_cpsr_save>
8780216a:	9901      	ldr	r1, [sp, #4]
8780216c:	8221      	strh	r1, [r4, #16]
8780216e:	f002 fd79 	bl	87804c64 <tos_cpu_cpsr_restore>
87802172:	2000      	movs	r0, #0
87802174:	e7f0      	b.n	87802158 <tos_countdownlatch_reset+0x18>
87802176:	bf00      	nop

87802178 <tick_list_add>:
87802178:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
8780217c:	4616      	mov	r6, r2
8780217e:	461f      	mov	r7, r3
87802180:	4680      	mov	r8, r0
87802182:	f002 fd6d 	bl	87804c60 <tos_cpu_cpsr_save>
87802186:	f64d 6e78 	movw	lr, #56952	; 0xde78
8780218a:	4682      	mov	sl, r0
8780218c:	f2c8 7e80 	movt	lr, #34688	; 0x8780
87802190:	e9c8 670e 	strd	r6, r7, [r8, #56]	; 0x38
87802194:	f8de 3000 	ldr.w	r3, [lr]
87802198:	461a      	mov	r2, r3
8780219a:	3b40      	subs	r3, #64	; 0x40
8780219c:	4572      	cmp	r2, lr
8780219e:	d048      	beq.n	87802232 <tick_list_add+0xba>
878021a0:	e952 0102 	ldrd	r0, r1, [r2, #-8]
878021a4:	428f      	cmp	r7, r1
878021a6:	bf08      	it	eq
878021a8:	4286      	cmpeq	r6, r0
878021aa:	bf24      	itt	cs
878021ac:	f04f 0900 	movcs.w	r9, #0
878021b0:	46cc      	movcs	ip, r9
878021b2:	d212      	bcs.n	878021da <tick_list_add+0x62>
878021b4:	e049      	b.n	8780224a <tick_list_add+0xd2>
878021b6:	6c1c      	ldr	r4, [r3, #64]	; 0x40
878021b8:	4681      	mov	r9, r0
878021ba:	468c      	mov	ip, r1
878021bc:	4574      	cmp	r4, lr
878021be:	4622      	mov	r2, r4
878021c0:	f1a4 0340 	sub.w	r3, r4, #64	; 0x40
878021c4:	d037      	beq.n	87802236 <tick_list_add+0xbe>
878021c6:	e954 4502 	ldrd	r4, r5, [r4, #-8]
878021ca:	1824      	adds	r4, r4, r0
878021cc:	414d      	adcs	r5, r1
878021ce:	42af      	cmp	r7, r5
878021d0:	bf08      	it	eq
878021d2:	42a6      	cmpeq	r6, r4
878021d4:	d30d      	bcc.n	878021f2 <tick_list_add+0x7a>
878021d6:	4620      	mov	r0, r4
878021d8:	4629      	mov	r1, r5
878021da:	428f      	cmp	r7, r1
878021dc:	bf08      	it	eq
878021de:	4286      	cmpeq	r6, r0
878021e0:	d1e9      	bne.n	878021b6 <tick_list_add+0x3e>
878021e2:	f898 5025 	ldrb.w	r5, [r8, #37]	; 0x25
878021e6:	f893 4025 	ldrb.w	r4, [r3, #37]	; 0x25
878021ea:	42a5      	cmp	r5, r4
878021ec:	d2e3      	bcs.n	878021b6 <tick_list_add+0x3e>
878021ee:	4648      	mov	r0, r9
878021f0:	4661      	mov	r1, ip
878021f2:	1a30      	subs	r0, r6, r0
878021f4:	eb67 0101 	sbc.w	r1, r7, r1
878021f8:	e9c8 010e 	strd	r0, r1, [r8, #56]	; 0x38
878021fc:	e9d3 450e 	ldrd	r4, r5, [r3, #56]	; 0x38
87802200:	1a24      	subs	r4, r4, r0
87802202:	eb65 0501 	sbc.w	r5, r5, r1
87802206:	e9c3 450e 	strd	r4, r5, [r3, #56]	; 0x38
8780220a:	6c59      	ldr	r1, [r3, #68]	; 0x44
8780220c:	f108 0440 	add.w	r4, r8, #64	; 0x40
87802210:	645c      	str	r4, [r3, #68]	; 0x44
87802212:	4650      	mov	r0, sl
87802214:	f8c8 2040 	str.w	r2, [r8, #64]	; 0x40
87802218:	f8c8 1044 	str.w	r1, [r8, #68]	; 0x44
8780221c:	600c      	str	r4, [r1, #0]
8780221e:	f002 fd21 	bl	87804c64 <tos_cpu_cpsr_restore>
87802222:	f898 3024 	ldrb.w	r3, [r8, #36]	; 0x24
87802226:	f043 0301 	orr.w	r3, r3, #1
8780222a:	f888 3024 	strb.w	r3, [r8, #36]	; 0x24
8780222e:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
87802232:	2000      	movs	r0, #0
87802234:	2100      	movs	r1, #0
87802236:	1a30      	subs	r0, r6, r0
87802238:	f64d 6278 	movw	r2, #56952	; 0xde78
8780223c:	eb67 0101 	sbc.w	r1, r7, r1
87802240:	f2c8 7280 	movt	r2, #34688	; 0x8780
87802244:	e9c8 010e 	strd	r0, r1, [r8, #56]	; 0x38
87802248:	e7df      	b.n	8780220a <tick_list_add+0x92>
8780224a:	2000      	movs	r0, #0
8780224c:	2100      	movs	r1, #0
8780224e:	e7d0      	b.n	878021f2 <tick_list_add+0x7a>

87802250 <tick_list_remove>:
87802250:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
87802254:	4682      	mov	sl, r0
87802256:	f002 fd03 	bl	87804c60 <tos_cpu_cpsr_save>
8780225a:	4651      	mov	r1, sl
8780225c:	f851 3f40 	ldr.w	r3, [r1, #64]!
87802260:	428b      	cmp	r3, r1
87802262:	d019      	beq.n	87802298 <tick_list_remove+0x48>
87802264:	f64d 6178 	movw	r1, #56952	; 0xde78
87802268:	f2c8 7180 	movt	r1, #34688	; 0x8780
8780226c:	2b40      	cmp	r3, #64	; 0x40
8780226e:	bf18      	it	ne
87802270:	428b      	cmpne	r3, r1
87802272:	d011      	beq.n	87802298 <tick_list_remove+0x48>
87802274:	e9da 890e 	ldrd	r8, r9, [sl, #56]	; 0x38
87802278:	f06f 0601 	mvn.w	r6, #1
8780227c:	e953 4502 	ldrd	r4, r5, [r3, #-8]
87802280:	f04f 37ff 	mov.w	r7, #4294967295	; 0xffffffff
87802284:	ebb6 0108 	subs.w	r1, r6, r8
87802288:	eb67 0209 	sbc.w	r2, r7, r9
8780228c:	42aa      	cmp	r2, r5
8780228e:	bf08      	it	eq
87802290:	42a1      	cmpeq	r1, r4
87802292:	d20f      	bcs.n	878022b4 <tick_list_remove+0x64>
87802294:	e943 6702 	strd	r6, r7, [r3, #-8]
87802298:	f8da 1044 	ldr.w	r1, [sl, #68]	; 0x44
8780229c:	6059      	str	r1, [r3, #4]
8780229e:	600b      	str	r3, [r1, #0]
878022a0:	f002 fce0 	bl	87804c64 <tos_cpu_cpsr_restore>
878022a4:	f89a 3024 	ldrb.w	r3, [sl, #36]	; 0x24
878022a8:	f023 0301 	bic.w	r3, r3, #1
878022ac:	f88a 3024 	strb.w	r3, [sl, #36]	; 0x24
878022b0:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
878022b4:	eb14 0408 	adds.w	r4, r4, r8
878022b8:	eb45 0509 	adc.w	r5, r5, r9
878022bc:	e943 4502 	strd	r4, r5, [r3, #-8]
878022c0:	e7ea      	b.n	87802298 <tick_list_remove+0x48>
878022c2:	bf00      	nop

878022c4 <tick_update>:
878022c4:	e92d 4370 	stmdb	sp!, {r4, r5, r6, r8, r9, lr}
878022c8:	f640 34a8 	movw	r4, #2984	; 0xba8
878022cc:	f2c8 7481 	movt	r4, #34689	; 0x8781
878022d0:	f64d 6578 	movw	r5, #56952	; 0xde78
878022d4:	4689      	mov	r9, r1
878022d6:	4680      	mov	r8, r0
878022d8:	f2c8 7580 	movt	r5, #34688	; 0x8780
878022dc:	f002 fcc0 	bl	87804c60 <tos_cpu_cpsr_save>
878022e0:	e9d4 2300 	ldrd	r2, r3, [r4]
878022e4:	6829      	ldr	r1, [r5, #0]
878022e6:	eb12 0208 	adds.w	r2, r2, r8
878022ea:	eb43 0309 	adc.w	r3, r3, r9
878022ee:	42a9      	cmp	r1, r5
878022f0:	e9c4 2300 	strd	r2, r3, [r4]
878022f4:	d02b      	beq.n	8780234e <tick_update+0x8a>
878022f6:	e951 2302 	ldrd	r2, r3, [r1, #-8]
878022fa:	4599      	cmp	r9, r3
878022fc:	bf08      	it	eq
878022fe:	4590      	cmpeq	r8, r2
87802300:	d31f      	bcc.n	87802342 <tick_update+0x7e>
87802302:	680c      	ldr	r4, [r1, #0]
87802304:	2200      	movs	r2, #0
87802306:	2300      	movs	r3, #0
87802308:	4606      	mov	r6, r0
8780230a:	e941 2302 	strd	r2, r3, [r1, #-8]
8780230e:	f1a1 0040 	sub.w	r0, r1, #64	; 0x40
87802312:	3c40      	subs	r4, #64	; 0x40
87802314:	e005      	b.n	87802322 <tick_update+0x5e>
87802316:	e9d4 890e 	ldrd	r8, r9, [r4, #56]	; 0x38
8780231a:	4614      	mov	r4, r2
8780231c:	ea58 0309 	orrs.w	r3, r8, r9
87802320:	d10a      	bne.n	87802338 <tick_update+0x74>
87802322:	2102      	movs	r1, #2
87802324:	f002 f828 	bl	87804378 <pend_task_wakeup>
87802328:	4623      	mov	r3, r4
8780232a:	4620      	mov	r0, r4
8780232c:	f853 2f40 	ldr.w	r2, [r3, #64]!
87802330:	42ab      	cmp	r3, r5
87802332:	f1a2 0240 	sub.w	r2, r2, #64	; 0x40
87802336:	d1ee      	bne.n	87802316 <tick_update+0x52>
87802338:	4630      	mov	r0, r6
8780233a:	e8bd 4370 	ldmia.w	sp!, {r4, r5, r6, r8, r9, lr}
8780233e:	f002 bc91 	b.w	87804c64 <tos_cpu_cpsr_restore>
87802342:	ebb2 0808 	subs.w	r8, r2, r8
87802346:	eb63 0909 	sbc.w	r9, r3, r9
8780234a:	e941 8902 	strd	r8, r9, [r1, #-8]
8780234e:	e8bd 4370 	ldmia.w	sp!, {r4, r5, r6, r8, r9, lr}
87802352:	f002 bc87 	b.w	87804c64 <tos_cpu_cpsr_restore>
87802356:	bf00      	nop

87802358 <tick_next_expires_get>:
87802358:	b538      	push	{r3, r4, r5, lr}
8780235a:	f002 fc81 	bl	87804c60 <tos_cpu_cpsr_save>
8780235e:	f64d 6378 	movw	r3, #56952	; 0xde78
87802362:	f2c8 7380 	movt	r3, #34688	; 0x8780
87802366:	681a      	ldr	r2, [r3, #0]
87802368:	429a      	cmp	r2, r3
8780236a:	d008      	beq.n	8780237e <tick_next_expires_get+0x26>
8780236c:	2a40      	cmp	r2, #64	; 0x40
8780236e:	d006      	beq.n	8780237e <tick_next_expires_get+0x26>
87802370:	e952 4502 	ldrd	r4, r5, [r2, #-8]
87802374:	f002 fc76 	bl	87804c64 <tos_cpu_cpsr_restore>
87802378:	4620      	mov	r0, r4
8780237a:	4629      	mov	r1, r5
8780237c:	bd38      	pop	{r3, r4, r5, pc}
8780237e:	f04f 34ff 	mov.w	r4, #4294967295	; 0xffffffff
87802382:	f04f 35ff 	mov.w	r5, #4294967295	; 0xffffffff
87802386:	f002 fc6d 	bl	87804c64 <tos_cpu_cpsr_restore>
8780238a:	4620      	mov	r0, r4
8780238c:	4629      	mov	r1, r5
8780238e:	bd38      	pop	{r3, r4, r5, pc}

87802390 <tos_tick_handler>:
87802390:	b508      	push	{r3, lr}
87802392:	f001 feb9 	bl	87804108 <tos_knl_is_running>
87802396:	b128      	cbz	r0, 878023a4 <tos_tick_handler+0x14>
87802398:	2001      	movs	r0, #1
8780239a:	2100      	movs	r1, #0
8780239c:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
878023a0:	f7ff bf90 	b.w	878022c4 <tick_update>
878023a4:	bd08      	pop	{r3, pc}
878023a6:	bf00      	nop

878023a8 <tos_rwlock_rpend_timed.part.3>:
878023a8:	1c59      	adds	r1, r3, #1
878023aa:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
878023ae:	bf08      	it	eq
878023b0:	f1b2 3fff 	cmpeq.w	r2, #4294967295	; 0xffffffff
878023b4:	b085      	sub	sp, #20
878023b6:	4616      	mov	r6, r2
878023b8:	461f      	mov	r7, r3
878023ba:	4605      	mov	r5, r0
878023bc:	d007      	beq.n	878023ce <tos_rwlock_rpend_timed.part.3+0x26>
878023be:	4668      	mov	r0, sp
878023c0:	f002 fa84 	bl	878048cc <tos_stopwatch_create>
878023c4:	4632      	mov	r2, r6
878023c6:	463b      	mov	r3, r7
878023c8:	4668      	mov	r0, sp
878023ca:	f002 faa3 	bl	87804914 <tos_stopwatch_countdown>
878023ce:	f105 0808 	add.w	r8, r5, #8
878023d2:	4632      	mov	r2, r6
878023d4:	463b      	mov	r3, r7
878023d6:	4640      	mov	r0, r8
878023d8:	f000 fac0 	bl	8780295c <tos_mutex_pend_timed>
878023dc:	2800      	cmp	r0, #0
878023de:	d140      	bne.n	87802462 <tos_rwlock_rpend_timed.part.3+0xba>
878023e0:	f8b5 3040 	ldrh.w	r3, [r5, #64]	; 0x40
878023e4:	f64f 72ff 	movw	r2, #65535	; 0xffff
878023e8:	4293      	cmp	r3, r2
878023ea:	bf08      	it	eq
878023ec:	f44f 60c8 	moveq.w	r0, #1600	; 0x640
878023f0:	d037      	beq.n	87802462 <tos_rwlock_rpend_timed.part.3+0xba>
878023f2:	f8b5 1042 	ldrh.w	r1, [r5, #66]	; 0x42
878023f6:	b911      	cbnz	r1, 878023fe <tos_rwlock_rpend_timed.part.3+0x56>
878023f8:	6c6c      	ldr	r4, [r5, #68]	; 0x44
878023fa:	2c00      	cmp	r4, #0
878023fc:	d03d      	beq.n	8780247a <tos_rwlock_rpend_timed.part.3+0xd2>
878023fe:	1c7a      	adds	r2, r7, #1
87802400:	bf08      	it	eq
87802402:	f1b6 3fff 	cmpeq.w	r6, #4294967295	; 0xffffffff
87802406:	d00b      	beq.n	87802420 <tos_rwlock_rpend_timed.part.3+0x78>
87802408:	4668      	mov	r0, sp
8780240a:	f002 fad7 	bl	878049bc <tos_stopwatch_remain>
8780240e:	460f      	mov	r7, r1
87802410:	4606      	mov	r6, r0
87802412:	f8b5 1042 	ldrh.w	r1, [r5, #66]	; 0x42
87802416:	ea56 0307 	orrs.w	r3, r6, r7
8780241a:	bf04      	itt	eq
8780241c:	2601      	moveq	r6, #1
8780241e:	2700      	moveq	r7, #0
87802420:	f105 092c 	add.w	r9, r5, #44	; 0x2c
87802424:	e00e      	b.n	87802444 <tos_rwlock_rpend_timed.part.3+0x9c>
87802426:	1c7b      	adds	r3, r7, #1
87802428:	4668      	mov	r0, sp
8780242a:	bf08      	it	eq
8780242c:	f1b6 3fff 	cmpeq.w	r6, #4294967295	; 0xffffffff
87802430:	d006      	beq.n	87802440 <tos_rwlock_rpend_timed.part.3+0x98>
87802432:	f002 fac3 	bl	878049bc <tos_stopwatch_remain>
87802436:	4606      	mov	r6, r0
87802438:	460f      	mov	r7, r1
8780243a:	ea56 0307 	orrs.w	r3, r6, r7
8780243e:	d019      	beq.n	87802474 <tos_rwlock_rpend_timed.part.3+0xcc>
87802440:	f8b5 1042 	ldrh.w	r1, [r5, #66]	; 0x42
87802444:	4632      	mov	r2, r6
87802446:	463b      	mov	r3, r7
87802448:	4648      	mov	r0, r9
8780244a:	b909      	cbnz	r1, 87802450 <tos_rwlock_rpend_timed.part.3+0xa8>
8780244c:	6c6c      	ldr	r4, [r5, #68]	; 0x44
8780244e:	b15c      	cbz	r4, 87802468 <tos_rwlock_rpend_timed.part.3+0xc0>
87802450:	f001 f9ee 	bl	87803830 <tos_sem_pend>
87802454:	4604      	mov	r4, r0
87802456:	2800      	cmp	r0, #0
87802458:	d0e5      	beq.n	87802426 <tos_rwlock_rpend_timed.part.3+0x7e>
8780245a:	4640      	mov	r0, r8
8780245c:	f000 fb0a 	bl	87802a74 <tos_mutex_post>
87802460:	4620      	mov	r0, r4
87802462:	b005      	add	sp, #20
87802464:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
87802468:	460c      	mov	r4, r1
8780246a:	4640      	mov	r0, r8
8780246c:	f000 fb02 	bl	87802a74 <tos_mutex_post>
87802470:	4620      	mov	r0, r4
87802472:	e7f6      	b.n	87802462 <tos_rwlock_rpend_timed.part.3+0xba>
87802474:	f240 44b3 	movw	r4, #1203	; 0x4b3
87802478:	e7ef      	b.n	8780245a <tos_rwlock_rpend_timed.part.3+0xb2>
8780247a:	3301      	adds	r3, #1
8780247c:	4640      	mov	r0, r8
8780247e:	f8a5 3040 	strh.w	r3, [r5, #64]	; 0x40
87802482:	f000 faf7 	bl	87802a74 <tos_mutex_post>
87802486:	4620      	mov	r0, r4
87802488:	e7eb      	b.n	87802462 <tos_rwlock_rpend_timed.part.3+0xba>
8780248a:	bf00      	nop

8780248c <tos_rwlock_wpend_timed.part.4>:
8780248c:	1c59      	adds	r1, r3, #1
8780248e:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
87802492:	bf08      	it	eq
87802494:	f1b2 3fff 	cmpeq.w	r2, #4294967295	; 0xffffffff
87802498:	b085      	sub	sp, #20
8780249a:	4616      	mov	r6, r2
8780249c:	461f      	mov	r7, r3
8780249e:	4605      	mov	r5, r0
878024a0:	d007      	beq.n	878024b2 <tos_rwlock_wpend_timed.part.4+0x26>
878024a2:	4668      	mov	r0, sp
878024a4:	f002 fa12 	bl	878048cc <tos_stopwatch_create>
878024a8:	4632      	mov	r2, r6
878024aa:	463b      	mov	r3, r7
878024ac:	4668      	mov	r0, sp
878024ae:	f002 fa31 	bl	87804914 <tos_stopwatch_countdown>
878024b2:	f105 0808 	add.w	r8, r5, #8
878024b6:	4632      	mov	r2, r6
878024b8:	463b      	mov	r3, r7
878024ba:	4640      	mov	r0, r8
878024bc:	f000 fa4e 	bl	8780295c <tos_mutex_pend_timed>
878024c0:	2800      	cmp	r0, #0
878024c2:	d144      	bne.n	8780254e <tos_rwlock_wpend_timed.part.4+0xc2>
878024c4:	f8b5 3042 	ldrh.w	r3, [r5, #66]	; 0x42
878024c8:	f64f 72ff 	movw	r2, #65535	; 0xffff
878024cc:	4293      	cmp	r3, r2
878024ce:	bf08      	it	eq
878024d0:	f240 6046 	movweq	r0, #1606	; 0x646
878024d4:	d03b      	beq.n	8780254e <tos_rwlock_wpend_timed.part.4+0xc2>
878024d6:	1c7a      	adds	r2, r7, #1
878024d8:	f103 0301 	add.w	r3, r3, #1
878024dc:	bf08      	it	eq
878024de:	f1b6 3fff 	cmpeq.w	r6, #4294967295	; 0xffffffff
878024e2:	f8a5 3042 	strh.w	r3, [r5, #66]	; 0x42
878024e6:	d009      	beq.n	878024fc <tos_rwlock_wpend_timed.part.4+0x70>
878024e8:	4668      	mov	r0, sp
878024ea:	f002 fa67 	bl	878049bc <tos_stopwatch_remain>
878024ee:	4606      	mov	r6, r0
878024f0:	460f      	mov	r7, r1
878024f2:	ea56 0307 	orrs.w	r3, r6, r7
878024f6:	bf04      	itt	eq
878024f8:	2601      	moveq	r6, #1
878024fa:	2700      	moveq	r7, #0
878024fc:	f105 092c 	add.w	r9, r5, #44	; 0x2c
87802500:	f8b5 1040 	ldrh.w	r1, [r5, #64]	; 0x40
87802504:	4632      	mov	r2, r6
87802506:	463b      	mov	r3, r7
87802508:	4648      	mov	r0, r9
8780250a:	b909      	cbnz	r1, 87802510 <tos_rwlock_wpend_timed.part.4+0x84>
8780250c:	6c6c      	ldr	r4, [r5, #68]	; 0x44
8780250e:	b30c      	cbz	r4, 87802554 <tos_rwlock_wpend_timed.part.4+0xc8>
87802510:	f001 f98e 	bl	87803830 <tos_sem_pend>
87802514:	4604      	mov	r4, r0
87802516:	b970      	cbnz	r0, 87802536 <tos_rwlock_wpend_timed.part.4+0xaa>
87802518:	1c7b      	adds	r3, r7, #1
8780251a:	4668      	mov	r0, sp
8780251c:	bf08      	it	eq
8780251e:	f1b6 3fff 	cmpeq.w	r6, #4294967295	; 0xffffffff
87802522:	d0ed      	beq.n	87802500 <tos_rwlock_wpend_timed.part.4+0x74>
87802524:	f002 fa4a 	bl	878049bc <tos_stopwatch_remain>
87802528:	4606      	mov	r6, r0
8780252a:	460f      	mov	r7, r1
8780252c:	ea56 0307 	orrs.w	r3, r6, r7
87802530:	d1e6      	bne.n	87802500 <tos_rwlock_wpend_timed.part.4+0x74>
87802532:	f240 44b3 	movw	r4, #1203	; 0x4b3
87802536:	4648      	mov	r0, r9
87802538:	f001 f96c 	bl	87803814 <tos_sem_post_all>
8780253c:	f8b5 3042 	ldrh.w	r3, [r5, #66]	; 0x42
87802540:	4640      	mov	r0, r8
87802542:	3b01      	subs	r3, #1
87802544:	f8a5 3042 	strh.w	r3, [r5, #66]	; 0x42
87802548:	f000 fa94 	bl	87802a74 <tos_mutex_post>
8780254c:	4620      	mov	r0, r4
8780254e:	b005      	add	sp, #20
87802550:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
87802554:	2301      	movs	r3, #1
87802556:	646b      	str	r3, [r5, #68]	; 0x44
87802558:	e7f0      	b.n	8780253c <tos_rwlock_wpend_timed.part.4+0xb0>
8780255a:	bf00      	nop

8780255c <tos_rwlock_create>:
8780255c:	b1c0      	cbz	r0, 87802590 <tos_rwlock_create+0x34>
8780255e:	b570      	push	{r4, r5, r6, lr}
87802560:	f100 062c 	add.w	r6, r0, #44	; 0x2c
87802564:	4604      	mov	r4, r0
87802566:	2100      	movs	r1, #0
87802568:	4630      	mov	r0, r6
8780256a:	f001 f90f 	bl	8780378c <tos_sem_create>
8780256e:	b100      	cbz	r0, 87802572 <tos_rwlock_create+0x16>
87802570:	bd70      	pop	{r4, r5, r6, pc}
87802572:	f104 0008 	add.w	r0, r4, #8
87802576:	f000 f987 	bl	87802888 <tos_mutex_create>
8780257a:	4605      	mov	r5, r0
8780257c:	b958      	cbnz	r0, 87802596 <tos_rwlock_create+0x3a>
8780257e:	f649 33ee 	movw	r3, #39918	; 0x9bee
87802582:	f8a4 0040 	strh.w	r0, [r4, #64]	; 0x40
87802586:	f8a4 0042 	strh.w	r0, [r4, #66]	; 0x42
8780258a:	6465      	str	r5, [r4, #68]	; 0x44
8780258c:	6063      	str	r3, [r4, #4]
8780258e:	bd70      	pop	{r4, r5, r6, pc}
87802590:	f240 404c 	movw	r0, #1100	; 0x44c
87802594:	4770      	bx	lr
87802596:	4630      	mov	r0, r6
87802598:	f001 f90a 	bl	878037b0 <tos_sem_destroy>
8780259c:	4628      	mov	r0, r5
8780259e:	bd70      	pop	{r4, r5, r6, pc}

878025a0 <tos_rwlock_destroy>:
878025a0:	b140      	cbz	r0, 878025b4 <tos_rwlock_destroy+0x14>
878025a2:	6842      	ldr	r2, [r0, #4]
878025a4:	b538      	push	{r3, r4, r5, lr}
878025a6:	f649 33ee 	movw	r3, #39918	; 0x9bee
878025aa:	429a      	cmp	r2, r3
878025ac:	d005      	beq.n	878025ba <tos_rwlock_destroy+0x1a>
878025ae:	f240 404d 	movw	r0, #1101	; 0x44d
878025b2:	bd38      	pop	{r3, r4, r5, pc}
878025b4:	f240 404c 	movw	r0, #1100	; 0x44c
878025b8:	4770      	bx	lr
878025ba:	4604      	mov	r4, r0
878025bc:	302c      	adds	r0, #44	; 0x2c
878025be:	f001 f8f7 	bl	878037b0 <tos_sem_destroy>
878025c2:	4605      	mov	r5, r0
878025c4:	f104 0008 	add.w	r0, r4, #8
878025c8:	f000 f97c 	bl	878028c4 <tos_mutex_destroy>
878025cc:	2300      	movs	r3, #0
878025ce:	429d      	cmp	r5, r3
878025d0:	bf18      	it	ne
878025d2:	4628      	movne	r0, r5
878025d4:	f8a4 3040 	strh.w	r3, [r4, #64]	; 0x40
878025d8:	f8a4 3042 	strh.w	r3, [r4, #66]	; 0x42
878025dc:	6463      	str	r3, [r4, #68]	; 0x44
878025de:	6063      	str	r3, [r4, #4]
878025e0:	bd38      	pop	{r3, r4, r5, pc}
878025e2:	bf00      	nop

878025e4 <tos_rwlock_rpend_timed>:
878025e4:	b168      	cbz	r0, 87802602 <tos_rwlock_rpend_timed+0x1e>
878025e6:	b410      	push	{r4}
878025e8:	f649 31ee 	movw	r1, #39918	; 0x9bee
878025ec:	6844      	ldr	r4, [r0, #4]
878025ee:	428c      	cmp	r4, r1
878025f0:	d004      	beq.n	878025fc <tos_rwlock_rpend_timed+0x18>
878025f2:	f240 404d 	movw	r0, #1101	; 0x44d
878025f6:	f85d 4b04 	ldr.w	r4, [sp], #4
878025fa:	4770      	bx	lr
878025fc:	f85d 4b04 	ldr.w	r4, [sp], #4
87802600:	e6d2      	b.n	878023a8 <tos_rwlock_rpend_timed.part.3>
87802602:	f240 404c 	movw	r0, #1100	; 0x44c
87802606:	4770      	bx	lr

87802608 <tos_rwlock_rpend>:
87802608:	b138      	cbz	r0, 8780261a <tos_rwlock_rpend+0x12>
8780260a:	6842      	ldr	r2, [r0, #4]
8780260c:	f649 33ee 	movw	r3, #39918	; 0x9bee
87802610:	429a      	cmp	r2, r3
87802612:	d005      	beq.n	87802620 <tos_rwlock_rpend+0x18>
87802614:	f240 404d 	movw	r0, #1101	; 0x44d
87802618:	4770      	bx	lr
8780261a:	f240 404c 	movw	r0, #1100	; 0x44c
8780261e:	4770      	bx	lr
87802620:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
87802624:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
87802628:	e6be      	b.n	878023a8 <tos_rwlock_rpend_timed.part.3>
8780262a:	bf00      	nop

8780262c <tos_rwlock_rpend_try>:
8780262c:	b138      	cbz	r0, 8780263e <tos_rwlock_rpend_try+0x12>
8780262e:	6842      	ldr	r2, [r0, #4]
87802630:	f649 33ee 	movw	r3, #39918	; 0x9bee
87802634:	429a      	cmp	r2, r3
87802636:	d005      	beq.n	87802644 <tos_rwlock_rpend_try+0x18>
87802638:	f240 404d 	movw	r0, #1101	; 0x44d
8780263c:	4770      	bx	lr
8780263e:	f240 404c 	movw	r0, #1100	; 0x44c
87802642:	4770      	bx	lr
87802644:	b570      	push	{r4, r5, r6, lr}
87802646:	f100 0608 	add.w	r6, r0, #8
8780264a:	4604      	mov	r4, r0
8780264c:	2200      	movs	r2, #0
8780264e:	2300      	movs	r3, #0
87802650:	4630      	mov	r0, r6
87802652:	f000 f983 	bl	8780295c <tos_mutex_pend_timed>
87802656:	b990      	cbnz	r0, 8780267e <tos_rwlock_rpend_try+0x52>
87802658:	f8b4 3040 	ldrh.w	r3, [r4, #64]	; 0x40
8780265c:	f64f 72ff 	movw	r2, #65535	; 0xffff
87802660:	4293      	cmp	r3, r2
87802662:	d00a      	beq.n	8780267a <tos_rwlock_rpend_try+0x4e>
87802664:	f8b4 2042 	ldrh.w	r2, [r4, #66]	; 0x42
87802668:	b90a      	cbnz	r2, 8780266e <tos_rwlock_rpend_try+0x42>
8780266a:	6c65      	ldr	r5, [r4, #68]	; 0x44
8780266c:	b145      	cbz	r5, 87802680 <tos_rwlock_rpend_try+0x54>
8780266e:	4630      	mov	r0, r6
87802670:	f000 fa00 	bl	87802a74 <tos_mutex_post>
87802674:	f240 6042 	movw	r0, #1602	; 0x642
87802678:	bd70      	pop	{r4, r5, r6, pc}
8780267a:	f44f 60c8 	mov.w	r0, #1600	; 0x640
8780267e:	bd70      	pop	{r4, r5, r6, pc}
87802680:	3301      	adds	r3, #1
87802682:	4630      	mov	r0, r6
87802684:	f8a4 3040 	strh.w	r3, [r4, #64]	; 0x40
87802688:	f000 f9f4 	bl	87802a74 <tos_mutex_post>
8780268c:	4628      	mov	r0, r5
8780268e:	bd70      	pop	{r4, r5, r6, pc}

87802690 <tos_rwlock_wpend_timed>:
87802690:	b168      	cbz	r0, 878026ae <tos_rwlock_wpend_timed+0x1e>
87802692:	b410      	push	{r4}
87802694:	f649 31ee 	movw	r1, #39918	; 0x9bee
87802698:	6844      	ldr	r4, [r0, #4]
8780269a:	428c      	cmp	r4, r1
8780269c:	d004      	beq.n	878026a8 <tos_rwlock_wpend_timed+0x18>
8780269e:	f240 404d 	movw	r0, #1101	; 0x44d
878026a2:	f85d 4b04 	ldr.w	r4, [sp], #4
878026a6:	4770      	bx	lr
878026a8:	f85d 4b04 	ldr.w	r4, [sp], #4
878026ac:	e6ee      	b.n	8780248c <tos_rwlock_wpend_timed.part.4>
878026ae:	f240 404c 	movw	r0, #1100	; 0x44c
878026b2:	4770      	bx	lr

878026b4 <tos_rwlock_wpend>:
878026b4:	b138      	cbz	r0, 878026c6 <tos_rwlock_wpend+0x12>
878026b6:	6842      	ldr	r2, [r0, #4]
878026b8:	f649 33ee 	movw	r3, #39918	; 0x9bee
878026bc:	429a      	cmp	r2, r3
878026be:	d005      	beq.n	878026cc <tos_rwlock_wpend+0x18>
878026c0:	f240 404d 	movw	r0, #1101	; 0x44d
878026c4:	4770      	bx	lr
878026c6:	f240 404c 	movw	r0, #1100	; 0x44c
878026ca:	4770      	bx	lr
878026cc:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
878026d0:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
878026d4:	e6da      	b.n	8780248c <tos_rwlock_wpend_timed.part.4>
878026d6:	bf00      	nop

878026d8 <tos_rwlock_wpend_try>:
878026d8:	b138      	cbz	r0, 878026ea <tos_rwlock_wpend_try+0x12>
878026da:	6842      	ldr	r2, [r0, #4]
878026dc:	f649 33ee 	movw	r3, #39918	; 0x9bee
878026e0:	429a      	cmp	r2, r3
878026e2:	d005      	beq.n	878026f0 <tos_rwlock_wpend_try+0x18>
878026e4:	f240 404d 	movw	r0, #1101	; 0x44d
878026e8:	4770      	bx	lr
878026ea:	f240 404c 	movw	r0, #1100	; 0x44c
878026ee:	4770      	bx	lr
878026f0:	b570      	push	{r4, r5, r6, lr}
878026f2:	f100 0608 	add.w	r6, r0, #8
878026f6:	4604      	mov	r4, r0
878026f8:	2200      	movs	r2, #0
878026fa:	2300      	movs	r3, #0
878026fc:	4630      	mov	r0, r6
878026fe:	f000 f92d 	bl	8780295c <tos_mutex_pend_timed>
87802702:	4605      	mov	r5, r0
87802704:	b958      	cbnz	r0, 8780271e <tos_rwlock_wpend_try+0x46>
87802706:	f8b4 3040 	ldrh.w	r3, [r4, #64]	; 0x40
8780270a:	b94b      	cbnz	r3, 87802720 <tos_rwlock_wpend_try+0x48>
8780270c:	6c63      	ldr	r3, [r4, #68]	; 0x44
8780270e:	b953      	cbnz	r3, 87802726 <tos_rwlock_wpend_try+0x4e>
87802710:	2301      	movs	r3, #1
87802712:	6463      	str	r3, [r4, #68]	; 0x44
87802714:	4630      	mov	r0, r6
87802716:	f000 f9ad 	bl	87802a74 <tos_mutex_post>
8780271a:	4628      	mov	r0, r5
8780271c:	bd70      	pop	{r4, r5, r6, pc}
8780271e:	bd70      	pop	{r4, r5, r6, pc}
87802720:	f240 6541 	movw	r5, #1601	; 0x641
87802724:	e7f6      	b.n	87802714 <tos_rwlock_wpend_try+0x3c>
87802726:	f240 6542 	movw	r5, #1602	; 0x642
8780272a:	e7f3      	b.n	87802714 <tos_rwlock_wpend_try+0x3c>

8780272c <tos_rwlock_rpost>:
8780272c:	b138      	cbz	r0, 8780273e <tos_rwlock_rpost+0x12>
8780272e:	6842      	ldr	r2, [r0, #4]
87802730:	f649 33ee 	movw	r3, #39918	; 0x9bee
87802734:	429a      	cmp	r2, r3
87802736:	d005      	beq.n	87802744 <tos_rwlock_rpost+0x18>
87802738:	f240 404d 	movw	r0, #1101	; 0x44d
8780273c:	4770      	bx	lr
8780273e:	f240 404c 	movw	r0, #1100	; 0x44c
87802742:	4770      	bx	lr
87802744:	b570      	push	{r4, r5, r6, lr}
87802746:	f100 0608 	add.w	r6, r0, #8
8780274a:	4604      	mov	r4, r0
8780274c:	4630      	mov	r0, r6
8780274e:	f000 f98b 	bl	87802a68 <tos_mutex_pend>
87802752:	4605      	mov	r5, r0
87802754:	b960      	cbnz	r0, 87802770 <tos_rwlock_rpost+0x44>
87802756:	f8b4 3040 	ldrh.w	r3, [r4, #64]	; 0x40
8780275a:	b153      	cbz	r3, 87802772 <tos_rwlock_rpost+0x46>
8780275c:	3b01      	subs	r3, #1
8780275e:	b29b      	uxth	r3, r3
87802760:	f8a4 3040 	strh.w	r3, [r4, #64]	; 0x40
87802764:	b143      	cbz	r3, 87802778 <tos_rwlock_rpost+0x4c>
87802766:	4630      	mov	r0, r6
87802768:	f000 f984 	bl	87802a74 <tos_mutex_post>
8780276c:	4628      	mov	r0, r5
8780276e:	bd70      	pop	{r4, r5, r6, pc}
87802770:	bd70      	pop	{r4, r5, r6, pc}
87802772:	f240 6543 	movw	r5, #1603	; 0x643
87802776:	e7f6      	b.n	87802766 <tos_rwlock_rpost+0x3a>
87802778:	f104 002c 	add.w	r0, r4, #44	; 0x2c
8780277c:	f001 f84a 	bl	87803814 <tos_sem_post_all>
87802780:	4605      	mov	r5, r0
87802782:	e7f0      	b.n	87802766 <tos_rwlock_rpost+0x3a>

87802784 <tos_rwlock_wpost>:
87802784:	b148      	cbz	r0, 8780279a <tos_rwlock_wpost+0x16>
87802786:	6842      	ldr	r2, [r0, #4]
87802788:	b538      	push	{r3, r4, r5, lr}
8780278a:	f649 33ee 	movw	r3, #39918	; 0x9bee
8780278e:	429a      	cmp	r2, r3
87802790:	bf18      	it	ne
87802792:	f240 404d 	movwne	r0, #1101	; 0x44d
87802796:	d003      	beq.n	878027a0 <tos_rwlock_wpost+0x1c>
87802798:	bd38      	pop	{r3, r4, r5, pc}
8780279a:	f240 404c 	movw	r0, #1100	; 0x44c
8780279e:	4770      	bx	lr
878027a0:	f100 0508 	add.w	r5, r0, #8
878027a4:	4604      	mov	r4, r0
878027a6:	4628      	mov	r0, r5
878027a8:	f000 f95e 	bl	87802a68 <tos_mutex_pend>
878027ac:	2800      	cmp	r0, #0
878027ae:	d1f3      	bne.n	87802798 <tos_rwlock_wpost+0x14>
878027b0:	6c63      	ldr	r3, [r4, #68]	; 0x44
878027b2:	b933      	cbnz	r3, 878027c2 <tos_rwlock_wpost+0x3e>
878027b4:	f240 6444 	movw	r4, #1604	; 0x644
878027b8:	4628      	mov	r0, r5
878027ba:	f000 f95b 	bl	87802a74 <tos_mutex_post>
878027be:	4620      	mov	r0, r4
878027c0:	bd38      	pop	{r3, r4, r5, pc}
878027c2:	6460      	str	r0, [r4, #68]	; 0x44
878027c4:	f104 002c 	add.w	r0, r4, #44	; 0x2c
878027c8:	f001 f824 	bl	87803814 <tos_sem_post_all>
878027cc:	4604      	mov	r4, r0
878027ce:	e7f3      	b.n	878027b8 <tos_rwlock_wpost+0x34>

878027d0 <tos_rwlock_post>:
878027d0:	b138      	cbz	r0, 878027e2 <tos_rwlock_post+0x12>
878027d2:	6842      	ldr	r2, [r0, #4]
878027d4:	f649 33ee 	movw	r3, #39918	; 0x9bee
878027d8:	429a      	cmp	r2, r3
878027da:	d005      	beq.n	878027e8 <tos_rwlock_post+0x18>
878027dc:	f240 404d 	movw	r0, #1101	; 0x44d
878027e0:	4770      	bx	lr
878027e2:	f240 404c 	movw	r0, #1100	; 0x44c
878027e6:	4770      	bx	lr
878027e8:	b570      	push	{r4, r5, r6, lr}
878027ea:	f100 0608 	add.w	r6, r0, #8
878027ee:	4604      	mov	r4, r0
878027f0:	4630      	mov	r0, r6
878027f2:	f000 f939 	bl	87802a68 <tos_mutex_pend>
878027f6:	4605      	mov	r5, r0
878027f8:	b960      	cbnz	r0, 87802814 <tos_rwlock_post+0x44>
878027fa:	f8b4 3040 	ldrh.w	r3, [r4, #64]	; 0x40
878027fe:	b153      	cbz	r3, 87802816 <tos_rwlock_post+0x46>
87802800:	3b01      	subs	r3, #1
87802802:	b29b      	uxth	r3, r3
87802804:	f8a4 3040 	strh.w	r3, [r4, #64]	; 0x40
87802808:	b15b      	cbz	r3, 87802822 <tos_rwlock_post+0x52>
8780280a:	4630      	mov	r0, r6
8780280c:	f000 f932 	bl	87802a74 <tos_mutex_post>
87802810:	4628      	mov	r0, r5
87802812:	bd70      	pop	{r4, r5, r6, pc}
87802814:	bd70      	pop	{r4, r5, r6, pc}
87802816:	6c63      	ldr	r3, [r4, #68]	; 0x44
87802818:	b913      	cbnz	r3, 87802820 <tos_rwlock_post+0x50>
8780281a:	f240 6545 	movw	r5, #1605	; 0x645
8780281e:	e7f4      	b.n	8780280a <tos_rwlock_post+0x3a>
87802820:	6460      	str	r0, [r4, #68]	; 0x44
87802822:	f104 002c 	add.w	r0, r4, #44	; 0x2c
87802826:	f000 fff5 	bl	87803814 <tos_sem_post_all>
8780282a:	4605      	mov	r5, r0
8780282c:	e7ed      	b.n	8780280a <tos_rwlock_post+0x3a>
8780282e:	bf00      	nop

87802830 <mutex_release>:
87802830:	b570      	push	{r4, r5, r6, lr}
87802832:	2300      	movs	r3, #0
87802834:	6945      	ldr	r5, [r0, #20]
87802836:	4604      	mov	r4, r0
87802838:	6a02      	ldr	r2, [r0, #32]
8780283a:	69c1      	ldr	r1, [r0, #28]
8780283c:	604a      	str	r2, [r1, #4]
8780283e:	6011      	str	r1, [r2, #0]
87802840:	6143      	str	r3, [r0, #20]
87802842:	7403      	strb	r3, [r0, #16]
87802844:	f895 6058 	ldrb.w	r6, [r5, #88]	; 0x58
87802848:	2e0a      	cmp	r6, #10
8780284a:	d10f      	bne.n	8780286c <mutex_release+0x3c>
8780284c:	7e01      	ldrb	r1, [r0, #24]
8780284e:	f895 3025 	ldrb.w	r3, [r5, #37]	; 0x25
87802852:	428b      	cmp	r3, r1
87802854:	d003      	beq.n	8780285e <mutex_release+0x2e>
87802856:	4628      	mov	r0, r5
87802858:	f000 fd1a 	bl	87803290 <tos_task_prio_change>
8780285c:	7626      	strb	r6, [r4, #24]
8780285e:	f104 0008 	add.w	r0, r4, #8
87802862:	2104      	movs	r1, #4
87802864:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
87802868:	f001 bdca 	b.w	87804400 <pend_wakeup_all>
8780286c:	4631      	mov	r1, r6
8780286e:	4628      	mov	r0, r5
87802870:	f000 fd0e 	bl	87803290 <tos_task_prio_change>
87802874:	230a      	movs	r3, #10
87802876:	f104 0008 	add.w	r0, r4, #8
8780287a:	f885 3058 	strb.w	r3, [r5, #88]	; 0x58
8780287e:	2104      	movs	r1, #4
87802880:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
87802884:	f001 bdbc 	b.w	87804400 <pend_wakeup_all>

87802888 <tos_mutex_create>:
87802888:	b538      	push	{r3, r4, r5, lr}
8780288a:	4604      	mov	r4, r0
8780288c:	f001 fca8 	bl	878041e0 <knl_is_inirq>
87802890:	4605      	mov	r5, r0
87802892:	b988      	cbnz	r0, 878028b8 <tos_mutex_create+0x30>
87802894:	b19c      	cbz	r4, 878028be <tos_mutex_create+0x36>
87802896:	f104 0008 	add.w	r0, r4, #8
8780289a:	f001 fd3d 	bl	87804318 <pend_object_init>
8780289e:	f104 031c 	add.w	r3, r4, #28
878028a2:	210a      	movs	r1, #10
878028a4:	f646 32ee 	movw	r2, #27630	; 0x6bee
878028a8:	7425      	strb	r5, [r4, #16]
878028aa:	6165      	str	r5, [r4, #20]
878028ac:	4628      	mov	r0, r5
878028ae:	7621      	strb	r1, [r4, #24]
878028b0:	61e3      	str	r3, [r4, #28]
878028b2:	6223      	str	r3, [r4, #32]
878028b4:	6062      	str	r2, [r4, #4]
878028b6:	bd38      	pop	{r3, r4, r5, pc}
878028b8:	f44f 70c8 	mov.w	r0, #400	; 0x190
878028bc:	bd38      	pop	{r3, r4, r5, pc}
878028be:	f240 404c 	movw	r0, #1100	; 0x44c
878028c2:	bd38      	pop	{r3, r4, r5, pc}

878028c4 <tos_mutex_destroy>:
878028c4:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
878028c8:	4604      	mov	r4, r0
878028ca:	f001 fc89 	bl	878041e0 <knl_is_inirq>
878028ce:	4605      	mov	r5, r0
878028d0:	b948      	cbnz	r0, 878028e6 <tos_mutex_destroy+0x22>
878028d2:	b164      	cbz	r4, 878028ee <tos_mutex_destroy+0x2a>
878028d4:	6862      	ldr	r2, [r4, #4]
878028d6:	f646 33ee 	movw	r3, #27630	; 0x6bee
878028da:	429a      	cmp	r2, r3
878028dc:	d00b      	beq.n	878028f6 <tos_mutex_destroy+0x32>
878028de:	f240 404d 	movw	r0, #1101	; 0x44d
878028e2:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878028e6:	f44f 70c8 	mov.w	r0, #400	; 0x190
878028ea:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878028ee:	f240 404c 	movw	r0, #1100	; 0x44c
878028f2:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878028f6:	f104 0708 	add.w	r7, r4, #8
878028fa:	f002 f9b1 	bl	87804c60 <tos_cpu_cpsr_save>
878028fe:	2103      	movs	r1, #3
87802900:	4680      	mov	r8, r0
87802902:	4638      	mov	r0, r7
87802904:	f001 fd7c 	bl	87804400 <pend_wakeup_all>
87802908:	6966      	ldr	r6, [r4, #20]
8780290a:	b196      	cbz	r6, 87802932 <tos_mutex_destroy+0x6e>
8780290c:	6a23      	ldr	r3, [r4, #32]
8780290e:	69e2      	ldr	r2, [r4, #28]
87802910:	6053      	str	r3, [r2, #4]
87802912:	601a      	str	r2, [r3, #0]
87802914:	6165      	str	r5, [r4, #20]
87802916:	7425      	strb	r5, [r4, #16]
87802918:	f896 5058 	ldrb.w	r5, [r6, #88]	; 0x58
8780291c:	2d0a      	cmp	r5, #10
8780291e:	d115      	bne.n	8780294c <tos_mutex_destroy+0x88>
87802920:	7e21      	ldrb	r1, [r4, #24]
87802922:	f896 3025 	ldrb.w	r3, [r6, #37]	; 0x25
87802926:	428b      	cmp	r3, r1
87802928:	d003      	beq.n	87802932 <tos_mutex_destroy+0x6e>
8780292a:	4630      	mov	r0, r6
8780292c:	f000 fcb0 	bl	87803290 <tos_task_prio_change>
87802930:	7625      	strb	r5, [r4, #24]
87802932:	4638      	mov	r0, r7
87802934:	2500      	movs	r5, #0
87802936:	f001 fcf3 	bl	87804320 <pend_object_deinit>
8780293a:	4640      	mov	r0, r8
8780293c:	6065      	str	r5, [r4, #4]
8780293e:	f002 f991 	bl	87804c64 <tos_cpu_cpsr_restore>
87802942:	f001 fbed 	bl	87804120 <knl_sched>
87802946:	4628      	mov	r0, r5
87802948:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780294c:	4629      	mov	r1, r5
8780294e:	4630      	mov	r0, r6
87802950:	f000 fc9e 	bl	87803290 <tos_task_prio_change>
87802954:	230a      	movs	r3, #10
87802956:	f886 3058 	strb.w	r3, [r6, #88]	; 0x58
8780295a:	e7ea      	b.n	87802932 <tos_mutex_destroy+0x6e>

8780295c <tos_mutex_pend_timed>:
8780295c:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87802960:	4604      	mov	r4, r0
87802962:	4616      	mov	r6, r2
87802964:	461f      	mov	r7, r3
87802966:	f001 fc3b 	bl	878041e0 <knl_is_inirq>
8780296a:	b948      	cbnz	r0, 87802980 <tos_mutex_pend_timed+0x24>
8780296c:	b164      	cbz	r4, 87802988 <tos_mutex_pend_timed+0x2c>
8780296e:	6862      	ldr	r2, [r4, #4]
87802970:	f646 33ee 	movw	r3, #27630	; 0x6bee
87802974:	429a      	cmp	r2, r3
87802976:	d00b      	beq.n	87802990 <tos_mutex_pend_timed+0x34>
87802978:	f240 404d 	movw	r0, #1101	; 0x44d
8780297c:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87802980:	f44f 70c8 	mov.w	r0, #400	; 0x190
87802984:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87802988:	f240 404c 	movw	r0, #1100	; 0x44c
8780298c:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87802990:	f002 f966 	bl	87804c60 <tos_cpu_cpsr_save>
87802994:	7c25      	ldrb	r5, [r4, #16]
87802996:	4680      	mov	r8, r0
87802998:	b17d      	cbz	r5, 878029ba <tos_mutex_pend_timed+0x5e>
8780299a:	6960      	ldr	r0, [r4, #20]
8780299c:	f001 fc34 	bl	87804208 <knl_is_self>
878029a0:	b320      	cbz	r0, 878029ec <tos_mutex_pend_timed+0x90>
878029a2:	7c23      	ldrb	r3, [r4, #16]
878029a4:	2bff      	cmp	r3, #255	; 0xff
878029a6:	d04b      	beq.n	87802a40 <tos_mutex_pend_timed+0xe4>
878029a8:	3301      	adds	r3, #1
878029aa:	4640      	mov	r0, r8
878029ac:	7423      	strb	r3, [r4, #16]
878029ae:	f002 f959 	bl	87804c64 <tos_cpu_cpsr_restore>
878029b2:	f240 30e9 	movw	r0, #1001	; 0x3e9
878029b6:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878029ba:	f640 33a0 	movw	r3, #2976	; 0xba0
878029be:	2701      	movs	r7, #1
878029c0:	f2c8 7381 	movt	r3, #34689	; 0x8781
878029c4:	f104 011c 	add.w	r1, r4, #28
878029c8:	681b      	ldr	r3, [r3, #0]
878029ca:	6163      	str	r3, [r4, #20]
878029cc:	461a      	mov	r2, r3
878029ce:	f893 6025 	ldrb.w	r6, [r3, #37]	; 0x25
878029d2:	7427      	strb	r7, [r4, #16]
878029d4:	7626      	strb	r6, [r4, #24]
878029d6:	f852 6f50 	ldr.w	r6, [r2, #80]!
878029da:	6071      	str	r1, [r6, #4]
878029dc:	6222      	str	r2, [r4, #32]
878029de:	61e6      	str	r6, [r4, #28]
878029e0:	6519      	str	r1, [r3, #80]	; 0x50
878029e2:	f002 f93f 	bl	87804c64 <tos_cpu_cpsr_restore>
878029e6:	4628      	mov	r0, r5
878029e8:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878029ec:	ea56 0307 	orrs.w	r3, r6, r7
878029f0:	d106      	bne.n	87802a00 <tos_mutex_pend_timed+0xa4>
878029f2:	4640      	mov	r0, r8
878029f4:	f002 f936 	bl	87804c64 <tos_cpu_cpsr_restore>
878029f8:	f44f 6096 	mov.w	r0, #1200	; 0x4b0
878029fc:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87802a00:	f001 fbe4 	bl	878041cc <knl_is_sched_locked>
87802a04:	bb18      	cbnz	r0, 87802a4e <tos_mutex_pend_timed+0xf2>
87802a06:	f640 35a0 	movw	r5, #2976	; 0xba0
87802a0a:	6963      	ldr	r3, [r4, #20]
87802a0c:	f2c8 7581 	movt	r5, #34689	; 0x8781
87802a10:	6828      	ldr	r0, [r5, #0]
87802a12:	f893 2025 	ldrb.w	r2, [r3, #37]	; 0x25
87802a16:	f890 1025 	ldrb.w	r1, [r0, #37]	; 0x25
87802a1a:	428a      	cmp	r2, r1
87802a1c:	d81e      	bhi.n	87802a5c <tos_mutex_pend_timed+0x100>
87802a1e:	463b      	mov	r3, r7
87802a20:	4632      	mov	r2, r6
87802a22:	f104 0108 	add.w	r1, r4, #8
87802a26:	f001 fccb 	bl	878043c0 <pend_task_block>
87802a2a:	4640      	mov	r0, r8
87802a2c:	f002 f91a 	bl	87804c64 <tos_cpu_cpsr_restore>
87802a30:	f001 fb76 	bl	87804120 <knl_sched>
87802a34:	682b      	ldr	r3, [r5, #0]
87802a36:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
87802a3a:	6e18      	ldr	r0, [r3, #96]	; 0x60
87802a3c:	f001 bc84 	b.w	87804348 <pend_state2errno>
87802a40:	4640      	mov	r0, r8
87802a42:	f002 f90f 	bl	87804c64 <tos_cpu_cpsr_restore>
87802a46:	f240 30ea 	movw	r0, #1002	; 0x3ea
87802a4a:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87802a4e:	4640      	mov	r0, r8
87802a50:	f002 f908 	bl	87804c64 <tos_cpu_cpsr_restore>
87802a54:	f240 40b1 	movw	r0, #1201	; 0x4b1
87802a58:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87802a5c:	4618      	mov	r0, r3
87802a5e:	f000 fc17 	bl	87803290 <tos_task_prio_change>
87802a62:	6828      	ldr	r0, [r5, #0]
87802a64:	e7db      	b.n	87802a1e <tos_mutex_pend_timed+0xc2>
87802a66:	bf00      	nop

87802a68 <tos_mutex_pend>:
87802a68:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
87802a6c:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
87802a70:	f7ff bf74 	b.w	8780295c <tos_mutex_pend_timed>

87802a74 <tos_mutex_post>:
87802a74:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87802a76:	4604      	mov	r4, r0
87802a78:	f001 fbb2 	bl	878041e0 <knl_is_inirq>
87802a7c:	b940      	cbnz	r0, 87802a90 <tos_mutex_post+0x1c>
87802a7e:	b154      	cbz	r4, 87802a96 <tos_mutex_post+0x22>
87802a80:	6862      	ldr	r2, [r4, #4]
87802a82:	f646 33ee 	movw	r3, #27630	; 0x6bee
87802a86:	429a      	cmp	r2, r3
87802a88:	d008      	beq.n	87802a9c <tos_mutex_post+0x28>
87802a8a:	f240 404d 	movw	r0, #1101	; 0x44d
87802a8e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802a90:	f44f 70c8 	mov.w	r0, #400	; 0x190
87802a94:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802a96:	f240 404c 	movw	r0, #1100	; 0x44c
87802a9a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802a9c:	f002 f8e0 	bl	87804c60 <tos_cpu_cpsr_save>
87802aa0:	4605      	mov	r5, r0
87802aa2:	6960      	ldr	r0, [r4, #20]
87802aa4:	f001 fbb0 	bl	87804208 <knl_is_self>
87802aa8:	b150      	cbz	r0, 87802ac0 <tos_mutex_post+0x4c>
87802aaa:	7c23      	ldrb	r3, [r4, #16]
87802aac:	3b01      	subs	r3, #1
87802aae:	b2db      	uxtb	r3, r3
87802ab0:	7423      	strb	r3, [r4, #16]
87802ab2:	b15b      	cbz	r3, 87802acc <tos_mutex_post+0x58>
87802ab4:	4628      	mov	r0, r5
87802ab6:	f002 f8d5 	bl	87804c64 <tos_cpu_cpsr_restore>
87802aba:	f240 30e9 	movw	r0, #1001	; 0x3e9
87802abe:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802ac0:	4628      	mov	r0, r5
87802ac2:	f002 f8cf 	bl	87804c64 <tos_cpu_cpsr_restore>
87802ac6:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
87802aca:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802acc:	6966      	ldr	r6, [r4, #20]
87802ace:	6a22      	ldr	r2, [r4, #32]
87802ad0:	69e1      	ldr	r1, [r4, #28]
87802ad2:	604a      	str	r2, [r1, #4]
87802ad4:	6011      	str	r1, [r2, #0]
87802ad6:	6163      	str	r3, [r4, #20]
87802ad8:	7423      	strb	r3, [r4, #16]
87802ada:	f896 7058 	ldrb.w	r7, [r6, #88]	; 0x58
87802ade:	2f0a      	cmp	r7, #10
87802ae0:	d137      	bne.n	87802b52 <tos_mutex_post+0xde>
87802ae2:	7e21      	ldrb	r1, [r4, #24]
87802ae4:	f896 3025 	ldrb.w	r3, [r6, #37]	; 0x25
87802ae8:	428b      	cmp	r3, r1
87802aea:	d003      	beq.n	87802af4 <tos_mutex_post+0x80>
87802aec:	4630      	mov	r0, r6
87802aee:	f000 fbcf 	bl	87803290 <tos_task_prio_change>
87802af2:	7627      	strb	r7, [r4, #24]
87802af4:	f104 0608 	add.w	r6, r4, #8
87802af8:	4630      	mov	r0, r6
87802afa:	f001 fc15 	bl	87804328 <pend_is_nopending>
87802afe:	bb80      	cbnz	r0, 87802b62 <tos_mutex_post+0xee>
87802b00:	4630      	mov	r0, r6
87802b02:	f001 fbf7 	bl	878042f4 <pend_highest_pending_task_get>
87802b06:	4607      	mov	r7, r0
87802b08:	2201      	movs	r2, #1
87802b0a:	6167      	str	r7, [r4, #20]
87802b0c:	463b      	mov	r3, r7
87802b0e:	f897 1025 	ldrb.w	r1, [r7, #37]	; 0x25
87802b12:	4630      	mov	r0, r6
87802b14:	7422      	strb	r2, [r4, #16]
87802b16:	f104 021c 	add.w	r2, r4, #28
87802b1a:	7621      	strb	r1, [r4, #24]
87802b1c:	f853 1f50 	ldr.w	r1, [r3, #80]!
87802b20:	604a      	str	r2, [r1, #4]
87802b22:	6223      	str	r3, [r4, #32]
87802b24:	61e1      	str	r1, [r4, #28]
87802b26:	653a      	str	r2, [r7, #80]	; 0x50
87802b28:	f001 fbda 	bl	878042e0 <pend_highest_pending_prio_get>
87802b2c:	f897 3025 	ldrb.w	r3, [r7, #37]	; 0x25
87802b30:	4283      	cmp	r3, r0
87802b32:	d903      	bls.n	87802b3c <tos_mutex_post+0xc8>
87802b34:	4601      	mov	r1, r0
87802b36:	4638      	mov	r0, r7
87802b38:	f000 fbaa 	bl	87803290 <tos_task_prio_change>
87802b3c:	2101      	movs	r1, #1
87802b3e:	4630      	mov	r0, r6
87802b40:	f001 fc5a 	bl	878043f8 <pend_wakeup_one>
87802b44:	4628      	mov	r0, r5
87802b46:	f002 f88d 	bl	87804c64 <tos_cpu_cpsr_restore>
87802b4a:	f001 fae9 	bl	87804120 <knl_sched>
87802b4e:	2000      	movs	r0, #0
87802b50:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802b52:	4639      	mov	r1, r7
87802b54:	4630      	mov	r0, r6
87802b56:	f000 fb9b 	bl	87803290 <tos_task_prio_change>
87802b5a:	230a      	movs	r3, #10
87802b5c:	f886 3058 	strb.w	r3, [r6, #88]	; 0x58
87802b60:	e7c8      	b.n	87802af4 <tos_mutex_post+0x80>
87802b62:	4628      	mov	r0, r5
87802b64:	f002 f87e 	bl	87804c64 <tos_cpu_cpsr_restore>
87802b68:	2000      	movs	r0, #0
87802b6a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87802b6c <readyqueue_is_prio_onlyone>:
87802b6c:	f242 13d0 	movw	r3, #8656	; 0x21d0
87802b70:	f2c8 7381 	movt	r3, #34689	; 0x8781
87802b74:	f853 2030 	ldr.w	r2, [r3, r0, lsl #3]
87802b78:	eb03 03c0 	add.w	r3, r3, r0, lsl #3
87802b7c:	6810      	ldr	r0, [r2, #0]
87802b7e:	1ac0      	subs	r0, r0, r3
87802b80:	fab0 f080 	clz	r0, r0
87802b84:	0940      	lsrs	r0, r0, #5
87802b86:	4770      	bx	lr

87802b88 <readyqueue_first_task_get>:
87802b88:	f242 13d0 	movw	r3, #8656	; 0x21d0
87802b8c:	f2c8 7381 	movt	r3, #34689	; 0x8781
87802b90:	f853 2030 	ldr.w	r2, [r3, r0, lsl #3]
87802b94:	eb03 03c0 	add.w	r3, r3, r0, lsl #3
87802b98:	429a      	cmp	r2, r3
87802b9a:	bf14      	ite	ne
87802b9c:	f1a2 0048 	subne.w	r0, r2, #72	; 0x48
87802ba0:	2000      	moveq	r0, #0
87802ba2:	4770      	bx	lr

87802ba4 <readyqueue_highest_ready_task_get>:
87802ba4:	f242 13d0 	movw	r3, #8656	; 0x21d0
87802ba8:	f2c8 7381 	movt	r3, #34689	; 0x8781
87802bac:	f893 2054 	ldrb.w	r2, [r3, #84]	; 0x54
87802bb0:	f853 0032 	ldr.w	r0, [r3, r2, lsl #3]
87802bb4:	3848      	subs	r0, #72	; 0x48
87802bb6:	4770      	bx	lr

87802bb8 <readyqueue_init>:
87802bb8:	f242 13d0 	movw	r3, #8656	; 0x21d0
87802bbc:	220a      	movs	r2, #10
87802bbe:	f2c8 7381 	movt	r3, #34689	; 0x8781
87802bc2:	f883 2054 	strb.w	r2, [r3, #84]	; 0x54
87802bc6:	4619      	mov	r1, r3
87802bc8:	f103 0250 	add.w	r2, r3, #80	; 0x50
87802bcc:	601b      	str	r3, [r3, #0]
87802bce:	605b      	str	r3, [r3, #4]
87802bd0:	3308      	adds	r3, #8
87802bd2:	4293      	cmp	r3, r2
87802bd4:	d1fa      	bne.n	87802bcc <readyqueue_init+0x14>
87802bd6:	2300      	movs	r3, #0
87802bd8:	650b      	str	r3, [r1, #80]	; 0x50
87802bda:	4770      	bx	lr

87802bdc <readyqueue_add_head>:
87802bdc:	f890 2025 	ldrb.w	r2, [r0, #37]	; 0x25
87802be0:	f242 13d0 	movw	r3, #8656	; 0x21d0
87802be4:	f2c8 7381 	movt	r3, #34689	; 0x8781
87802be8:	b5f0      	push	{r4, r5, r6, r7, lr}
87802bea:	f853 1032 	ldr.w	r1, [r3, r2, lsl #3]
87802bee:	eb03 05c2 	add.w	r5, r3, r2, lsl #3
87802bf2:	428d      	cmp	r5, r1
87802bf4:	d007      	beq.n	87802c06 <readyqueue_add_head+0x2a>
87802bf6:	f100 0448 	add.w	r4, r0, #72	; 0x48
87802bfa:	604c      	str	r4, [r1, #4]
87802bfc:	6481      	str	r1, [r0, #72]	; 0x48
87802bfe:	64c5      	str	r5, [r0, #76]	; 0x4c
87802c00:	f843 4032 	str.w	r4, [r3, r2, lsl #3]
87802c04:	bdf0      	pop	{r4, r5, r6, r7, pc}
87802c06:	0954      	lsrs	r4, r2, #5
87802c08:	ea6f 0e02 	mvn.w	lr, r2
87802c0c:	f893 c054 	ldrb.w	ip, [r3, #84]	; 0x54
87802c10:	f00e 0e1f 	and.w	lr, lr, #31
87802c14:	3414      	adds	r4, #20
87802c16:	2701      	movs	r7, #1
87802c18:	f853 6024 	ldr.w	r6, [r3, r4, lsl #2]
87802c1c:	fa07 f70e 	lsl.w	r7, r7, lr
87802c20:	4594      	cmp	ip, r2
87802c22:	bf88      	it	hi
87802c24:	f883 2054 	strbhi.w	r2, [r3, #84]	; 0x54
87802c28:	433e      	orrs	r6, r7
87802c2a:	f843 6024 	str.w	r6, [r3, r4, lsl #2]
87802c2e:	e7e2      	b.n	87802bf6 <readyqueue_add_head+0x1a>

87802c30 <readyqueue_add_tail>:
87802c30:	f890 1025 	ldrb.w	r1, [r0, #37]	; 0x25
87802c34:	f242 13d0 	movw	r3, #8656	; 0x21d0
87802c38:	f2c8 7381 	movt	r3, #34689	; 0x8781
87802c3c:	b5f0      	push	{r4, r5, r6, r7, lr}
87802c3e:	00ca      	lsls	r2, r1, #3
87802c40:	f853 5031 	ldr.w	r5, [r3, r1, lsl #3]
87802c44:	189c      	adds	r4, r3, r2
87802c46:	42ac      	cmp	r4, r5
87802c48:	d008      	beq.n	87802c5c <readyqueue_add_tail+0x2c>
87802c4a:	4413      	add	r3, r2
87802c4c:	f100 0148 	add.w	r1, r0, #72	; 0x48
87802c50:	685a      	ldr	r2, [r3, #4]
87802c52:	6059      	str	r1, [r3, #4]
87802c54:	6484      	str	r4, [r0, #72]	; 0x48
87802c56:	64c2      	str	r2, [r0, #76]	; 0x4c
87802c58:	6011      	str	r1, [r2, #0]
87802c5a:	bdf0      	pop	{r4, r5, r6, r7, pc}
87802c5c:	094d      	lsrs	r5, r1, #5
87802c5e:	ea6f 0e01 	mvn.w	lr, r1
87802c62:	f893 c054 	ldrb.w	ip, [r3, #84]	; 0x54
87802c66:	f00e 0e1f 	and.w	lr, lr, #31
87802c6a:	3514      	adds	r5, #20
87802c6c:	2701      	movs	r7, #1
87802c6e:	f853 6025 	ldr.w	r6, [r3, r5, lsl #2]
87802c72:	fa07 f70e 	lsl.w	r7, r7, lr
87802c76:	458c      	cmp	ip, r1
87802c78:	bf88      	it	hi
87802c7a:	f883 1054 	strbhi.w	r1, [r3, #84]	; 0x54
87802c7e:	433e      	orrs	r6, r7
87802c80:	f843 6025 	str.w	r6, [r3, r5, lsl #2]
87802c84:	e7e1      	b.n	87802c4a <readyqueue_add_tail+0x1a>
87802c86:	bf00      	nop

87802c88 <readyqueue_add>:
87802c88:	f640 33a0 	movw	r3, #2976	; 0xba0
87802c8c:	f890 2025 	ldrb.w	r2, [r0, #37]	; 0x25
87802c90:	f2c8 7381 	movt	r3, #34689	; 0x8781
87802c94:	681b      	ldr	r3, [r3, #0]
87802c96:	f893 3025 	ldrb.w	r3, [r3, #37]	; 0x25
87802c9a:	429a      	cmp	r2, r3
87802c9c:	d001      	beq.n	87802ca2 <readyqueue_add+0x1a>
87802c9e:	f7ff bf9d 	b.w	87802bdc <readyqueue_add_head>
87802ca2:	f7ff bfc5 	b.w	87802c30 <readyqueue_add_tail>
87802ca6:	bf00      	nop

87802ca8 <readyqueue_remove>:
87802ca8:	b538      	push	{r3, r4, r5, lr}
87802caa:	f242 15d0 	movw	r5, #8656	; 0x21d0
87802cae:	f890 2025 	ldrb.w	r2, [r0, #37]	; 0x25
87802cb2:	f2c8 7581 	movt	r5, #34689	; 0x8781
87802cb6:	6cc3      	ldr	r3, [r0, #76]	; 0x4c
87802cb8:	6c81      	ldr	r1, [r0, #72]	; 0x48
87802cba:	eb05 00c2 	add.w	r0, r5, r2, lsl #3
87802cbe:	604b      	str	r3, [r1, #4]
87802cc0:	6019      	str	r1, [r3, #0]
87802cc2:	f855 3032 	ldr.w	r3, [r5, r2, lsl #3]
87802cc6:	4283      	cmp	r3, r0
87802cc8:	d018      	beq.n	87802cfc <readyqueue_remove+0x54>
87802cca:	f895 1054 	ldrb.w	r1, [r5, #84]	; 0x54
87802cce:	f242 13d0 	movw	r3, #8656	; 0x21d0
87802cd2:	f2c8 7381 	movt	r3, #34689	; 0x8781
87802cd6:	4291      	cmp	r1, r2
87802cd8:	d000      	beq.n	87802cdc <readyqueue_remove+0x34>
87802cda:	bd38      	pop	{r3, r4, r5, pc}
87802cdc:	6d18      	ldr	r0, [r3, #80]	; 0x50
87802cde:	b9d8      	cbnz	r0, 87802d18 <readyqueue_remove+0x70>
87802ce0:	3350      	adds	r3, #80	; 0x50
87802ce2:	4604      	mov	r4, r0
87802ce4:	f853 0f04 	ldr.w	r0, [r3, #4]!
87802ce8:	3420      	adds	r4, #32
87802cea:	b2e4      	uxtb	r4, r4
87802cec:	2800      	cmp	r0, #0
87802cee:	d0f9      	beq.n	87802ce4 <readyqueue_remove+0x3c>
87802cf0:	f001 ff92 	bl	87804c18 <tos_cpu_clz>
87802cf4:	4420      	add	r0, r4
87802cf6:	f885 0054 	strb.w	r0, [r5, #84]	; 0x54
87802cfa:	bd38      	pop	{r3, r4, r5, pc}
87802cfc:	0953      	lsrs	r3, r2, #5
87802cfe:	43d4      	mvns	r4, r2
87802d00:	f004 041f 	and.w	r4, r4, #31
87802d04:	3314      	adds	r3, #20
87802d06:	2001      	movs	r0, #1
87802d08:	f855 1023 	ldr.w	r1, [r5, r3, lsl #2]
87802d0c:	40a0      	lsls	r0, r4
87802d0e:	ea21 0100 	bic.w	r1, r1, r0
87802d12:	f845 1023 	str.w	r1, [r5, r3, lsl #2]
87802d16:	e7d8      	b.n	87802cca <readyqueue_remove+0x22>
87802d18:	2400      	movs	r4, #0
87802d1a:	e7e9      	b.n	87802cf0 <readyqueue_remove+0x48>

87802d1c <readyqueue_move_head_to_tail>:
87802d1c:	f242 13d0 	movw	r3, #8656	; 0x21d0
87802d20:	f2c8 7381 	movt	r3, #34689	; 0x8781
87802d24:	f853 2030 	ldr.w	r2, [r3, r0, lsl #3]
87802d28:	eb03 03c0 	add.w	r3, r3, r0, lsl #3
87802d2c:	4293      	cmp	r3, r2
87802d2e:	d00b      	beq.n	87802d48 <readyqueue_move_head_to_tail+0x2c>
87802d30:	b410      	push	{r4}
87802d32:	6850      	ldr	r0, [r2, #4]
87802d34:	6814      	ldr	r4, [r2, #0]
87802d36:	6060      	str	r0, [r4, #4]
87802d38:	6859      	ldr	r1, [r3, #4]
87802d3a:	6004      	str	r4, [r0, #0]
87802d3c:	f85d 4b04 	ldr.w	r4, [sp], #4
87802d40:	605a      	str	r2, [r3, #4]
87802d42:	6051      	str	r1, [r2, #4]
87802d44:	6013      	str	r3, [r2, #0]
87802d46:	600a      	str	r2, [r1, #0]
87802d48:	4770      	bx	lr
87802d4a:	bf00      	nop

87802d4c <msg_q_do_post>:
87802d4c:	b5f0      	push	{r4, r5, r6, r7, lr}
87802d4e:	b083      	sub	sp, #12
87802d50:	9101      	str	r1, [sp, #4]
87802d52:	b148      	cbz	r0, 87802d68 <msg_q_do_post+0x1c>
87802d54:	f645 33ee 	movw	r3, #23534	; 0x5bee
87802d58:	6841      	ldr	r1, [r0, #4]
87802d5a:	4299      	cmp	r1, r3
87802d5c:	bf18      	it	ne
87802d5e:	f240 404d 	movwne	r0, #1101	; 0x44d
87802d62:	d005      	beq.n	87802d70 <msg_q_do_post+0x24>
87802d64:	b003      	add	sp, #12
87802d66:	bdf0      	pop	{r4, r5, r6, r7, pc}
87802d68:	f240 404c 	movw	r0, #1100	; 0x44c
87802d6c:	b003      	add	sp, #12
87802d6e:	bdf0      	pop	{r4, r5, r6, r7, pc}
87802d70:	f100 0708 	add.w	r7, r0, #8
87802d74:	4604      	mov	r4, r0
87802d76:	4615      	mov	r5, r2
87802d78:	f001 ff72 	bl	87804c60 <tos_cpu_cpsr_save>
87802d7c:	4606      	mov	r6, r0
87802d7e:	4638      	mov	r0, r7
87802d80:	f001 fad2 	bl	87804328 <pend_is_nopending>
87802d84:	b9e8      	cbnz	r0, 87802dc2 <msg_q_do_post+0x76>
87802d86:	68a3      	ldr	r3, [r4, #8]
87802d88:	b345      	cbz	r5, 87802ddc <msg_q_do_post+0x90>
87802d8a:	681c      	ldr	r4, [r3, #0]
87802d8c:	429f      	cmp	r7, r3
87802d8e:	f1a3 0248 	sub.w	r2, r3, #72	; 0x48
87802d92:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
87802d96:	d00d      	beq.n	87802db4 <msg_q_do_post+0x68>
87802d98:	9b01      	ldr	r3, [sp, #4]
87802d9a:	4610      	mov	r0, r2
87802d9c:	2101      	movs	r1, #1
87802d9e:	6653      	str	r3, [r2, #100]	; 0x64
87802da0:	f001 faea 	bl	87804378 <pend_task_wakeup>
87802da4:	4623      	mov	r3, r4
87802da6:	4622      	mov	r2, r4
87802da8:	f853 4f48 	ldr.w	r4, [r3, #72]!
87802dac:	429f      	cmp	r7, r3
87802dae:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
87802db2:	d1f1      	bne.n	87802d98 <msg_q_do_post+0x4c>
87802db4:	4630      	mov	r0, r6
87802db6:	f001 ff55 	bl	87804c64 <tos_cpu_cpsr_restore>
87802dba:	f001 f9b1 	bl	87804120 <knl_sched>
87802dbe:	2000      	movs	r0, #0
87802dc0:	e7d0      	b.n	87802d64 <msg_q_do_post+0x18>
87802dc2:	2204      	movs	r2, #4
87802dc4:	f104 0010 	add.w	r0, r4, #16
87802dc8:	eb0d 0102 	add.w	r1, sp, r2
87802dcc:	f001 fcac 	bl	87804728 <tos_ring_q_enqueue>
87802dd0:	4604      	mov	r4, r0
87802dd2:	4630      	mov	r0, r6
87802dd4:	f001 ff46 	bl	87804c64 <tos_cpu_cpsr_restore>
87802dd8:	4620      	mov	r0, r4
87802dda:	e7c3      	b.n	87802d64 <msg_q_do_post+0x18>
87802ddc:	9a01      	ldr	r2, [sp, #4]
87802dde:	2101      	movs	r1, #1
87802de0:	f1a3 0048 	sub.w	r0, r3, #72	; 0x48
87802de4:	61da      	str	r2, [r3, #28]
87802de6:	f001 fac7 	bl	87804378 <pend_task_wakeup>
87802dea:	e7e3      	b.n	87802db4 <msg_q_do_post+0x68>

87802dec <tos_msg_q_create>:
87802dec:	2900      	cmp	r1, #0
87802dee:	bf18      	it	ne
87802df0:	2800      	cmpne	r0, #0
87802df2:	d013      	beq.n	87802e1c <tos_msg_q_create+0x30>
87802df4:	b538      	push	{r3, r4, r5, lr}
87802df6:	4604      	mov	r4, r0
87802df8:	2304      	movs	r3, #4
87802dfa:	3010      	adds	r0, #16
87802dfc:	f001 fc1a 	bl	87804634 <tos_ring_q_create>
87802e00:	4605      	mov	r5, r0
87802e02:	b100      	cbz	r0, 87802e06 <tos_msg_q_create+0x1a>
87802e04:	bd38      	pop	{r3, r4, r5, pc}
87802e06:	f104 0008 	add.w	r0, r4, #8
87802e0a:	f001 fa85 	bl	87804318 <pend_object_init>
87802e0e:	f645 32ee 	movw	r2, #23534	; 0x5bee
87802e12:	2301      	movs	r3, #1
87802e14:	4628      	mov	r0, r5
87802e16:	6062      	str	r2, [r4, #4]
87802e18:	6023      	str	r3, [r4, #0]
87802e1a:	bd38      	pop	{r3, r4, r5, pc}
87802e1c:	f240 404c 	movw	r0, #1100	; 0x44c
87802e20:	4770      	bx	lr
87802e22:	bf00      	nop

87802e24 <tos_msg_q_destroy>:
87802e24:	b170      	cbz	r0, 87802e44 <tos_msg_q_destroy+0x20>
87802e26:	6842      	ldr	r2, [r0, #4]
87802e28:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87802e2a:	f645 33ee 	movw	r3, #23534	; 0x5bee
87802e2e:	429a      	cmp	r2, r3
87802e30:	d002      	beq.n	87802e38 <tos_msg_q_destroy+0x14>
87802e32:	f240 404d 	movw	r0, #1101	; 0x44d
87802e36:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802e38:	6803      	ldr	r3, [r0, #0]
87802e3a:	2b01      	cmp	r3, #1
87802e3c:	d005      	beq.n	87802e4a <tos_msg_q_destroy+0x26>
87802e3e:	f240 404e 	movw	r0, #1102	; 0x44e
87802e42:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802e44:	f240 404c 	movw	r0, #1100	; 0x44c
87802e48:	4770      	bx	lr
87802e4a:	4604      	mov	r4, r0
87802e4c:	f001 ff08 	bl	87804c60 <tos_cpu_cpsr_save>
87802e50:	4606      	mov	r6, r0
87802e52:	f104 0010 	add.w	r0, r4, #16
87802e56:	f001 fc09 	bl	8780466c <tos_ring_q_destroy>
87802e5a:	4605      	mov	r5, r0
87802e5c:	b988      	cbnz	r0, 87802e82 <tos_msg_q_destroy+0x5e>
87802e5e:	f104 0708 	add.w	r7, r4, #8
87802e62:	2103      	movs	r1, #3
87802e64:	4638      	mov	r0, r7
87802e66:	f001 facb 	bl	87804400 <pend_wakeup_all>
87802e6a:	4638      	mov	r0, r7
87802e6c:	f001 fa58 	bl	87804320 <pend_object_deinit>
87802e70:	4630      	mov	r0, r6
87802e72:	6065      	str	r5, [r4, #4]
87802e74:	6025      	str	r5, [r4, #0]
87802e76:	f001 fef5 	bl	87804c64 <tos_cpu_cpsr_restore>
87802e7a:	f001 f951 	bl	87804120 <knl_sched>
87802e7e:	4628      	mov	r0, r5
87802e80:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802e82:	4630      	mov	r0, r6
87802e84:	f001 feee 	bl	87804c64 <tos_cpu_cpsr_restore>
87802e88:	4628      	mov	r0, r5
87802e8a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87802e8c <tos_msg_q_create_dyn>:
87802e8c:	b198      	cbz	r0, 87802eb6 <tos_msg_q_create_dyn+0x2a>
87802e8e:	b538      	push	{r3, r4, r5, lr}
87802e90:	2204      	movs	r2, #4
87802e92:	4604      	mov	r4, r0
87802e94:	3010      	adds	r0, #16
87802e96:	f001 fc07 	bl	878046a8 <tos_ring_q_create_dyn>
87802e9a:	4605      	mov	r5, r0
87802e9c:	b100      	cbz	r0, 87802ea0 <tos_msg_q_create_dyn+0x14>
87802e9e:	bd38      	pop	{r3, r4, r5, pc}
87802ea0:	f104 0008 	add.w	r0, r4, #8
87802ea4:	f001 fa38 	bl	87804318 <pend_object_init>
87802ea8:	f645 32ee 	movw	r2, #23534	; 0x5bee
87802eac:	2302      	movs	r3, #2
87802eae:	4628      	mov	r0, r5
87802eb0:	6062      	str	r2, [r4, #4]
87802eb2:	6023      	str	r3, [r4, #0]
87802eb4:	bd38      	pop	{r3, r4, r5, pc}
87802eb6:	f240 404c 	movw	r0, #1100	; 0x44c
87802eba:	4770      	bx	lr

87802ebc <tos_msg_q_destroy_dyn>:
87802ebc:	b170      	cbz	r0, 87802edc <tos_msg_q_destroy_dyn+0x20>
87802ebe:	6842      	ldr	r2, [r0, #4]
87802ec0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87802ec2:	f645 33ee 	movw	r3, #23534	; 0x5bee
87802ec6:	429a      	cmp	r2, r3
87802ec8:	d002      	beq.n	87802ed0 <tos_msg_q_destroy_dyn+0x14>
87802eca:	f240 404d 	movw	r0, #1101	; 0x44d
87802ece:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802ed0:	6803      	ldr	r3, [r0, #0]
87802ed2:	2b02      	cmp	r3, #2
87802ed4:	d005      	beq.n	87802ee2 <tos_msg_q_destroy_dyn+0x26>
87802ed6:	f240 404e 	movw	r0, #1102	; 0x44e
87802eda:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802edc:	f240 404c 	movw	r0, #1100	; 0x44c
87802ee0:	4770      	bx	lr
87802ee2:	4604      	mov	r4, r0
87802ee4:	f001 febc 	bl	87804c60 <tos_cpu_cpsr_save>
87802ee8:	4606      	mov	r6, r0
87802eea:	f104 0010 	add.w	r0, r4, #16
87802eee:	f001 fbf9 	bl	878046e4 <tos_ring_q_destroy_dyn>
87802ef2:	4605      	mov	r5, r0
87802ef4:	b988      	cbnz	r0, 87802f1a <tos_msg_q_destroy_dyn+0x5e>
87802ef6:	f104 0708 	add.w	r7, r4, #8
87802efa:	2103      	movs	r1, #3
87802efc:	4638      	mov	r0, r7
87802efe:	f001 fa7f 	bl	87804400 <pend_wakeup_all>
87802f02:	4638      	mov	r0, r7
87802f04:	f001 fa0c 	bl	87804320 <pend_object_deinit>
87802f08:	4630      	mov	r0, r6
87802f0a:	6065      	str	r5, [r4, #4]
87802f0c:	6025      	str	r5, [r4, #0]
87802f0e:	f001 fea9 	bl	87804c64 <tos_cpu_cpsr_restore>
87802f12:	f001 f905 	bl	87804120 <knl_sched>
87802f16:	4628      	mov	r0, r5
87802f18:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87802f1a:	4630      	mov	r0, r6
87802f1c:	f001 fea2 	bl	87804c64 <tos_cpu_cpsr_restore>
87802f20:	4628      	mov	r0, r5
87802f22:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87802f24 <tos_msg_q_flush>:
87802f24:	b138      	cbz	r0, 87802f36 <tos_msg_q_flush+0x12>
87802f26:	6842      	ldr	r2, [r0, #4]
87802f28:	f645 33ee 	movw	r3, #23534	; 0x5bee
87802f2c:	429a      	cmp	r2, r3
87802f2e:	d005      	beq.n	87802f3c <tos_msg_q_flush+0x18>
87802f30:	f240 404d 	movw	r0, #1101	; 0x44d
87802f34:	4770      	bx	lr
87802f36:	f240 404c 	movw	r0, #1100	; 0x44c
87802f3a:	4770      	bx	lr
87802f3c:	3010      	adds	r0, #16
87802f3e:	f001 bc7f 	b.w	87804840 <tos_ring_q_flush>
87802f42:	bf00      	nop

87802f44 <tos_msg_q_pend>:
87802f44:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
87802f48:	4604      	mov	r4, r0
87802f4a:	460d      	mov	r5, r1
87802f4c:	4690      	mov	r8, r2
87802f4e:	4699      	mov	r9, r3
87802f50:	f001 f946 	bl	878041e0 <knl_is_inirq>
87802f54:	b988      	cbnz	r0, 87802f7a <tos_msg_q_pend+0x36>
87802f56:	2d00      	cmp	r5, #0
87802f58:	bf18      	it	ne
87802f5a:	2c00      	cmpne	r4, #0
87802f5c:	bf0c      	ite	eq
87802f5e:	2701      	moveq	r7, #1
87802f60:	2700      	movne	r7, #0
87802f62:	d00f      	beq.n	87802f84 <tos_msg_q_pend+0x40>
87802f64:	6862      	ldr	r2, [r4, #4]
87802f66:	f645 33ee 	movw	r3, #23534	; 0x5bee
87802f6a:	429a      	cmp	r2, r3
87802f6c:	bf18      	it	ne
87802f6e:	f240 464d 	movwne	r6, #1101	; 0x44d
87802f72:	d00a      	beq.n	87802f8a <tos_msg_q_pend+0x46>
87802f74:	4630      	mov	r0, r6
87802f76:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
87802f7a:	f44f 76c8 	mov.w	r6, #400	; 0x190
87802f7e:	4630      	mov	r0, r6
87802f80:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
87802f84:	f240 464c 	movw	r6, #1100	; 0x44c
87802f88:	e7f4      	b.n	87802f74 <tos_msg_q_pend+0x30>
87802f8a:	f001 fe69 	bl	87804c60 <tos_cpu_cpsr_save>
87802f8e:	463a      	mov	r2, r7
87802f90:	4682      	mov	sl, r0
87802f92:	4629      	mov	r1, r5
87802f94:	f104 0010 	add.w	r0, r4, #16
87802f98:	f001 fc0e 	bl	878047b8 <tos_ring_q_dequeue>
87802f9c:	4606      	mov	r6, r0
87802f9e:	b148      	cbz	r0, 87802fb4 <tos_msg_q_pend+0x70>
87802fa0:	ea58 0309 	orrs.w	r3, r8, r9
87802fa4:	d10a      	bne.n	87802fbc <tos_msg_q_pend+0x78>
87802fa6:	602f      	str	r7, [r5, #0]
87802fa8:	4650      	mov	r0, sl
87802faa:	f001 fe5b 	bl	87804c64 <tos_cpu_cpsr_restore>
87802fae:	f44f 6696 	mov.w	r6, #1200	; 0x4b0
87802fb2:	e7df      	b.n	87802f74 <tos_msg_q_pend+0x30>
87802fb4:	4650      	mov	r0, sl
87802fb6:	f001 fe55 	bl	87804c64 <tos_cpu_cpsr_restore>
87802fba:	e7db      	b.n	87802f74 <tos_msg_q_pend+0x30>
87802fbc:	f001 f906 	bl	878041cc <knl_is_sched_locked>
87802fc0:	b128      	cbz	r0, 87802fce <tos_msg_q_pend+0x8a>
87802fc2:	4650      	mov	r0, sl
87802fc4:	f240 46b1 	movw	r6, #1201	; 0x4b1
87802fc8:	f001 fe4c 	bl	87804c64 <tos_cpu_cpsr_restore>
87802fcc:	e7d2      	b.n	87802f74 <tos_msg_q_pend+0x30>
87802fce:	f640 37a0 	movw	r7, #2976	; 0xba0
87802fd2:	4642      	mov	r2, r8
87802fd4:	f2c8 7781 	movt	r7, #34689	; 0x8781
87802fd8:	464b      	mov	r3, r9
87802fda:	f104 0108 	add.w	r1, r4, #8
87802fde:	6838      	ldr	r0, [r7, #0]
87802fe0:	f001 f9ee 	bl	878043c0 <pend_task_block>
87802fe4:	4650      	mov	r0, sl
87802fe6:	f001 fe3d 	bl	87804c64 <tos_cpu_cpsr_restore>
87802fea:	f001 f899 	bl	87804120 <knl_sched>
87802fee:	683b      	ldr	r3, [r7, #0]
87802ff0:	6e18      	ldr	r0, [r3, #96]	; 0x60
87802ff2:	f001 f9a9 	bl	87804348 <pend_state2errno>
87802ff6:	4606      	mov	r6, r0
87802ff8:	2800      	cmp	r0, #0
87802ffa:	d1bb      	bne.n	87802f74 <tos_msg_q_pend+0x30>
87802ffc:	683b      	ldr	r3, [r7, #0]
87802ffe:	4606      	mov	r6, r0
87803000:	6e5b      	ldr	r3, [r3, #100]	; 0x64
87803002:	602b      	str	r3, [r5, #0]
87803004:	683b      	ldr	r3, [r7, #0]
87803006:	6658      	str	r0, [r3, #100]	; 0x64
87803008:	e7b4      	b.n	87802f74 <tos_msg_q_pend+0x30>
8780300a:	bf00      	nop

8780300c <tos_msg_q_post>:
8780300c:	2200      	movs	r2, #0
8780300e:	e69d      	b.n	87802d4c <msg_q_do_post>

87803010 <tos_msg_q_post_all>:
87803010:	2201      	movs	r2, #1
87803012:	e69b      	b.n	87802d4c <msg_q_do_post>

87803014 <tos_task_create>:
87803014:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87803018:	b083      	sub	sp, #12
8780301a:	4604      	mov	r4, r0
8780301c:	4689      	mov	r9, r1
8780301e:	4615      	mov	r5, r2
87803020:	4698      	mov	r8, r3
87803022:	f89d 7030 	ldrb.w	r7, [sp, #48]	; 0x30
87803026:	9e0d      	ldr	r6, [sp, #52]	; 0x34
87803028:	f001 f8da 	bl	878041e0 <knl_is_inirq>
8780302c:	2800      	cmp	r0, #0
8780302e:	d179      	bne.n	87803124 <tos_task_create+0x110>
87803030:	2e00      	cmp	r6, #0
87803032:	bf18      	it	ne
87803034:	2d00      	cmpne	r5, #0
87803036:	bf0c      	ite	eq
87803038:	2301      	moveq	r3, #1
8780303a:	2300      	movne	r3, #0
8780303c:	2c00      	cmp	r4, #0
8780303e:	bf08      	it	eq
87803040:	f043 0301 	orreq.w	r3, r3, #1
87803044:	2b00      	cmp	r3, #0
87803046:	d172      	bne.n	8780312e <tos_task_create+0x11a>
87803048:	68a2      	ldr	r2, [r4, #8]
8780304a:	f64d 23d8 	movw	r3, #56024	; 0xdad8
8780304e:	429a      	cmp	r2, r3
87803050:	bf08      	it	eq
87803052:	f240 706c 	movweq	r0, #1900	; 0x76c
87803056:	d05c      	beq.n	87803112 <tos_task_create+0xfe>
87803058:	9b0e      	ldr	r3, [sp, #56]	; 0x38
8780305a:	2b3f      	cmp	r3, #63	; 0x3f
8780305c:	d96a      	bls.n	87803134 <tos_task_create+0x120>
8780305e:	2f09      	cmp	r7, #9
87803060:	d06b      	beq.n	8780313a <tos_task_create+0x126>
87803062:	d86f      	bhi.n	87803144 <tos_task_create+0x130>
87803064:	f64d 6080 	movw	r0, #56960	; 0xde80
87803068:	f104 0e40 	add.w	lr, r4, #64	; 0x40
8780306c:	f2c8 7080 	movt	r0, #34688	; 0x8780
87803070:	f04f 0a00 	mov.w	sl, #0
87803074:	6801      	ldr	r1, [r0, #0]
87803076:	f104 0248 	add.w	r2, r4, #72	; 0x48
8780307a:	f8c4 e040 	str.w	lr, [r4, #64]	; 0x40
8780307e:	f104 0350 	add.w	r3, r4, #80	; 0x50
87803082:	f8c4 e044 	str.w	lr, [r4, #68]	; 0x44
87803086:	f04f 0b0a 	mov.w	fp, #10
8780308a:	f104 0e30 	add.w	lr, r4, #48	; 0x30
8780308e:	64a2      	str	r2, [r4, #72]	; 0x48
87803090:	64e2      	str	r2, [r4, #76]	; 0x4c
87803092:	f64d 2cd8 	movw	ip, #56024	; 0xdad8
87803096:	6523      	str	r3, [r4, #80]	; 0x50
87803098:	f243 2255 	movw	r2, #12885	; 0x3255
8780309c:	6563      	str	r3, [r4, #84]	; 0x54
8780309e:	f2c8 7280 	movt	r2, #34688	; 0x8780
878030a2:	f884 b058 	strb.w	fp, [r4, #88]	; 0x58
878030a6:	4633      	mov	r3, r6
878030a8:	f8c4 a060 	str.w	sl, [r4, #96]	; 0x60
878030ac:	f8c4 a05c 	str.w	sl, [r4, #92]	; 0x5c
878030b0:	f8c4 a064 	str.w	sl, [r4, #100]	; 0x64
878030b4:	f8c4 a068 	str.w	sl, [r4, #104]	; 0x68
878030b8:	f8c4 a06c 	str.w	sl, [r4, #108]	; 0x6c
878030bc:	f8c1 e004 	str.w	lr, [r1, #4]
878030c0:	6321      	str	r1, [r4, #48]	; 0x30
878030c2:	990e      	ldr	r1, [sp, #56]	; 0x38
878030c4:	6360      	str	r0, [r4, #52]	; 0x34
878030c6:	9100      	str	r1, [sp, #0]
878030c8:	4641      	mov	r1, r8
878030ca:	f8c0 e000 	str.w	lr, [r0]
878030ce:	4628      	mov	r0, r5
878030d0:	f8c4 c008 	str.w	ip, [r4, #8]
878030d4:	f001 fdf8 	bl	87804cc8 <cpu_task_stk_init>
878030d8:	9b0e      	ldr	r3, [sp, #56]	; 0x38
878030da:	4649      	mov	r1, r9
878030dc:	2210      	movs	r2, #16
878030de:	61e5      	str	r5, [r4, #28]
878030e0:	6020      	str	r0, [r4, #0]
878030e2:	f104 000c 	add.w	r0, r4, #12
878030e6:	62e3      	str	r3, [r4, #44]	; 0x2c
878030e8:	f8c4 8020 	str.w	r8, [r4, #32]
878030ec:	f884 7025 	strb.w	r7, [r4, #37]	; 0x25
878030f0:	62a6      	str	r6, [r4, #40]	; 0x28
878030f2:	f002 f82b 	bl	8780514c <strncpy>
878030f6:	f001 fdb3 	bl	87804c60 <tos_cpu_cpsr_save>
878030fa:	f884 a024 	strb.w	sl, [r4, #36]	; 0x24
878030fe:	4605      	mov	r5, r0
87803100:	4620      	mov	r0, r4
87803102:	f7ff fd95 	bl	87802c30 <readyqueue_add_tail>
87803106:	4628      	mov	r0, r5
87803108:	f001 fdac 	bl	87804c64 <tos_cpu_cpsr_restore>
8780310c:	f000 fffc 	bl	87804108 <tos_knl_is_running>
87803110:	b910      	cbnz	r0, 87803118 <tos_task_create+0x104>
87803112:	b003      	add	sp, #12
87803114:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87803118:	f001 f802 	bl	87804120 <knl_sched>
8780311c:	4650      	mov	r0, sl
8780311e:	b003      	add	sp, #12
87803120:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87803124:	f44f 70c8 	mov.w	r0, #400	; 0x190
87803128:	b003      	add	sp, #12
8780312a:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
8780312e:	f240 404c 	movw	r0, #1100	; 0x44c
87803132:	e7ee      	b.n	87803112 <tos_task_create+0xfe>
87803134:	f240 7074 	movw	r0, #1908	; 0x774
87803138:	e7eb      	b.n	87803112 <tos_task_create+0xfe>
8780313a:	4620      	mov	r0, r4
8780313c:	f001 f85a 	bl	878041f4 <knl_is_idle>
87803140:	2800      	cmp	r0, #0
87803142:	d18f      	bne.n	87803064 <tos_task_create+0x50>
87803144:	f240 706f 	movw	r0, #1903	; 0x76f
87803148:	e7e3      	b.n	87803112 <tos_task_create+0xfe>
8780314a:	bf00      	nop

8780314c <tos_task_destroy>:
8780314c:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87803150:	4604      	mov	r4, r0
87803152:	f001 f845 	bl	878041e0 <knl_is_inirq>
87803156:	b948      	cbnz	r0, 8780316c <tos_task_destroy+0x20>
87803158:	b164      	cbz	r4, 87803174 <tos_task_destroy+0x28>
8780315a:	68a2      	ldr	r2, [r4, #8]
8780315c:	f64d 23d8 	movw	r3, #56024	; 0xdad8
87803160:	429a      	cmp	r2, r3
87803162:	d00d      	beq.n	87803180 <tos_task_destroy+0x34>
87803164:	f240 404d 	movw	r0, #1101	; 0x44d
87803168:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780316c:	f44f 70c8 	mov.w	r0, #400	; 0x190
87803170:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803174:	f640 33a0 	movw	r3, #2976	; 0xba0
87803178:	f2c8 7381 	movt	r3, #34689	; 0x8781
8780317c:	681c      	ldr	r4, [r3, #0]
8780317e:	e7ec      	b.n	8780315a <tos_task_destroy+0xe>
87803180:	4620      	mov	r0, r4
87803182:	f001 f841 	bl	87804208 <knl_is_self>
87803186:	b130      	cbz	r0, 87803196 <tos_task_destroy+0x4a>
87803188:	f001 f820 	bl	878041cc <knl_is_sched_locked>
8780318c:	b118      	cbz	r0, 87803196 <tos_task_destroy+0x4a>
8780318e:	f240 60a4 	movw	r0, #1700	; 0x6a4
87803192:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803196:	4620      	mov	r0, r4
87803198:	f001 f82c 	bl	878041f4 <knl_is_idle>
8780319c:	b118      	cbz	r0, 878031a6 <tos_task_destroy+0x5a>
8780319e:	f240 706d 	movw	r0, #1901	; 0x76d
878031a2:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878031a6:	f001 fd5b 	bl	87804c60 <tos_cpu_cpsr_save>
878031aa:	4607      	mov	r7, r0
878031ac:	6d20      	ldr	r0, [r4, #80]	; 0x50
878031ae:	f104 0650 	add.w	r6, r4, #80	; 0x50
878031b2:	4286      	cmp	r6, r0
878031b4:	d00c      	beq.n	878031d0 <tos_task_destroy+0x84>
878031b6:	6805      	ldr	r5, [r0, #0]
878031b8:	381c      	subs	r0, #28
878031ba:	3d1c      	subs	r5, #28
878031bc:	f7ff fb38 	bl	87802830 <mutex_release>
878031c0:	462b      	mov	r3, r5
878031c2:	4628      	mov	r0, r5
878031c4:	f853 5f1c 	ldr.w	r5, [r3, #28]!
878031c8:	429e      	cmp	r6, r3
878031ca:	f1a5 051c 	sub.w	r5, r5, #28
878031ce:	d1f5      	bne.n	878031bc <tos_task_destroy+0x70>
878031d0:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
878031d4:	b373      	cbz	r3, 87803234 <tos_task_destroy+0xe8>
878031d6:	07da      	lsls	r2, r3, #31
878031d8:	d436      	bmi.n	87803248 <tos_task_destroy+0xfc>
878031da:	079b      	lsls	r3, r3, #30
878031dc:	d430      	bmi.n	87803240 <tos_task_destroy+0xf4>
878031de:	4623      	mov	r3, r4
878031e0:	f8d4 e034 	ldr.w	lr, [r4, #52]	; 0x34
878031e4:	f853 cf30 	ldr.w	ip, [r3, #48]!
878031e8:	2500      	movs	r5, #0
878031ea:	f104 0140 	add.w	r1, r4, #64	; 0x40
878031ee:	f104 0248 	add.w	r2, r4, #72	; 0x48
878031f2:	4638      	mov	r0, r7
878031f4:	f04f 080a 	mov.w	r8, #10
878031f8:	2708      	movs	r7, #8
878031fa:	f8cc e004 	str.w	lr, [ip, #4]
878031fe:	f8ce c000 	str.w	ip, [lr]
87803202:	6323      	str	r3, [r4, #48]	; 0x30
87803204:	6363      	str	r3, [r4, #52]	; 0x34
87803206:	6421      	str	r1, [r4, #64]	; 0x40
87803208:	6461      	str	r1, [r4, #68]	; 0x44
8780320a:	64a2      	str	r2, [r4, #72]	; 0x48
8780320c:	64e2      	str	r2, [r4, #76]	; 0x4c
8780320e:	6526      	str	r6, [r4, #80]	; 0x50
87803210:	6566      	str	r6, [r4, #84]	; 0x54
87803212:	f884 8058 	strb.w	r8, [r4, #88]	; 0x58
87803216:	6625      	str	r5, [r4, #96]	; 0x60
87803218:	65e5      	str	r5, [r4, #92]	; 0x5c
8780321a:	6665      	str	r5, [r4, #100]	; 0x64
8780321c:	66a5      	str	r5, [r4, #104]	; 0x68
8780321e:	66e5      	str	r5, [r4, #108]	; 0x6c
87803220:	60a5      	str	r5, [r4, #8]
87803222:	f884 7024 	strb.w	r7, [r4, #36]	; 0x24
87803226:	f001 fd1d 	bl	87804c64 <tos_cpu_cpsr_restore>
8780322a:	f000 ff79 	bl	87804120 <knl_sched>
8780322e:	4628      	mov	r0, r5
87803230:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803234:	4620      	mov	r0, r4
87803236:	f7ff fd37 	bl	87802ca8 <readyqueue_remove>
8780323a:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
8780323e:	e7ca      	b.n	878031d6 <tos_task_destroy+0x8a>
87803240:	4620      	mov	r0, r4
87803242:	f001 f85b 	bl	878042fc <pend_list_remove>
87803246:	e7ca      	b.n	878031de <tos_task_destroy+0x92>
87803248:	4620      	mov	r0, r4
8780324a:	f7ff f801 	bl	87802250 <tick_list_remove>
8780324e:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
87803252:	e7c2      	b.n	878031da <tos_task_destroy+0x8e>

87803254 <task_exit>:
87803254:	2000      	movs	r0, #0
87803256:	f7ff bf79 	b.w	8780314c <tos_task_destroy>
8780325a:	bf00      	nop

8780325c <tos_task_yield>:
8780325c:	b538      	push	{r3, r4, r5, lr}
8780325e:	f000 ffbf 	bl	878041e0 <knl_is_inirq>
87803262:	b100      	cbz	r0, 87803266 <tos_task_yield+0xa>
87803264:	bd38      	pop	{r3, r4, r5, pc}
87803266:	f640 34a0 	movw	r4, #2976	; 0xba0
8780326a:	f2c8 7481 	movt	r4, #34689	; 0x8781
8780326e:	f001 fcf7 	bl	87804c60 <tos_cpu_cpsr_save>
87803272:	4605      	mov	r5, r0
87803274:	6820      	ldr	r0, [r4, #0]
87803276:	f7ff fd17 	bl	87802ca8 <readyqueue_remove>
8780327a:	6820      	ldr	r0, [r4, #0]
8780327c:	f7ff fcd8 	bl	87802c30 <readyqueue_add_tail>
87803280:	4628      	mov	r0, r5
87803282:	f001 fcef 	bl	87804c64 <tos_cpu_cpsr_restore>
87803286:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
8780328a:	f000 bf49 	b.w	87804120 <knl_sched>
8780328e:	bf00      	nop

87803290 <tos_task_prio_change>:
87803290:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
87803294:	4605      	mov	r5, r0
87803296:	460f      	mov	r7, r1
87803298:	f000 ffa2 	bl	878041e0 <knl_is_inirq>
8780329c:	2800      	cmp	r0, #0
8780329e:	d14a      	bne.n	87803336 <tos_task_prio_change+0xa6>
878032a0:	2d00      	cmp	r5, #0
878032a2:	d04c      	beq.n	8780333e <tos_task_prio_change+0xae>
878032a4:	68aa      	ldr	r2, [r5, #8]
878032a6:	f64d 23d8 	movw	r3, #56024	; 0xdad8
878032aa:	429a      	cmp	r2, r3
878032ac:	d003      	beq.n	878032b6 <tos_task_prio_change+0x26>
878032ae:	f240 404d 	movw	r0, #1101	; 0x44d
878032b2:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
878032b6:	2f08      	cmp	r7, #8
878032b8:	d84b      	bhi.n	87803352 <tos_task_prio_change+0xc2>
878032ba:	f001 fcd1 	bl	87804c60 <tos_cpu_cpsr_save>
878032be:	f895 3025 	ldrb.w	r3, [r5, #37]	; 0x25
878032c2:	4680      	mov	r8, r0
878032c4:	42bb      	cmp	r3, r7
878032c6:	d02f      	beq.n	87803328 <tos_task_prio_change+0x98>
878032c8:	6d2e      	ldr	r6, [r5, #80]	; 0x50
878032ca:	f105 0950 	add.w	r9, r5, #80	; 0x50
878032ce:	45b1      	cmp	r9, r6
878032d0:	d013      	beq.n	878032fa <tos_task_prio_change+0x6a>
878032d2:	3e1c      	subs	r6, #28
878032d4:	240a      	movs	r4, #10
878032d6:	f106 0008 	add.w	r0, r6, #8
878032da:	f001 f801 	bl	878042e0 <pend_highest_pending_prio_get>
878032de:	69f3      	ldr	r3, [r6, #28]
878032e0:	42a0      	cmp	r0, r4
878032e2:	bf28      	it	cs
878032e4:	4620      	movcs	r0, r4
878032e6:	b2c4      	uxtb	r4, r0
878032e8:	4599      	cmp	r9, r3
878032ea:	f1a3 061c 	sub.w	r6, r3, #28
878032ee:	d1f2      	bne.n	878032d6 <tos_task_prio_change+0x46>
878032f0:	42a7      	cmp	r7, r4
878032f2:	bf84      	itt	hi
878032f4:	f885 7058 	strbhi.w	r7, [r5, #88]	; 0x58
878032f8:	4627      	movhi	r7, r4
878032fa:	f895 3024 	ldrb.w	r3, [r5, #36]	; 0x24
878032fe:	0799      	lsls	r1, r3, #30
87803300:	d421      	bmi.n	87803346 <tos_task_prio_change+0xb6>
87803302:	07da      	lsls	r2, r3, #31
87803304:	bf48      	it	mi
87803306:	f885 7025 	strbmi.w	r7, [r5, #37]	; 0x25
8780330a:	d40c      	bmi.n	87803326 <tos_task_prio_change+0x96>
8780330c:	b95b      	cbnz	r3, 87803326 <tos_task_prio_change+0x96>
8780330e:	4628      	mov	r0, r5
87803310:	f7ff fcca 	bl	87802ca8 <readyqueue_remove>
87803314:	f885 7025 	strb.w	r7, [r5, #37]	; 0x25
87803318:	4628      	mov	r0, r5
8780331a:	f000 ff75 	bl	87804208 <knl_is_self>
8780331e:	b1e0      	cbz	r0, 8780335a <tos_task_prio_change+0xca>
87803320:	4628      	mov	r0, r5
87803322:	f7ff fc5b 	bl	87802bdc <readyqueue_add_head>
87803326:	4640      	mov	r0, r8
87803328:	f001 fc9c 	bl	87804c64 <tos_cpu_cpsr_restore>
8780332c:	f000 fef8 	bl	87804120 <knl_sched>
87803330:	2000      	movs	r0, #0
87803332:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87803336:	f44f 70c8 	mov.w	r0, #400	; 0x190
8780333a:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
8780333e:	f240 404c 	movw	r0, #1100	; 0x44c
87803342:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
87803346:	f885 7025 	strb.w	r7, [r5, #37]	; 0x25
8780334a:	4628      	mov	r0, r5
8780334c:	f000 fff2 	bl	87804334 <pend_list_adjust>
87803350:	e7e9      	b.n	87803326 <tos_task_prio_change+0x96>
87803352:	f240 706f 	movw	r0, #1903	; 0x76f
87803356:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
8780335a:	4628      	mov	r0, r5
8780335c:	f7ff fc68 	bl	87802c30 <readyqueue_add_tail>
87803360:	e7e1      	b.n	87803326 <tos_task_prio_change+0x96>
87803362:	bf00      	nop

87803364 <tos_task_suspend>:
87803364:	b538      	push	{r3, r4, r5, lr}
87803366:	4604      	mov	r4, r0
87803368:	b138      	cbz	r0, 8780337a <tos_task_suspend+0x16>
8780336a:	f64d 23d8 	movw	r3, #56024	; 0xdad8
8780336e:	68a2      	ldr	r2, [r4, #8]
87803370:	429a      	cmp	r2, r3
87803372:	d008      	beq.n	87803386 <tos_task_suspend+0x22>
87803374:	f240 404d 	movw	r0, #1101	; 0x44d
87803378:	bd38      	pop	{r3, r4, r5, pc}
8780337a:	f640 33a0 	movw	r3, #2976	; 0xba0
8780337e:	f2c8 7381 	movt	r3, #34689	; 0x8781
87803382:	681c      	ldr	r4, [r3, #0]
87803384:	e7f1      	b.n	8780336a <tos_task_suspend+0x6>
87803386:	4620      	mov	r0, r4
87803388:	f000 ff34 	bl	878041f4 <knl_is_idle>
8780338c:	b110      	cbz	r0, 87803394 <tos_task_suspend+0x30>
8780338e:	f240 7072 	movw	r0, #1906	; 0x772
87803392:	bd38      	pop	{r3, r4, r5, pc}
87803394:	4620      	mov	r0, r4
87803396:	f000 ff37 	bl	87804208 <knl_is_self>
8780339a:	b9b0      	cbnz	r0, 878033ca <tos_task_suspend+0x66>
8780339c:	f001 fc60 	bl	87804c60 <tos_cpu_cpsr_save>
878033a0:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
878033a4:	4605      	mov	r5, r0
878033a6:	b153      	cbz	r3, 878033be <tos_task_suspend+0x5a>
878033a8:	f043 0304 	orr.w	r3, r3, #4
878033ac:	4628      	mov	r0, r5
878033ae:	f884 3024 	strb.w	r3, [r4, #36]	; 0x24
878033b2:	f001 fc57 	bl	87804c64 <tos_cpu_cpsr_restore>
878033b6:	f000 feb3 	bl	87804120 <knl_sched>
878033ba:	2000      	movs	r0, #0
878033bc:	bd38      	pop	{r3, r4, r5, pc}
878033be:	4620      	mov	r0, r4
878033c0:	f7ff fc72 	bl	87802ca8 <readyqueue_remove>
878033c4:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
878033c8:	e7ee      	b.n	878033a8 <tos_task_suspend+0x44>
878033ca:	f000 feff 	bl	878041cc <knl_is_sched_locked>
878033ce:	2800      	cmp	r0, #0
878033d0:	d0e4      	beq.n	8780339c <tos_task_suspend+0x38>
878033d2:	f240 60a4 	movw	r0, #1700	; 0x6a4
878033d6:	bd38      	pop	{r3, r4, r5, pc}

878033d8 <tos_task_resume>:
878033d8:	b138      	cbz	r0, 878033ea <tos_task_resume+0x12>
878033da:	6881      	ldr	r1, [r0, #8]
878033dc:	f64d 22d8 	movw	r2, #56024	; 0xdad8
878033e0:	4291      	cmp	r1, r2
878033e2:	d005      	beq.n	878033f0 <tos_task_resume+0x18>
878033e4:	f240 404d 	movw	r0, #1101	; 0x44d
878033e8:	4770      	bx	lr
878033ea:	f240 404c 	movw	r0, #1100	; 0x44c
878033ee:	4770      	bx	lr
878033f0:	b570      	push	{r4, r5, r6, lr}
878033f2:	4604      	mov	r4, r0
878033f4:	f000 ff08 	bl	87804208 <knl_is_self>
878033f8:	b9c0      	cbnz	r0, 8780342c <tos_task_resume+0x54>
878033fa:	f001 fc31 	bl	87804c60 <tos_cpu_cpsr_save>
878033fe:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
87803402:	4606      	mov	r6, r0
87803404:	f003 0204 	and.w	r2, r3, #4
87803408:	b12a      	cbz	r2, 87803416 <tos_task_resume+0x3e>
8780340a:	f003 03fb 	and.w	r3, r3, #251	; 0xfb
8780340e:	f884 3024 	strb.w	r3, [r4, #36]	; 0x24
87803412:	b133      	cbz	r3, 87803422 <tos_task_resume+0x4a>
87803414:	4630      	mov	r0, r6
87803416:	f001 fc25 	bl	87804c64 <tos_cpu_cpsr_restore>
8780341a:	f000 fe81 	bl	87804120 <knl_sched>
8780341e:	2000      	movs	r0, #0
87803420:	bd70      	pop	{r4, r5, r6, pc}
87803422:	4620      	mov	r0, r4
87803424:	f7ff fc30 	bl	87802c88 <readyqueue_add>
87803428:	4630      	mov	r0, r6
8780342a:	e7f4      	b.n	87803416 <tos_task_resume+0x3e>
8780342c:	f44f 60ee 	mov.w	r0, #1904	; 0x770
87803430:	bd70      	pop	{r4, r5, r6, pc}
87803432:	bf00      	nop

87803434 <tos_task_delay>:
87803434:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87803438:	4606      	mov	r6, r0
8780343a:	460f      	mov	r7, r1
8780343c:	f000 fed0 	bl	878041e0 <knl_is_inirq>
87803440:	bb30      	cbnz	r0, 87803490 <tos_task_delay+0x5c>
87803442:	f000 fec3 	bl	878041cc <knl_is_sched_locked>
87803446:	4604      	mov	r4, r0
87803448:	b118      	cbz	r0, 87803452 <tos_task_delay+0x1e>
8780344a:	f240 60a4 	movw	r0, #1700	; 0x6a4
8780344e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803452:	ea56 0307 	orrs.w	r3, r6, r7
87803456:	d01f      	beq.n	87803498 <tos_task_delay+0x64>
87803458:	1c7b      	adds	r3, r7, #1
8780345a:	bf08      	it	eq
8780345c:	f1b6 3fff 	cmpeq.w	r6, #4294967295	; 0xffffffff
87803460:	d01f      	beq.n	878034a2 <tos_task_delay+0x6e>
87803462:	f640 35a0 	movw	r5, #2976	; 0xba0
87803466:	f2c8 7581 	movt	r5, #34689	; 0x8781
8780346a:	f001 fbf9 	bl	87804c60 <tos_cpu_cpsr_save>
8780346e:	4632      	mov	r2, r6
87803470:	463b      	mov	r3, r7
87803472:	4680      	mov	r8, r0
87803474:	6828      	ldr	r0, [r5, #0]
87803476:	f7fe fe7f 	bl	87802178 <tick_list_add>
8780347a:	6828      	ldr	r0, [r5, #0]
8780347c:	f7ff fc14 	bl	87802ca8 <readyqueue_remove>
87803480:	4640      	mov	r0, r8
87803482:	f001 fbef 	bl	87804c64 <tos_cpu_cpsr_restore>
87803486:	f000 fe4b 	bl	87804120 <knl_sched>
8780348a:	4620      	mov	r0, r4
8780348c:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803490:	f44f 70c8 	mov.w	r0, #400	; 0x190
87803494:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803498:	f7ff fee0 	bl	8780325c <tos_task_yield>
8780349c:	4620      	mov	r0, r4
8780349e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878034a2:	2065      	movs	r0, #101	; 0x65
878034a4:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}

878034a8 <tos_task_delay_abort>:
878034a8:	b570      	push	{r4, r5, r6, lr}
878034aa:	4604      	mov	r4, r0
878034ac:	f000 fe98 	bl	878041e0 <knl_is_inirq>
878034b0:	b940      	cbnz	r0, 878034c4 <tos_task_delay_abort+0x1c>
878034b2:	b154      	cbz	r4, 878034ca <tos_task_delay_abort+0x22>
878034b4:	68a2      	ldr	r2, [r4, #8]
878034b6:	f64d 23d8 	movw	r3, #56024	; 0xdad8
878034ba:	429a      	cmp	r2, r3
878034bc:	d008      	beq.n	878034d0 <tos_task_delay_abort+0x28>
878034be:	f240 404d 	movw	r0, #1101	; 0x44d
878034c2:	bd70      	pop	{r4, r5, r6, pc}
878034c4:	f44f 70c8 	mov.w	r0, #400	; 0x190
878034c8:	bd70      	pop	{r4, r5, r6, pc}
878034ca:	f240 404c 	movw	r0, #1100	; 0x44c
878034ce:	bd70      	pop	{r4, r5, r6, pc}
878034d0:	f001 fbc6 	bl	87804c60 <tos_cpu_cpsr_save>
878034d4:	4606      	mov	r6, r0
878034d6:	4620      	mov	r0, r4
878034d8:	f000 fe96 	bl	87804208 <knl_is_self>
878034dc:	b918      	cbnz	r0, 878034e6 <tos_task_delay_abort+0x3e>
878034de:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
878034e2:	07da      	lsls	r2, r3, #31
878034e4:	d405      	bmi.n	878034f2 <tos_task_delay_abort+0x4a>
878034e6:	4630      	mov	r0, r6
878034e8:	f001 fbbc 	bl	87804c64 <tos_cpu_cpsr_restore>
878034ec:	f240 706e 	movw	r0, #1902	; 0x76e
878034f0:	bd70      	pop	{r4, r5, r6, pc}
878034f2:	f003 0304 	and.w	r3, r3, #4
878034f6:	f003 05ff 	and.w	r5, r3, #255	; 0xff
878034fa:	b12b      	cbz	r3, 87803508 <tos_task_delay_abort+0x60>
878034fc:	4630      	mov	r0, r6
878034fe:	f001 fbb1 	bl	87804c64 <tos_cpu_cpsr_restore>
87803502:	f240 7071 	movw	r0, #1905	; 0x771
87803506:	bd70      	pop	{r4, r5, r6, pc}
87803508:	4620      	mov	r0, r4
8780350a:	f7fe fea1 	bl	87802250 <tick_list_remove>
8780350e:	4620      	mov	r0, r4
87803510:	f7ff fbba 	bl	87802c88 <readyqueue_add>
87803514:	4630      	mov	r0, r6
87803516:	f001 fba5 	bl	87804c64 <tos_cpu_cpsr_restore>
8780351a:	f000 fe01 	bl	87804120 <knl_sched>
8780351e:	4628      	mov	r0, r5
87803520:	bd70      	pop	{r4, r5, r6, pc}
87803522:	bf00      	nop

87803524 <tos_task_curr_task_get>:
87803524:	b538      	push	{r3, r4, r5, lr}
87803526:	f001 fb9b 	bl	87804c60 <tos_cpu_cpsr_save>
8780352a:	4605      	mov	r5, r0
8780352c:	f000 fdec 	bl	87804108 <tos_knl_is_running>
87803530:	b148      	cbz	r0, 87803546 <tos_task_curr_task_get+0x22>
87803532:	f640 33a0 	movw	r3, #2976	; 0xba0
87803536:	4628      	mov	r0, r5
87803538:	f2c8 7381 	movt	r3, #34689	; 0x8781
8780353c:	681c      	ldr	r4, [r3, #0]
8780353e:	f001 fb91 	bl	87804c64 <tos_cpu_cpsr_restore>
87803542:	4620      	mov	r0, r4
87803544:	bd38      	pop	{r3, r4, r5, pc}
87803546:	4604      	mov	r4, r0
87803548:	4628      	mov	r0, r5
8780354a:	f001 fb8b 	bl	87804c64 <tos_cpu_cpsr_restore>
8780354e:	4620      	mov	r0, r4
87803550:	bd38      	pop	{r3, r4, r5, pc}
87803552:	bf00      	nop

87803554 <task_default_walker>:
87803554:	b510      	push	{r4, lr}
87803556:	4604      	mov	r4, r0
87803558:	f64d 40b8 	movw	r0, #56504	; 0xdcb8
8780355c:	f104 010c 	add.w	r1, r4, #12
87803560:	f2c8 7080 	movt	r0, #34688	; 0x8780
87803564:	f001 fd72 	bl	8780504c <printf>
87803568:	f64d 4044 	movw	r0, #56388	; 0xdc44
8780356c:	f2c8 7080 	movt	r0, #34688	; 0x8780
87803570:	f001 fd6c 	bl	8780504c <printf>
87803574:	f7ff ffd6 	bl	87803524 <tos_task_curr_task_get>
87803578:	4284      	cmp	r4, r0
8780357a:	d066      	beq.n	8780364a <task_default_walker+0xf6>
8780357c:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
87803580:	2b07      	cmp	r3, #7
87803582:	d058      	beq.n	87803636 <task_default_walker+0xe2>
87803584:	2b06      	cmp	r3, #6
87803586:	d05b      	beq.n	87803640 <task_default_walker+0xec>
87803588:	2b05      	cmp	r3, #5
8780358a:	d04f      	beq.n	8780362c <task_default_walker+0xd8>
8780358c:	2b03      	cmp	r3, #3
8780358e:	d061      	beq.n	87803654 <task_default_walker+0x100>
87803590:	2b04      	cmp	r3, #4
87803592:	d064      	beq.n	8780365e <task_default_walker+0x10a>
87803594:	2b02      	cmp	r3, #2
87803596:	d067      	beq.n	87803668 <task_default_walker+0x114>
87803598:	2b01      	cmp	r3, #1
8780359a:	d06a      	beq.n	87803672 <task_default_walker+0x11e>
8780359c:	f64d 42a4 	movw	r2, #56484	; 0xdca4
878035a0:	f64d 41b0 	movw	r1, #56496	; 0xdcb0
878035a4:	f2c8 7280 	movt	r2, #34688	; 0x8780
878035a8:	f2c8 7180 	movt	r1, #34688	; 0x8780
878035ac:	2b00      	cmp	r3, #0
878035ae:	bf18      	it	ne
878035b0:	4611      	movne	r1, r2
878035b2:	f64d 40c8 	movw	r0, #56520	; 0xdcc8
878035b6:	f2c8 7080 	movt	r0, #34688	; 0x8780
878035ba:	f001 fd47 	bl	8780504c <printf>
878035be:	f64d 4044 	movw	r0, #56388	; 0xdc44
878035c2:	f2c8 7080 	movt	r0, #34688	; 0x8780
878035c6:	f001 fd41 	bl	8780504c <printf>
878035ca:	f64d 40d8 	movw	r0, #56536	; 0xdcd8
878035ce:	6ae1      	ldr	r1, [r4, #44]	; 0x2c
878035d0:	f2c8 7080 	movt	r0, #34688	; 0x8780
878035d4:	f001 fd3a 	bl	8780504c <printf>
878035d8:	f64d 4044 	movw	r0, #56388	; 0xdc44
878035dc:	f2c8 7080 	movt	r0, #34688	; 0x8780
878035e0:	f001 fd34 	bl	8780504c <printf>
878035e4:	f64d 40e8 	movw	r0, #56552	; 0xdce8
878035e8:	6aa1      	ldr	r1, [r4, #40]	; 0x28
878035ea:	f2c8 7080 	movt	r0, #34688	; 0x8780
878035ee:	f001 fd2d 	bl	8780504c <printf>
878035f2:	f64d 4044 	movw	r0, #56388	; 0xdc44
878035f6:	f2c8 7080 	movt	r0, #34688	; 0x8780
878035fa:	f001 fd27 	bl	8780504c <printf>
878035fe:	6ae3      	ldr	r3, [r4, #44]	; 0x2c
87803600:	f64d 40f8 	movw	r0, #56568	; 0xdcf8
87803604:	6aa1      	ldr	r1, [r4, #40]	; 0x28
87803606:	f2c8 7080 	movt	r0, #34688	; 0x8780
8780360a:	4419      	add	r1, r3
8780360c:	f001 fd1e 	bl	8780504c <printf>
87803610:	f64d 4044 	movw	r0, #56388	; 0xdc44
87803614:	f2c8 7080 	movt	r0, #34688	; 0x8780
87803618:	f001 fd18 	bl	8780504c <printf>
8780361c:	f64d 4044 	movw	r0, #56388	; 0xdc44
87803620:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
87803624:	f2c8 7080 	movt	r0, #34688	; 0x8780
87803628:	f001 bd10 	b.w	8780504c <printf>
8780362c:	f64d 4178 	movw	r1, #56440	; 0xdc78
87803630:	f2c8 7180 	movt	r1, #34688	; 0x8780
87803634:	e7bd      	b.n	878035b2 <task_default_walker+0x5e>
87803636:	f64d 4150 	movw	r1, #56400	; 0xdc50
8780363a:	f2c8 7180 	movt	r1, #34688	; 0x8780
8780363e:	e7b8      	b.n	878035b2 <task_default_walker+0x5e>
87803640:	f64d 4168 	movw	r1, #56424	; 0xdc68
87803644:	f2c8 7180 	movt	r1, #34688	; 0x8780
87803648:	e7b3      	b.n	878035b2 <task_default_walker+0x5e>
8780364a:	f64d 4148 	movw	r1, #56392	; 0xdc48
8780364e:	f2c8 7180 	movt	r1, #34688	; 0x8780
87803652:	e7ae      	b.n	878035b2 <task_default_walker+0x5e>
87803654:	f64d 4188 	movw	r1, #56456	; 0xdc88
87803658:	f2c8 7180 	movt	r1, #34688	; 0x8780
8780365c:	e7a9      	b.n	878035b2 <task_default_walker+0x5e>
8780365e:	f64d 415c 	movw	r1, #56412	; 0xdc5c
87803662:	f2c8 7180 	movt	r1, #34688	; 0x8780
87803666:	e7a4      	b.n	878035b2 <task_default_walker+0x5e>
87803668:	f64d 4194 	movw	r1, #56468	; 0xdc94
8780366c:	f2c8 7180 	movt	r1, #34688	; 0x8780
87803670:	e79f      	b.n	878035b2 <task_default_walker+0x5e>
87803672:	f64d 419c 	movw	r1, #56476	; 0xdc9c
87803676:	f2c8 7180 	movt	r1, #34688	; 0x8780
8780367a:	e79a      	b.n	878035b2 <task_default_walker+0x5e>

8780367c <tos_task_walkthru>:
8780367c:	b1c8      	cbz	r0, 878036b2 <tos_task_walkthru+0x36>
8780367e:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87803680:	f64d 6580 	movw	r5, #56960	; 0xde80
87803684:	f2c8 7580 	movt	r5, #34688	; 0x8780
87803688:	4606      	mov	r6, r0
8780368a:	f001 fae9 	bl	87804c60 <tos_cpu_cpsr_save>
8780368e:	682b      	ldr	r3, [r5, #0]
87803690:	4607      	mov	r7, r0
87803692:	42ab      	cmp	r3, r5
87803694:	f1a3 0430 	sub.w	r4, r3, #48	; 0x30
87803698:	d006      	beq.n	878036a8 <tos_task_walkthru+0x2c>
8780369a:	4620      	mov	r0, r4
8780369c:	47b0      	blx	r6
8780369e:	6b23      	ldr	r3, [r4, #48]	; 0x30
878036a0:	42ab      	cmp	r3, r5
878036a2:	f1a3 0430 	sub.w	r4, r3, #48	; 0x30
878036a6:	d1f8      	bne.n	8780369a <tos_task_walkthru+0x1e>
878036a8:	4638      	mov	r0, r7
878036aa:	e8bd 40f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, lr}
878036ae:	f001 bad9 	b.w	87804c64 <tos_cpu_cpsr_restore>
878036b2:	4770      	bx	lr

878036b4 <tos_task_info_display>:
878036b4:	f243 5055 	movw	r0, #13653	; 0x3555
878036b8:	f2c8 7080 	movt	r0, #34688	; 0x8780
878036bc:	f7ff bfde 	b.w	8780367c <tos_task_walkthru>

878036c0 <tos_task_stack_draught_depth>:
878036c0:	b169      	cbz	r1, 878036de <tos_task_stack_draught_depth+0x1e>
878036c2:	b530      	push	{r4, r5, lr}
878036c4:	4604      	mov	r4, r0
878036c6:	b083      	sub	sp, #12
878036c8:	b160      	cbz	r0, 878036e4 <tos_task_stack_draught_depth+0x24>
878036ca:	f64d 23d8 	movw	r3, #56024	; 0xdad8
878036ce:	68a2      	ldr	r2, [r4, #8]
878036d0:	429a      	cmp	r2, r3
878036d2:	bf18      	it	ne
878036d4:	f240 404d 	movwne	r0, #1101	; 0x44d
878036d8:	d00a      	beq.n	878036f0 <tos_task_stack_draught_depth+0x30>
878036da:	b003      	add	sp, #12
878036dc:	bd30      	pop	{r4, r5, pc}
878036de:	f240 404c 	movw	r0, #1100	; 0x44c
878036e2:	4770      	bx	lr
878036e4:	f640 33a0 	movw	r3, #2976	; 0xba0
878036e8:	f2c8 7381 	movt	r3, #34689	; 0x8781
878036ec:	681c      	ldr	r4, [r3, #0]
878036ee:	e7ec      	b.n	878036ca <tos_task_stack_draught_depth+0xa>
878036f0:	9101      	str	r1, [sp, #4]
878036f2:	f001 fab5 	bl	87804c60 <tos_cpu_cpsr_save>
878036f6:	9901      	ldr	r1, [sp, #4]
878036f8:	4605      	mov	r5, r0
878036fa:	6aa0      	ldr	r0, [r4, #40]	; 0x28
878036fc:	460a      	mov	r2, r1
878036fe:	6ae1      	ldr	r1, [r4, #44]	; 0x2c
87803700:	f001 fb32 	bl	87804d68 <cpu_task_stack_draught_depth>
87803704:	4604      	mov	r4, r0
87803706:	4628      	mov	r0, r5
87803708:	f001 faac 	bl	87804c64 <tos_cpu_cpsr_restore>
8780370c:	4620      	mov	r0, r4
8780370e:	e7e4      	b.n	878036da <tos_task_stack_draught_depth+0x1a>

87803710 <sem_do_post.part.3>:
87803710:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87803714:	4604      	mov	r4, r0
87803716:	4688      	mov	r8, r1
87803718:	f001 faa2 	bl	87804c60 <tos_cpu_cpsr_save>
8780371c:	8a22      	ldrh	r2, [r4, #16]
8780371e:	8a63      	ldrh	r3, [r4, #18]
87803720:	429a      	cmp	r2, r3
87803722:	d01d      	beq.n	87803760 <sem_do_post.part.3+0x50>
87803724:	f104 0708 	add.w	r7, r4, #8
87803728:	4606      	mov	r6, r0
8780372a:	4638      	mov	r0, r7
8780372c:	f000 fdfc 	bl	87804328 <pend_is_nopending>
87803730:	4605      	mov	r5, r0
87803732:	b960      	cbnz	r0, 8780374e <sem_do_post.part.3+0x3e>
87803734:	4642      	mov	r2, r8
87803736:	2101      	movs	r1, #1
87803738:	4638      	mov	r0, r7
8780373a:	f000 fe79 	bl	87804430 <pend_wakeup>
8780373e:	4630      	mov	r0, r6
87803740:	f001 fa90 	bl	87804c64 <tos_cpu_cpsr_restore>
87803744:	f000 fcec 	bl	87804120 <knl_sched>
87803748:	4628      	mov	r0, r5
8780374a:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780374e:	8a23      	ldrh	r3, [r4, #16]
87803750:	4630      	mov	r0, r6
87803752:	3301      	adds	r3, #1
87803754:	8223      	strh	r3, [r4, #16]
87803756:	f001 fa85 	bl	87804c64 <tos_cpu_cpsr_restore>
8780375a:	2000      	movs	r0, #0
8780375c:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803760:	f001 fa80 	bl	87804c64 <tos_cpu_cpsr_restore>
87803764:	f44f 60e1 	mov.w	r0, #1800	; 0x708
87803768:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}

8780376c <tos_sem_create_max>:
8780376c:	b510      	push	{r4, lr}
8780376e:	4604      	mov	r4, r0
87803770:	b148      	cbz	r0, 87803786 <tos_sem_create_max+0x1a>
87803772:	3008      	adds	r0, #8
87803774:	8101      	strh	r1, [r0, #8]
87803776:	8262      	strh	r2, [r4, #18]
87803778:	f000 fdce 	bl	87804318 <pend_object_init>
8780377c:	f64a 33ee 	movw	r3, #44014	; 0xabee
87803780:	2000      	movs	r0, #0
87803782:	6063      	str	r3, [r4, #4]
87803784:	bd10      	pop	{r4, pc}
87803786:	f240 404c 	movw	r0, #1100	; 0x44c
8780378a:	bd10      	pop	{r4, pc}

8780378c <tos_sem_create>:
8780378c:	b510      	push	{r4, lr}
8780378e:	4604      	mov	r4, r0
87803790:	b158      	cbz	r0, 878037aa <tos_sem_create+0x1e>
87803792:	f64f 73ff 	movw	r3, #65535	; 0xffff
87803796:	3008      	adds	r0, #8
87803798:	8101      	strh	r1, [r0, #8]
8780379a:	8143      	strh	r3, [r0, #10]
8780379c:	f000 fdbc 	bl	87804318 <pend_object_init>
878037a0:	f64a 33ee 	movw	r3, #44014	; 0xabee
878037a4:	2000      	movs	r0, #0
878037a6:	6063      	str	r3, [r4, #4]
878037a8:	bd10      	pop	{r4, pc}
878037aa:	f240 404c 	movw	r0, #1100	; 0x44c
878037ae:	bd10      	pop	{r4, pc}

878037b0 <tos_sem_destroy>:
878037b0:	b140      	cbz	r0, 878037c4 <tos_sem_destroy+0x14>
878037b2:	6842      	ldr	r2, [r0, #4]
878037b4:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
878037b6:	f64a 33ee 	movw	r3, #44014	; 0xabee
878037ba:	429a      	cmp	r2, r3
878037bc:	d005      	beq.n	878037ca <tos_sem_destroy+0x1a>
878037be:	f240 404d 	movw	r0, #1101	; 0x44d
878037c2:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878037c4:	f240 404c 	movw	r0, #1100	; 0x44c
878037c8:	4770      	bx	lr
878037ca:	f100 0608 	add.w	r6, r0, #8
878037ce:	4604      	mov	r4, r0
878037d0:	f001 fa46 	bl	87804c60 <tos_cpu_cpsr_save>
878037d4:	2103      	movs	r1, #3
878037d6:	4607      	mov	r7, r0
878037d8:	4630      	mov	r0, r6
878037da:	2500      	movs	r5, #0
878037dc:	f000 fe10 	bl	87804400 <pend_wakeup_all>
878037e0:	4630      	mov	r0, r6
878037e2:	f000 fd9d 	bl	87804320 <pend_object_deinit>
878037e6:	4638      	mov	r0, r7
878037e8:	6065      	str	r5, [r4, #4]
878037ea:	f001 fa3b 	bl	87804c64 <tos_cpu_cpsr_restore>
878037ee:	f000 fc97 	bl	87804120 <knl_sched>
878037f2:	4628      	mov	r0, r5
878037f4:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878037f6:	bf00      	nop

878037f8 <tos_sem_post>:
878037f8:	b138      	cbz	r0, 8780380a <tos_sem_post+0x12>
878037fa:	6842      	ldr	r2, [r0, #4]
878037fc:	f64a 33ee 	movw	r3, #44014	; 0xabee
87803800:	429a      	cmp	r2, r3
87803802:	d005      	beq.n	87803810 <tos_sem_post+0x18>
87803804:	f240 404d 	movw	r0, #1101	; 0x44d
87803808:	4770      	bx	lr
8780380a:	f240 404c 	movw	r0, #1100	; 0x44c
8780380e:	4770      	bx	lr
87803810:	2100      	movs	r1, #0
87803812:	e77d      	b.n	87803710 <sem_do_post.part.3>

87803814 <tos_sem_post_all>:
87803814:	b138      	cbz	r0, 87803826 <tos_sem_post_all+0x12>
87803816:	6842      	ldr	r2, [r0, #4]
87803818:	f64a 33ee 	movw	r3, #44014	; 0xabee
8780381c:	429a      	cmp	r2, r3
8780381e:	d005      	beq.n	8780382c <tos_sem_post_all+0x18>
87803820:	f240 404d 	movw	r0, #1101	; 0x44d
87803824:	4770      	bx	lr
87803826:	f240 404c 	movw	r0, #1100	; 0x44c
8780382a:	4770      	bx	lr
8780382c:	2101      	movs	r1, #1
8780382e:	e76f      	b.n	87803710 <sem_do_post.part.3>

87803830 <tos_sem_pend>:
87803830:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87803834:	4605      	mov	r5, r0
87803836:	4616      	mov	r6, r2
87803838:	461f      	mov	r7, r3
8780383a:	f000 fcd1 	bl	878041e0 <knl_is_inirq>
8780383e:	4604      	mov	r4, r0
87803840:	b948      	cbnz	r0, 87803856 <tos_sem_pend+0x26>
87803842:	b165      	cbz	r5, 8780385e <tos_sem_pend+0x2e>
87803844:	686a      	ldr	r2, [r5, #4]
87803846:	f64a 33ee 	movw	r3, #44014	; 0xabee
8780384a:	429a      	cmp	r2, r3
8780384c:	d00b      	beq.n	87803866 <tos_sem_pend+0x36>
8780384e:	f240 404d 	movw	r0, #1101	; 0x44d
87803852:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803856:	f44f 70c8 	mov.w	r0, #400	; 0x190
8780385a:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780385e:	f240 404c 	movw	r0, #1100	; 0x44c
87803862:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803866:	f001 f9fb 	bl	87804c60 <tos_cpu_cpsr_save>
8780386a:	8a2b      	ldrh	r3, [r5, #16]
8780386c:	4680      	mov	r8, r0
8780386e:	b943      	cbnz	r3, 87803882 <tos_sem_pend+0x52>
87803870:	ea56 0307 	orrs.w	r3, r6, r7
87803874:	d10c      	bne.n	87803890 <tos_sem_pend+0x60>
87803876:	f001 f9f5 	bl	87804c64 <tos_cpu_cpsr_restore>
8780387a:	f44f 6096 	mov.w	r0, #1200	; 0x4b0
8780387e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803882:	3b01      	subs	r3, #1
87803884:	822b      	strh	r3, [r5, #16]
87803886:	f001 f9ed 	bl	87804c64 <tos_cpu_cpsr_restore>
8780388a:	4620      	mov	r0, r4
8780388c:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803890:	f000 fc9c 	bl	878041cc <knl_is_sched_locked>
87803894:	b130      	cbz	r0, 878038a4 <tos_sem_pend+0x74>
87803896:	4640      	mov	r0, r8
87803898:	f001 f9e4 	bl	87804c64 <tos_cpu_cpsr_restore>
8780389c:	f240 40b1 	movw	r0, #1201	; 0x4b1
878038a0:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878038a4:	f640 34a0 	movw	r4, #2976	; 0xba0
878038a8:	463b      	mov	r3, r7
878038aa:	f2c8 7481 	movt	r4, #34689	; 0x8781
878038ae:	4632      	mov	r2, r6
878038b0:	f105 0108 	add.w	r1, r5, #8
878038b4:	6820      	ldr	r0, [r4, #0]
878038b6:	f000 fd83 	bl	878043c0 <pend_task_block>
878038ba:	4640      	mov	r0, r8
878038bc:	f001 f9d2 	bl	87804c64 <tos_cpu_cpsr_restore>
878038c0:	f000 fc2e 	bl	87804120 <knl_sched>
878038c4:	6823      	ldr	r3, [r4, #0]
878038c6:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
878038ca:	6e18      	ldr	r0, [r3, #96]	; 0x60
878038cc:	f000 bd3c 	b.w	87804348 <pend_state2errno>

878038d0 <prio_msg_q_do_post>:
878038d0:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
878038d4:	b082      	sub	sp, #8
878038d6:	9101      	str	r1, [sp, #4]
878038d8:	b150      	cbz	r0, 878038f0 <prio_msg_q_do_post+0x20>
878038da:	f648 31ee 	movw	r1, #35822	; 0x8bee
878038de:	6844      	ldr	r4, [r0, #4]
878038e0:	428c      	cmp	r4, r1
878038e2:	bf18      	it	ne
878038e4:	f240 404d 	movwne	r0, #1101	; 0x44d
878038e8:	d007      	beq.n	878038fa <prio_msg_q_do_post+0x2a>
878038ea:	b002      	add	sp, #8
878038ec:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878038f0:	f240 404c 	movw	r0, #1100	; 0x44c
878038f4:	b002      	add	sp, #8
878038f6:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878038fa:	f100 0708 	add.w	r7, r0, #8
878038fe:	4604      	mov	r4, r0
87803900:	461d      	mov	r5, r3
87803902:	4690      	mov	r8, r2
87803904:	f001 f9ac 	bl	87804c60 <tos_cpu_cpsr_save>
87803908:	4606      	mov	r6, r0
8780390a:	4638      	mov	r0, r7
8780390c:	f000 fd0c 	bl	87804328 <pend_is_nopending>
87803910:	b9e8      	cbnz	r0, 8780394e <prio_msg_q_do_post+0x7e>
87803912:	68a3      	ldr	r3, [r4, #8]
87803914:	b34d      	cbz	r5, 8780396a <prio_msg_q_do_post+0x9a>
87803916:	681c      	ldr	r4, [r3, #0]
87803918:	429f      	cmp	r7, r3
8780391a:	f1a3 0248 	sub.w	r2, r3, #72	; 0x48
8780391e:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
87803922:	d00d      	beq.n	87803940 <prio_msg_q_do_post+0x70>
87803924:	9b01      	ldr	r3, [sp, #4]
87803926:	4610      	mov	r0, r2
87803928:	2101      	movs	r1, #1
8780392a:	6653      	str	r3, [r2, #100]	; 0x64
8780392c:	f000 fd24 	bl	87804378 <pend_task_wakeup>
87803930:	4623      	mov	r3, r4
87803932:	4622      	mov	r2, r4
87803934:	f853 4f48 	ldr.w	r4, [r3, #72]!
87803938:	429f      	cmp	r7, r3
8780393a:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
8780393e:	d1f1      	bne.n	87803924 <prio_msg_q_do_post+0x54>
87803940:	4630      	mov	r0, r6
87803942:	f001 f98f 	bl	87804c64 <tos_cpu_cpsr_restore>
87803946:	f000 fbeb 	bl	87804120 <knl_sched>
8780394a:	2000      	movs	r0, #0
8780394c:	e7cd      	b.n	878038ea <prio_msg_q_do_post+0x1a>
8780394e:	2204      	movs	r2, #4
87803950:	4643      	mov	r3, r8
87803952:	f104 0014 	add.w	r0, r4, #20
87803956:	eb0d 0102 	add.w	r1, sp, r2
8780395a:	f7fd fb3f 	bl	87800fdc <tos_prio_q_enqueue>
8780395e:	4604      	mov	r4, r0
87803960:	4630      	mov	r0, r6
87803962:	f001 f97f 	bl	87804c64 <tos_cpu_cpsr_restore>
87803966:	4620      	mov	r0, r4
87803968:	e7bf      	b.n	878038ea <prio_msg_q_do_post+0x1a>
8780396a:	9a01      	ldr	r2, [sp, #4]
8780396c:	2101      	movs	r1, #1
8780396e:	f1a3 0048 	sub.w	r0, r3, #72	; 0x48
87803972:	61da      	str	r2, [r3, #28]
87803974:	f000 fd00 	bl	87804378 <pend_task_wakeup>
87803978:	e7e2      	b.n	87803940 <prio_msg_q_do_post+0x70>
8780397a:	bf00      	nop

8780397c <tos_prio_msg_q_create>:
8780397c:	b358      	cbz	r0, 878039d6 <tos_prio_msg_q_create+0x5a>
8780397e:	b5f0      	push	{r4, r5, r6, r7, lr}
87803980:	4606      	mov	r6, r0
87803982:	00d0      	lsls	r0, r2, #3
87803984:	b083      	sub	sp, #12
87803986:	4614      	mov	r4, r2
87803988:	eba0 0042 	sub.w	r0, r0, r2, lsl #1
8780398c:	460d      	mov	r5, r1
8780398e:	f7fc fffb 	bl	87800988 <tos_mmheap_alloc>
87803992:	4607      	mov	r7, r0
87803994:	f240 407e 	movw	r0, #1150	; 0x47e
87803998:	b1af      	cbz	r7, 878039c6 <tos_prio_msg_q_create+0x4a>
8780399a:	2104      	movs	r1, #4
8780399c:	4623      	mov	r3, r4
8780399e:	9100      	str	r1, [sp, #0]
878039a0:	462a      	mov	r2, r5
878039a2:	4639      	mov	r1, r7
878039a4:	f106 0014 	add.w	r0, r6, #20
878039a8:	f7fd fa4e 	bl	87800e48 <tos_prio_q_create>
878039ac:	4604      	mov	r4, r0
878039ae:	b960      	cbnz	r0, 878039ca <tos_prio_msg_q_create+0x4e>
878039b0:	f106 0008 	add.w	r0, r6, #8
878039b4:	6137      	str	r7, [r6, #16]
878039b6:	f000 fcaf 	bl	87804318 <pend_object_init>
878039ba:	4620      	mov	r0, r4
878039bc:	f648 32ee 	movw	r2, #35822	; 0x8bee
878039c0:	2301      	movs	r3, #1
878039c2:	6072      	str	r2, [r6, #4]
878039c4:	6033      	str	r3, [r6, #0]
878039c6:	b003      	add	sp, #12
878039c8:	bdf0      	pop	{r4, r5, r6, r7, pc}
878039ca:	4638      	mov	r0, r7
878039cc:	f7fd f87e 	bl	87800acc <tos_mmheap_free>
878039d0:	4620      	mov	r0, r4
878039d2:	b003      	add	sp, #12
878039d4:	bdf0      	pop	{r4, r5, r6, r7, pc}
878039d6:	f240 404c 	movw	r0, #1100	; 0x44c
878039da:	4770      	bx	lr

878039dc <tos_prio_msg_q_destroy>:
878039dc:	b170      	cbz	r0, 878039fc <tos_prio_msg_q_destroy+0x20>
878039de:	6842      	ldr	r2, [r0, #4]
878039e0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
878039e2:	f648 33ee 	movw	r3, #35822	; 0x8bee
878039e6:	429a      	cmp	r2, r3
878039e8:	d002      	beq.n	878039f0 <tos_prio_msg_q_destroy+0x14>
878039ea:	f240 404d 	movw	r0, #1101	; 0x44d
878039ee:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878039f0:	6803      	ldr	r3, [r0, #0]
878039f2:	2b01      	cmp	r3, #1
878039f4:	d005      	beq.n	87803a02 <tos_prio_msg_q_destroy+0x26>
878039f6:	f240 404e 	movw	r0, #1102	; 0x44e
878039fa:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878039fc:	f240 404c 	movw	r0, #1100	; 0x44c
87803a00:	4770      	bx	lr
87803a02:	4604      	mov	r4, r0
87803a04:	f001 f92c 	bl	87804c60 <tos_cpu_cpsr_save>
87803a08:	4606      	mov	r6, r0
87803a0a:	f104 0014 	add.w	r0, r4, #20
87803a0e:	f7fd fa61 	bl	87800ed4 <tos_prio_q_destroy>
87803a12:	4605      	mov	r5, r0
87803a14:	b9a8      	cbnz	r0, 87803a42 <tos_prio_msg_q_destroy+0x66>
87803a16:	f104 0708 	add.w	r7, r4, #8
87803a1a:	2103      	movs	r1, #3
87803a1c:	4638      	mov	r0, r7
87803a1e:	f000 fcef 	bl	87804400 <pend_wakeup_all>
87803a22:	6920      	ldr	r0, [r4, #16]
87803a24:	f7fd f852 	bl	87800acc <tos_mmheap_free>
87803a28:	4638      	mov	r0, r7
87803a2a:	6125      	str	r5, [r4, #16]
87803a2c:	f000 fc78 	bl	87804320 <pend_object_deinit>
87803a30:	4630      	mov	r0, r6
87803a32:	6065      	str	r5, [r4, #4]
87803a34:	6025      	str	r5, [r4, #0]
87803a36:	f001 f915 	bl	87804c64 <tos_cpu_cpsr_restore>
87803a3a:	f000 fb71 	bl	87804120 <knl_sched>
87803a3e:	4628      	mov	r0, r5
87803a40:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87803a42:	4630      	mov	r0, r6
87803a44:	f001 f90e 	bl	87804c64 <tos_cpu_cpsr_restore>
87803a48:	4628      	mov	r0, r5
87803a4a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87803a4c <tos_prio_msg_q_create_dyn>:
87803a4c:	b198      	cbz	r0, 87803a76 <tos_prio_msg_q_create_dyn+0x2a>
87803a4e:	b538      	push	{r3, r4, r5, lr}
87803a50:	2204      	movs	r2, #4
87803a52:	4604      	mov	r4, r0
87803a54:	3014      	adds	r0, #20
87803a56:	f7fd fa61 	bl	87800f1c <tos_prio_q_create_dyn>
87803a5a:	4605      	mov	r5, r0
87803a5c:	b100      	cbz	r0, 87803a60 <tos_prio_msg_q_create_dyn+0x14>
87803a5e:	bd38      	pop	{r3, r4, r5, pc}
87803a60:	f104 0008 	add.w	r0, r4, #8
87803a64:	f000 fc58 	bl	87804318 <pend_object_init>
87803a68:	f648 32ee 	movw	r2, #35822	; 0x8bee
87803a6c:	2302      	movs	r3, #2
87803a6e:	4628      	mov	r0, r5
87803a70:	6062      	str	r2, [r4, #4]
87803a72:	6023      	str	r3, [r4, #0]
87803a74:	bd38      	pop	{r3, r4, r5, pc}
87803a76:	f240 404c 	movw	r0, #1100	; 0x44c
87803a7a:	4770      	bx	lr

87803a7c <tos_prio_msg_q_destroy_dyn>:
87803a7c:	b170      	cbz	r0, 87803a9c <tos_prio_msg_q_destroy_dyn+0x20>
87803a7e:	6842      	ldr	r2, [r0, #4]
87803a80:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87803a82:	f648 33ee 	movw	r3, #35822	; 0x8bee
87803a86:	429a      	cmp	r2, r3
87803a88:	d002      	beq.n	87803a90 <tos_prio_msg_q_destroy_dyn+0x14>
87803a8a:	f240 404d 	movw	r0, #1101	; 0x44d
87803a8e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87803a90:	6803      	ldr	r3, [r0, #0]
87803a92:	2b02      	cmp	r3, #2
87803a94:	d005      	beq.n	87803aa2 <tos_prio_msg_q_destroy_dyn+0x26>
87803a96:	f240 404e 	movw	r0, #1102	; 0x44e
87803a9a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87803a9c:	f240 404c 	movw	r0, #1100	; 0x44c
87803aa0:	4770      	bx	lr
87803aa2:	4604      	mov	r4, r0
87803aa4:	f001 f8dc 	bl	87804c60 <tos_cpu_cpsr_save>
87803aa8:	4606      	mov	r6, r0
87803aaa:	f104 0014 	add.w	r0, r4, #20
87803aae:	f7fd fa6b 	bl	87800f88 <tos_prio_q_destroy_dyn>
87803ab2:	4605      	mov	r5, r0
87803ab4:	b9a8      	cbnz	r0, 87803ae2 <tos_prio_msg_q_destroy_dyn+0x66>
87803ab6:	f104 0708 	add.w	r7, r4, #8
87803aba:	2103      	movs	r1, #3
87803abc:	4638      	mov	r0, r7
87803abe:	f000 fc9f 	bl	87804400 <pend_wakeup_all>
87803ac2:	6920      	ldr	r0, [r4, #16]
87803ac4:	f7fd f802 	bl	87800acc <tos_mmheap_free>
87803ac8:	4638      	mov	r0, r7
87803aca:	6125      	str	r5, [r4, #16]
87803acc:	f000 fc28 	bl	87804320 <pend_object_deinit>
87803ad0:	4630      	mov	r0, r6
87803ad2:	6065      	str	r5, [r4, #4]
87803ad4:	6025      	str	r5, [r4, #0]
87803ad6:	f001 f8c5 	bl	87804c64 <tos_cpu_cpsr_restore>
87803ada:	f000 fb21 	bl	87804120 <knl_sched>
87803ade:	4628      	mov	r0, r5
87803ae0:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87803ae2:	4630      	mov	r0, r6
87803ae4:	f001 f8be 	bl	87804c64 <tos_cpu_cpsr_restore>
87803ae8:	4628      	mov	r0, r5
87803aea:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87803aec <tos_prio_msg_q_flush>:
87803aec:	b138      	cbz	r0, 87803afe <tos_prio_msg_q_flush+0x12>
87803aee:	6842      	ldr	r2, [r0, #4]
87803af0:	f648 33ee 	movw	r3, #35822	; 0x8bee
87803af4:	429a      	cmp	r2, r3
87803af6:	d005      	beq.n	87803b04 <tos_prio_msg_q_flush+0x18>
87803af8:	f240 404d 	movw	r0, #1101	; 0x44d
87803afc:	4770      	bx	lr
87803afe:	f240 404c 	movw	r0, #1100	; 0x44c
87803b02:	4770      	bx	lr
87803b04:	3014      	adds	r0, #20
87803b06:	f7fd bb5b 	b.w	878011c0 <tos_prio_q_flush>
87803b0a:	bf00      	nop

87803b0c <tos_prio_msg_q_pend>:
87803b0c:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
87803b10:	4604      	mov	r4, r0
87803b12:	460d      	mov	r5, r1
87803b14:	4690      	mov	r8, r2
87803b16:	4699      	mov	r9, r3
87803b18:	f000 fb62 	bl	878041e0 <knl_is_inirq>
87803b1c:	b988      	cbnz	r0, 87803b42 <tos_prio_msg_q_pend+0x36>
87803b1e:	2d00      	cmp	r5, #0
87803b20:	bf18      	it	ne
87803b22:	2c00      	cmpne	r4, #0
87803b24:	bf0c      	ite	eq
87803b26:	2701      	moveq	r7, #1
87803b28:	2700      	movne	r7, #0
87803b2a:	d00f      	beq.n	87803b4c <tos_prio_msg_q_pend+0x40>
87803b2c:	6862      	ldr	r2, [r4, #4]
87803b2e:	f648 33ee 	movw	r3, #35822	; 0x8bee
87803b32:	429a      	cmp	r2, r3
87803b34:	bf18      	it	ne
87803b36:	f240 464d 	movwne	r6, #1101	; 0x44d
87803b3a:	d00a      	beq.n	87803b52 <tos_prio_msg_q_pend+0x46>
87803b3c:	4630      	mov	r0, r6
87803b3e:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
87803b42:	f44f 76c8 	mov.w	r6, #400	; 0x190
87803b46:	4630      	mov	r0, r6
87803b48:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
87803b4c:	f240 464c 	movw	r6, #1100	; 0x44c
87803b50:	e7f4      	b.n	87803b3c <tos_prio_msg_q_pend+0x30>
87803b52:	f001 f885 	bl	87804c60 <tos_cpu_cpsr_save>
87803b56:	463b      	mov	r3, r7
87803b58:	4682      	mov	sl, r0
87803b5a:	463a      	mov	r2, r7
87803b5c:	4629      	mov	r1, r5
87803b5e:	f104 0014 	add.w	r0, r4, #20
87803b62:	f7fd fab7 	bl	878010d4 <tos_prio_q_dequeue>
87803b66:	4606      	mov	r6, r0
87803b68:	b148      	cbz	r0, 87803b7e <tos_prio_msg_q_pend+0x72>
87803b6a:	ea58 0309 	orrs.w	r3, r8, r9
87803b6e:	d10a      	bne.n	87803b86 <tos_prio_msg_q_pend+0x7a>
87803b70:	602f      	str	r7, [r5, #0]
87803b72:	4650      	mov	r0, sl
87803b74:	f001 f876 	bl	87804c64 <tos_cpu_cpsr_restore>
87803b78:	f44f 6696 	mov.w	r6, #1200	; 0x4b0
87803b7c:	e7de      	b.n	87803b3c <tos_prio_msg_q_pend+0x30>
87803b7e:	4650      	mov	r0, sl
87803b80:	f001 f870 	bl	87804c64 <tos_cpu_cpsr_restore>
87803b84:	e7da      	b.n	87803b3c <tos_prio_msg_q_pend+0x30>
87803b86:	f000 fb21 	bl	878041cc <knl_is_sched_locked>
87803b8a:	b128      	cbz	r0, 87803b98 <tos_prio_msg_q_pend+0x8c>
87803b8c:	4650      	mov	r0, sl
87803b8e:	f240 46b1 	movw	r6, #1201	; 0x4b1
87803b92:	f001 f867 	bl	87804c64 <tos_cpu_cpsr_restore>
87803b96:	e7d1      	b.n	87803b3c <tos_prio_msg_q_pend+0x30>
87803b98:	f640 37a0 	movw	r7, #2976	; 0xba0
87803b9c:	4642      	mov	r2, r8
87803b9e:	f2c8 7781 	movt	r7, #34689	; 0x8781
87803ba2:	464b      	mov	r3, r9
87803ba4:	f104 0108 	add.w	r1, r4, #8
87803ba8:	6838      	ldr	r0, [r7, #0]
87803baa:	f000 fc09 	bl	878043c0 <pend_task_block>
87803bae:	4650      	mov	r0, sl
87803bb0:	f001 f858 	bl	87804c64 <tos_cpu_cpsr_restore>
87803bb4:	f000 fab4 	bl	87804120 <knl_sched>
87803bb8:	683b      	ldr	r3, [r7, #0]
87803bba:	6e18      	ldr	r0, [r3, #96]	; 0x60
87803bbc:	f000 fbc4 	bl	87804348 <pend_state2errno>
87803bc0:	4606      	mov	r6, r0
87803bc2:	2800      	cmp	r0, #0
87803bc4:	d1ba      	bne.n	87803b3c <tos_prio_msg_q_pend+0x30>
87803bc6:	683b      	ldr	r3, [r7, #0]
87803bc8:	4606      	mov	r6, r0
87803bca:	6e5b      	ldr	r3, [r3, #100]	; 0x64
87803bcc:	602b      	str	r3, [r5, #0]
87803bce:	683b      	ldr	r3, [r7, #0]
87803bd0:	6658      	str	r0, [r3, #100]	; 0x64
87803bd2:	e7b3      	b.n	87803b3c <tos_prio_msg_q_pend+0x30>

87803bd4 <tos_prio_msg_q_post>:
87803bd4:	2300      	movs	r3, #0
87803bd6:	e67b      	b.n	878038d0 <prio_msg_q_do_post>

87803bd8 <tos_prio_msg_q_post_all>:
87803bd8:	2301      	movs	r3, #1
87803bda:	e679      	b.n	878038d0 <prio_msg_q_do_post>

87803bdc <tos_chr_fifo_create>:
87803bdc:	2900      	cmp	r1, #0
87803bde:	bf18      	it	ne
87803be0:	2800      	cmpne	r0, #0
87803be2:	d00d      	beq.n	87803c00 <tos_chr_fifo_create+0x24>
87803be4:	b510      	push	{r4, lr}
87803be6:	2301      	movs	r3, #1
87803be8:	4604      	mov	r4, r0
87803bea:	3008      	adds	r0, #8
87803bec:	f000 fd22 	bl	87804634 <tos_ring_q_create>
87803bf0:	b928      	cbnz	r0, 87803bfe <tos_chr_fifo_create+0x22>
87803bf2:	2201      	movs	r2, #1
87803bf4:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803bf8:	e884 000c 	stmia.w	r4, {r2, r3}
87803bfc:	bd10      	pop	{r4, pc}
87803bfe:	bd10      	pop	{r4, pc}
87803c00:	f240 404c 	movw	r0, #1100	; 0x44c
87803c04:	4770      	bx	lr
87803c06:	bf00      	nop

87803c08 <tos_chr_fifo_destroy>:
87803c08:	b168      	cbz	r0, 87803c26 <tos_chr_fifo_destroy+0x1e>
87803c0a:	6842      	ldr	r2, [r0, #4]
87803c0c:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803c10:	429a      	cmp	r2, r3
87803c12:	d002      	beq.n	87803c1a <tos_chr_fifo_destroy+0x12>
87803c14:	f240 404d 	movw	r0, #1101	; 0x44d
87803c18:	4770      	bx	lr
87803c1a:	6803      	ldr	r3, [r0, #0]
87803c1c:	2b01      	cmp	r3, #1
87803c1e:	d005      	beq.n	87803c2c <tos_chr_fifo_destroy+0x24>
87803c20:	f240 404e 	movw	r0, #1102	; 0x44e
87803c24:	4770      	bx	lr
87803c26:	f240 404c 	movw	r0, #1100	; 0x44c
87803c2a:	4770      	bx	lr
87803c2c:	b510      	push	{r4, lr}
87803c2e:	4604      	mov	r4, r0
87803c30:	3008      	adds	r0, #8
87803c32:	f000 fd1b 	bl	8780466c <tos_ring_q_destroy>
87803c36:	4603      	mov	r3, r0
87803c38:	b910      	cbnz	r0, 87803c40 <tos_chr_fifo_destroy+0x38>
87803c3a:	6060      	str	r0, [r4, #4]
87803c3c:	6023      	str	r3, [r4, #0]
87803c3e:	bd10      	pop	{r4, pc}
87803c40:	bd10      	pop	{r4, pc}
87803c42:	bf00      	nop

87803c44 <tos_chr_fifo_create_dyn>:
87803c44:	b168      	cbz	r0, 87803c62 <tos_chr_fifo_create_dyn+0x1e>
87803c46:	b510      	push	{r4, lr}
87803c48:	2201      	movs	r2, #1
87803c4a:	4604      	mov	r4, r0
87803c4c:	3008      	adds	r0, #8
87803c4e:	f000 fd2b 	bl	878046a8 <tos_ring_q_create_dyn>
87803c52:	b928      	cbnz	r0, 87803c60 <tos_chr_fifo_create_dyn+0x1c>
87803c54:	2202      	movs	r2, #2
87803c56:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803c5a:	e884 000c 	stmia.w	r4, {r2, r3}
87803c5e:	bd10      	pop	{r4, pc}
87803c60:	bd10      	pop	{r4, pc}
87803c62:	f240 404c 	movw	r0, #1100	; 0x44c
87803c66:	4770      	bx	lr

87803c68 <tos_chr_fifo_destroy_dyn>:
87803c68:	b168      	cbz	r0, 87803c86 <tos_chr_fifo_destroy_dyn+0x1e>
87803c6a:	6842      	ldr	r2, [r0, #4]
87803c6c:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803c70:	429a      	cmp	r2, r3
87803c72:	d002      	beq.n	87803c7a <tos_chr_fifo_destroy_dyn+0x12>
87803c74:	f240 404d 	movw	r0, #1101	; 0x44d
87803c78:	4770      	bx	lr
87803c7a:	6803      	ldr	r3, [r0, #0]
87803c7c:	2b02      	cmp	r3, #2
87803c7e:	d005      	beq.n	87803c8c <tos_chr_fifo_destroy_dyn+0x24>
87803c80:	f240 404e 	movw	r0, #1102	; 0x44e
87803c84:	4770      	bx	lr
87803c86:	f240 404c 	movw	r0, #1100	; 0x44c
87803c8a:	4770      	bx	lr
87803c8c:	b510      	push	{r4, lr}
87803c8e:	4604      	mov	r4, r0
87803c90:	3008      	adds	r0, #8
87803c92:	f000 fd27 	bl	878046e4 <tos_ring_q_destroy_dyn>
87803c96:	4603      	mov	r3, r0
87803c98:	b910      	cbnz	r0, 87803ca0 <tos_chr_fifo_destroy_dyn+0x38>
87803c9a:	6060      	str	r0, [r4, #4]
87803c9c:	6023      	str	r3, [r4, #0]
87803c9e:	bd10      	pop	{r4, pc}
87803ca0:	bd10      	pop	{r4, pc}
87803ca2:	bf00      	nop

87803ca4 <tos_chr_fifo_push>:
87803ca4:	b500      	push	{lr}
87803ca6:	b083      	sub	sp, #12
87803ca8:	f88d 1007 	strb.w	r1, [sp, #7]
87803cac:	b150      	cbz	r0, 87803cc4 <tos_chr_fifo_push+0x20>
87803cae:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803cb2:	6842      	ldr	r2, [r0, #4]
87803cb4:	429a      	cmp	r2, r3
87803cb6:	bf18      	it	ne
87803cb8:	f240 404d 	movwne	r0, #1101	; 0x44d
87803cbc:	d007      	beq.n	87803cce <tos_chr_fifo_push+0x2a>
87803cbe:	b003      	add	sp, #12
87803cc0:	f85d fb04 	ldr.w	pc, [sp], #4
87803cc4:	f240 404c 	movw	r0, #1100	; 0x44c
87803cc8:	b003      	add	sp, #12
87803cca:	f85d fb04 	ldr.w	pc, [sp], #4
87803cce:	2201      	movs	r2, #1
87803cd0:	f10d 0107 	add.w	r1, sp, #7
87803cd4:	3008      	adds	r0, #8
87803cd6:	f000 fd27 	bl	87804728 <tos_ring_q_enqueue>
87803cda:	e7f0      	b.n	87803cbe <tos_chr_fifo_push+0x1a>

87803cdc <tos_chr_fifo_push_stream>:
87803cdc:	b318      	cbz	r0, 87803d26 <tos_chr_fifo_push_stream+0x4a>
87803cde:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87803ce2:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803ce6:	6844      	ldr	r4, [r0, #4]
87803ce8:	429c      	cmp	r4, r3
87803cea:	d002      	beq.n	87803cf2 <tos_chr_fifo_push_stream+0x16>
87803cec:	2000      	movs	r0, #0
87803cee:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803cf2:	4616      	mov	r6, r2
87803cf4:	4605      	mov	r5, r0
87803cf6:	460f      	mov	r7, r1
87803cf8:	f000 ffb2 	bl	87804c60 <tos_cpu_cpsr_save>
87803cfc:	4680      	mov	r8, r0
87803cfe:	b19e      	cbz	r6, 87803d28 <tos_chr_fifo_push_stream+0x4c>
87803d00:	3508      	adds	r5, #8
87803d02:	2400      	movs	r4, #0
87803d04:	e002      	b.n	87803d0c <tos_chr_fifo_push_stream+0x30>
87803d06:	3401      	adds	r4, #1
87803d08:	42b4      	cmp	r4, r6
87803d0a:	d00d      	beq.n	87803d28 <tos_chr_fifo_push_stream+0x4c>
87803d0c:	2201      	movs	r2, #1
87803d0e:	1939      	adds	r1, r7, r4
87803d10:	4628      	mov	r0, r5
87803d12:	f000 fd09 	bl	87804728 <tos_ring_q_enqueue>
87803d16:	2800      	cmp	r0, #0
87803d18:	d0f5      	beq.n	87803d06 <tos_chr_fifo_push_stream+0x2a>
87803d1a:	4640      	mov	r0, r8
87803d1c:	f000 ffa2 	bl	87804c64 <tos_cpu_cpsr_restore>
87803d20:	4620      	mov	r0, r4
87803d22:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803d26:	4770      	bx	lr
87803d28:	4640      	mov	r0, r8
87803d2a:	f000 ff9b 	bl	87804c64 <tos_cpu_cpsr_restore>
87803d2e:	4630      	mov	r0, r6
87803d30:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}

87803d34 <tos_chr_fifo_pop>:
87803d34:	b138      	cbz	r0, 87803d46 <tos_chr_fifo_pop+0x12>
87803d36:	6842      	ldr	r2, [r0, #4]
87803d38:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803d3c:	429a      	cmp	r2, r3
87803d3e:	d005      	beq.n	87803d4c <tos_chr_fifo_pop+0x18>
87803d40:	f240 404d 	movw	r0, #1101	; 0x44d
87803d44:	4770      	bx	lr
87803d46:	f240 404c 	movw	r0, #1100	; 0x44c
87803d4a:	4770      	bx	lr
87803d4c:	3008      	adds	r0, #8
87803d4e:	2200      	movs	r2, #0
87803d50:	f000 bd32 	b.w	878047b8 <tos_ring_q_dequeue>

87803d54 <tos_chr_fifo_pop_stream>:
87803d54:	b358      	cbz	r0, 87803dae <tos_chr_fifo_pop_stream+0x5a>
87803d56:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87803d5a:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803d5e:	6845      	ldr	r5, [r0, #4]
87803d60:	b082      	sub	sp, #8
87803d62:	429d      	cmp	r5, r3
87803d64:	bf18      	it	ne
87803d66:	2000      	movne	r0, #0
87803d68:	d002      	beq.n	87803d70 <tos_chr_fifo_pop_stream+0x1c>
87803d6a:	b002      	add	sp, #8
87803d6c:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87803d70:	4617      	mov	r7, r2
87803d72:	4605      	mov	r5, r0
87803d74:	460e      	mov	r6, r1
87803d76:	f000 ff73 	bl	87804c60 <tos_cpu_cpsr_save>
87803d7a:	4680      	mov	r8, r0
87803d7c:	b1c7      	cbz	r7, 87803db0 <tos_chr_fifo_pop_stream+0x5c>
87803d7e:	3508      	adds	r5, #8
87803d80:	3e01      	subs	r6, #1
87803d82:	2400      	movs	r4, #0
87803d84:	e006      	b.n	87803d94 <tos_chr_fifo_pop_stream+0x40>
87803d86:	f89d 3007 	ldrb.w	r3, [sp, #7]
87803d8a:	3401      	adds	r4, #1
87803d8c:	42bc      	cmp	r4, r7
87803d8e:	f806 3f01 	strb.w	r3, [r6, #1]!
87803d92:	d00d      	beq.n	87803db0 <tos_chr_fifo_pop_stream+0x5c>
87803d94:	2200      	movs	r2, #0
87803d96:	f10d 0107 	add.w	r1, sp, #7
87803d9a:	4628      	mov	r0, r5
87803d9c:	f000 fd0c 	bl	878047b8 <tos_ring_q_dequeue>
87803da0:	2800      	cmp	r0, #0
87803da2:	d0f0      	beq.n	87803d86 <tos_chr_fifo_pop_stream+0x32>
87803da4:	4640      	mov	r0, r8
87803da6:	f000 ff5d 	bl	87804c64 <tos_cpu_cpsr_restore>
87803daa:	4620      	mov	r0, r4
87803dac:	e7dd      	b.n	87803d6a <tos_chr_fifo_pop_stream+0x16>
87803dae:	4770      	bx	lr
87803db0:	4640      	mov	r0, r8
87803db2:	f000 ff57 	bl	87804c64 <tos_cpu_cpsr_restore>
87803db6:	4638      	mov	r0, r7
87803db8:	e7d7      	b.n	87803d6a <tos_chr_fifo_pop_stream+0x16>
87803dba:	bf00      	nop

87803dbc <tos_chr_fifo_flush>:
87803dbc:	b138      	cbz	r0, 87803dce <tos_chr_fifo_flush+0x12>
87803dbe:	6842      	ldr	r2, [r0, #4]
87803dc0:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803dc4:	429a      	cmp	r2, r3
87803dc6:	d005      	beq.n	87803dd4 <tos_chr_fifo_flush+0x18>
87803dc8:	f240 404d 	movw	r0, #1101	; 0x44d
87803dcc:	4770      	bx	lr
87803dce:	f240 404c 	movw	r0, #1100	; 0x44c
87803dd2:	4770      	bx	lr
87803dd4:	3008      	adds	r0, #8
87803dd6:	f000 bd33 	b.w	87804840 <tos_ring_q_flush>
87803dda:	bf00      	nop

87803ddc <tos_chr_fifo_is_empty>:
87803ddc:	b120      	cbz	r0, 87803de8 <tos_chr_fifo_is_empty+0xc>
87803dde:	6842      	ldr	r2, [r0, #4]
87803de0:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803de4:	429a      	cmp	r2, r3
87803de6:	d001      	beq.n	87803dec <tos_chr_fifo_is_empty+0x10>
87803de8:	2000      	movs	r0, #0
87803dea:	4770      	bx	lr
87803dec:	3008      	adds	r0, #8
87803dee:	f000 bd3f 	b.w	87804870 <tos_ring_q_is_empty>
87803df2:	bf00      	nop

87803df4 <tos_chr_fifo_is_full>:
87803df4:	b120      	cbz	r0, 87803e00 <tos_chr_fifo_is_full+0xc>
87803df6:	6842      	ldr	r2, [r0, #4]
87803df8:	f64d 23d2 	movw	r3, #56018	; 0xdad2
87803dfc:	429a      	cmp	r2, r3
87803dfe:	d001      	beq.n	87803e04 <tos_chr_fifo_is_full+0x10>
87803e00:	2000      	movs	r0, #0
87803e02:	4770      	bx	lr
87803e04:	3008      	adds	r0, #8
87803e06:	f000 bd49 	b.w	8780489c <tos_ring_q_is_full>
87803e0a:	bf00      	nop

87803e0c <tos_bitmap_create_empty>:
87803e0c:	b1c0      	cbz	r0, 87803e40 <tos_bitmap_create_empty+0x34>
87803e0e:	f102 031f 	add.w	r3, r2, #31
87803e12:	6101      	str	r1, [r0, #16]
87803e14:	095b      	lsrs	r3, r3, #5
87803e16:	60c2      	str	r2, [r0, #12]
87803e18:	6083      	str	r3, [r0, #8]
87803e1a:	b1ab      	cbz	r3, 87803e48 <tos_bitmap_create_empty+0x3c>
87803e1c:	2300      	movs	r3, #0
87803e1e:	b410      	push	{r4}
87803e20:	3904      	subs	r1, #4
87803e22:	461c      	mov	r4, r3
87803e24:	3301      	adds	r3, #1
87803e26:	f841 4f04 	str.w	r4, [r1, #4]!
87803e2a:	6882      	ldr	r2, [r0, #8]
87803e2c:	4293      	cmp	r3, r2
87803e2e:	d3f9      	bcc.n	87803e24 <tos_bitmap_create_empty+0x18>
87803e30:	f64d 22d1 	movw	r2, #56017	; 0xdad1
87803e34:	2300      	movs	r3, #0
87803e36:	6042      	str	r2, [r0, #4]
87803e38:	4618      	mov	r0, r3
87803e3a:	f85d 4b04 	ldr.w	r4, [sp], #4
87803e3e:	4770      	bx	lr
87803e40:	f240 434c 	movw	r3, #1100	; 0x44c
87803e44:	4618      	mov	r0, r3
87803e46:	4770      	bx	lr
87803e48:	f64d 22d1 	movw	r2, #56017	; 0xdad1
87803e4c:	2300      	movs	r3, #0
87803e4e:	6042      	str	r2, [r0, #4]
87803e50:	e7f8      	b.n	87803e44 <tos_bitmap_create_empty+0x38>
87803e52:	bf00      	nop

87803e54 <tos_bitmap_create_full>:
87803e54:	b1c8      	cbz	r0, 87803e8a <tos_bitmap_create_full+0x36>
87803e56:	f102 031f 	add.w	r3, r2, #31
87803e5a:	6101      	str	r1, [r0, #16]
87803e5c:	095b      	lsrs	r3, r3, #5
87803e5e:	60c2      	str	r2, [r0, #12]
87803e60:	6083      	str	r3, [r0, #8]
87803e62:	b1b3      	cbz	r3, 87803e92 <tos_bitmap_create_full+0x3e>
87803e64:	b410      	push	{r4}
87803e66:	3904      	subs	r1, #4
87803e68:	2300      	movs	r3, #0
87803e6a:	f04f 34ff 	mov.w	r4, #4294967295	; 0xffffffff
87803e6e:	3301      	adds	r3, #1
87803e70:	f841 4f04 	str.w	r4, [r1, #4]!
87803e74:	6882      	ldr	r2, [r0, #8]
87803e76:	4293      	cmp	r3, r2
87803e78:	d3f9      	bcc.n	87803e6e <tos_bitmap_create_full+0x1a>
87803e7a:	f64d 22d1 	movw	r2, #56017	; 0xdad1
87803e7e:	2300      	movs	r3, #0
87803e80:	6042      	str	r2, [r0, #4]
87803e82:	4618      	mov	r0, r3
87803e84:	f85d 4b04 	ldr.w	r4, [sp], #4
87803e88:	4770      	bx	lr
87803e8a:	f240 434c 	movw	r3, #1100	; 0x44c
87803e8e:	4618      	mov	r0, r3
87803e90:	4770      	bx	lr
87803e92:	f64d 22d1 	movw	r2, #56017	; 0xdad1
87803e96:	2300      	movs	r3, #0
87803e98:	6042      	str	r2, [r0, #4]
87803e9a:	e7f8      	b.n	87803e8e <tos_bitmap_create_full+0x3a>

87803e9c <tos_bitmap_destroy>:
87803e9c:	b158      	cbz	r0, 87803eb6 <tos_bitmap_destroy+0x1a>
87803e9e:	6842      	ldr	r2, [r0, #4]
87803ea0:	f64d 23d1 	movw	r3, #56017	; 0xdad1
87803ea4:	429a      	cmp	r2, r3
87803ea6:	d002      	beq.n	87803eae <tos_bitmap_destroy+0x12>
87803ea8:	f240 404d 	movw	r0, #1101	; 0x44d
87803eac:	4770      	bx	lr
87803eae:	2300      	movs	r3, #0
87803eb0:	6043      	str	r3, [r0, #4]
87803eb2:	4618      	mov	r0, r3
87803eb4:	4770      	bx	lr
87803eb6:	f240 404c 	movw	r0, #1100	; 0x44c
87803eba:	4770      	bx	lr

87803ebc <tos_bitmap_set>:
87803ebc:	b160      	cbz	r0, 87803ed8 <tos_bitmap_set+0x1c>
87803ebe:	6842      	ldr	r2, [r0, #4]
87803ec0:	f64d 23d1 	movw	r3, #56017	; 0xdad1
87803ec4:	429a      	cmp	r2, r3
87803ec6:	d002      	beq.n	87803ece <tos_bitmap_set+0x12>
87803ec8:	f240 404d 	movw	r0, #1101	; 0x44d
87803ecc:	4770      	bx	lr
87803ece:	68c3      	ldr	r3, [r0, #12]
87803ed0:	428b      	cmp	r3, r1
87803ed2:	d204      	bcs.n	87803ede <tos_bitmap_set+0x22>
87803ed4:	200a      	movs	r0, #10
87803ed6:	4770      	bx	lr
87803ed8:	f240 404c 	movw	r0, #1100	; 0x44c
87803edc:	4770      	bx	lr
87803ede:	6902      	ldr	r2, [r0, #16]
87803ee0:	2301      	movs	r3, #1
87803ee2:	b430      	push	{r4, r5}
87803ee4:	094c      	lsrs	r4, r1, #5
87803ee6:	43c9      	mvns	r1, r1
87803ee8:	f001 011f 	and.w	r1, r1, #31
87803eec:	2000      	movs	r0, #0
87803eee:	f852 5024 	ldr.w	r5, [r2, r4, lsl #2]
87803ef2:	408b      	lsls	r3, r1
87803ef4:	432b      	orrs	r3, r5
87803ef6:	f842 3024 	str.w	r3, [r2, r4, lsl #2]
87803efa:	bc30      	pop	{r4, r5}
87803efc:	4770      	bx	lr
87803efe:	bf00      	nop

87803f00 <tos_bitmap_reset>:
87803f00:	b160      	cbz	r0, 87803f1c <tos_bitmap_reset+0x1c>
87803f02:	6842      	ldr	r2, [r0, #4]
87803f04:	f64d 23d1 	movw	r3, #56017	; 0xdad1
87803f08:	429a      	cmp	r2, r3
87803f0a:	d002      	beq.n	87803f12 <tos_bitmap_reset+0x12>
87803f0c:	f240 404d 	movw	r0, #1101	; 0x44d
87803f10:	4770      	bx	lr
87803f12:	68c3      	ldr	r3, [r0, #12]
87803f14:	428b      	cmp	r3, r1
87803f16:	d204      	bcs.n	87803f22 <tos_bitmap_reset+0x22>
87803f18:	200a      	movs	r0, #10
87803f1a:	4770      	bx	lr
87803f1c:	f240 404c 	movw	r0, #1100	; 0x44c
87803f20:	4770      	bx	lr
87803f22:	b430      	push	{r4, r5}
87803f24:	094d      	lsrs	r5, r1, #5
87803f26:	6904      	ldr	r4, [r0, #16]
87803f28:	43c9      	mvns	r1, r1
87803f2a:	f001 011f 	and.w	r1, r1, #31
87803f2e:	2201      	movs	r2, #1
87803f30:	408a      	lsls	r2, r1
87803f32:	2000      	movs	r0, #0
87803f34:	f854 3025 	ldr.w	r3, [r4, r5, lsl #2]
87803f38:	ea23 0302 	bic.w	r3, r3, r2
87803f3c:	f844 3025 	str.w	r3, [r4, r5, lsl #2]
87803f40:	bc30      	pop	{r4, r5}
87803f42:	4770      	bx	lr

87803f44 <tos_bitmap_is_set>:
87803f44:	b128      	cbz	r0, 87803f52 <tos_bitmap_is_set+0xe>
87803f46:	6842      	ldr	r2, [r0, #4]
87803f48:	f64d 23d1 	movw	r3, #56017	; 0xdad1
87803f4c:	429a      	cmp	r2, r3
87803f4e:	d001      	beq.n	87803f54 <tos_bitmap_is_set+0x10>
87803f50:	2000      	movs	r0, #0
87803f52:	4770      	bx	lr
87803f54:	68c3      	ldr	r3, [r0, #12]
87803f56:	428b      	cmp	r3, r1
87803f58:	d3fa      	bcc.n	87803f50 <tos_bitmap_is_set+0xc>
87803f5a:	6902      	ldr	r2, [r0, #16]
87803f5c:	0948      	lsrs	r0, r1, #5
87803f5e:	43c9      	mvns	r1, r1
87803f60:	f001 011f 	and.w	r1, r1, #31
87803f64:	2301      	movs	r3, #1
87803f66:	f852 2020 	ldr.w	r2, [r2, r0, lsl #2]
87803f6a:	408b      	lsls	r3, r1
87803f6c:	4213      	tst	r3, r2
87803f6e:	bf14      	ite	ne
87803f70:	2001      	movne	r0, #1
87803f72:	2000      	moveq	r0, #0
87803f74:	4770      	bx	lr
87803f76:	bf00      	nop

87803f78 <tos_bitmap_is_reset>:
87803f78:	b538      	push	{r3, r4, r5, lr}
87803f7a:	b178      	cbz	r0, 87803f9c <tos_bitmap_is_reset+0x24>
87803f7c:	6844      	ldr	r4, [r0, #4]
87803f7e:	f64d 22d1 	movw	r2, #56017	; 0xdad1
87803f82:	4294      	cmp	r4, r2
87803f84:	d001      	beq.n	87803f8a <tos_bitmap_is_reset+0x12>
87803f86:	2000      	movs	r0, #0
87803f88:	bd38      	pop	{r3, r4, r5, pc}
87803f8a:	68c3      	ldr	r3, [r0, #12]
87803f8c:	428b      	cmp	r3, r1
87803f8e:	d3fa      	bcc.n	87803f86 <tos_bitmap_is_reset+0xe>
87803f90:	f7ff ffd8 	bl	87803f44 <tos_bitmap_is_set>
87803f94:	fab0 f080 	clz	r0, r0
87803f98:	0940      	lsrs	r0, r0, #5
87803f9a:	bd38      	pop	{r3, r4, r5, pc}
87803f9c:	bd38      	pop	{r3, r4, r5, pc}
87803f9e:	bf00      	nop

87803fa0 <tos_bitmap_lsb>:
87803fa0:	b318      	cbz	r0, 87803fea <tos_bitmap_lsb+0x4a>
87803fa2:	6842      	ldr	r2, [r0, #4]
87803fa4:	b538      	push	{r3, r4, r5, lr}
87803fa6:	f64d 23d1 	movw	r3, #56017	; 0xdad1
87803faa:	429a      	cmp	r2, r3
87803fac:	d11a      	bne.n	87803fe4 <tos_bitmap_lsb+0x44>
87803fae:	6884      	ldr	r4, [r0, #8]
87803fb0:	6901      	ldr	r1, [r0, #16]
87803fb2:	3c01      	subs	r4, #1
87803fb4:	bf18      	it	ne
87803fb6:	2300      	movne	r3, #0
87803fb8:	680a      	ldr	r2, [r1, #0]
87803fba:	bf18      	it	ne
87803fbc:	461d      	movne	r5, r3
87803fbe:	d007      	beq.n	87803fd0 <tos_bitmap_lsb+0x30>
87803fc0:	3301      	adds	r3, #1
87803fc2:	b912      	cbnz	r2, 87803fca <tos_bitmap_lsb+0x2a>
87803fc4:	684a      	ldr	r2, [r1, #4]
87803fc6:	3520      	adds	r5, #32
87803fc8:	3104      	adds	r1, #4
87803fca:	42a3      	cmp	r3, r4
87803fcc:	d1f8      	bne.n	87803fc0 <tos_bitmap_lsb+0x20>
87803fce:	462c      	mov	r4, r5
87803fd0:	4605      	mov	r5, r0
87803fd2:	4610      	mov	r0, r2
87803fd4:	f000 fe20 	bl	87804c18 <tos_cpu_clz>
87803fd8:	68eb      	ldr	r3, [r5, #12]
87803fda:	4420      	add	r0, r4
87803fdc:	4298      	cmp	r0, r3
87803fde:	d903      	bls.n	87803fe8 <tos_bitmap_lsb+0x48>
87803fe0:	1c58      	adds	r0, r3, #1
87803fe2:	bd38      	pop	{r3, r4, r5, pc}
87803fe4:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
87803fe8:	bd38      	pop	{r3, r4, r5, pc}
87803fea:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
87803fee:	4770      	bx	lr

87803ff0 <knl_idle_entry>:
87803ff0:	e7fe      	b.n	87803ff0 <knl_idle_entry>
87803ff2:	bf00      	nop

87803ff4 <tos_knl_irq_enter>:
87803ff4:	f640 33b0 	movw	r3, #2992	; 0xbb0
87803ff8:	f2c8 7381 	movt	r3, #34689	; 0x8781
87803ffc:	681b      	ldr	r3, [r3, #0]
87803ffe:	2b01      	cmp	r3, #1
87804000:	d108      	bne.n	87804014 <tos_knl_irq_enter+0x20>
87804002:	f640 33b5 	movw	r3, #2997	; 0xbb5
87804006:	f2c8 7381 	movt	r3, #34689	; 0x8781
8780400a:	781a      	ldrb	r2, [r3, #0]
8780400c:	2af9      	cmp	r2, #249	; 0xf9
8780400e:	bf9c      	itt	ls
87804010:	3201      	addls	r2, #1
87804012:	701a      	strbls	r2, [r3, #0]
87804014:	4770      	bx	lr
87804016:	bf00      	nop

87804018 <tos_knl_irq_leave>:
87804018:	f640 33b0 	movw	r3, #2992	; 0xbb0
8780401c:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804020:	681b      	ldr	r3, [r3, #0]
87804022:	2b01      	cmp	r3, #1
87804024:	d000      	beq.n	87804028 <tos_knl_irq_leave+0x10>
87804026:	4770      	bx	lr
87804028:	b510      	push	{r4, lr}
8780402a:	f000 fe19 	bl	87804c60 <tos_cpu_cpsr_save>
8780402e:	f640 32b5 	movw	r2, #2997	; 0xbb5
87804032:	4604      	mov	r4, r0
87804034:	f2c8 7281 	movt	r2, #34689	; 0x8781
87804038:	7813      	ldrb	r3, [r2, #0]
8780403a:	b1d3      	cbz	r3, 87804072 <tos_knl_irq_leave+0x5a>
8780403c:	3b01      	subs	r3, #1
8780403e:	b2db      	uxtb	r3, r3
87804040:	7013      	strb	r3, [r2, #0]
87804042:	b9b3      	cbnz	r3, 87804072 <tos_knl_irq_leave+0x5a>
87804044:	f640 33b4 	movw	r3, #2996	; 0xbb4
87804048:	f2c8 7381 	movt	r3, #34689	; 0x8781
8780404c:	781b      	ldrb	r3, [r3, #0]
8780404e:	b983      	cbnz	r3, 87804072 <tos_knl_irq_leave+0x5a>
87804050:	f7fe fda8 	bl	87802ba4 <readyqueue_highest_ready_task_get>
87804054:	f640 32a0 	movw	r2, #2976	; 0xba0
87804058:	f640 339c 	movw	r3, #2972	; 0xb9c
8780405c:	f2c8 7281 	movt	r2, #34689	; 0x8781
87804060:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804064:	6812      	ldr	r2, [r2, #0]
87804066:	6018      	str	r0, [r3, #0]
87804068:	4290      	cmp	r0, r2
8780406a:	d001      	beq.n	87804070 <tos_knl_irq_leave+0x58>
8780406c:	f000 fe20 	bl	87804cb0 <cpu_irq_context_switch>
87804070:	4620      	mov	r0, r4
87804072:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
87804076:	f000 bdf5 	b.w	87804c64 <tos_cpu_cpsr_restore>
8780407a:	bf00      	nop

8780407c <tos_knl_sched_lock>:
8780407c:	b538      	push	{r3, r4, r5, lr}
8780407e:	f640 33b5 	movw	r3, #2997	; 0xbb5
87804082:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804086:	781c      	ldrb	r4, [r3, #0]
87804088:	b99c      	cbnz	r4, 878040b2 <tos_knl_sched_lock+0x36>
8780408a:	f640 33b0 	movw	r3, #2992	; 0xbb0
8780408e:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804092:	681b      	ldr	r3, [r3, #0]
87804094:	2b01      	cmp	r3, #1
87804096:	d002      	beq.n	8780409e <tos_knl_sched_lock+0x22>
87804098:	f44f 70fa 	mov.w	r0, #500	; 0x1f4
8780409c:	bd38      	pop	{r3, r4, r5, pc}
8780409e:	f640 35b4 	movw	r5, #2996	; 0xbb4
878040a2:	f2c8 7581 	movt	r5, #34689	; 0x8781
878040a6:	782b      	ldrb	r3, [r5, #0]
878040a8:	2bf9      	cmp	r3, #249	; 0xf9
878040aa:	d905      	bls.n	878040b8 <tos_knl_sched_lock+0x3c>
878040ac:	f44f 7016 	mov.w	r0, #600	; 0x258
878040b0:	bd38      	pop	{r3, r4, r5, pc}
878040b2:	f44f 70c8 	mov.w	r0, #400	; 0x190
878040b6:	bd38      	pop	{r3, r4, r5, pc}
878040b8:	f000 fdd2 	bl	87804c60 <tos_cpu_cpsr_save>
878040bc:	782b      	ldrb	r3, [r5, #0]
878040be:	3301      	adds	r3, #1
878040c0:	702b      	strb	r3, [r5, #0]
878040c2:	f000 fdcf 	bl	87804c64 <tos_cpu_cpsr_restore>
878040c6:	4620      	mov	r0, r4
878040c8:	bd38      	pop	{r3, r4, r5, pc}
878040ca:	bf00      	nop

878040cc <tos_knl_start>:
878040cc:	b510      	push	{r4, lr}
878040ce:	f640 34b0 	movw	r4, #2992	; 0xbb0
878040d2:	f2c8 7481 	movt	r4, #34689	; 0x8781
878040d6:	6823      	ldr	r3, [r4, #0]
878040d8:	2b01      	cmp	r3, #1
878040da:	d011      	beq.n	87804100 <tos_knl_start+0x34>
878040dc:	f7fe fd62 	bl	87802ba4 <readyqueue_highest_ready_task_get>
878040e0:	f640 329c 	movw	r2, #2972	; 0xb9c
878040e4:	f640 33a0 	movw	r3, #2976	; 0xba0
878040e8:	f2c8 7281 	movt	r2, #34689	; 0x8781
878040ec:	f2c8 7381 	movt	r3, #34689	; 0x8781
878040f0:	2101      	movs	r1, #1
878040f2:	6010      	str	r0, [r2, #0]
878040f4:	6018      	str	r0, [r3, #0]
878040f6:	6021      	str	r1, [r4, #0]
878040f8:	f000 fdd4 	bl	87804ca4 <cpu_sched_start>
878040fc:	2000      	movs	r0, #0
878040fe:	bd10      	pop	{r4, pc}
87804100:	f240 10f5 	movw	r0, #501	; 0x1f5
87804104:	bd10      	pop	{r4, pc}
87804106:	bf00      	nop

87804108 <tos_knl_is_running>:
87804108:	f640 33b0 	movw	r3, #2992	; 0xbb0
8780410c:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804110:	6818      	ldr	r0, [r3, #0]
87804112:	f1a0 0001 	sub.w	r0, r0, #1
87804116:	fab0 f080 	clz	r0, r0
8780411a:	0940      	lsrs	r0, r0, #5
8780411c:	4770      	bx	lr
8780411e:	bf00      	nop

87804120 <knl_sched>:
87804120:	f640 33b0 	movw	r3, #2992	; 0xbb0
87804124:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804128:	681b      	ldr	r3, [r3, #0]
8780412a:	2b01      	cmp	r3, #1
8780412c:	d10b      	bne.n	87804146 <knl_sched+0x26>
8780412e:	f640 33b5 	movw	r3, #2997	; 0xbb5
87804132:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804136:	781b      	ldrb	r3, [r3, #0]
87804138:	b92b      	cbnz	r3, 87804146 <knl_sched+0x26>
8780413a:	f640 33b4 	movw	r3, #2996	; 0xbb4
8780413e:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804142:	781b      	ldrb	r3, [r3, #0]
87804144:	b103      	cbz	r3, 87804148 <knl_sched+0x28>
87804146:	4770      	bx	lr
87804148:	b510      	push	{r4, lr}
8780414a:	f000 fd89 	bl	87804c60 <tos_cpu_cpsr_save>
8780414e:	4604      	mov	r4, r0
87804150:	f7fe fd28 	bl	87802ba4 <readyqueue_highest_ready_task_get>
87804154:	f640 32a0 	movw	r2, #2976	; 0xba0
87804158:	f640 339c 	movw	r3, #2972	; 0xb9c
8780415c:	f2c8 7281 	movt	r2, #34689	; 0x8781
87804160:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804164:	6812      	ldr	r2, [r2, #0]
87804166:	6018      	str	r0, [r3, #0]
87804168:	4290      	cmp	r0, r2
8780416a:	d001      	beq.n	87804170 <knl_sched+0x50>
8780416c:	f000 fd9e 	bl	87804cac <cpu_context_switch>
87804170:	4620      	mov	r0, r4
87804172:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
87804176:	f000 bd75 	b.w	87804c64 <tos_cpu_cpsr_restore>
8780417a:	bf00      	nop

8780417c <tos_knl_sched_unlock>:
8780417c:	b538      	push	{r3, r4, r5, lr}
8780417e:	f640 33b5 	movw	r3, #2997	; 0xbb5
87804182:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804186:	781c      	ldrb	r4, [r3, #0]
87804188:	b994      	cbnz	r4, 878041b0 <tos_knl_sched_unlock+0x34>
8780418a:	f640 33b0 	movw	r3, #2992	; 0xbb0
8780418e:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804192:	681b      	ldr	r3, [r3, #0]
87804194:	2b01      	cmp	r3, #1
87804196:	d002      	beq.n	8780419e <tos_knl_sched_unlock+0x22>
87804198:	f44f 70fa 	mov.w	r0, #500	; 0x1f4
8780419c:	bd38      	pop	{r3, r4, r5, pc}
8780419e:	f640 35b4 	movw	r5, #2996	; 0xbb4
878041a2:	f2c8 7581 	movt	r5, #34689	; 0x8781
878041a6:	782b      	ldrb	r3, [r5, #0]
878041a8:	b92b      	cbnz	r3, 878041b6 <tos_knl_sched_unlock+0x3a>
878041aa:	f240 60a5 	movw	r0, #1701	; 0x6a5
878041ae:	bd38      	pop	{r3, r4, r5, pc}
878041b0:	f44f 70c8 	mov.w	r0, #400	; 0x190
878041b4:	bd38      	pop	{r3, r4, r5, pc}
878041b6:	f000 fd53 	bl	87804c60 <tos_cpu_cpsr_save>
878041ba:	782b      	ldrb	r3, [r5, #0]
878041bc:	3b01      	subs	r3, #1
878041be:	702b      	strb	r3, [r5, #0]
878041c0:	f000 fd50 	bl	87804c64 <tos_cpu_cpsr_restore>
878041c4:	f7ff ffac 	bl	87804120 <knl_sched>
878041c8:	4620      	mov	r0, r4
878041ca:	bd38      	pop	{r3, r4, r5, pc}

878041cc <knl_is_sched_locked>:
878041cc:	f640 33b4 	movw	r3, #2996	; 0xbb4
878041d0:	f2c8 7381 	movt	r3, #34689	; 0x8781
878041d4:	7818      	ldrb	r0, [r3, #0]
878041d6:	3000      	adds	r0, #0
878041d8:	bf18      	it	ne
878041da:	2001      	movne	r0, #1
878041dc:	4770      	bx	lr
878041de:	bf00      	nop

878041e0 <knl_is_inirq>:
878041e0:	f640 33b5 	movw	r3, #2997	; 0xbb5
878041e4:	f2c8 7381 	movt	r3, #34689	; 0x8781
878041e8:	7818      	ldrb	r0, [r3, #0]
878041ea:	3000      	adds	r0, #0
878041ec:	bf18      	it	ne
878041ee:	2001      	movne	r0, #1
878041f0:	4770      	bx	lr
878041f2:	bf00      	nop

878041f4 <knl_is_idle>:
878041f4:	f243 03b0 	movw	r3, #12464	; 0x30b0
878041f8:	f2c8 7381 	movt	r3, #34689	; 0x8781
878041fc:	1a18      	subs	r0, r3, r0
878041fe:	fab0 f080 	clz	r0, r0
87804202:	0940      	lsrs	r0, r0, #5
87804204:	4770      	bx	lr
87804206:	bf00      	nop

87804208 <knl_is_self>:
87804208:	f640 33a0 	movw	r3, #2976	; 0xba0
8780420c:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804210:	681b      	ldr	r3, [r3, #0]
87804212:	1a18      	subs	r0, r3, r0
87804214:	fab0 f080 	clz	r0, r0
87804218:	0940      	lsrs	r0, r0, #5
8780421a:	4770      	bx	lr

8780421c <knl_idle_init>:
8780421c:	f249 4274 	movw	r2, #38004	; 0x9474
87804220:	f249 4378 	movw	r3, #38008	; 0x9478
87804224:	f2c8 7280 	movt	r2, #34688	; 0x8780
87804228:	f2c8 7380 	movt	r3, #34688	; 0x8780
8780422c:	b570      	push	{r4, r5, r6, lr}
8780422e:	f64d 5108 	movw	r1, #56584	; 0xdd08
87804232:	6816      	ldr	r6, [r2, #0]
87804234:	b084      	sub	sp, #16
87804236:	2400      	movs	r4, #0
87804238:	2509      	movs	r5, #9
8780423a:	681b      	ldr	r3, [r3, #0]
8780423c:	f643 72f1 	movw	r2, #16369	; 0x3ff1
87804240:	f243 00b0 	movw	r0, #12464	; 0x30b0
87804244:	9602      	str	r6, [sp, #8]
87804246:	9403      	str	r4, [sp, #12]
87804248:	f2c8 7280 	movt	r2, #34688	; 0x8780
8780424c:	9301      	str	r3, [sp, #4]
8780424e:	f2c8 7180 	movt	r1, #34688	; 0x8780
87804252:	4623      	mov	r3, r4
87804254:	9500      	str	r5, [sp, #0]
87804256:	f2c8 7081 	movt	r0, #34689	; 0x8781
8780425a:	f7fe fedb 	bl	87803014 <tos_task_create>
8780425e:	b004      	add	sp, #16
87804260:	bd70      	pop	{r4, r5, r6, pc}
87804262:	bf00      	nop

87804264 <tos_knl_init>:
87804264:	b508      	push	{r3, lr}
87804266:	f000 fcff 	bl	87804c68 <cpu_init>
8780426a:	f7fe fca5 	bl	87802bb8 <readyqueue_init>
8780426e:	f243 1030 	movw	r0, #12592	; 0x3130
87804272:	f44f 4100 	mov.w	r1, #32768	; 0x8000
87804276:	f2c8 7081 	movt	r0, #34689	; 0x8781
8780427a:	f7fc fd25 	bl	87800cc8 <mmheap_init_with_pool>
8780427e:	b100      	cbz	r0, 87804282 <tos_knl_init+0x1e>
87804280:	bd08      	pop	{r3, pc}
87804282:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
87804286:	f7ff bfc9 	b.w	8780421c <knl_idle_init>
8780428a:	bf00      	nop

8780428c <pend_list_add>:
8780428c:	680b      	ldr	r3, [r1, #0]
8780428e:	b470      	push	{r4, r5, r6}
87804290:	460e      	mov	r6, r1
87804292:	428b      	cmp	r3, r1
87804294:	f1a3 0248 	sub.w	r2, r3, #72	; 0x48
87804298:	d00f      	beq.n	878042ba <pend_list_add+0x2e>
8780429a:	f890 5025 	ldrb.w	r5, [r0, #37]	; 0x25
8780429e:	f813 4c23 	ldrb.w	r4, [r3, #-35]
878042a2:	42ac      	cmp	r4, r5
878042a4:	d904      	bls.n	878042b0 <pend_list_add+0x24>
878042a6:	e018      	b.n	878042da <pend_list_add+0x4e>
878042a8:	f813 4c23 	ldrb.w	r4, [r3, #-35]
878042ac:	42ac      	cmp	r4, r5
878042ae:	d814      	bhi.n	878042da <pend_list_add+0x4e>
878042b0:	6c93      	ldr	r3, [r2, #72]	; 0x48
878042b2:	428b      	cmp	r3, r1
878042b4:	f1a3 0248 	sub.w	r2, r3, #72	; 0x48
878042b8:	d1f6      	bne.n	878042a8 <pend_list_add+0x1c>
878042ba:	6cd3      	ldr	r3, [r2, #76]	; 0x4c
878042bc:	f100 0448 	add.w	r4, r0, #72	; 0x48
878042c0:	64d4      	str	r4, [r2, #76]	; 0x4c
878042c2:	6486      	str	r6, [r0, #72]	; 0x48
878042c4:	64c3      	str	r3, [r0, #76]	; 0x4c
878042c6:	601c      	str	r4, [r3, #0]
878042c8:	f890 3024 	ldrb.w	r3, [r0, #36]	; 0x24
878042cc:	65c1      	str	r1, [r0, #92]	; 0x5c
878042ce:	bc70      	pop	{r4, r5, r6}
878042d0:	f043 0302 	orr.w	r3, r3, #2
878042d4:	f880 3024 	strb.w	r3, [r0, #36]	; 0x24
878042d8:	4770      	bx	lr
878042da:	461e      	mov	r6, r3
878042dc:	e7ed      	b.n	878042ba <pend_list_add+0x2e>
878042de:	bf00      	nop

878042e0 <pend_highest_pending_prio_get>:
878042e0:	6803      	ldr	r3, [r0, #0]
878042e2:	4298      	cmp	r0, r3
878042e4:	d004      	beq.n	878042f0 <pend_highest_pending_prio_get+0x10>
878042e6:	2b48      	cmp	r3, #72	; 0x48
878042e8:	d002      	beq.n	878042f0 <pend_highest_pending_prio_get+0x10>
878042ea:	f813 0c23 	ldrb.w	r0, [r3, #-35]
878042ee:	4770      	bx	lr
878042f0:	200a      	movs	r0, #10
878042f2:	4770      	bx	lr

878042f4 <pend_highest_pending_task_get>:
878042f4:	6800      	ldr	r0, [r0, #0]
878042f6:	3848      	subs	r0, #72	; 0x48
878042f8:	4770      	bx	lr
878042fa:	bf00      	nop

878042fc <pend_list_remove>:
878042fc:	6cc3      	ldr	r3, [r0, #76]	; 0x4c
878042fe:	2100      	movs	r1, #0
87804300:	6c82      	ldr	r2, [r0, #72]	; 0x48
87804302:	6053      	str	r3, [r2, #4]
87804304:	601a      	str	r2, [r3, #0]
87804306:	f890 3024 	ldrb.w	r3, [r0, #36]	; 0x24
8780430a:	65c1      	str	r1, [r0, #92]	; 0x5c
8780430c:	f023 0302 	bic.w	r3, r3, #2
87804310:	f880 3024 	strb.w	r3, [r0, #36]	; 0x24
87804314:	4770      	bx	lr
87804316:	bf00      	nop

87804318 <pend_object_init>:
87804318:	6000      	str	r0, [r0, #0]
8780431a:	6040      	str	r0, [r0, #4]
8780431c:	4770      	bx	lr
8780431e:	bf00      	nop

87804320 <pend_object_deinit>:
87804320:	6000      	str	r0, [r0, #0]
87804322:	6040      	str	r0, [r0, #4]
87804324:	4770      	bx	lr
87804326:	bf00      	nop

87804328 <pend_is_nopending>:
87804328:	6803      	ldr	r3, [r0, #0]
8780432a:	1a18      	subs	r0, r3, r0
8780432c:	fab0 f080 	clz	r0, r0
87804330:	0940      	lsrs	r0, r0, #5
87804332:	4770      	bx	lr

87804334 <pend_list_adjust>:
87804334:	b410      	push	{r4}
87804336:	6cc2      	ldr	r2, [r0, #76]	; 0x4c
87804338:	6c84      	ldr	r4, [r0, #72]	; 0x48
8780433a:	6dc1      	ldr	r1, [r0, #92]	; 0x5c
8780433c:	6062      	str	r2, [r4, #4]
8780433e:	6014      	str	r4, [r2, #0]
87804340:	f85d 4b04 	ldr.w	r4, [sp], #4
87804344:	e7a2      	b.n	8780428c <pend_list_add>
87804346:	bf00      	nop

87804348 <pend_state2errno>:
87804348:	2801      	cmp	r0, #1
8780434a:	d00f      	beq.n	8780436c <pend_state2errno+0x24>
8780434c:	2802      	cmp	r0, #2
8780434e:	d00a      	beq.n	87804366 <pend_state2errno+0x1e>
87804350:	2803      	cmp	r0, #3
87804352:	d00d      	beq.n	87804370 <pend_state2errno+0x28>
87804354:	f240 43b5 	movw	r3, #1205	; 0x4b5
87804358:	f240 42b2 	movw	r2, #1202	; 0x4b2
8780435c:	2804      	cmp	r0, #4
8780435e:	bf0c      	ite	eq
87804360:	4618      	moveq	r0, r3
87804362:	4610      	movne	r0, r2
87804364:	4770      	bx	lr
87804366:	f240 40b3 	movw	r0, #1203	; 0x4b3
8780436a:	4770      	bx	lr
8780436c:	2000      	movs	r0, #0
8780436e:	4770      	bx	lr
87804370:	f240 40b4 	movw	r0, #1204	; 0x4b4
87804374:	4770      	bx	lr
87804376:	bf00      	nop

87804378 <pend_task_wakeup>:
87804378:	b510      	push	{r4, lr}
8780437a:	4604      	mov	r4, r0
8780437c:	f890 3024 	ldrb.w	r3, [r0, #36]	; 0x24
87804380:	0798      	lsls	r0, r3, #30
87804382:	d50c      	bpl.n	8780439e <pend_task_wakeup+0x26>
87804384:	6ce3      	ldr	r3, [r4, #76]	; 0x4c
87804386:	2000      	movs	r0, #0
87804388:	6ca2      	ldr	r2, [r4, #72]	; 0x48
8780438a:	6621      	str	r1, [r4, #96]	; 0x60
8780438c:	6053      	str	r3, [r2, #4]
8780438e:	601a      	str	r2, [r3, #0]
87804390:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
87804394:	65e0      	str	r0, [r4, #92]	; 0x5c
87804396:	f003 03fd 	and.w	r3, r3, #253	; 0xfd
8780439a:	f884 3024 	strb.w	r3, [r4, #36]	; 0x24
8780439e:	07da      	lsls	r2, r3, #31
878043a0:	d407      	bmi.n	878043b2 <pend_task_wakeup+0x3a>
878043a2:	075b      	lsls	r3, r3, #29
878043a4:	d500      	bpl.n	878043a8 <pend_task_wakeup+0x30>
878043a6:	bd10      	pop	{r4, pc}
878043a8:	4620      	mov	r0, r4
878043aa:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
878043ae:	f7fe bc6b 	b.w	87802c88 <readyqueue_add>
878043b2:	4620      	mov	r0, r4
878043b4:	f7fd ff4c 	bl	87802250 <tick_list_remove>
878043b8:	f894 3024 	ldrb.w	r3, [r4, #36]	; 0x24
878043bc:	e7f1      	b.n	878043a2 <pend_task_wakeup+0x2a>
878043be:	bf00      	nop

878043c0 <pend_task_block>:
878043c0:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
878043c4:	4616      	mov	r6, r2
878043c6:	461f      	mov	r7, r3
878043c8:	4604      	mov	r4, r0
878043ca:	460d      	mov	r5, r1
878043cc:	f7fe fc6c 	bl	87802ca8 <readyqueue_remove>
878043d0:	2300      	movs	r3, #0
878043d2:	4629      	mov	r1, r5
878043d4:	6623      	str	r3, [r4, #96]	; 0x60
878043d6:	4620      	mov	r0, r4
878043d8:	f7ff ff58 	bl	8780428c <pend_list_add>
878043dc:	1c7b      	adds	r3, r7, #1
878043de:	bf08      	it	eq
878043e0:	f1b6 3fff 	cmpeq.w	r6, #4294967295	; 0xffffffff
878043e4:	d006      	beq.n	878043f4 <pend_task_block+0x34>
878043e6:	4632      	mov	r2, r6
878043e8:	463b      	mov	r3, r7
878043ea:	4620      	mov	r0, r4
878043ec:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
878043f0:	f7fd bec2 	b.w	87802178 <tick_list_add>
878043f4:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}

878043f8 <pend_wakeup_one>:
878043f8:	6800      	ldr	r0, [r0, #0]
878043fa:	3848      	subs	r0, #72	; 0x48
878043fc:	f7ff bfbc 	b.w	87804378 <pend_task_wakeup>

87804400 <pend_wakeup_all>:
87804400:	6803      	ldr	r3, [r0, #0]
87804402:	b570      	push	{r4, r5, r6, lr}
87804404:	4605      	mov	r5, r0
87804406:	460e      	mov	r6, r1
87804408:	429d      	cmp	r5, r3
8780440a:	f1a3 0048 	sub.w	r0, r3, #72	; 0x48
8780440e:	681c      	ldr	r4, [r3, #0]
87804410:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
87804414:	d00a      	beq.n	8780442c <pend_wakeup_all+0x2c>
87804416:	4631      	mov	r1, r6
87804418:	f7ff ffae 	bl	87804378 <pend_task_wakeup>
8780441c:	4623      	mov	r3, r4
8780441e:	4620      	mov	r0, r4
87804420:	f853 4f48 	ldr.w	r4, [r3, #72]!
87804424:	42ab      	cmp	r3, r5
87804426:	f1a4 0448 	sub.w	r4, r4, #72	; 0x48
8780442a:	d1f4      	bne.n	87804416 <pend_wakeup_all+0x16>
8780442c:	bd70      	pop	{r4, r5, r6, pc}
8780442e:	bf00      	nop

87804430 <pend_wakeup>:
87804430:	b10a      	cbz	r2, 87804436 <pend_wakeup+0x6>
87804432:	f7ff bfe5 	b.w	87804400 <pend_wakeup_all>
87804436:	6800      	ldr	r0, [r0, #0]
87804438:	3848      	subs	r0, #72	; 0x48
8780443a:	f7ff bf9d 	b.w	87804378 <pend_task_wakeup>
8780443e:	bf00      	nop

87804440 <completion_do_post.part.3>:
87804440:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87804442:	4604      	mov	r4, r0
87804444:	460f      	mov	r7, r1
87804446:	f000 fc0b 	bl	87804c60 <tos_cpu_cpsr_save>
8780444a:	8a23      	ldrh	r3, [r4, #16]
8780444c:	f64f 72ff 	movw	r2, #65535	; 0xffff
87804450:	4293      	cmp	r3, r2
87804452:	d01a      	beq.n	8780448a <completion_do_post.part.3+0x4a>
87804454:	3301      	adds	r3, #1
87804456:	f104 0608 	add.w	r6, r4, #8
8780445a:	8223      	strh	r3, [r4, #16]
8780445c:	4605      	mov	r5, r0
8780445e:	4630      	mov	r0, r6
87804460:	f7ff ff62 	bl	87804328 <pend_is_nopending>
87804464:	4604      	mov	r4, r0
87804466:	b958      	cbnz	r0, 87804480 <completion_do_post.part.3+0x40>
87804468:	463a      	mov	r2, r7
8780446a:	2101      	movs	r1, #1
8780446c:	4630      	mov	r0, r6
8780446e:	f7ff ffdf 	bl	87804430 <pend_wakeup>
87804472:	4628      	mov	r0, r5
87804474:	f000 fbf6 	bl	87804c64 <tos_cpu_cpsr_restore>
87804478:	f7ff fe52 	bl	87804120 <knl_sched>
8780447c:	4620      	mov	r0, r4
8780447e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87804480:	4628      	mov	r0, r5
87804482:	f000 fbef 	bl	87804c64 <tos_cpu_cpsr_restore>
87804486:	2000      	movs	r0, #0
87804488:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
8780448a:	f000 fbeb 	bl	87804c64 <tos_cpu_cpsr_restore>
8780448e:	2019      	movs	r0, #25
87804490:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87804492:	bf00      	nop

87804494 <tos_completion_create>:
87804494:	b158      	cbz	r0, 878044ae <tos_completion_create+0x1a>
87804496:	b538      	push	{r3, r4, r5, lr}
87804498:	4604      	mov	r4, r0
8780449a:	2500      	movs	r5, #0
8780449c:	3008      	adds	r0, #8
8780449e:	8225      	strh	r5, [r4, #16]
878044a0:	f7ff ff3a 	bl	87804318 <pend_object_init>
878044a4:	f641 33ee 	movw	r3, #7150	; 0x1bee
878044a8:	4628      	mov	r0, r5
878044aa:	6063      	str	r3, [r4, #4]
878044ac:	bd38      	pop	{r3, r4, r5, pc}
878044ae:	f240 404c 	movw	r0, #1100	; 0x44c
878044b2:	4770      	bx	lr

878044b4 <tos_completion_destroy>:
878044b4:	b140      	cbz	r0, 878044c8 <tos_completion_destroy+0x14>
878044b6:	6842      	ldr	r2, [r0, #4]
878044b8:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
878044ba:	f641 33ee 	movw	r3, #7150	; 0x1bee
878044be:	429a      	cmp	r2, r3
878044c0:	d005      	beq.n	878044ce <tos_completion_destroy+0x1a>
878044c2:	f240 404d 	movw	r0, #1101	; 0x44d
878044c6:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878044c8:	f240 404c 	movw	r0, #1100	; 0x44c
878044cc:	4770      	bx	lr
878044ce:	f100 0608 	add.w	r6, r0, #8
878044d2:	4604      	mov	r4, r0
878044d4:	f000 fbc4 	bl	87804c60 <tos_cpu_cpsr_save>
878044d8:	2103      	movs	r1, #3
878044da:	4607      	mov	r7, r0
878044dc:	4630      	mov	r0, r6
878044de:	2500      	movs	r5, #0
878044e0:	f7ff ff8e 	bl	87804400 <pend_wakeup_all>
878044e4:	4630      	mov	r0, r6
878044e6:	f7ff ff1b 	bl	87804320 <pend_object_deinit>
878044ea:	4638      	mov	r0, r7
878044ec:	6065      	str	r5, [r4, #4]
878044ee:	f000 fbb9 	bl	87804c64 <tos_cpu_cpsr_restore>
878044f2:	f7ff fe15 	bl	87804120 <knl_sched>
878044f6:	4628      	mov	r0, r5
878044f8:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878044fa:	bf00      	nop

878044fc <tos_completion_pend_timed>:
878044fc:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87804500:	4605      	mov	r5, r0
87804502:	4616      	mov	r6, r2
87804504:	461f      	mov	r7, r3
87804506:	f7ff fe6b 	bl	878041e0 <knl_is_inirq>
8780450a:	4604      	mov	r4, r0
8780450c:	b948      	cbnz	r0, 87804522 <tos_completion_pend_timed+0x26>
8780450e:	b165      	cbz	r5, 8780452a <tos_completion_pend_timed+0x2e>
87804510:	686a      	ldr	r2, [r5, #4]
87804512:	f641 33ee 	movw	r3, #7150	; 0x1bee
87804516:	429a      	cmp	r2, r3
87804518:	d00b      	beq.n	87804532 <tos_completion_pend_timed+0x36>
8780451a:	f240 404d 	movw	r0, #1101	; 0x44d
8780451e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87804522:	f44f 70c8 	mov.w	r0, #400	; 0x190
87804526:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780452a:	f240 404c 	movw	r0, #1100	; 0x44c
8780452e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87804532:	f000 fb95 	bl	87804c60 <tos_cpu_cpsr_save>
87804536:	8a2b      	ldrh	r3, [r5, #16]
87804538:	4680      	mov	r8, r0
8780453a:	b943      	cbnz	r3, 8780454e <tos_completion_pend_timed+0x52>
8780453c:	ea56 0307 	orrs.w	r3, r6, r7
87804540:	d10a      	bne.n	87804558 <tos_completion_pend_timed+0x5c>
87804542:	f000 fb8f 	bl	87804c64 <tos_cpu_cpsr_restore>
87804546:	f44f 6096 	mov.w	r0, #1200	; 0x4b0
8780454a:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780454e:	f000 fb89 	bl	87804c64 <tos_cpu_cpsr_restore>
87804552:	4620      	mov	r0, r4
87804554:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87804558:	f7ff fe38 	bl	878041cc <knl_is_sched_locked>
8780455c:	b130      	cbz	r0, 8780456c <tos_completion_pend_timed+0x70>
8780455e:	4640      	mov	r0, r8
87804560:	f000 fb80 	bl	87804c64 <tos_cpu_cpsr_restore>
87804564:	f240 40b1 	movw	r0, #1201	; 0x4b1
87804568:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780456c:	f640 34a0 	movw	r4, #2976	; 0xba0
87804570:	463b      	mov	r3, r7
87804572:	f2c8 7481 	movt	r4, #34689	; 0x8781
87804576:	4632      	mov	r2, r6
87804578:	f105 0108 	add.w	r1, r5, #8
8780457c:	6820      	ldr	r0, [r4, #0]
8780457e:	f7ff ff1f 	bl	878043c0 <pend_task_block>
87804582:	4640      	mov	r0, r8
87804584:	f000 fb6e 	bl	87804c64 <tos_cpu_cpsr_restore>
87804588:	f7ff fdca 	bl	87804120 <knl_sched>
8780458c:	6823      	ldr	r3, [r4, #0]
8780458e:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
87804592:	6e18      	ldr	r0, [r3, #96]	; 0x60
87804594:	f7ff bed8 	b.w	87804348 <pend_state2errno>

87804598 <tos_completion_pend>:
87804598:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
8780459c:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
878045a0:	f7ff bfac 	b.w	878044fc <tos_completion_pend_timed>

878045a4 <tos_completion_post>:
878045a4:	b138      	cbz	r0, 878045b6 <tos_completion_post+0x12>
878045a6:	6842      	ldr	r2, [r0, #4]
878045a8:	f641 33ee 	movw	r3, #7150	; 0x1bee
878045ac:	429a      	cmp	r2, r3
878045ae:	d005      	beq.n	878045bc <tos_completion_post+0x18>
878045b0:	f240 404d 	movw	r0, #1101	; 0x44d
878045b4:	4770      	bx	lr
878045b6:	f240 404c 	movw	r0, #1100	; 0x44c
878045ba:	4770      	bx	lr
878045bc:	2100      	movs	r1, #0
878045be:	e73f      	b.n	87804440 <completion_do_post.part.3>

878045c0 <tos_completion_post_all>:
878045c0:	b138      	cbz	r0, 878045d2 <tos_completion_post_all+0x12>
878045c2:	6842      	ldr	r2, [r0, #4]
878045c4:	f641 33ee 	movw	r3, #7150	; 0x1bee
878045c8:	429a      	cmp	r2, r3
878045ca:	d005      	beq.n	878045d8 <tos_completion_post_all+0x18>
878045cc:	f240 404d 	movw	r0, #1101	; 0x44d
878045d0:	4770      	bx	lr
878045d2:	f240 404c 	movw	r0, #1100	; 0x44c
878045d6:	4770      	bx	lr
878045d8:	2101      	movs	r1, #1
878045da:	e731      	b.n	87804440 <completion_do_post.part.3>

878045dc <tos_completion_reset>:
878045dc:	b140      	cbz	r0, 878045f0 <tos_completion_reset+0x14>
878045de:	6842      	ldr	r2, [r0, #4]
878045e0:	b538      	push	{r3, r4, r5, lr}
878045e2:	f641 33ee 	movw	r3, #7150	; 0x1bee
878045e6:	429a      	cmp	r2, r3
878045e8:	d005      	beq.n	878045f6 <tos_completion_reset+0x1a>
878045ea:	f240 404d 	movw	r0, #1101	; 0x44d
878045ee:	bd38      	pop	{r3, r4, r5, pc}
878045f0:	f240 404c 	movw	r0, #1100	; 0x44c
878045f4:	4770      	bx	lr
878045f6:	4604      	mov	r4, r0
878045f8:	2500      	movs	r5, #0
878045fa:	f000 fb31 	bl	87804c60 <tos_cpu_cpsr_save>
878045fe:	8225      	strh	r5, [r4, #16]
87804600:	f000 fb30 	bl	87804c64 <tos_cpu_cpsr_restore>
87804604:	4628      	mov	r0, r5
87804606:	bd38      	pop	{r3, r4, r5, pc}

87804608 <tos_completion_is_done>:
87804608:	b130      	cbz	r0, 87804618 <tos_completion_is_done+0x10>
8780460a:	6842      	ldr	r2, [r0, #4]
8780460c:	f641 33ee 	movw	r3, #7150	; 0x1bee
87804610:	429a      	cmp	r2, r3
87804612:	d002      	beq.n	8780461a <tos_completion_is_done+0x12>
87804614:	2000      	movs	r0, #0
87804616:	4770      	bx	lr
87804618:	4770      	bx	lr
8780461a:	b510      	push	{r4, lr}
8780461c:	4604      	mov	r4, r0
8780461e:	f000 fb1f 	bl	87804c60 <tos_cpu_cpsr_save>
87804622:	8a24      	ldrh	r4, [r4, #16]
87804624:	3400      	adds	r4, #0
87804626:	bf18      	it	ne
87804628:	2401      	movne	r4, #1
8780462a:	f000 fb1b 	bl	87804c64 <tos_cpu_cpsr_restore>
8780462e:	4620      	mov	r0, r4
87804630:	bd10      	pop	{r4, pc}
87804632:	bf00      	nop

87804634 <tos_ring_q_create>:
87804634:	2900      	cmp	r1, #0
87804636:	bf18      	it	ne
87804638:	2800      	cmpne	r0, #0
8780463a:	b4f0      	push	{r4, r5, r6, r7}
8780463c:	bf0c      	ite	eq
8780463e:	2401      	moveq	r4, #1
87804640:	2400      	movne	r4, #0
87804642:	d00e      	beq.n	87804662 <tos_ring_q_create+0x2e>
87804644:	f64d 27d6 	movw	r7, #56022	; 0xdad6
87804648:	2601      	movs	r6, #1
8780464a:	4625      	mov	r5, r4
8780464c:	8104      	strh	r4, [r0, #8]
8780464e:	8144      	strh	r4, [r0, #10]
87804650:	60c4      	str	r4, [r0, #12]
87804652:	e880 00c0 	stmia.w	r0, {r6, r7}
87804656:	6101      	str	r1, [r0, #16]
87804658:	6143      	str	r3, [r0, #20]
8780465a:	6182      	str	r2, [r0, #24]
8780465c:	4628      	mov	r0, r5
8780465e:	bcf0      	pop	{r4, r5, r6, r7}
87804660:	4770      	bx	lr
87804662:	f240 454c 	movw	r5, #1100	; 0x44c
87804666:	4628      	mov	r0, r5
87804668:	bcf0      	pop	{r4, r5, r6, r7}
8780466a:	4770      	bx	lr

8780466c <tos_ring_q_destroy>:
8780466c:	4603      	mov	r3, r0
8780466e:	b168      	cbz	r0, 8780468c <tos_ring_q_destroy+0x20>
87804670:	6841      	ldr	r1, [r0, #4]
87804672:	f64d 22d6 	movw	r2, #56022	; 0xdad6
87804676:	4291      	cmp	r1, r2
87804678:	d002      	beq.n	87804680 <tos_ring_q_destroy+0x14>
8780467a:	f240 404d 	movw	r0, #1101	; 0x44d
8780467e:	4770      	bx	lr
87804680:	6802      	ldr	r2, [r0, #0]
87804682:	2a01      	cmp	r2, #1
87804684:	d005      	beq.n	87804692 <tos_ring_q_destroy+0x26>
87804686:	f240 404e 	movw	r0, #1102	; 0x44e
8780468a:	4770      	bx	lr
8780468c:	f240 404c 	movw	r0, #1100	; 0x44c
87804690:	4770      	bx	lr
87804692:	2200      	movs	r2, #0
87804694:	8102      	strh	r2, [r0, #8]
87804696:	4610      	mov	r0, r2
87804698:	815a      	strh	r2, [r3, #10]
8780469a:	60da      	str	r2, [r3, #12]
8780469c:	611a      	str	r2, [r3, #16]
8780469e:	615a      	str	r2, [r3, #20]
878046a0:	619a      	str	r2, [r3, #24]
878046a2:	605a      	str	r2, [r3, #4]
878046a4:	601a      	str	r2, [r3, #0]
878046a6:	4770      	bx	lr

878046a8 <tos_ring_q_create_dyn>:
878046a8:	b1c8      	cbz	r0, 878046de <tos_ring_q_create_dyn+0x36>
878046aa:	b570      	push	{r4, r5, r6, lr}
878046ac:	4604      	mov	r4, r0
878046ae:	fb02 f001 	mul.w	r0, r2, r1
878046b2:	4616      	mov	r6, r2
878046b4:	460d      	mov	r5, r1
878046b6:	f7fc f967 	bl	87800988 <tos_mmheap_alloc>
878046ba:	b168      	cbz	r0, 878046d8 <tos_ring_q_create_dyn+0x30>
878046bc:	2300      	movs	r3, #0
878046be:	f64d 21d6 	movw	r1, #56022	; 0xdad6
878046c2:	2202      	movs	r2, #2
878046c4:	6120      	str	r0, [r4, #16]
878046c6:	6166      	str	r6, [r4, #20]
878046c8:	4618      	mov	r0, r3
878046ca:	61a5      	str	r5, [r4, #24]
878046cc:	8123      	strh	r3, [r4, #8]
878046ce:	8163      	strh	r3, [r4, #10]
878046d0:	60e3      	str	r3, [r4, #12]
878046d2:	6061      	str	r1, [r4, #4]
878046d4:	6022      	str	r2, [r4, #0]
878046d6:	bd70      	pop	{r4, r5, r6, pc}
878046d8:	f240 407e 	movw	r0, #1150	; 0x47e
878046dc:	bd70      	pop	{r4, r5, r6, pc}
878046de:	f240 404c 	movw	r0, #1100	; 0x44c
878046e2:	4770      	bx	lr

878046e4 <tos_ring_q_destroy_dyn>:
878046e4:	b168      	cbz	r0, 87804702 <tos_ring_q_destroy_dyn+0x1e>
878046e6:	6842      	ldr	r2, [r0, #4]
878046e8:	f64d 23d6 	movw	r3, #56022	; 0xdad6
878046ec:	429a      	cmp	r2, r3
878046ee:	d002      	beq.n	878046f6 <tos_ring_q_destroy_dyn+0x12>
878046f0:	f240 404d 	movw	r0, #1101	; 0x44d
878046f4:	4770      	bx	lr
878046f6:	6803      	ldr	r3, [r0, #0]
878046f8:	2b02      	cmp	r3, #2
878046fa:	d005      	beq.n	87804708 <tos_ring_q_destroy_dyn+0x24>
878046fc:	f240 404e 	movw	r0, #1102	; 0x44e
87804700:	4770      	bx	lr
87804702:	f240 404c 	movw	r0, #1100	; 0x44c
87804706:	4770      	bx	lr
87804708:	b510      	push	{r4, lr}
8780470a:	4604      	mov	r4, r0
8780470c:	6900      	ldr	r0, [r0, #16]
8780470e:	f7fc f9dd 	bl	87800acc <tos_mmheap_free>
87804712:	2300      	movs	r3, #0
87804714:	8123      	strh	r3, [r4, #8]
87804716:	4618      	mov	r0, r3
87804718:	8163      	strh	r3, [r4, #10]
8780471a:	60e3      	str	r3, [r4, #12]
8780471c:	6123      	str	r3, [r4, #16]
8780471e:	6163      	str	r3, [r4, #20]
87804720:	61a3      	str	r3, [r4, #24]
87804722:	6063      	str	r3, [r4, #4]
87804724:	6023      	str	r3, [r4, #0]
87804726:	bd10      	pop	{r4, pc}

87804728 <tos_ring_q_enqueue>:
87804728:	2900      	cmp	r1, #0
8780472a:	bf18      	it	ne
8780472c:	2800      	cmpne	r0, #0
8780472e:	d011      	beq.n	87804754 <tos_ring_q_enqueue+0x2c>
87804730:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87804734:	f64d 23d6 	movw	r3, #56022	; 0xdad6
87804738:	6846      	ldr	r6, [r0, #4]
8780473a:	429e      	cmp	r6, r3
8780473c:	d003      	beq.n	87804746 <tos_ring_q_enqueue+0x1e>
8780473e:	f240 404d 	movw	r0, #1101	; 0x44d
87804742:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87804746:	6943      	ldr	r3, [r0, #20]
87804748:	4293      	cmp	r3, r2
8780474a:	d006      	beq.n	8780475a <tos_ring_q_enqueue+0x32>
8780474c:	f240 50de 	movw	r0, #1502	; 0x5de
87804750:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87804754:	f240 404c 	movw	r0, #1100	; 0x44c
87804758:	4770      	bx	lr
8780475a:	4604      	mov	r4, r0
8780475c:	460d      	mov	r5, r1
8780475e:	f000 fa7f 	bl	87804c60 <tos_cpu_cpsr_save>
87804762:	6863      	ldr	r3, [r4, #4]
87804764:	4607      	mov	r7, r0
87804766:	42b3      	cmp	r3, r6
87804768:	d016      	beq.n	87804798 <tos_ring_q_enqueue+0x70>
8780476a:	6962      	ldr	r2, [r4, #20]
8780476c:	4629      	mov	r1, r5
8780476e:	6923      	ldr	r3, [r4, #16]
87804770:	8960      	ldrh	r0, [r4, #10]
87804772:	fb02 3000 	mla	r0, r2, r0, r3
87804776:	f000 fe53 	bl	87805420 <memcpy>
8780477a:	8960      	ldrh	r0, [r4, #10]
8780477c:	69a1      	ldr	r1, [r4, #24]
8780477e:	3001      	adds	r0, #1
87804780:	f004 fd34 	bl	878091ec <__aeabi_uidivmod>
87804784:	68e3      	ldr	r3, [r4, #12]
87804786:	4638      	mov	r0, r7
87804788:	8161      	strh	r1, [r4, #10]
8780478a:	3301      	adds	r3, #1
8780478c:	60e3      	str	r3, [r4, #12]
8780478e:	f000 fa69 	bl	87804c64 <tos_cpu_cpsr_restore>
87804792:	2000      	movs	r0, #0
87804794:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87804798:	f000 fa62 	bl	87804c60 <tos_cpu_cpsr_save>
8780479c:	f8d4 800c 	ldr.w	r8, [r4, #12]
878047a0:	69a6      	ldr	r6, [r4, #24]
878047a2:	f000 fa5f 	bl	87804c64 <tos_cpu_cpsr_restore>
878047a6:	45b0      	cmp	r8, r6
878047a8:	d1df      	bne.n	8780476a <tos_ring_q_enqueue+0x42>
878047aa:	4638      	mov	r0, r7
878047ac:	f000 fa5a 	bl	87804c64 <tos_cpu_cpsr_restore>
878047b0:	f240 50dc 	movw	r0, #1500	; 0x5dc
878047b4:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}

878047b8 <tos_ring_q_dequeue>:
878047b8:	2900      	cmp	r1, #0
878047ba:	bf18      	it	ne
878047bc:	2800      	cmpne	r0, #0
878047be:	d00a      	beq.n	878047d6 <tos_ring_q_dequeue+0x1e>
878047c0:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
878047c4:	f64d 23d6 	movw	r3, #56022	; 0xdad6
878047c8:	6846      	ldr	r6, [r0, #4]
878047ca:	429e      	cmp	r6, r3
878047cc:	d006      	beq.n	878047dc <tos_ring_q_dequeue+0x24>
878047ce:	f240 404d 	movw	r0, #1101	; 0x44d
878047d2:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
878047d6:	f240 404c 	movw	r0, #1100	; 0x44c
878047da:	4770      	bx	lr
878047dc:	4604      	mov	r4, r0
878047de:	460d      	mov	r5, r1
878047e0:	4617      	mov	r7, r2
878047e2:	f000 fa3d 	bl	87804c60 <tos_cpu_cpsr_save>
878047e6:	6863      	ldr	r3, [r4, #4]
878047e8:	4680      	mov	r8, r0
878047ea:	42b3      	cmp	r3, r6
878047ec:	d019      	beq.n	87804822 <tos_ring_q_dequeue+0x6a>
878047ee:	6962      	ldr	r2, [r4, #20]
878047f0:	4628      	mov	r0, r5
878047f2:	8921      	ldrh	r1, [r4, #8]
878047f4:	6923      	ldr	r3, [r4, #16]
878047f6:	fb02 3101 	mla	r1, r2, r1, r3
878047fa:	f000 fe11 	bl	87805420 <memcpy>
878047fe:	b10f      	cbz	r7, 87804804 <tos_ring_q_dequeue+0x4c>
87804800:	6963      	ldr	r3, [r4, #20]
87804802:	603b      	str	r3, [r7, #0]
87804804:	8920      	ldrh	r0, [r4, #8]
87804806:	69a1      	ldr	r1, [r4, #24]
87804808:	3001      	adds	r0, #1
8780480a:	f004 fcef 	bl	878091ec <__aeabi_uidivmod>
8780480e:	68e3      	ldr	r3, [r4, #12]
87804810:	4640      	mov	r0, r8
87804812:	8121      	strh	r1, [r4, #8]
87804814:	3b01      	subs	r3, #1
87804816:	60e3      	str	r3, [r4, #12]
87804818:	f000 fa24 	bl	87804c64 <tos_cpu_cpsr_restore>
8780481c:	2000      	movs	r0, #0
8780481e:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87804822:	f000 fa1d 	bl	87804c60 <tos_cpu_cpsr_save>
87804826:	68e6      	ldr	r6, [r4, #12]
87804828:	f000 fa1c 	bl	87804c64 <tos_cpu_cpsr_restore>
8780482c:	2e00      	cmp	r6, #0
8780482e:	d1de      	bne.n	878047ee <tos_ring_q_dequeue+0x36>
87804830:	4640      	mov	r0, r8
87804832:	f000 fa17 	bl	87804c64 <tos_cpu_cpsr_restore>
87804836:	f240 50dd 	movw	r0, #1501	; 0x5dd
8780483a:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
8780483e:	bf00      	nop

87804840 <tos_ring_q_flush>:
87804840:	b140      	cbz	r0, 87804854 <tos_ring_q_flush+0x14>
87804842:	6842      	ldr	r2, [r0, #4]
87804844:	b538      	push	{r3, r4, r5, lr}
87804846:	f64d 23d6 	movw	r3, #56022	; 0xdad6
8780484a:	429a      	cmp	r2, r3
8780484c:	d005      	beq.n	8780485a <tos_ring_q_flush+0x1a>
8780484e:	f240 404d 	movw	r0, #1101	; 0x44d
87804852:	bd38      	pop	{r3, r4, r5, pc}
87804854:	f240 404c 	movw	r0, #1100	; 0x44c
87804858:	4770      	bx	lr
8780485a:	4604      	mov	r4, r0
8780485c:	2500      	movs	r5, #0
8780485e:	f000 f9ff 	bl	87804c60 <tos_cpu_cpsr_save>
87804862:	8125      	strh	r5, [r4, #8]
87804864:	8165      	strh	r5, [r4, #10]
87804866:	60e5      	str	r5, [r4, #12]
87804868:	f000 f9fc 	bl	87804c64 <tos_cpu_cpsr_restore>
8780486c:	4628      	mov	r0, r5
8780486e:	bd38      	pop	{r3, r4, r5, pc}

87804870 <tos_ring_q_is_empty>:
87804870:	b130      	cbz	r0, 87804880 <tos_ring_q_is_empty+0x10>
87804872:	6842      	ldr	r2, [r0, #4]
87804874:	f64d 23d6 	movw	r3, #56022	; 0xdad6
87804878:	429a      	cmp	r2, r3
8780487a:	d002      	beq.n	87804882 <tos_ring_q_is_empty+0x12>
8780487c:	2000      	movs	r0, #0
8780487e:	4770      	bx	lr
87804880:	4770      	bx	lr
87804882:	b510      	push	{r4, lr}
87804884:	4604      	mov	r4, r0
87804886:	f000 f9eb 	bl	87804c60 <tos_cpu_cpsr_save>
8780488a:	68e4      	ldr	r4, [r4, #12]
8780488c:	f000 f9ea 	bl	87804c64 <tos_cpu_cpsr_restore>
87804890:	fab4 f484 	clz	r4, r4
87804894:	0964      	lsrs	r4, r4, #5
87804896:	4620      	mov	r0, r4
87804898:	bd10      	pop	{r4, pc}
8780489a:	bf00      	nop

8780489c <tos_ring_q_is_full>:
8780489c:	b130      	cbz	r0, 878048ac <tos_ring_q_is_full+0x10>
8780489e:	6842      	ldr	r2, [r0, #4]
878048a0:	f64d 23d6 	movw	r3, #56022	; 0xdad6
878048a4:	429a      	cmp	r2, r3
878048a6:	d002      	beq.n	878048ae <tos_ring_q_is_full+0x12>
878048a8:	2000      	movs	r0, #0
878048aa:	4770      	bx	lr
878048ac:	4770      	bx	lr
878048ae:	b510      	push	{r4, lr}
878048b0:	4604      	mov	r4, r0
878048b2:	f000 f9d5 	bl	87804c60 <tos_cpu_cpsr_save>
878048b6:	68e2      	ldr	r2, [r4, #12]
878048b8:	69a3      	ldr	r3, [r4, #24]
878048ba:	1ad4      	subs	r4, r2, r3
878048bc:	fab4 f484 	clz	r4, r4
878048c0:	0964      	lsrs	r4, r4, #5
878048c2:	f000 f9cf 	bl	87804c64 <tos_cpu_cpsr_restore>
878048c6:	4620      	mov	r0, r4
878048c8:	bd10      	pop	{r4, pc}
878048ca:	bf00      	nop

878048cc <tos_stopwatch_create>:
878048cc:	4603      	mov	r3, r0
878048ce:	b138      	cbz	r0, 878048e0 <tos_stopwatch_create+0x14>
878048d0:	2000      	movs	r0, #0
878048d2:	2100      	movs	r1, #0
878048d4:	f64d 22d7 	movw	r2, #56023	; 0xdad7
878048d8:	e9c3 0102 	strd	r0, r1, [r3, #8]
878048dc:	605a      	str	r2, [r3, #4]
878048de:	4770      	bx	lr
878048e0:	f240 404c 	movw	r0, #1100	; 0x44c
878048e4:	4770      	bx	lr
878048e6:	bf00      	nop

878048e8 <tos_stopwatch_destroy>:
878048e8:	4603      	mov	r3, r0
878048ea:	b178      	cbz	r0, 8780490c <tos_stopwatch_destroy+0x24>
878048ec:	6841      	ldr	r1, [r0, #4]
878048ee:	f64d 22d7 	movw	r2, #56023	; 0xdad7
878048f2:	4291      	cmp	r1, r2
878048f4:	d002      	beq.n	878048fc <tos_stopwatch_destroy+0x14>
878048f6:	f240 404d 	movw	r0, #1101	; 0x44d
878048fa:	4770      	bx	lr
878048fc:	2200      	movs	r2, #0
878048fe:	2000      	movs	r0, #0
87804900:	2100      	movs	r1, #0
87804902:	605a      	str	r2, [r3, #4]
87804904:	e9c3 0102 	strd	r0, r1, [r3, #8]
87804908:	4610      	mov	r0, r2
8780490a:	4770      	bx	lr
8780490c:	f240 404c 	movw	r0, #1100	; 0x44c
87804910:	4770      	bx	lr
87804912:	bf00      	nop

87804914 <tos_stopwatch_countdown>:
87804914:	b148      	cbz	r0, 8780492a <tos_stopwatch_countdown+0x16>
87804916:	6841      	ldr	r1, [r0, #4]
87804918:	b5d0      	push	{r4, r6, r7, lr}
8780491a:	461f      	mov	r7, r3
8780491c:	f64d 23d7 	movw	r3, #56023	; 0xdad7
87804920:	4299      	cmp	r1, r3
87804922:	d005      	beq.n	87804930 <tos_stopwatch_countdown+0x1c>
87804924:	f240 404d 	movw	r0, #1101	; 0x44d
87804928:	bdd0      	pop	{r4, r6, r7, pc}
8780492a:	f240 404c 	movw	r0, #1100	; 0x44c
8780492e:	4770      	bx	lr
87804930:	4616      	mov	r6, r2
87804932:	4604      	mov	r4, r0
87804934:	f7fc fe2a 	bl	8780158c <tos_systick_get>
87804938:	1836      	adds	r6, r6, r0
8780493a:	f04f 0000 	mov.w	r0, #0
8780493e:	414f      	adcs	r7, r1
87804940:	e9c4 6702 	strd	r6, r7, [r4, #8]
87804944:	bdd0      	pop	{r4, r6, r7, pc}
87804946:	bf00      	nop

87804948 <tos_stopwatch_countdown_ms>:
87804948:	b140      	cbz	r0, 8780495c <tos_stopwatch_countdown_ms+0x14>
8780494a:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
8780494c:	f64d 23d7 	movw	r3, #56023	; 0xdad7
87804950:	6845      	ldr	r5, [r0, #4]
87804952:	429d      	cmp	r5, r3
87804954:	d005      	beq.n	87804962 <tos_stopwatch_countdown_ms+0x1a>
87804956:	f240 404d 	movw	r0, #1101	; 0x44d
8780495a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
8780495c:	f240 404c 	movw	r0, #1100	; 0x44c
87804960:	4770      	bx	lr
87804962:	4604      	mov	r4, r0
87804964:	4608      	mov	r0, r1
87804966:	f7fc fe49 	bl	878015fc <tos_millisec2tick>
8780496a:	6863      	ldr	r3, [r4, #4]
8780496c:	4606      	mov	r6, r0
8780496e:	460f      	mov	r7, r1
87804970:	42ab      	cmp	r3, r5
87804972:	d1f0      	bne.n	87804956 <tos_stopwatch_countdown_ms+0xe>
87804974:	f7fc fe0a 	bl	8780158c <tos_systick_get>
87804978:	1836      	adds	r6, r6, r0
8780497a:	f04f 0000 	mov.w	r0, #0
8780497e:	414f      	adcs	r7, r1
87804980:	e9c4 6702 	strd	r6, r7, [r4, #8]
87804984:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87804986:	bf00      	nop

87804988 <tos_stopwatch_delay>:
87804988:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
8780498a:	4606      	mov	r6, r0
8780498c:	460f      	mov	r7, r1
8780498e:	f7fc fdfd 	bl	8780158c <tos_systick_get>
87804992:	4604      	mov	r4, r0
87804994:	460d      	mov	r5, r1
87804996:	f7fc fdf9 	bl	8780158c <tos_systick_get>
8780499a:	1b00      	subs	r0, r0, r4
8780499c:	eb61 0105 	sbc.w	r1, r1, r5
878049a0:	42b9      	cmp	r1, r7
878049a2:	bf08      	it	eq
878049a4:	42b0      	cmpeq	r0, r6
878049a6:	d3f6      	bcc.n	87804996 <tos_stopwatch_delay+0xe>
878049a8:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
878049aa:	bf00      	nop

878049ac <tos_stopwatch_delay_ms>:
878049ac:	b508      	push	{r3, lr}
878049ae:	f7fc fe25 	bl	878015fc <tos_millisec2tick>
878049b2:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
878049b6:	f7ff bfe7 	b.w	87804988 <tos_stopwatch_delay>
878049ba:	bf00      	nop

878049bc <tos_stopwatch_remain>:
878049bc:	b120      	cbz	r0, 878049c8 <tos_stopwatch_remain+0xc>
878049be:	6842      	ldr	r2, [r0, #4]
878049c0:	f64d 23d7 	movw	r3, #56023	; 0xdad7
878049c4:	429a      	cmp	r2, r3
878049c6:	d004      	beq.n	878049d2 <tos_stopwatch_remain+0x16>
878049c8:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
878049cc:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
878049d0:	4770      	bx	lr
878049d2:	b510      	push	{r4, lr}
878049d4:	4604      	mov	r4, r0
878049d6:	f7fc fdd9 	bl	8780158c <tos_systick_get>
878049da:	e9d4 2302 	ldrd	r2, r3, [r4, #8]
878049de:	4299      	cmp	r1, r3
878049e0:	bf08      	it	eq
878049e2:	4290      	cmpeq	r0, r2
878049e4:	d302      	bcc.n	878049ec <tos_stopwatch_remain+0x30>
878049e6:	2000      	movs	r0, #0
878049e8:	2100      	movs	r1, #0
878049ea:	bd10      	pop	{r4, pc}
878049ec:	f7fc fdce 	bl	8780158c <tos_systick_get>
878049f0:	e9d4 2302 	ldrd	r2, r3, [r4, #8]
878049f4:	1a10      	subs	r0, r2, r0
878049f6:	eb63 0101 	sbc.w	r1, r3, r1
878049fa:	bd10      	pop	{r4, pc}

878049fc <tos_stopwatch_remain_ms>:
878049fc:	b140      	cbz	r0, 87804a10 <tos_stopwatch_remain_ms+0x14>
878049fe:	6842      	ldr	r2, [r0, #4]
87804a00:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87804a02:	f64d 23d7 	movw	r3, #56023	; 0xdad7
87804a06:	429a      	cmp	r2, r3
87804a08:	d005      	beq.n	87804a16 <tos_stopwatch_remain_ms+0x1a>
87804a0a:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
87804a0e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87804a10:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
87804a14:	4770      	bx	lr
87804a16:	4604      	mov	r4, r0
87804a18:	f7fc fdb8 	bl	8780158c <tos_systick_get>
87804a1c:	e9d4 2302 	ldrd	r2, r3, [r4, #8]
87804a20:	4299      	cmp	r1, r3
87804a22:	bf08      	it	eq
87804a24:	4290      	cmpeq	r0, r2
87804a26:	d301      	bcc.n	87804a2c <tos_stopwatch_remain_ms+0x30>
87804a28:	2000      	movs	r0, #0
87804a2a:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
87804a2c:	f7fc fdae 	bl	8780158c <tos_systick_get>
87804a30:	e9d4 6702 	ldrd	r6, r7, [r4, #8]
87804a34:	f240 34e7 	movw	r4, #999	; 0x3e7
87804a38:	2500      	movs	r5, #0
87804a3a:	f44f 727a 	mov.w	r2, #1000	; 0x3e8
87804a3e:	2300      	movs	r3, #0
87804a40:	19a4      	adds	r4, r4, r6
87804a42:	417d      	adcs	r5, r7
87804a44:	1a20      	subs	r0, r4, r0
87804a46:	eb65 0101 	sbc.w	r1, r5, r1
87804a4a:	f004 fbdd 	bl	87809208 <__aeabi_uldivmod>
87804a4e:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

87804a50 <tos_stopwatch_is_expired>:
87804a50:	b130      	cbz	r0, 87804a60 <tos_stopwatch_is_expired+0x10>
87804a52:	6842      	ldr	r2, [r0, #4]
87804a54:	f64d 23d7 	movw	r3, #56023	; 0xdad7
87804a58:	429a      	cmp	r2, r3
87804a5a:	d002      	beq.n	87804a62 <tos_stopwatch_is_expired+0x12>
87804a5c:	2000      	movs	r0, #0
87804a5e:	4770      	bx	lr
87804a60:	4770      	bx	lr
87804a62:	b510      	push	{r4, lr}
87804a64:	4604      	mov	r4, r0
87804a66:	f7fc fd91 	bl	8780158c <tos_systick_get>
87804a6a:	e9d4 2302 	ldrd	r2, r3, [r4, #8]
87804a6e:	4299      	cmp	r1, r3
87804a70:	bf08      	it	eq
87804a72:	4290      	cmpeq	r0, r2
87804a74:	bf2c      	ite	cs
87804a76:	2001      	movcs	r0, #1
87804a78:	2000      	movcc	r0, #0
87804a7a:	bd10      	pop	{r4, pc}

87804a7c <port_cpu_reset>:
87804a7c:	4770      	bx	lr
87804a7e:	bf00      	nop

87804a80 <port_systick_config>:
87804a80:	4770      	bx	lr
87804a82:	bf00      	nop

87804a84 <port_systick_priority_set>:
87804a84:	4770      	bx	lr
87804a86:	bf00      	nop

87804a88 <interrupt_irq>:
87804a88:	b510      	push	{r4, lr}
87804a8a:	2000      	movs	r0, #0
87804a8c:	f000 f988 	bl	87804da0 <gic_interrupt_id_get>
87804a90:	2880      	cmp	r0, #128	; 0x80
87804a92:	4604      	mov	r4, r0
87804a94:	d810      	bhi.n	87804ab8 <interrupt_irq+0x30>
87804a96:	f640 33b8 	movw	r3, #3000	; 0xbb8
87804a9a:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804a9e:	f853 2030 	ldr.w	r2, [r3, r0, lsl #3]
87804aa2:	b11a      	cbz	r2, 87804aac <interrupt_irq+0x24>
87804aa4:	eb03 03c0 	add.w	r3, r3, r0, lsl #3
87804aa8:	6858      	ldr	r0, [r3, #4]
87804aaa:	4790      	blx	r2
87804aac:	4621      	mov	r1, r4
87804aae:	2000      	movs	r0, #0
87804ab0:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
87804ab4:	f000 b984 	b.w	87804dc0 <gic_interrupt_end>
87804ab8:	bd10      	pop	{r4, pc}
87804aba:	bf00      	nop

87804abc <interrupt_init>:
87804abc:	b508      	push	{r3, lr}
87804abe:	2000      	movs	r0, #0
87804ac0:	f000 f98a 	bl	87804dd8 <gic_init>
87804ac4:	2000      	movs	r0, #0
87804ac6:	bd08      	pop	{r3, pc}

87804ac8 <tos_interrupt_handler_register>:
87804ac8:	287f      	cmp	r0, #127	; 0x7f
87804aca:	b430      	push	{r4, r5}
87804acc:	bf9d      	ittte	ls
87804ace:	f640 33b8 	movwls	r3, #3000	; 0xbb8
87804ad2:	2400      	movls	r4, #0
87804ad4:	f2c8 7381 	movtls	r3, #34689	; 0x8781
87804ad8:	f04f 34ff 	movhi.w	r4, #4294967295	; 0xffffffff
87804adc:	bf9c      	itt	ls
87804ade:	eb03 05c0 	addls.w	r5, r3, r0, lsl #3
87804ae2:	f843 1030 	strls.w	r1, [r3, r0, lsl #3]
87804ae6:	4620      	mov	r0, r4
87804ae8:	bf98      	it	ls
87804aea:	606a      	strls	r2, [r5, #4]
87804aec:	bc30      	pop	{r4, r5}
87804aee:	4770      	bx	lr

87804af0 <tos_interrupt_enable>:
87804af0:	4601      	mov	r1, r0
87804af2:	2000      	movs	r0, #0
87804af4:	f000 b99c 	b.w	87804e30 <gic_interrupt_enable>

87804af8 <tos_interrupt_disable>:
87804af8:	4601      	mov	r1, r0
87804afa:	2000      	movs	r0, #0
87804afc:	f000 b9aa 	b.w	87804e54 <gic_interrupt_disable>

87804b00 <dump_fault_frame>:
87804b00:	4604      	mov	r4, r0
87804b02:	b500      	push	{lr}
87804b04:	68e5      	ldr	r5, [r4, #12]
87804b06:	b083      	sub	sp, #12
87804b08:	f64d 5010 	movw	r0, #56592	; 0xdd10
87804b0c:	e894 0006 	ldmia.w	r4, {r1, r2}
87804b10:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804b14:	68a3      	ldr	r3, [r4, #8]
87804b16:	9500      	str	r5, [sp, #0]
87804b18:	f000 fa98 	bl	8780504c <printf>
87804b1c:	69e5      	ldr	r5, [r4, #28]
87804b1e:	f64d 5040 	movw	r0, #56640	; 0xdd40
87804b22:	69a3      	ldr	r3, [r4, #24]
87804b24:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804b28:	6962      	ldr	r2, [r4, #20]
87804b2a:	6921      	ldr	r1, [r4, #16]
87804b2c:	9500      	str	r5, [sp, #0]
87804b2e:	f000 fa8d 	bl	8780504c <printf>
87804b32:	6ae5      	ldr	r5, [r4, #44]	; 0x2c
87804b34:	f64d 5070 	movw	r0, #56688	; 0xdd70
87804b38:	6aa3      	ldr	r3, [r4, #40]	; 0x28
87804b3a:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804b3e:	6a62      	ldr	r2, [r4, #36]	; 0x24
87804b40:	6a21      	ldr	r1, [r4, #32]
87804b42:	9500      	str	r5, [sp, #0]
87804b44:	f000 fa82 	bl	8780504c <printf>
87804b48:	f64d 50a0 	movw	r0, #56736	; 0xdda0
87804b4c:	6b21      	ldr	r1, [r4, #48]	; 0x30
87804b4e:	6ba2      	ldr	r2, [r4, #56]	; 0x38
87804b50:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804b54:	f000 fa7a 	bl	8780504c <printf>
87804b58:	f64d 50b8 	movw	r0, #56760	; 0xddb8
87804b5c:	6be1      	ldr	r1, [r4, #60]	; 0x3c
87804b5e:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804b62:	f000 fa73 	bl	8780504c <printf>
87804b66:	e7fe      	b.n	87804b66 <dump_fault_frame+0x66>

87804b68 <arm_undefined_handler>:
87804b68:	b530      	push	{r4, r5, lr}
87804b6a:	4604      	mov	r4, r0
87804b6c:	f64d 50c8 	movw	r0, #56776	; 0xddc8
87804b70:	b083      	sub	sp, #12
87804b72:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804b76:	f000 fa69 	bl	8780504c <printf>
87804b7a:	69e3      	ldr	r3, [r4, #28]
87804b7c:	f64d 50d4 	movw	r0, #56788	; 0xddd4
87804b80:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804b84:	f013 0f20 	tst.w	r3, #32
87804b88:	69a3      	ldr	r3, [r4, #24]
87804b8a:	bf14      	ite	ne
87804b8c:	3b02      	subne	r3, #2
87804b8e:	3b04      	subeq	r3, #4
87804b90:	61a3      	str	r3, [r4, #24]
87804b92:	6819      	ldr	r1, [r3, #0]
87804b94:	f000 fa5a 	bl	8780504c <printf>
87804b98:	f64d 51ec 	movw	r1, #56812	; 0xddec
87804b9c:	f64d 6008 	movw	r0, #56840	; 0xde08
87804ba0:	f2c8 7180 	movt	r1, #34688	; 0x8780
87804ba4:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804ba8:	f000 fa50 	bl	8780504c <printf>
87804bac:	68e5      	ldr	r5, [r4, #12]
87804bae:	f64d 5010 	movw	r0, #56592	; 0xdd10
87804bb2:	68a3      	ldr	r3, [r4, #8]
87804bb4:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804bb8:	6862      	ldr	r2, [r4, #4]
87804bba:	6821      	ldr	r1, [r4, #0]
87804bbc:	9500      	str	r5, [sp, #0]
87804bbe:	f000 fa45 	bl	8780504c <printf>
87804bc2:	f64d 50a0 	movw	r0, #56736	; 0xdda0
87804bc6:	6921      	ldr	r1, [r4, #16]
87804bc8:	69a2      	ldr	r2, [r4, #24]
87804bca:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804bce:	f000 fa3d 	bl	8780504c <printf>
87804bd2:	f64d 50b8 	movw	r0, #56760	; 0xddb8
87804bd6:	69e1      	ldr	r1, [r4, #28]
87804bd8:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804bdc:	f000 fa36 	bl	8780504c <printf>
87804be0:	e7fe      	b.n	87804be0 <arm_undefined_handler+0x78>
87804be2:	bf00      	nop

87804be4 <arm_data_abort_handler>:
87804be4:	4604      	mov	r4, r0
87804be6:	f64d 600c 	movw	r0, #56844	; 0xde0c
87804bea:	b508      	push	{r3, lr}
87804bec:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804bf0:	f000 fa2c 	bl	8780504c <printf>
87804bf4:	4620      	mov	r0, r4
87804bf6:	f7ff ff83 	bl	87804b00 <dump_fault_frame>
87804bfa:	bf00      	nop

87804bfc <arm_prefetch_abort_handler>:
87804bfc:	4604      	mov	r4, r0
87804bfe:	f64d 601c 	movw	r0, #56860	; 0xde1c
87804c02:	b508      	push	{r3, lr}
87804c04:	f2c8 7080 	movt	r0, #34688	; 0x8780
87804c08:	f000 fa20 	bl	8780504c <printf>
87804c0c:	4620      	mov	r0, r4
87804c0e:	f7ff ff77 	bl	87804b00 <dump_fault_frame>
87804c12:	bf00      	nop
87804c14:	4770      	bx	lr
87804c16:	bf00      	nop

87804c18 <tos_cpu_clz>:
87804c18:	0c02      	lsrs	r2, r0, #16
87804c1a:	4603      	mov	r3, r0
87804c1c:	2000      	movs	r0, #0
87804c1e:	0412      	lsls	r2, r2, #16
87804c20:	b90a      	cbnz	r2, 87804c26 <tos_cpu_clz+0xe>
87804c22:	041b      	lsls	r3, r3, #16
87804c24:	2010      	movs	r0, #16
87804c26:	f013 4f7f 	tst.w	r3, #4278190080	; 0xff000000
87804c2a:	bf04      	itt	eq
87804c2c:	021b      	lsleq	r3, r3, #8
87804c2e:	3008      	addeq	r0, #8
87804c30:	f013 4f70 	tst.w	r3, #4026531840	; 0xf0000000
87804c34:	bf04      	itt	eq
87804c36:	011b      	lsleq	r3, r3, #4
87804c38:	3004      	addeq	r0, #4
87804c3a:	f013 4f40 	tst.w	r3, #3221225472	; 0xc0000000
87804c3e:	d106      	bne.n	87804c4e <tos_cpu_clz+0x36>
87804c40:	009b      	lsls	r3, r3, #2
87804c42:	3002      	adds	r0, #2
87804c44:	2b00      	cmp	r3, #0
87804c46:	db04      	blt.n	87804c52 <tos_cpu_clz+0x3a>
87804c48:	d104      	bne.n	87804c54 <tos_cpu_clz+0x3c>
87804c4a:	3002      	adds	r0, #2
87804c4c:	4770      	bx	lr
87804c4e:	2b00      	cmp	r3, #0
87804c50:	da00      	bge.n	87804c54 <tos_cpu_clz+0x3c>
87804c52:	4770      	bx	lr
87804c54:	3001      	adds	r0, #1
87804c56:	4770      	bx	lr

87804c58 <tos_cpu_int_disable>:
87804c58:	f004 bbd2 	b.w	87809400 <__port_int_disable_from_thumb>

87804c5c <tos_cpu_int_enable>:
87804c5c:	f004 bbd4 	b.w	87809408 <__port_int_enable_from_thumb>

87804c60 <tos_cpu_cpsr_save>:
87804c60:	f004 bbd6 	b.w	87809410 <__port_cpsr_save_from_thumb>

87804c64 <tos_cpu_cpsr_restore>:
87804c64:	f004 bbc0 	b.w	878093e8 <__port_cpsr_restore_from_thumb>

87804c68 <cpu_init>:
87804c68:	f64d 6388 	movw	r3, #56968	; 0xde88
87804c6c:	b510      	push	{r4, lr}
87804c6e:	f2c8 7380 	movt	r3, #34688	; 0x8780
87804c72:	f640 3498 	movw	r4, #2968	; 0xb98
87804c76:	f2c8 7481 	movt	r4, #34689	; 0x8781
87804c7a:	e9d3 2300 	ldrd	r2, r3, [r3]
87804c7e:	a106      	add	r1, pc, #24	; (adr r1, 87804c98 <cpu_init+0x30>)
87804c80:	e9d1 0100 	ldrd	r0, r1, [r1]
87804c84:	f004 fac0 	bl	87809208 <__aeabi_uldivmod>
87804c88:	6020      	str	r0, [r4, #0]
87804c8a:	f7ff ff17 	bl	87804abc <interrupt_init>
87804c8e:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
87804c92:	f000 b9a9 	b.w	87804fe8 <chip_init>
87804c96:	bf00      	nop
87804c98:	1f78a400 	svcne	0x0078a400
87804c9c:	00000000 	andeq	r0, r0, r0

87804ca0 <cpu_reset>:
87804ca0:	f7ff beec 	b.w	87804a7c <port_cpu_reset>

87804ca4 <cpu_sched_start>:
87804ca4:	b508      	push	{r3, lr}
87804ca6:	f7fb ea98 	blx	878001d8 <port_sched_start>
87804caa:	bf00      	nop

87804cac <cpu_context_switch>:
87804cac:	f004 bba4 	b.w	878093f8 <__port_context_switch_from_thumb>

87804cb0 <cpu_irq_context_switch>:
87804cb0:	f004 bb9e 	b.w	878093f0 <__port_irq_context_switch_from_thumb>

87804cb4 <cpu_systick_init>:
87804cb4:	b510      	push	{r4, lr}
87804cb6:	4604      	mov	r4, r0
87804cb8:	2000      	movs	r0, #0
87804cba:	f7ff fee3 	bl	87804a84 <port_systick_priority_set>
87804cbe:	4620      	mov	r0, r4
87804cc0:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
87804cc4:	f7ff bedc 	b.w	87804a80 <port_systick_config>

87804cc8 <cpu_task_stk_init>:
87804cc8:	b5f0      	push	{r4, r5, r6, r7, lr}
87804cca:	9c05      	ldr	r4, [sp, #20]
87804ccc:	441c      	add	r4, r3
87804cce:	f024 0407 	bic.w	r4, r4, #7
87804cd2:	42a3      	cmp	r3, r4
87804cd4:	bf38      	it	cc
87804cd6:	25cc      	movcc	r5, #204	; 0xcc
87804cd8:	d203      	bcs.n	87804ce2 <cpu_task_stk_init+0x1a>
87804cda:	f803 5b01 	strb.w	r5, [r3], #1
87804cde:	42a3      	cmp	r3, r4
87804ce0:	d1fb      	bne.n	87804cda <cpu_task_stk_init+0x12>
87804ce2:	07c3      	lsls	r3, r0, #31
87804ce4:	f1a4 0c40 	sub.w	ip, r4, #64	; 0x40
87804ce8:	bf44      	itt	mi
87804cea:	2333      	movmi	r3, #51	; 0x33
87804cec:	f844 3c04 	strmi.w	r3, [r4, #-4]
87804cf0:	d402      	bmi.n	87804cf8 <cpu_task_stk_init+0x30>
87804cf2:	2313      	movs	r3, #19
87804cf4:	f844 3c04 	str.w	r3, [r4, #-4]
87804cf8:	f04f 3712 	mov.w	r7, #303174162	; 0x12121212
87804cfc:	f04f 3611 	mov.w	r6, #286331153	; 0x11111111
87804d00:	f04f 3510 	mov.w	r5, #269488144	; 0x10101010
87804d04:	f844 2c0c 	str.w	r2, [r4, #-12]
87804d08:	f04f 3307 	mov.w	r3, #117901063	; 0x7070707
87804d0c:	f04f 3208 	mov.w	r2, #134744072	; 0x8080808
87804d10:	f04f 3e09 	mov.w	lr, #151587081	; 0x9090909
87804d14:	f844 1c40 	str.w	r1, [r4, #-64]
87804d18:	f844 7c10 	str.w	r7, [r4, #-16]
87804d1c:	f04f 3103 	mov.w	r1, #50529027	; 0x3030303
87804d20:	f04f 3706 	mov.w	r7, #101058054	; 0x6060606
87804d24:	f844 6c14 	str.w	r6, [r4, #-20]
87804d28:	f844 5c18 	str.w	r5, [r4, #-24]
87804d2c:	f04f 3605 	mov.w	r6, #84215045	; 0x5050505
87804d30:	f04f 3504 	mov.w	r5, #67372036	; 0x4040404
87804d34:	f844 2c20 	str.w	r2, [r4, #-32]
87804d38:	f844 3c24 	str.w	r3, [r4, #-36]
87804d3c:	f04f 3202 	mov.w	r2, #33686018	; 0x2020202
87804d40:	f04f 3301 	mov.w	r3, #16843009	; 0x1010101
87804d44:	f844 0c08 	str.w	r0, [r4, #-8]
87804d48:	f844 ec1c 	str.w	lr, [r4, #-28]
87804d4c:	4660      	mov	r0, ip
87804d4e:	f844 7c28 	str.w	r7, [r4, #-40]
87804d52:	f844 6c2c 	str.w	r6, [r4, #-44]
87804d56:	f844 5c30 	str.w	r5, [r4, #-48]
87804d5a:	f844 1c34 	str.w	r1, [r4, #-52]
87804d5e:	f844 2c38 	str.w	r2, [r4, #-56]
87804d62:	f844 3c3c 	str.w	r3, [r4, #-60]
87804d66:	bdf0      	pop	{r4, r5, r6, r7, pc}

87804d68 <cpu_task_stack_draught_depth>:
87804d68:	b470      	push	{r4, r5, r6}
87804d6a:	1846      	adds	r6, r0, r1
87804d6c:	f026 0607 	bic.w	r6, r6, #7
87804d70:	1e73      	subs	r3, r6, #1
87804d72:	4298      	cmp	r0, r3
87804d74:	d810      	bhi.n	87804d98 <cpu_task_stack_draught_depth+0x30>
87804d76:	2500      	movs	r5, #0
87804d78:	781c      	ldrb	r4, [r3, #0]
87804d7a:	2ccc      	cmp	r4, #204	; 0xcc
87804d7c:	bf18      	it	ne
87804d7e:	1af5      	subne	r5, r6, r3
87804d80:	3b01      	subs	r3, #1
87804d82:	4298      	cmp	r0, r3
87804d84:	d9f8      	bls.n	87804d78 <cpu_task_stack_draught_depth+0x10>
87804d86:	462b      	mov	r3, r5
87804d88:	f240 7073 	movw	r0, #1907	; 0x773
87804d8c:	6015      	str	r5, [r2, #0]
87804d8e:	428b      	cmp	r3, r1
87804d90:	bf18      	it	ne
87804d92:	2000      	movne	r0, #0
87804d94:	bc70      	pop	{r4, r5, r6}
87804d96:	4770      	bx	lr
87804d98:	2300      	movs	r3, #0
87804d9a:	461d      	mov	r5, r3
87804d9c:	e7f4      	b.n	87804d88 <cpu_task_stack_draught_depth+0x20>
87804d9e:	bf00      	nop

87804da0 <gic_interrupt_id_get>:
87804da0:	b958      	cbnz	r0, 87804dba <gic_interrupt_id_get+0x1a>
87804da2:	f640 73b8 	movw	r3, #4024	; 0xfb8
87804da6:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804daa:	685a      	ldr	r2, [r3, #4]
87804dac:	b12a      	cbz	r2, 87804dba <gic_interrupt_id_get+0x1a>
87804dae:	681b      	ldr	r3, [r3, #0]
87804db0:	b11b      	cbz	r3, 87804dba <gic_interrupt_id_get+0x1a>
87804db2:	68d0      	ldr	r0, [r2, #12]
87804db4:	f3c0 0009 	ubfx	r0, r0, #0, #10
87804db8:	4770      	bx	lr
87804dba:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
87804dbe:	4770      	bx	lr

87804dc0 <gic_interrupt_end>:
87804dc0:	b940      	cbnz	r0, 87804dd4 <gic_interrupt_end+0x14>
87804dc2:	f640 73b8 	movw	r3, #4024	; 0xfb8
87804dc6:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804dca:	685a      	ldr	r2, [r3, #4]
87804dcc:	b112      	cbz	r2, 87804dd4 <gic_interrupt_end+0x14>
87804dce:	681b      	ldr	r3, [r3, #0]
87804dd0:	b103      	cbz	r3, 87804dd4 <gic_interrupt_end+0x14>
87804dd2:	6111      	str	r1, [r2, #16]
87804dd4:	4770      	bx	lr
87804dd6:	bf00      	nop

87804dd8 <gic_init>:
87804dd8:	bb30      	cbnz	r0, 87804e28 <gic_init+0x50>
87804dda:	b430      	push	{r4, r5}
87804ddc:	ee9f 4f10 	mrc	15, 4, r4, cr15, cr0, {0}
87804de0:	0c24      	lsrs	r4, r4, #16
87804de2:	f640 75b8 	movw	r5, #4024	; 0xfb8
87804de6:	f04f 31ff 	mov.w	r1, #4294967295	; 0xffffffff
87804dea:	0424      	lsls	r4, r4, #16
87804dec:	f2c8 7581 	movt	r5, #34689	; 0x8781
87804df0:	f504 5080 	add.w	r0, r4, #4096	; 0x1000
87804df4:	f504 5400 	add.w	r4, r4, #8192	; 0x2000
87804df8:	6842      	ldr	r2, [r0, #4]
87804dfa:	f500 73be 	add.w	r3, r0, #380	; 0x17c
87804dfe:	e885 0011 	stmia.w	r5, {r0, r4}
87804e02:	f002 021f 	and.w	r2, r2, #31
87804e06:	3260      	adds	r2, #96	; 0x60
87804e08:	eb00 0282 	add.w	r2, r0, r2, lsl #2
87804e0c:	f843 1f04 	str.w	r1, [r3, #4]!
87804e10:	4293      	cmp	r3, r2
87804e12:	d1fb      	bne.n	87804e0c <gic_init+0x34>
87804e14:	2301      	movs	r3, #1
87804e16:	21f8      	movs	r1, #248	; 0xf8
87804e18:	2202      	movs	r2, #2
87804e1a:	6003      	str	r3, [r0, #0]
87804e1c:	2000      	movs	r0, #0
87804e1e:	6023      	str	r3, [r4, #0]
87804e20:	6061      	str	r1, [r4, #4]
87804e22:	60a2      	str	r2, [r4, #8]
87804e24:	bc30      	pop	{r4, r5}
87804e26:	4770      	bx	lr
87804e28:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
87804e2c:	4770      	bx	lr
87804e2e:	bf00      	nop

87804e30 <gic_interrupt_enable>:
87804e30:	b978      	cbnz	r0, 87804e52 <gic_interrupt_enable+0x22>
87804e32:	f640 73b8 	movw	r3, #4024	; 0xfb8
87804e36:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804e3a:	685a      	ldr	r2, [r3, #4]
87804e3c:	b14a      	cbz	r2, 87804e52 <gic_interrupt_enable+0x22>
87804e3e:	6818      	ldr	r0, [r3, #0]
87804e40:	b138      	cbz	r0, 87804e52 <gic_interrupt_enable+0x22>
87804e42:	094b      	lsrs	r3, r1, #5
87804e44:	2201      	movs	r2, #1
87804e46:	f001 011f 	and.w	r1, r1, #31
87804e4a:	408a      	lsls	r2, r1
87804e4c:	3340      	adds	r3, #64	; 0x40
87804e4e:	f840 2023 	str.w	r2, [r0, r3, lsl #2]
87804e52:	4770      	bx	lr

87804e54 <gic_interrupt_disable>:
87804e54:	b978      	cbnz	r0, 87804e76 <gic_interrupt_disable+0x22>
87804e56:	f640 73b8 	movw	r3, #4024	; 0xfb8
87804e5a:	f2c8 7381 	movt	r3, #34689	; 0x8781
87804e5e:	685a      	ldr	r2, [r3, #4]
87804e60:	b14a      	cbz	r2, 87804e76 <gic_interrupt_disable+0x22>
87804e62:	6818      	ldr	r0, [r3, #0]
87804e64:	b138      	cbz	r0, 87804e76 <gic_interrupt_disable+0x22>
87804e66:	094b      	lsrs	r3, r1, #5
87804e68:	2201      	movs	r2, #1
87804e6a:	f001 011f 	and.w	r1, r1, #31
87804e6e:	408a      	lsls	r2, r1
87804e70:	3360      	adds	r3, #96	; 0x60
87804e72:	f840 2023 	str.w	r2, [r0, r3, lsl #2]
87804e76:	4770      	bx	lr

87804e78 <clock_init>:
87804e78:	f44f 4380 	mov.w	r3, #16384	; 0x4000
87804e7c:	b470      	push	{r4, r5, r6}
87804e7e:	f2c0 230c 	movt	r3, #524	; 0x20c
87804e82:	68da      	ldr	r2, [r3, #12]
87804e84:	0751      	lsls	r1, r2, #29
87804e86:	d407      	bmi.n	87804e98 <clock_init+0x20>
87804e88:	68da      	ldr	r2, [r3, #12]
87804e8a:	f422 7280 	bic.w	r2, r2, #256	; 0x100
87804e8e:	60da      	str	r2, [r3, #12]
87804e90:	68da      	ldr	r2, [r3, #12]
87804e92:	f042 0204 	orr.w	r2, r2, #4
87804e96:	60da      	str	r2, [r3, #12]
87804e98:	f44f 4200 	mov.w	r2, #32768	; 0x8000
87804e9c:	f44f 4380 	mov.w	r3, #16384	; 0x4000
87804ea0:	f2c0 230c 	movt	r3, #524	; 0x20c
87804ea4:	f2c0 220c 	movt	r2, #524	; 0x20c
87804ea8:	f242 0158 	movw	r1, #8280	; 0x2058
87804eac:	2601      	movs	r6, #1
87804eae:	6011      	str	r1, [r2, #0]
87804eb0:	f241 041b 	movw	r4, #4123	; 0x101b
87804eb4:	68dd      	ldr	r5, [r3, #12]
87804eb6:	f2c2 0418 	movt	r4, #8216	; 0x2018
87804eba:	f241 000c 	movw	r0, #4108	; 0x100c
87804ebe:	4619      	mov	r1, r3
87804ec0:	f2c1 3011 	movt	r0, #4881	; 0x1311
87804ec4:	f025 0504 	bic.w	r5, r5, #4
87804ec8:	60dd      	str	r5, [r3, #12]
87804eca:	611e      	str	r6, [r3, #16]
87804ecc:	f8d2 5100 	ldr.w	r5, [r2, #256]	; 0x100
87804ed0:	f005 35c0 	and.w	r5, r5, #3233857728	; 0xc0c0c0c0
87804ed4:	432c      	orrs	r4, r5
87804ed6:	f8c2 4100 	str.w	r4, [r2, #256]	; 0x100
87804eda:	f8d2 40f0 	ldr.w	r4, [r2, #240]	; 0xf0
87804ede:	f004 34c0 	and.w	r4, r4, #3233857728	; 0xc0c0c0c0
87804ee2:	4320      	orrs	r0, r4
87804ee4:	f8c2 00f0 	str.w	r0, [r2, #240]	; 0xf0
87804ee8:	699a      	ldr	r2, [r3, #24]
87804eea:	f422 2240 	bic.w	r2, r2, #786432	; 0xc0000
87804eee:	619a      	str	r2, [r3, #24]
87804ef0:	699a      	ldr	r2, [r3, #24]
87804ef2:	f442 2280 	orr.w	r2, r2, #262144	; 0x40000
87804ef6:	619a      	str	r2, [r3, #24]
87804ef8:	695a      	ldr	r2, [r3, #20]
87804efa:	f022 7200 	bic.w	r2, r2, #33554432	; 0x2000000
87804efe:	615a      	str	r2, [r3, #20]
87804f00:	6c8a      	ldr	r2, [r1, #72]	; 0x48
87804f02:	f44f 4380 	mov.w	r3, #16384	; 0x4000
87804f06:	f2c0 230c 	movt	r3, #524	; 0x20c
87804f0a:	0692      	lsls	r2, r2, #26
87804f0c:	d4f8      	bmi.n	87804f00 <clock_init+0x88>
87804f0e:	695a      	ldr	r2, [r3, #20]
87804f10:	bc70      	pop	{r4, r5, r6}
87804f12:	f422 7240 	bic.w	r2, r2, #768	; 0x300
87804f16:	615a      	str	r2, [r3, #20]
87804f18:	695a      	ldr	r2, [r3, #20]
87804f1a:	f442 7280 	orr.w	r2, r2, #256	; 0x100
87804f1e:	615a      	str	r2, [r3, #20]
87804f20:	69da      	ldr	r2, [r3, #28]
87804f22:	f022 0240 	bic.w	r2, r2, #64	; 0x40
87804f26:	61da      	str	r2, [r3, #28]
87804f28:	69da      	ldr	r2, [r3, #28]
87804f2a:	f022 0207 	bic.w	r2, r2, #7
87804f2e:	61da      	str	r2, [r3, #28]
87804f30:	6a5a      	ldr	r2, [r3, #36]	; 0x24
87804f32:	f022 0240 	bic.w	r2, r2, #64	; 0x40
87804f36:	625a      	str	r2, [r3, #36]	; 0x24
87804f38:	6a5a      	ldr	r2, [r3, #36]	; 0x24
87804f3a:	f022 023f 	bic.w	r2, r2, #63	; 0x3f
87804f3e:	625a      	str	r2, [r3, #36]	; 0x24
87804f40:	4770      	bx	lr
87804f42:	bf00      	nop

87804f44 <clock_enable>:
87804f44:	f44f 4380 	mov.w	r3, #16384	; 0x4000
87804f48:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
87804f4c:	f2c0 230c 	movt	r3, #524	; 0x20c
87804f50:	669a      	str	r2, [r3, #104]	; 0x68
87804f52:	66da      	str	r2, [r3, #108]	; 0x6c
87804f54:	671a      	str	r2, [r3, #112]	; 0x70
87804f56:	675a      	str	r2, [r3, #116]	; 0x74
87804f58:	679a      	str	r2, [r3, #120]	; 0x78
87804f5a:	67da      	str	r2, [r3, #124]	; 0x7c
87804f5c:	f8c3 2080 	str.w	r2, [r3, #128]	; 0x80
87804f60:	4770      	bx	lr
87804f62:	bf00      	nop
87804f64:	0000      	movs	r0, r0
	...

87804f68 <systick_handler>:
87804f68:	b508      	push	{r3, lr}
87804f6a:	2300      	movs	r3, #0
87804f6c:	f2c0 230d 	movt	r3, #525	; 0x20d
87804f70:	685b      	ldr	r3, [r3, #4]
87804f72:	07db      	lsls	r3, r3, #31
87804f74:	d501      	bpl.n	87804f7a <systick_handler+0x12>
87804f76:	f7fd fa0b 	bl	87802390 <tos_tick_handler>
87804f7a:	2300      	movs	r3, #0
87804f7c:	f2c0 230d 	movt	r3, #525	; 0x20d
87804f80:	685a      	ldr	r2, [r3, #4]
87804f82:	f042 0201 	orr.w	r2, r2, #1
87804f86:	605a      	str	r2, [r3, #4]
87804f88:	bd08      	pop	{r3, pc}
87804f8a:	bf00      	nop

87804f8c <systick_init>:
87804f8c:	b538      	push	{r3, r4, r5, lr}
87804f8e:	4602      	mov	r2, r0
87804f90:	460b      	mov	r3, r1
87804f92:	a113      	add	r1, pc, #76	; (adr r1, 87804fe0 <systick_init+0x54>)
87804f94:	e9d1 0100 	ldrd	r0, r1, [r1]
87804f98:	f004 f936 	bl	87809208 <__aeabi_uldivmod>
87804f9c:	2300      	movs	r3, #0
87804f9e:	210e      	movs	r1, #14
87804fa0:	f2c0 230d 	movt	r3, #525	; 0x20d
87804fa4:	2200      	movs	r2, #0
87804fa6:	f2c0 1100 	movt	r1, #256	; 0x100
87804faa:	601a      	str	r2, [r3, #0]
87804fac:	6019      	str	r1, [r3, #0]
87804fae:	6098      	str	r0, [r3, #8]
87804fb0:	60da      	str	r2, [r3, #12]
87804fb2:	ee9f 0f10 	mrc	15, 4, r0, cr15, cr0, {0}
87804fb6:	0c00      	lsrs	r0, r0, #16
87804fb8:	f241 1408 	movw	r4, #4360	; 0x1108
87804fbc:	f04f 7580 	mov.w	r5, #16777216	; 0x1000000
87804fc0:	0400      	lsls	r0, r0, #16
87804fc2:	f644 7169 	movw	r1, #20329	; 0x4f69
87804fc6:	f2c8 7180 	movt	r1, #34688	; 0x8780
87804fca:	5105      	str	r5, [r0, r4]
87804fcc:	2058      	movs	r0, #88	; 0x58
87804fce:	681c      	ldr	r4, [r3, #0]
87804fd0:	f044 0401 	orr.w	r4, r4, #1
87804fd4:	601c      	str	r4, [r3, #0]
87804fd6:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
87804fda:	f7ff bd75 	b.w	87804ac8 <tos_interrupt_handler_register>
87804fde:	bf00      	nop
87804fe0:	03ef1480 	mvneq	r1, #128, 8	; 0x80000000
87804fe4:	00000000 	andeq	r0, r0, r0

87804fe8 <chip_init>:
87804fe8:	b508      	push	{r3, lr}
87804fea:	f7ff ff45 	bl	87804e78 <clock_init>
87804fee:	f7ff ffa9 	bl	87804f44 <clock_enable>
87804ff2:	f64d 6388 	movw	r3, #56968	; 0xde88
87804ff6:	f2c8 7380 	movt	r3, #34688	; 0x8780
87804ffa:	e9d3 0100 	ldrd	r0, r1, [r3]
87804ffe:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
87805002:	f7ff bfc3 	b.w	87804f8c <systick_init>
87805006:	bf00      	nop

87805008 <__muldi3>:
87805008:	fb02 f101 	mul.w	r1, r2, r1
8780500c:	b4f0      	push	{r4, r5, r6, r7}
8780500e:	4604      	mov	r4, r0
87805010:	ea4f 4610 	mov.w	r6, r0, lsr #16
87805014:	ea4f 4512 	mov.w	r5, r2, lsr #16
87805018:	ea20 4706 	bic.w	r7, r0, r6, lsl #16
8780501c:	ea22 4205 	bic.w	r2, r2, r5, lsl #16
87805020:	fb07 f002 	mul.w	r0, r7, r2
87805024:	fb06 f202 	mul.w	r2, r6, r2
87805028:	fb05 f707 	mul.w	r7, r5, r7
8780502c:	fb06 f505 	mul.w	r5, r6, r5
87805030:	19d7      	adds	r7, r2, r7
87805032:	bf28      	it	cs
87805034:	f505 3580 	addcs.w	r5, r5, #65536	; 0x10000
87805038:	eb10 4007 	adds.w	r0, r0, r7, lsl #16
8780503c:	eb45 4517 	adc.w	r5, r5, r7, lsr #16
87805040:	fb03 1104 	mla	r1, r3, r4, r1
87805044:	4429      	add	r1, r5
87805046:	bcf0      	pop	{r4, r5, r6, r7}
87805048:	4770      	bx	lr
8780504a:	bf00      	nop

8780504c <printf>:
8780504c:	b40f      	push	{r0, r1, r2, r3}
8780504e:	f640 70c0 	movw	r0, #4032	; 0xfc0
87805052:	b570      	push	{r4, r5, r6, lr}
87805054:	f2c8 7081 	movt	r0, #34689	; 0x8781
87805058:	aa05      	add	r2, sp, #20
8780505a:	2400      	movs	r4, #0
8780505c:	9904      	ldr	r1, [sp, #16]
8780505e:	f001 f975 	bl	8780634c <vsprintf>
87805062:	4d0b      	ldr	r5, [pc, #44]	; (87805090 <printf+0x44>)
87805064:	4606      	mov	r6, r0
87805066:	e003      	b.n	87805070 <printf+0x24>
87805068:	f815 0f01 	ldrb.w	r0, [r5, #1]!
8780506c:	f002 f9e0 	bl	87807430 <uart_putc>
87805070:	f640 70c0 	movw	r0, #4032	; 0xfc0
87805074:	f2c8 7081 	movt	r0, #34689	; 0x8781
87805078:	f000 f8fc 	bl	87805274 <strlen>
8780507c:	4284      	cmp	r4, r0
8780507e:	f104 0401 	add.w	r4, r4, #1
87805082:	d3f1      	bcc.n	87805068 <printf+0x1c>
87805084:	4630      	mov	r0, r6
87805086:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
8780508a:	b004      	add	sp, #16
8780508c:	4770      	bx	lr
8780508e:	bf00      	nop
87805090:	87810fbf 			; <UNDEFINED> instruction: 0x87810fbf

87805094 <scanf>:
87805094:	b40f      	push	{r0, r1, r2, r3}
87805096:	b570      	push	{r4, r5, r6, lr}
87805098:	2500      	movs	r5, #0
8780509a:	4e10      	ldr	r6, [pc, #64]	; (878050dc <scanf+0x48>)
8780509c:	e002      	b.n	878050a4 <scanf+0x10>
8780509e:	3501      	adds	r5, #1
878050a0:	f806 4f01 	strb.w	r4, [r6, #1]!
878050a4:	f002 f9e4 	bl	87807470 <uart_getc>
878050a8:	4604      	mov	r4, r0
878050aa:	f002 f9c1 	bl	87807430 <uart_putc>
878050ae:	2c0d      	cmp	r4, #13
878050b0:	bf18      	it	ne
878050b2:	2c0a      	cmpne	r4, #10
878050b4:	d1f3      	bne.n	8780509e <scanf+0xa>
878050b6:	f640 70c0 	movw	r0, #4032	; 0xfc0
878050ba:	2300      	movs	r3, #0
878050bc:	f2c8 7081 	movt	r0, #34689	; 0x8781
878050c0:	aa05      	add	r2, sp, #20
878050c2:	4405      	add	r5, r0
878050c4:	9904      	ldr	r1, [sp, #16]
878050c6:	f500 6080 	add.w	r0, r0, #1024	; 0x400
878050ca:	f885 3400 	strb.w	r3, [r5, #1024]	; 0x400
878050ce:	f001 f94f 	bl	87806370 <vsscanf>
878050d2:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
878050d6:	b004      	add	sp, #16
878050d8:	4770      	bx	lr
878050da:	bf00      	nop
878050dc:	878113bf 			; <UNDEFINED> instruction: 0x878113bf

878050e0 <strnicmp>:
878050e0:	b322      	cbz	r2, 8780512c <strnicmp+0x4c>
878050e2:	b5f0      	push	{r4, r5, r6, r7, lr}
878050e4:	f64d 6590 	movw	r5, #56976	; 0xde90
878050e8:	3801      	subs	r0, #1
878050ea:	f2c8 7580 	movt	r5, #34688	; 0x8780
878050ee:	e008      	b.n	87805102 <strnicmp+0x22>
878050f0:	07f6      	lsls	r6, r6, #31
878050f2:	fa5f f38e 	uxtb.w	r3, lr
878050f6:	d500      	bpl.n	878050fa <strnicmp+0x1a>
878050f8:	b2fc      	uxtb	r4, r7
878050fa:	42a3      	cmp	r3, r4
878050fc:	d114      	bne.n	87805128 <strnicmp+0x48>
878050fe:	3a01      	subs	r2, #1
87805100:	d01a      	beq.n	87805138 <strnicmp+0x58>
87805102:	f810 3f01 	ldrb.w	r3, [r0, #1]!
87805106:	f811 4b01 	ldrb.w	r4, [r1], #1
8780510a:	b18b      	cbz	r3, 87805130 <strnicmp+0x50>
8780510c:	b194      	cbz	r4, 87805134 <strnicmp+0x54>
8780510e:	42a3      	cmp	r3, r4
87805110:	d0f5      	beq.n	878050fe <strnicmp+0x1e>
87805112:	5cee      	ldrb	r6, [r5, r3]
87805114:	f104 0720 	add.w	r7, r4, #32
87805118:	f103 0e20 	add.w	lr, r3, #32
8780511c:	f016 0f01 	tst.w	r6, #1
87805120:	5d2e      	ldrb	r6, [r5, r4]
87805122:	d1e5      	bne.n	878050f0 <strnicmp+0x10>
87805124:	07f6      	lsls	r6, r6, #31
87805126:	d4e7      	bmi.n	878050f8 <strnicmp+0x18>
87805128:	1b18      	subs	r0, r3, r4
8780512a:	bdf0      	pop	{r4, r5, r6, r7, pc}
8780512c:	4610      	mov	r0, r2
8780512e:	4770      	bx	lr
87805130:	4260      	negs	r0, r4
87805132:	bdf0      	pop	{r4, r5, r6, r7, pc}
87805134:	4618      	mov	r0, r3
87805136:	bdf0      	pop	{r4, r5, r6, r7, pc}
87805138:	4610      	mov	r0, r2
8780513a:	bdf0      	pop	{r4, r5, r6, r7, pc}

8780513c <strcpy>:
8780513c:	1e42      	subs	r2, r0, #1
8780513e:	f811 3b01 	ldrb.w	r3, [r1], #1
87805142:	f802 3f01 	strb.w	r3, [r2, #1]!
87805146:	2b00      	cmp	r3, #0
87805148:	d1f9      	bne.n	8780513e <strcpy+0x2>
8780514a:	4770      	bx	lr

8780514c <strncpy>:
8780514c:	b410      	push	{r4}
8780514e:	440a      	add	r2, r1
87805150:	1e44      	subs	r4, r0, #1
87805152:	e004      	b.n	8780515e <strncpy+0x12>
87805154:	f811 3b01 	ldrb.w	r3, [r1], #1
87805158:	f804 3f01 	strb.w	r3, [r4, #1]!
8780515c:	b10b      	cbz	r3, 87805162 <strncpy+0x16>
8780515e:	4291      	cmp	r1, r2
87805160:	d1f8      	bne.n	87805154 <strncpy+0x8>
87805162:	f85d 4b04 	ldr.w	r4, [sp], #4
87805166:	4770      	bx	lr

87805168 <strcat>:
87805168:	7803      	ldrb	r3, [r0, #0]
8780516a:	b163      	cbz	r3, 87805186 <strcat+0x1e>
8780516c:	4603      	mov	r3, r0
8780516e:	f813 2f01 	ldrb.w	r2, [r3, #1]!
87805172:	2a00      	cmp	r2, #0
87805174:	d1fb      	bne.n	8780516e <strcat+0x6>
87805176:	3b01      	subs	r3, #1
87805178:	f811 2b01 	ldrb.w	r2, [r1], #1
8780517c:	f803 2f01 	strb.w	r2, [r3, #1]!
87805180:	2a00      	cmp	r2, #0
87805182:	d1f9      	bne.n	87805178 <strcat+0x10>
87805184:	4770      	bx	lr
87805186:	4603      	mov	r3, r0
87805188:	e7f5      	b.n	87805176 <strcat+0xe>
8780518a:	bf00      	nop

8780518c <strncat>:
8780518c:	b1ba      	cbz	r2, 878051be <strncat+0x32>
8780518e:	7803      	ldrb	r3, [r0, #0]
87805190:	b430      	push	{r4, r5}
87805192:	b1bb      	cbz	r3, 878051c4 <strncat+0x38>
87805194:	4603      	mov	r3, r0
87805196:	f813 4f01 	ldrb.w	r4, [r3, #1]!
8780519a:	2c00      	cmp	r4, #0
8780519c:	d1fb      	bne.n	87805196 <strncat+0xa>
8780519e:	f811 4b01 	ldrb.w	r4, [r1], #1
878051a2:	461d      	mov	r5, r3
878051a4:	f803 4b01 	strb.w	r4, [r3], #1
878051a8:	b144      	cbz	r4, 878051bc <strncat+0x30>
878051aa:	3a01      	subs	r2, #1
878051ac:	d008      	beq.n	878051c0 <strncat+0x34>
878051ae:	f811 4b01 	ldrb.w	r4, [r1], #1
878051b2:	461d      	mov	r5, r3
878051b4:	f803 4b01 	strb.w	r4, [r3], #1
878051b8:	2c00      	cmp	r4, #0
878051ba:	d1f6      	bne.n	878051aa <strncat+0x1e>
878051bc:	bc30      	pop	{r4, r5}
878051be:	4770      	bx	lr
878051c0:	706a      	strb	r2, [r5, #1]
878051c2:	e7fb      	b.n	878051bc <strncat+0x30>
878051c4:	4603      	mov	r3, r0
878051c6:	e7f2      	b.n	878051ae <strncat+0x22>

878051c8 <strcmp>:
878051c8:	1e42      	subs	r2, r0, #1
878051ca:	e000      	b.n	878051ce <strcmp+0x6>
878051cc:	b148      	cbz	r0, 878051e2 <strcmp+0x1a>
878051ce:	f812 0f01 	ldrb.w	r0, [r2, #1]!
878051d2:	f811 3b01 	ldrb.w	r3, [r1], #1
878051d6:	1ac3      	subs	r3, r0, r3
878051d8:	f013 03ff 	ands.w	r3, r3, #255	; 0xff
878051dc:	d0f6      	beq.n	878051cc <strcmp+0x4>
878051de:	b258      	sxtb	r0, r3
878051e0:	4770      	bx	lr
878051e2:	4770      	bx	lr

878051e4 <strncmp>:
878051e4:	b302      	cbz	r2, 87805228 <strncmp+0x44>
878051e6:	b470      	push	{r4, r5, r6}
878051e8:	1c4d      	adds	r5, r1, #1
878051ea:	7803      	ldrb	r3, [r0, #0]
878051ec:	780e      	ldrb	r6, [r1, #0]
878051ee:	1b9e      	subs	r6, r3, r6
878051f0:	f016 06ff 	ands.w	r6, r6, #255	; 0xff
878051f4:	bf18      	it	ne
878051f6:	b270      	sxtbne	r0, r6
878051f8:	d10e      	bne.n	87805218 <strncmp+0x34>
878051fa:	b1bb      	cbz	r3, 8780522c <strncmp+0x48>
878051fc:	440a      	add	r2, r1
878051fe:	e008      	b.n	87805212 <strncmp+0x2e>
87805200:	f810 4f01 	ldrb.w	r4, [r0, #1]!
87805204:	f815 3b01 	ldrb.w	r3, [r5], #1
87805208:	1ae3      	subs	r3, r4, r3
8780520a:	f013 03ff 	ands.w	r3, r3, #255	; 0xff
8780520e:	d105      	bne.n	8780521c <strncmp+0x38>
87805210:	b13c      	cbz	r4, 87805222 <strncmp+0x3e>
87805212:	4295      	cmp	r5, r2
87805214:	d1f4      	bne.n	87805200 <strncmp+0x1c>
87805216:	2000      	movs	r0, #0
87805218:	bc70      	pop	{r4, r5, r6}
8780521a:	4770      	bx	lr
8780521c:	b258      	sxtb	r0, r3
8780521e:	bc70      	pop	{r4, r5, r6}
87805220:	4770      	bx	lr
87805222:	4620      	mov	r0, r4
87805224:	bc70      	pop	{r4, r5, r6}
87805226:	4770      	bx	lr
87805228:	4610      	mov	r0, r2
8780522a:	4770      	bx	lr
8780522c:	4618      	mov	r0, r3
8780522e:	e7f3      	b.n	87805218 <strncmp+0x34>

87805230 <strchr>:
87805230:	7803      	ldrb	r3, [r0, #0]
87805232:	b2c9      	uxtb	r1, r1
87805234:	428b      	cmp	r3, r1
87805236:	d006      	beq.n	87805246 <strchr+0x16>
87805238:	b123      	cbz	r3, 87805244 <strchr+0x14>
8780523a:	f810 3f01 	ldrb.w	r3, [r0, #1]!
8780523e:	428b      	cmp	r3, r1
87805240:	d1fa      	bne.n	87805238 <strchr+0x8>
87805242:	4770      	bx	lr
87805244:	4618      	mov	r0, r3
87805246:	4770      	bx	lr

87805248 <strrchr>:
87805248:	7803      	ldrb	r3, [r0, #0]
8780524a:	b18b      	cbz	r3, 87805270 <strrchr+0x28>
8780524c:	4603      	mov	r3, r0
8780524e:	f813 2f01 	ldrb.w	r2, [r3, #1]!
87805252:	2a00      	cmp	r2, #0
87805254:	d1fb      	bne.n	8780524e <strrchr+0x6>
87805256:	b2c9      	uxtb	r1, r1
87805258:	2200      	movs	r2, #0
8780525a:	e003      	b.n	87805264 <strrchr+0x1c>
8780525c:	3b01      	subs	r3, #1
8780525e:	4298      	cmp	r0, r3
87805260:	d804      	bhi.n	8780526c <strrchr+0x24>
87805262:	781a      	ldrb	r2, [r3, #0]
87805264:	428a      	cmp	r2, r1
87805266:	d1f9      	bne.n	8780525c <strrchr+0x14>
87805268:	4618      	mov	r0, r3
8780526a:	4770      	bx	lr
8780526c:	2000      	movs	r0, #0
8780526e:	4770      	bx	lr
87805270:	4603      	mov	r3, r0
87805272:	e7f0      	b.n	87805256 <strrchr+0xe>

87805274 <strlen>:
87805274:	7802      	ldrb	r2, [r0, #0]
87805276:	b132      	cbz	r2, 87805286 <strlen+0x12>
87805278:	4603      	mov	r3, r0
8780527a:	f813 1f01 	ldrb.w	r1, [r3, #1]!
8780527e:	2900      	cmp	r1, #0
87805280:	d1fb      	bne.n	8780527a <strlen+0x6>
87805282:	1a18      	subs	r0, r3, r0
87805284:	4770      	bx	lr
87805286:	4610      	mov	r0, r2
87805288:	4770      	bx	lr
8780528a:	bf00      	nop

8780528c <strnlen>:
8780528c:	b199      	cbz	r1, 878052b6 <strnlen+0x2a>
8780528e:	7803      	ldrb	r3, [r0, #0]
87805290:	b19b      	cbz	r3, 878052ba <strnlen+0x2e>
87805292:	b410      	push	{r4}
87805294:	4401      	add	r1, r0
87805296:	1c43      	adds	r3, r0, #1
87805298:	e002      	b.n	878052a0 <strnlen+0x14>
8780529a:	781c      	ldrb	r4, [r3, #0]
8780529c:	3301      	adds	r3, #1
8780529e:	b134      	cbz	r4, 878052ae <strnlen+0x22>
878052a0:	428b      	cmp	r3, r1
878052a2:	461a      	mov	r2, r3
878052a4:	d1f9      	bne.n	8780529a <strnlen+0xe>
878052a6:	1a18      	subs	r0, r3, r0
878052a8:	f85d 4b04 	ldr.w	r4, [sp], #4
878052ac:	4770      	bx	lr
878052ae:	1a10      	subs	r0, r2, r0
878052b0:	f85d 4b04 	ldr.w	r4, [sp], #4
878052b4:	4770      	bx	lr
878052b6:	4608      	mov	r0, r1
878052b8:	4770      	bx	lr
878052ba:	4618      	mov	r0, r3
878052bc:	4770      	bx	lr
878052be:	bf00      	nop

878052c0 <strspn>:
878052c0:	b470      	push	{r4, r5, r6}
878052c2:	4605      	mov	r5, r0
878052c4:	7804      	ldrb	r4, [r0, #0]
878052c6:	b1ac      	cbz	r4, 878052f4 <strspn+0x34>
878052c8:	780e      	ldrb	r6, [r1, #0]
878052ca:	2000      	movs	r0, #0
878052cc:	b14e      	cbz	r6, 878052e2 <strspn+0x22>
878052ce:	42a6      	cmp	r6, r4
878052d0:	d009      	beq.n	878052e6 <strspn+0x26>
878052d2:	460a      	mov	r2, r1
878052d4:	e001      	b.n	878052da <strspn+0x1a>
878052d6:	429c      	cmp	r4, r3
878052d8:	d005      	beq.n	878052e6 <strspn+0x26>
878052da:	f812 3f01 	ldrb.w	r3, [r2, #1]!
878052de:	2b00      	cmp	r3, #0
878052e0:	d1f9      	bne.n	878052d6 <strspn+0x16>
878052e2:	bc70      	pop	{r4, r5, r6}
878052e4:	4770      	bx	lr
878052e6:	f815 4f01 	ldrb.w	r4, [r5, #1]!
878052ea:	3001      	adds	r0, #1
878052ec:	2c00      	cmp	r4, #0
878052ee:	d1ed      	bne.n	878052cc <strspn+0xc>
878052f0:	bc70      	pop	{r4, r5, r6}
878052f2:	4770      	bx	lr
878052f4:	4620      	mov	r0, r4
878052f6:	e7f4      	b.n	878052e2 <strspn+0x22>

878052f8 <strpbrk>:
878052f8:	b430      	push	{r4, r5}
878052fa:	7804      	ldrb	r4, [r0, #0]
878052fc:	b17c      	cbz	r4, 8780531e <strpbrk+0x26>
878052fe:	780d      	ldrb	r5, [r1, #0]
87805300:	b14d      	cbz	r5, 87805316 <strpbrk+0x1e>
87805302:	42a5      	cmp	r5, r4
87805304:	d00c      	beq.n	87805320 <strpbrk+0x28>
87805306:	460a      	mov	r2, r1
87805308:	e001      	b.n	8780530e <strpbrk+0x16>
8780530a:	429c      	cmp	r4, r3
8780530c:	d008      	beq.n	87805320 <strpbrk+0x28>
8780530e:	f812 3f01 	ldrb.w	r3, [r2, #1]!
87805312:	2b00      	cmp	r3, #0
87805314:	d1f9      	bne.n	8780530a <strpbrk+0x12>
87805316:	f810 4f01 	ldrb.w	r4, [r0, #1]!
8780531a:	2c00      	cmp	r4, #0
8780531c:	d1f0      	bne.n	87805300 <strpbrk+0x8>
8780531e:	4620      	mov	r0, r4
87805320:	bc30      	pop	{r4, r5}
87805322:	4770      	bx	lr

87805324 <strtok>:
87805324:	b5f0      	push	{r4, r5, r6, r7, lr}
87805326:	f24b 1e30 	movw	lr, #45360	; 0xb130
8780532a:	f2c8 7e81 	movt	lr, #34689	; 0x8781
8780532e:	2800      	cmp	r0, #0
87805330:	d03e      	beq.n	878053b0 <strtok+0x8c>
87805332:	7804      	ldrb	r4, [r0, #0]
87805334:	2c00      	cmp	r4, #0
87805336:	d037      	beq.n	878053a8 <strtok+0x84>
87805338:	780d      	ldrb	r5, [r1, #0]
8780533a:	4607      	mov	r7, r0
8780533c:	2600      	movs	r6, #0
8780533e:	b14d      	cbz	r5, 87805354 <strtok+0x30>
87805340:	42a5      	cmp	r5, r4
87805342:	d01e      	beq.n	87805382 <strtok+0x5e>
87805344:	460a      	mov	r2, r1
87805346:	e001      	b.n	8780534c <strtok+0x28>
87805348:	429c      	cmp	r4, r3
8780534a:	d01a      	beq.n	87805382 <strtok+0x5e>
8780534c:	f812 3f01 	ldrb.w	r3, [r2, #1]!
87805350:	2b00      	cmp	r3, #0
87805352:	d1f9      	bne.n	87805348 <strtok+0x24>
87805354:	5d84      	ldrb	r4, [r0, r6]
87805356:	4430      	add	r0, r6
87805358:	4606      	mov	r6, r0
8780535a:	b32c      	cbz	r4, 878053a8 <strtok+0x84>
8780535c:	b14d      	cbz	r5, 87805372 <strtok+0x4e>
8780535e:	42a5      	cmp	r5, r4
87805360:	d01a      	beq.n	87805398 <strtok+0x74>
87805362:	460a      	mov	r2, r1
87805364:	e001      	b.n	8780536a <strtok+0x46>
87805366:	429c      	cmp	r4, r3
87805368:	d016      	beq.n	87805398 <strtok+0x74>
8780536a:	f812 3f01 	ldrb.w	r3, [r2, #1]!
8780536e:	2b00      	cmp	r3, #0
87805370:	d1f9      	bne.n	87805366 <strtok+0x42>
87805372:	f816 4f01 	ldrb.w	r4, [r6, #1]!
87805376:	2c00      	cmp	r4, #0
87805378:	d1f0      	bne.n	8780535c <strtok+0x38>
8780537a:	4626      	mov	r6, r4
8780537c:	f8ce 6000 	str.w	r6, [lr]
87805380:	bdf0      	pop	{r4, r5, r6, r7, pc}
87805382:	f817 4f01 	ldrb.w	r4, [r7, #1]!
87805386:	3601      	adds	r6, #1
87805388:	2c00      	cmp	r4, #0
8780538a:	d1d8      	bne.n	8780533e <strtok+0x1a>
8780538c:	5d84      	ldrb	r4, [r0, r6]
8780538e:	4430      	add	r0, r6
87805390:	4606      	mov	r6, r0
87805392:	2c00      	cmp	r4, #0
87805394:	d1e2      	bne.n	8780535c <strtok+0x38>
87805396:	e007      	b.n	878053a8 <strtok+0x84>
87805398:	7833      	ldrb	r3, [r6, #0]
8780539a:	4632      	mov	r2, r6
8780539c:	2b00      	cmp	r3, #0
8780539e:	d0ed      	beq.n	8780537c <strtok+0x58>
878053a0:	2300      	movs	r3, #0
878053a2:	3601      	adds	r6, #1
878053a4:	7013      	strb	r3, [r2, #0]
878053a6:	e7e9      	b.n	8780537c <strtok+0x58>
878053a8:	2000      	movs	r0, #0
878053aa:	f8ce 0000 	str.w	r0, [lr]
878053ae:	bdf0      	pop	{r4, r5, r6, r7, pc}
878053b0:	f8de 0000 	ldr.w	r0, [lr]
878053b4:	2800      	cmp	r0, #0
878053b6:	d1bc      	bne.n	87805332 <strtok+0xe>
878053b8:	e7e2      	b.n	87805380 <strtok+0x5c>
878053ba:	bf00      	nop

878053bc <strsep>:
878053bc:	b4f0      	push	{r4, r5, r6, r7}
878053be:	4607      	mov	r7, r0
878053c0:	6800      	ldr	r0, [r0, #0]
878053c2:	b198      	cbz	r0, 878053ec <strsep+0x30>
878053c4:	7804      	ldrb	r4, [r0, #0]
878053c6:	b184      	cbz	r4, 878053ea <strsep+0x2e>
878053c8:	780e      	ldrb	r6, [r1, #0]
878053ca:	4605      	mov	r5, r0
878053cc:	b14e      	cbz	r6, 878053e2 <strsep+0x26>
878053ce:	42a6      	cmp	r6, r4
878053d0:	d00e      	beq.n	878053f0 <strsep+0x34>
878053d2:	460a      	mov	r2, r1
878053d4:	e001      	b.n	878053da <strsep+0x1e>
878053d6:	429c      	cmp	r4, r3
878053d8:	d00a      	beq.n	878053f0 <strsep+0x34>
878053da:	f812 3f01 	ldrb.w	r3, [r2, #1]!
878053de:	2b00      	cmp	r3, #0
878053e0:	d1f9      	bne.n	878053d6 <strsep+0x1a>
878053e2:	f815 4f01 	ldrb.w	r4, [r5, #1]!
878053e6:	2c00      	cmp	r4, #0
878053e8:	d1f0      	bne.n	878053cc <strsep+0x10>
878053ea:	603c      	str	r4, [r7, #0]
878053ec:	bcf0      	pop	{r4, r5, r6, r7}
878053ee:	4770      	bx	lr
878053f0:	2300      	movs	r3, #0
878053f2:	1c6c      	adds	r4, r5, #1
878053f4:	702b      	strb	r3, [r5, #0]
878053f6:	e7f8      	b.n	878053ea <strsep+0x2e>

878053f8 <memset>:
878053f8:	b132      	cbz	r2, 87805408 <memset+0x10>
878053fa:	b2c9      	uxtb	r1, r1
878053fc:	4402      	add	r2, r0
878053fe:	4603      	mov	r3, r0
87805400:	f803 1b01 	strb.w	r1, [r3], #1
87805404:	4293      	cmp	r3, r2
87805406:	d1fb      	bne.n	87805400 <memset+0x8>
87805408:	4770      	bx	lr
8780540a:	bf00      	nop

8780540c <bcopy>:
8780540c:	b13a      	cbz	r2, 8780541e <bcopy+0x12>
8780540e:	3901      	subs	r1, #1
87805410:	4402      	add	r2, r0
87805412:	f810 3b01 	ldrb.w	r3, [r0], #1
87805416:	4290      	cmp	r0, r2
87805418:	f801 3f01 	strb.w	r3, [r1, #1]!
8780541c:	d1f9      	bne.n	87805412 <bcopy+0x6>
8780541e:	4770      	bx	lr

87805420 <memcpy>:
87805420:	b152      	cbz	r2, 87805438 <memcpy+0x18>
87805422:	440a      	add	r2, r1
87805424:	1e43      	subs	r3, r0, #1
87805426:	b410      	push	{r4}
87805428:	f811 4b01 	ldrb.w	r4, [r1], #1
8780542c:	4291      	cmp	r1, r2
8780542e:	f803 4f01 	strb.w	r4, [r3, #1]!
87805432:	d1f9      	bne.n	87805428 <memcpy+0x8>
87805434:	f85d 4b04 	ldr.w	r4, [sp], #4
87805438:	4770      	bx	lr
8780543a:	bf00      	nop

8780543c <memmove>:
8780543c:	4288      	cmp	r0, r1
8780543e:	b410      	push	{r4}
87805440:	d80b      	bhi.n	8780545a <memmove+0x1e>
87805442:	b13a      	cbz	r2, 87805454 <memmove+0x18>
87805444:	440a      	add	r2, r1
87805446:	1e43      	subs	r3, r0, #1
87805448:	f811 4b01 	ldrb.w	r4, [r1], #1
8780544c:	4291      	cmp	r1, r2
8780544e:	f803 4f01 	strb.w	r4, [r3, #1]!
87805452:	d1f9      	bne.n	87805448 <memmove+0xc>
87805454:	f85d 4b04 	ldr.w	r4, [sp], #4
87805458:	4770      	bx	lr
8780545a:	4411      	add	r1, r2
8780545c:	1883      	adds	r3, r0, r2
8780545e:	2a00      	cmp	r2, #0
87805460:	d0f8      	beq.n	87805454 <memmove+0x18>
87805462:	1a8a      	subs	r2, r1, r2
87805464:	f811 4d01 	ldrb.w	r4, [r1, #-1]!
87805468:	4291      	cmp	r1, r2
8780546a:	f803 4d01 	strb.w	r4, [r3, #-1]!
8780546e:	d1f9      	bne.n	87805464 <memmove+0x28>
87805470:	f85d 4b04 	ldr.w	r4, [sp], #4
87805474:	4770      	bx	lr
87805476:	bf00      	nop

87805478 <memcmp>:
87805478:	b1aa      	cbz	r2, 878054a6 <memcmp+0x2e>
8780547a:	b430      	push	{r4, r5}
8780547c:	4604      	mov	r4, r0
8780547e:	7803      	ldrb	r3, [r0, #0]
87805480:	7808      	ldrb	r0, [r1, #0]
87805482:	1a18      	subs	r0, r3, r0
87805484:	d10a      	bne.n	8780549c <memcmp+0x24>
87805486:	4422      	add	r2, r4
87805488:	3401      	adds	r4, #1
8780548a:	e005      	b.n	87805498 <memcmp+0x20>
8780548c:	f814 3b01 	ldrb.w	r3, [r4], #1
87805490:	f811 5f01 	ldrb.w	r5, [r1, #1]!
87805494:	1b5b      	subs	r3, r3, r5
87805496:	d103      	bne.n	878054a0 <memcmp+0x28>
87805498:	4294      	cmp	r4, r2
8780549a:	d1f7      	bne.n	8780548c <memcmp+0x14>
8780549c:	bc30      	pop	{r4, r5}
8780549e:	4770      	bx	lr
878054a0:	4618      	mov	r0, r3
878054a2:	bc30      	pop	{r4, r5}
878054a4:	4770      	bx	lr
878054a6:	4610      	mov	r0, r2
878054a8:	4770      	bx	lr
878054aa:	bf00      	nop

878054ac <memscan>:
878054ac:	b182      	cbz	r2, 878054d0 <memscan+0x24>
878054ae:	7803      	ldrb	r3, [r0, #0]
878054b0:	428b      	cmp	r3, r1
878054b2:	d00d      	beq.n	878054d0 <memscan+0x24>
878054b4:	b410      	push	{r4}
878054b6:	4402      	add	r2, r0
878054b8:	1c43      	adds	r3, r0, #1
878054ba:	e002      	b.n	878054c2 <memscan+0x16>
878054bc:	7804      	ldrb	r4, [r0, #0]
878054be:	428c      	cmp	r4, r1
878054c0:	d004      	beq.n	878054cc <memscan+0x20>
878054c2:	4293      	cmp	r3, r2
878054c4:	4618      	mov	r0, r3
878054c6:	f103 0301 	add.w	r3, r3, #1
878054ca:	d1f7      	bne.n	878054bc <memscan+0x10>
878054cc:	f85d 4b04 	ldr.w	r4, [sp], #4
878054d0:	4770      	bx	lr
878054d2:	bf00      	nop

878054d4 <strstr>:
878054d4:	b5f0      	push	{r4, r5, r6, r7, lr}
878054d6:	468e      	mov	lr, r1
878054d8:	f891 c000 	ldrb.w	ip, [r1]
878054dc:	f1bc 0f00 	cmp.w	ip, #0
878054e0:	d028      	beq.n	87805534 <strstr+0x60>
878054e2:	460e      	mov	r6, r1
878054e4:	f816 3f01 	ldrb.w	r3, [r6, #1]!
878054e8:	2b00      	cmp	r3, #0
878054ea:	d1fb      	bne.n	878054e4 <strstr+0x10>
878054ec:	ebb6 010e 	subs.w	r1, r6, lr
878054f0:	d020      	beq.n	87805534 <strstr+0x60>
878054f2:	7807      	ldrb	r7, [r0, #0]
878054f4:	b12f      	cbz	r7, 87805502 <strstr+0x2e>
878054f6:	4607      	mov	r7, r0
878054f8:	f817 3f01 	ldrb.w	r3, [r7, #1]!
878054fc:	2b00      	cmp	r3, #0
878054fe:	d1fb      	bne.n	878054f8 <strstr+0x24>
87805500:	1a3f      	subs	r7, r7, r0
87805502:	42b9      	cmp	r1, r7
87805504:	dc15      	bgt.n	87805532 <strstr+0x5e>
87805506:	3f01      	subs	r7, #1
87805508:	b1a1      	cbz	r1, 87805534 <strstr+0x60>
8780550a:	7803      	ldrb	r3, [r0, #0]
8780550c:	4563      	cmp	r3, ip
8780550e:	d10c      	bne.n	8780552a <strstr+0x56>
87805510:	4602      	mov	r2, r0
87805512:	f10e 0301 	add.w	r3, lr, #1
87805516:	e005      	b.n	87805524 <strstr+0x50>
87805518:	f813 4b01 	ldrb.w	r4, [r3], #1
8780551c:	f812 5f01 	ldrb.w	r5, [r2, #1]!
87805520:	42a5      	cmp	r5, r4
87805522:	d102      	bne.n	8780552a <strstr+0x56>
87805524:	42b3      	cmp	r3, r6
87805526:	d1f7      	bne.n	87805518 <strstr+0x44>
87805528:	bdf0      	pop	{r4, r5, r6, r7, pc}
8780552a:	428f      	cmp	r7, r1
8780552c:	f100 0001 	add.w	r0, r0, #1
87805530:	dae9      	bge.n	87805506 <strstr+0x32>
87805532:	2000      	movs	r0, #0
87805534:	bdf0      	pop	{r4, r5, r6, r7, pc}
87805536:	bf00      	nop

87805538 <memchr>:
87805538:	b410      	push	{r4}
8780553a:	b2c9      	uxtb	r1, r1
8780553c:	4402      	add	r2, r0
8780553e:	e004      	b.n	8780554a <memchr+0x12>
87805540:	f813 4b01 	ldrb.w	r4, [r3], #1
87805544:	428c      	cmp	r4, r1
87805546:	d004      	beq.n	87805552 <memchr+0x1a>
87805548:	4618      	mov	r0, r3
8780554a:	4290      	cmp	r0, r2
8780554c:	4603      	mov	r3, r0
8780554e:	d1f7      	bne.n	87805540 <memchr+0x8>
87805550:	2000      	movs	r0, #0
87805552:	f85d 4b04 	ldr.w	r4, [sp], #4
87805556:	4770      	bx	lr

87805558 <__div64_32>:
87805558:	e92d 4ff8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, lr}
8780555c:	4688      	mov	r8, r1
8780555e:	e9d0 6700 	ldrd	r6, r7, [r0]
87805562:	f04f 0b00 	mov.w	fp, #0
87805566:	4681      	mov	r9, r0
87805568:	468a      	mov	sl, r1
8780556a:	4644      	mov	r4, r8
8780556c:	465d      	mov	r5, fp
8780556e:	42b9      	cmp	r1, r7
87805570:	bf84      	itt	hi
87805572:	2200      	movhi	r2, #0
87805574:	2300      	movhi	r3, #0
87805576:	d80b      	bhi.n	87805590 <__div64_32+0x38>
87805578:	4638      	mov	r0, r7
8780557a:	f003 fd09 	bl	87808f90 <__aeabi_uidiv>
8780557e:	4686      	mov	lr, r0
87805580:	2000      	movs	r0, #0
87805582:	fb08 f10e 	mul.w	r1, r8, lr
87805586:	1a36      	subs	r6, r6, r0
87805588:	4602      	mov	r2, r0
8780558a:	4673      	mov	r3, lr
8780558c:	eb67 0701 	sbc.w	r7, r7, r1
87805590:	45bb      	cmp	fp, r7
87805592:	bf08      	it	eq
87805594:	45b2      	cmpeq	sl, r6
87805596:	bf34      	ite	cc
87805598:	2101      	movcc	r1, #1
8780559a:	2100      	movcs	r1, #0
8780559c:	f1ba 0f01 	cmp.w	sl, #1
878055a0:	f001 0101 	and.w	r1, r1, #1
878055a4:	f17b 0000 	sbcs.w	r0, fp, #0
878055a8:	f04f 0001 	mov.w	r0, #1
878055ac:	bfb8      	it	lt
878055ae:	2100      	movlt	r1, #0
878055b0:	b371      	cbz	r1, 87805610 <__div64_32+0xb8>
878055b2:	2100      	movs	r1, #0
878055b4:	1924      	adds	r4, r4, r4
878055b6:	416d      	adcs	r5, r5
878055b8:	1800      	adds	r0, r0, r0
878055ba:	4149      	adcs	r1, r1
878055bc:	2c01      	cmp	r4, #1
878055be:	f175 0e00 	sbcs.w	lr, r5, #0
878055c2:	bfac      	ite	ge
878055c4:	f04f 0e01 	movge.w	lr, #1
878055c8:	f04f 0e00 	movlt.w	lr, #0
878055cc:	42bd      	cmp	r5, r7
878055ce:	f00e 0e01 	and.w	lr, lr, #1
878055d2:	bf08      	it	eq
878055d4:	42b4      	cmpeq	r4, r6
878055d6:	bf28      	it	cs
878055d8:	f04f 0e00 	movcs.w	lr, #0
878055dc:	f1be 0f00 	cmp.w	lr, #0
878055e0:	d1e8      	bne.n	878055b4 <__div64_32+0x5c>
878055e2:	42af      	cmp	r7, r5
878055e4:	bf08      	it	eq
878055e6:	42a6      	cmpeq	r6, r4
878055e8:	d304      	bcc.n	878055f4 <__div64_32+0x9c>
878055ea:	1b36      	subs	r6, r6, r4
878055ec:	eb67 0705 	sbc.w	r7, r7, r5
878055f0:	1812      	adds	r2, r2, r0
878055f2:	414b      	adcs	r3, r1
878055f4:	0849      	lsrs	r1, r1, #1
878055f6:	ea4f 0030 	mov.w	r0, r0, rrx
878055fa:	086d      	lsrs	r5, r5, #1
878055fc:	ea4f 0434 	mov.w	r4, r4, rrx
87805600:	ea50 0e01 	orrs.w	lr, r0, r1
87805604:	d1ed      	bne.n	878055e2 <__div64_32+0x8a>
87805606:	e9c9 2300 	strd	r2, r3, [r9]
8780560a:	4630      	mov	r0, r6
8780560c:	e8bd 8ff8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, pc}
87805610:	2100      	movs	r1, #0
87805612:	e7e6      	b.n	878055e2 <__div64_32+0x8a>

87805614 <number>:
87805614:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87805618:	b0a9      	sub	sp, #164	; 0xa4
8780561a:	460e      	mov	r6, r1
8780561c:	9932      	ldr	r1, [sp, #200]	; 0xc8
8780561e:	4604      	mov	r4, r0
87805620:	9835      	ldr	r0, [sp, #212]	; 0xd4
87805622:	390a      	subs	r1, #10
87805624:	bf18      	it	ne
87805626:	2101      	movne	r1, #1
87805628:	f000 0820 	and.w	r8, r0, #32
8780562c:	ea01 1050 	and.w	r0, r1, r0, lsr #5
87805630:	fa5f f888 	uxtb.w	r8, r8
87805634:	9000      	str	r0, [sp, #0]
87805636:	9835      	ldr	r0, [sp, #212]	; 0xd4
87805638:	f010 0010 	ands.w	r0, r0, #16
8780563c:	9002      	str	r0, [sp, #8]
8780563e:	bf1e      	ittt	ne
87805640:	9835      	ldrne	r0, [sp, #212]	; 0xd4
87805642:	f020 0001 	bicne.w	r0, r0, #1
87805646:	9035      	strne	r0, [sp, #212]	; 0xd4
87805648:	9835      	ldr	r0, [sp, #212]	; 0xd4
8780564a:	f010 0002 	ands.w	r0, r0, #2
8780564e:	d012      	beq.n	87805676 <number+0x62>
87805650:	2a00      	cmp	r2, #0
87805652:	f173 0000 	sbcs.w	r0, r3, #0
87805656:	f2c0 80e4 	blt.w	87805822 <number+0x20e>
8780565a:	9835      	ldr	r0, [sp, #212]	; 0xd4
8780565c:	0740      	lsls	r0, r0, #29
8780565e:	f100 80c6 	bmi.w	878057ee <number+0x1da>
87805662:	9835      	ldr	r0, [sp, #212]	; 0xd4
87805664:	f010 0008 	ands.w	r0, r0, #8
87805668:	d005      	beq.n	87805676 <number+0x62>
8780566a:	9833      	ldr	r0, [sp, #204]	; 0xcc
8780566c:	3801      	subs	r0, #1
8780566e:	9033      	str	r0, [sp, #204]	; 0xcc
87805670:	2020      	movs	r0, #32
87805672:	9001      	str	r0, [sp, #4]
87805674:	e000      	b.n	87805678 <number+0x64>
87805676:	9001      	str	r0, [sp, #4]
87805678:	9800      	ldr	r0, [sp, #0]
8780567a:	b130      	cbz	r0, 8780568a <number+0x76>
8780567c:	9832      	ldr	r0, [sp, #200]	; 0xc8
8780567e:	2810      	cmp	r0, #16
87805680:	f000 80c4 	beq.w	8780580c <number+0x1f8>
87805684:	9833      	ldr	r0, [sp, #204]	; 0xcc
87805686:	3801      	subs	r0, #1
87805688:	9033      	str	r0, [sp, #204]	; 0xcc
8780568a:	ea52 0003 	orrs.w	r0, r2, r3
8780568e:	d17b      	bne.n	87805788 <number+0x174>
87805690:	2200      	movs	r2, #0
87805692:	2501      	movs	r5, #1
87805694:	2330      	movs	r3, #48	; 0x30
87805696:	f88d 303c 	strb.w	r3, [sp, #60]	; 0x3c
8780569a:	9b34      	ldr	r3, [sp, #208]	; 0xd0
8780569c:	9933      	ldr	r1, [sp, #204]	; 0xcc
8780569e:	42ab      	cmp	r3, r5
878056a0:	bfb8      	it	lt
878056a2:	462b      	movlt	r3, r5
878056a4:	1ac8      	subs	r0, r1, r3
878056a6:	9935      	ldr	r1, [sp, #212]	; 0xd4
878056a8:	f011 0f11 	tst.w	r1, #17
878056ac:	d10f      	bne.n	878056ce <number+0xba>
878056ae:	f1b0 0e01 	subs.w	lr, r0, #1
878056b2:	f100 81c5 	bmi.w	87805a40 <number+0x42c>
878056b6:	1821      	adds	r1, r4, r0
878056b8:	2720      	movs	r7, #32
878056ba:	42a6      	cmp	r6, r4
878056bc:	f104 0401 	add.w	r4, r4, #1
878056c0:	bf88      	it	hi
878056c2:	f804 7c01 	strbhi.w	r7, [r4, #-1]
878056c6:	428c      	cmp	r4, r1
878056c8:	d1f7      	bne.n	878056ba <number+0xa6>
878056ca:	ebc0 000e 	rsb	r0, r0, lr
878056ce:	9901      	ldr	r1, [sp, #4]
878056d0:	b139      	cbz	r1, 878056e2 <number+0xce>
878056d2:	42b4      	cmp	r4, r6
878056d4:	f104 0401 	add.w	r4, r4, #1
878056d8:	bf3c      	itt	cc
878056da:	f89d 1004 	ldrbcc.w	r1, [sp, #4]
878056de:	f804 1c01 	strbcc.w	r1, [r4, #-1]
878056e2:	9900      	ldr	r1, [sp, #0]
878056e4:	b149      	cbz	r1, 878056fa <number+0xe6>
878056e6:	9f32      	ldr	r7, [sp, #200]	; 0xc8
878056e8:	42b4      	cmp	r4, r6
878056ea:	bf3c      	itt	cc
878056ec:	2130      	movcc	r1, #48	; 0x30
878056ee:	7021      	strbcc	r1, [r4, #0]
878056f0:	1c61      	adds	r1, r4, #1
878056f2:	2f10      	cmp	r7, #16
878056f4:	bf18      	it	ne
878056f6:	460c      	movne	r4, r1
878056f8:	d07f      	beq.n	878057fa <number+0x1e6>
878056fa:	9902      	ldr	r1, [sp, #8]
878056fc:	b999      	cbnz	r1, 87805726 <number+0x112>
878056fe:	9935      	ldr	r1, [sp, #212]	; 0xd4
87805700:	f011 0f01 	tst.w	r1, #1
87805704:	bf14      	ite	ne
87805706:	2130      	movne	r1, #48	; 0x30
87805708:	2120      	moveq	r1, #32
8780570a:	1e47      	subs	r7, r0, #1
8780570c:	f100 819a 	bmi.w	87805a44 <number+0x430>
87805710:	4420      	add	r0, r4
87805712:	42a6      	cmp	r6, r4
87805714:	f104 0401 	add.w	r4, r4, #1
87805718:	bf88      	it	hi
8780571a:	f804 1c01 	strbhi.w	r1, [r4, #-1]
8780571e:	4284      	cmp	r4, r0
87805720:	d1f7      	bne.n	87805712 <number+0xfe>
87805722:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
87805726:	429d      	cmp	r5, r3
87805728:	f280 811f 	bge.w	8780596a <number+0x356>
8780572c:	1b5b      	subs	r3, r3, r5
8780572e:	2130      	movs	r1, #48	; 0x30
87805730:	4423      	add	r3, r4
87805732:	42a6      	cmp	r6, r4
87805734:	f104 0401 	add.w	r4, r4, #1
87805738:	bf88      	it	hi
8780573a:	f804 1c01 	strbhi.w	r1, [r4, #-1]
8780573e:	429c      	cmp	r4, r3
87805740:	d1f7      	bne.n	87805732 <number+0x11e>
87805742:	2a00      	cmp	r2, #0
87805744:	f2c0 810d 	blt.w	87805962 <number+0x34e>
87805748:	1c51      	adds	r1, r2, #1
8780574a:	ac0f      	add	r4, sp, #60	; 0x3c
8780574c:	4422      	add	r2, r4
8780574e:	4419      	add	r1, r3
87805750:	429e      	cmp	r6, r3
87805752:	f103 0301 	add.w	r3, r3, #1
87805756:	bf88      	it	hi
87805758:	7814      	ldrbhi	r4, [r2, #0]
8780575a:	f102 32ff 	add.w	r2, r2, #4294967295	; 0xffffffff
8780575e:	bf88      	it	hi
87805760:	f803 4c01 	strbhi.w	r4, [r3, #-1]
87805764:	428b      	cmp	r3, r1
87805766:	d1f3      	bne.n	87805750 <number+0x13c>
87805768:	2800      	cmp	r0, #0
8780576a:	f340 80fc 	ble.w	87805966 <number+0x352>
8780576e:	4408      	add	r0, r1
87805770:	2320      	movs	r3, #32
87805772:	428e      	cmp	r6, r1
87805774:	f101 0101 	add.w	r1, r1, #1
87805778:	bf88      	it	hi
8780577a:	f801 3c01 	strbhi.w	r3, [r1, #-1]
8780577e:	4281      	cmp	r1, r0
87805780:	d1f7      	bne.n	87805772 <number+0x15e>
87805782:	b029      	add	sp, #164	; 0xa4
87805784:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87805788:	2900      	cmp	r1, #0
8780578a:	d053      	beq.n	87805834 <number+0x220>
8780578c:	9932      	ldr	r1, [sp, #200]	; 0xc8
8780578e:	2910      	cmp	r1, #16
87805790:	f101 3cff 	add.w	ip, r1, #4294967295	; 0xffffffff
87805794:	bf12      	itee	ne
87805796:	2003      	movne	r0, #3
87805798:	f04f 0c0f 	moveq.w	ip, #15
8780579c:	2004      	moveq	r0, #4
8780579e:	f249 497c 	movw	r9, #38012	; 0x947c
878057a2:	f10d 073b 	add.w	r7, sp, #59	; 0x3b
878057a6:	f2c8 7980 	movt	r9, #34688	; 0x8780
878057aa:	2500      	movs	r5, #0
878057ac:	f1c0 0a20 	rsb	sl, r0, #32
878057b0:	fa5f fb82 	uxtb.w	fp, r2
878057b4:	fa22 f100 	lsr.w	r1, r2, r0
878057b8:	fa43 fe00 	asr.w	lr, r3, r0
878057bc:	ea0b 020c 	and.w	r2, fp, ip
878057c0:	f819 b002 	ldrb.w	fp, [r9, r2]
878057c4:	fa03 f20a 	lsl.w	r2, r3, sl
878057c8:	3501      	adds	r5, #1
878057ca:	4311      	orrs	r1, r2
878057cc:	f1b0 0220 	subs.w	r2, r0, #32
878057d0:	fa43 f202 	asr.w	r2, r3, r2
878057d4:	ea48 030b 	orr.w	r3, r8, fp
878057d8:	f807 3f01 	strb.w	r3, [r7, #1]!
878057dc:	4673      	mov	r3, lr
878057de:	bf58      	it	pl
878057e0:	4311      	orrpl	r1, r2
878057e2:	460a      	mov	r2, r1
878057e4:	ea52 0103 	orrs.w	r1, r2, r3
878057e8:	d1e2      	bne.n	878057b0 <number+0x19c>
878057ea:	1e6a      	subs	r2, r5, #1
878057ec:	e755      	b.n	8780569a <number+0x86>
878057ee:	9833      	ldr	r0, [sp, #204]	; 0xcc
878057f0:	3801      	subs	r0, #1
878057f2:	9033      	str	r0, [sp, #204]	; 0xcc
878057f4:	202b      	movs	r0, #43	; 0x2b
878057f6:	9001      	str	r0, [sp, #4]
878057f8:	e73e      	b.n	87805678 <number+0x64>
878057fa:	428e      	cmp	r6, r1
878057fc:	f104 0402 	add.w	r4, r4, #2
87805800:	bf84      	itt	hi
87805802:	f048 0858 	orrhi.w	r8, r8, #88	; 0x58
87805806:	f804 8c01 	strbhi.w	r8, [r4, #-1]
8780580a:	e776      	b.n	878056fa <number+0xe6>
8780580c:	9933      	ldr	r1, [sp, #204]	; 0xcc
8780580e:	3902      	subs	r1, #2
87805810:	9133      	str	r1, [sp, #204]	; 0xcc
87805812:	ea52 0103 	orrs.w	r1, r2, r3
87805816:	bf1c      	itt	ne
87805818:	f04f 0c0f 	movne.w	ip, #15
8780581c:	2004      	movne	r0, #4
8780581e:	d1be      	bne.n	8780579e <number+0x18a>
87805820:	e736      	b.n	87805690 <number+0x7c>
87805822:	9833      	ldr	r0, [sp, #204]	; 0xcc
87805824:	4252      	negs	r2, r2
87805826:	eb63 0343 	sbc.w	r3, r3, r3, lsl #1
8780582a:	3801      	subs	r0, #1
8780582c:	9033      	str	r0, [sp, #204]	; 0xcc
8780582e:	202d      	movs	r0, #45	; 0x2d
87805830:	9001      	str	r0, [sp, #4]
87805832:	e721      	b.n	87805678 <number+0x64>
87805834:	4692      	mov	sl, r2
87805836:	469b      	mov	fp, r3
87805838:	f248 629f 	movw	r2, #34463	; 0x869f
8780583c:	2300      	movs	r3, #0
8780583e:	f2c0 0201 	movt	r2, #1
87805842:	455b      	cmp	r3, fp
87805844:	bf08      	it	eq
87805846:	4552      	cmpeq	r2, sl
87805848:	e9cd ab0c 	strd	sl, fp, [sp, #48]	; 0x30
8780584c:	f080 80fc 	bcs.w	87805a48 <number+0x434>
87805850:	f248 61a0 	movw	r1, #34464	; 0x86a0
87805854:	f645 20c5 	movw	r0, #23237	; 0x5ac5
87805858:	f2c0 0101 	movt	r1, #1
8780585c:	f6c0 207c 	movt	r0, #2684	; 0xa7c
87805860:	f10d 0541 	add.w	r5, sp, #65	; 0x41
87805864:	e9cd 2304 	strd	r2, r3, [sp, #16]
87805868:	9106      	str	r1, [sp, #24]
8780586a:	462f      	mov	r7, r5
8780586c:	9003      	str	r0, [sp, #12]
8780586e:	f8cd 801c 	str.w	r8, [sp, #28]
87805872:	9408      	str	r4, [sp, #32]
87805874:	9609      	str	r6, [sp, #36]	; 0x24
87805876:	e060      	b.n	8780593a <number+0x326>
87805878:	9a06      	ldr	r2, [sp, #24]
8780587a:	09d8      	lsrs	r0, r3, #7
8780587c:	f04f 0b00 	mov.w	fp, #0
87805880:	4603      	mov	r3, r0
87805882:	fb02 a010 	mls	r0, r2, r0, sl
87805886:	469a      	mov	sl, r3
87805888:	e9cd ab0c 	strd	sl, fp, [sp, #48]	; 0x30
8780588c:	f3c0 1803 	ubfx	r8, r0, #4, #4
87805890:	f3c0 2303 	ubfx	r3, r0, #8, #4
87805894:	0b02      	lsrs	r2, r0, #12
87805896:	eb08 0103 	add.w	r1, r8, r3
8780589a:	f000 0e0f 	and.w	lr, r0, #15
8780589e:	eb03 0c83 	add.w	ip, r3, r3, lsl #2
878058a2:	4411      	add	r1, r2
878058a4:	eb02 00c2 	add.w	r0, r2, r2, lsl #3
878058a8:	00ce      	lsls	r6, r1, #3
878058aa:	4440      	add	r0, r8
878058ac:	eba6 0141 	sub.w	r1, r6, r1, lsl #1
878058b0:	26cd      	movs	r6, #205	; 0xcd
878058b2:	4460      	add	r0, ip
878058b4:	4471      	add	r1, lr
878058b6:	46bc      	mov	ip, r7
878058b8:	fb06 f901 	mul.w	r9, r6, r1
878058bc:	3705      	adds	r7, #5
878058be:	ea4f 29d9 	mov.w	r9, r9, lsr #11
878058c2:	4448      	add	r0, r9
878058c4:	ea4f 0ec9 	mov.w	lr, r9, lsl #3
878058c8:	fb06 f800 	mul.w	r8, r6, r0
878058cc:	eb0e 0e49 	add.w	lr, lr, r9, lsl #1
878058d0:	ebce 0101 	rsb	r1, lr, r1
878058d4:	ea4f 28d8 	mov.w	r8, r8, lsr #11
878058d8:	3130      	adds	r1, #48	; 0x30
878058da:	f807 1c0a 	strb.w	r1, [r7, #-10]
878058de:	eb08 0343 	add.w	r3, r8, r3, lsl #1
878058e2:	ea4f 0ec8 	mov.w	lr, r8, lsl #3
878058e6:	0119      	lsls	r1, r3, #4
878058e8:	eb0e 0848 	add.w	r8, lr, r8, lsl #1
878058ec:	eba1 0183 	sub.w	r1, r1, r3, lsl #2
878058f0:	ebc8 0800 	rsb	r8, r8, r0
878058f4:	f108 0030 	add.w	r0, r8, #48	; 0x30
878058f8:	4419      	add	r1, r3
878058fa:	f807 0c09 	strb.w	r0, [r7, #-9]
878058fe:	09c9      	lsrs	r1, r1, #7
87805900:	eb01 0282 	add.w	r2, r1, r2, lsl #2
87805904:	00c8      	lsls	r0, r1, #3
87805906:	fb06 f602 	mul.w	r6, r6, r2
8780590a:	eb00 0141 	add.w	r1, r0, r1, lsl #1
8780590e:	1a5b      	subs	r3, r3, r1
87805910:	3330      	adds	r3, #48	; 0x30
87805912:	0af6      	lsrs	r6, r6, #11
87805914:	f807 3c08 	strb.w	r3, [r7, #-8]
87805918:	00f3      	lsls	r3, r6, #3
8780591a:	f106 0130 	add.w	r1, r6, #48	; 0x30
8780591e:	f807 1c06 	strb.w	r1, [r7, #-6]
87805922:	eb03 0646 	add.w	r6, r3, r6, lsl #1
87805926:	1b92      	subs	r2, r2, r6
87805928:	3230      	adds	r2, #48	; 0x30
8780592a:	f807 2c07 	strb.w	r2, [r7, #-7]
8780592e:	e9dd 2304 	ldrd	r2, r3, [sp, #16]
87805932:	455b      	cmp	r3, fp
87805934:	bf08      	it	eq
87805936:	4552      	cmpeq	r2, sl
87805938:	d219      	bcs.n	8780596e <number+0x35a>
8780593a:	9b03      	ldr	r3, [sp, #12]
8780593c:	465c      	mov	r4, fp
8780593e:	2500      	movs	r5, #0
87805940:	ea4f 125a 	mov.w	r2, sl, lsr #5
87805944:	ea54 0105 	orrs.w	r1, r4, r5
87805948:	fba2 2303 	umull	r2, r3, r2, r3
8780594c:	d094      	beq.n	87805878 <number+0x264>
8780594e:	f248 61a0 	movw	r1, #34464	; 0x86a0
87805952:	a80c      	add	r0, sp, #48	; 0x30
87805954:	f2c0 0101 	movt	r1, #1
87805958:	f7ff fdfe 	bl	87805558 <__div64_32>
8780595c:	e9dd ab0c 	ldrd	sl, fp, [sp, #48]	; 0x30
87805960:	e794      	b.n	8780588c <number+0x278>
87805962:	4619      	mov	r1, r3
87805964:	e700      	b.n	87805768 <number+0x154>
87805966:	4608      	mov	r0, r1
87805968:	e70b      	b.n	87805782 <number+0x16e>
8780596a:	4623      	mov	r3, r4
8780596c:	e6e9      	b.n	87805742 <number+0x12e>
8780596e:	f8dd 801c 	ldr.w	r8, [sp, #28]
87805972:	ab0f      	add	r3, sp, #60	; 0x3c
87805974:	9c08      	ldr	r4, [sp, #32]
87805976:	9e09      	ldr	r6, [sp, #36]	; 0x24
87805978:	9303      	str	r3, [sp, #12]
8780597a:	f3ca 1303 	ubfx	r3, sl, #4, #4
8780597e:	f3ca 2903 	ubfx	r9, sl, #8, #4
87805982:	ea4f 3e1a 	mov.w	lr, sl, lsr #12
87805986:	eb03 0209 	add.w	r2, r3, r9
8780598a:	f00a 070f 	and.w	r7, sl, #15
8780598e:	eb09 0589 	add.w	r5, r9, r9, lsl #2
87805992:	4472      	add	r2, lr
87805994:	eb0e 00ce 	add.w	r0, lr, lr, lsl #3
87805998:	00d1      	lsls	r1, r2, #3
8780599a:	f04f 0acd 	mov.w	sl, #205	; 0xcd
8780599e:	4403      	add	r3, r0
878059a0:	eba1 0142 	sub.w	r1, r1, r2, lsl #1
878059a4:	442b      	add	r3, r5
878059a6:	4439      	add	r1, r7
878059a8:	fb0a f201 	mul.w	r2, sl, r1
878059ac:	0ad2      	lsrs	r2, r2, #11
878059ae:	00d0      	lsls	r0, r2, #3
878059b0:	eb00 0042 	add.w	r0, r0, r2, lsl #1
878059b4:	189a      	adds	r2, r3, r2
878059b6:	bf08      	it	eq
878059b8:	f10c 0501 	addeq.w	r5, ip, #1
878059bc:	eba1 0100 	sub.w	r1, r1, r0
878059c0:	f101 0130 	add.w	r1, r1, #48	; 0x30
878059c4:	f88c 1000 	strb.w	r1, [ip]
878059c8:	d037      	beq.n	87805a3a <number+0x426>
878059ca:	fb0a f102 	mul.w	r1, sl, r2
878059ce:	0ac9      	lsrs	r1, r1, #11
878059d0:	00c8      	lsls	r0, r1, #3
878059d2:	eb01 0349 	add.w	r3, r1, r9, lsl #1
878059d6:	eb00 0141 	add.w	r1, r0, r1, lsl #1
878059da:	1a52      	subs	r2, r2, r1
878059dc:	3230      	adds	r2, #48	; 0x30
878059de:	f88c 2001 	strb.w	r2, [ip, #1]
878059e2:	ea53 020e 	orrs.w	r2, r3, lr
878059e6:	bf08      	it	eq
878059e8:	f10c 0502 	addeq.w	r5, ip, #2
878059ec:	d025      	beq.n	87805a3a <number+0x426>
878059ee:	011a      	lsls	r2, r3, #4
878059f0:	eba2 0283 	sub.w	r2, r2, r3, lsl #2
878059f4:	441a      	add	r2, r3
878059f6:	09d2      	lsrs	r2, r2, #7
878059f8:	00d1      	lsls	r1, r2, #3
878059fa:	eb01 0142 	add.w	r1, r1, r2, lsl #1
878059fe:	eb12 028e 	adds.w	r2, r2, lr, lsl #2
87805a02:	eba3 0301 	sub.w	r3, r3, r1
87805a06:	bf08      	it	eq
87805a08:	f10c 0503 	addeq.w	r5, ip, #3
87805a0c:	f103 0330 	add.w	r3, r3, #48	; 0x30
87805a10:	f88c 3002 	strb.w	r3, [ip, #2]
87805a14:	d011      	beq.n	87805a3a <number+0x426>
87805a16:	fb0a f302 	mul.w	r3, sl, r2
87805a1a:	f10c 0504 	add.w	r5, ip, #4
87805a1e:	0adb      	lsrs	r3, r3, #11
87805a20:	00d9      	lsls	r1, r3, #3
87805a22:	eb01 0143 	add.w	r1, r1, r3, lsl #1
87805a26:	1a52      	subs	r2, r2, r1
87805a28:	3230      	adds	r2, #48	; 0x30
87805a2a:	f88c 2003 	strb.w	r2, [ip, #3]
87805a2e:	b123      	cbz	r3, 87805a3a <number+0x426>
87805a30:	f10c 0505 	add.w	r5, ip, #5
87805a34:	3330      	adds	r3, #48	; 0x30
87805a36:	f88c 3004 	strb.w	r3, [ip, #4]
87805a3a:	9b03      	ldr	r3, [sp, #12]
87805a3c:	1aed      	subs	r5, r5, r3
87805a3e:	e6d4      	b.n	878057ea <number+0x1d6>
87805a40:	4670      	mov	r0, lr
87805a42:	e644      	b.n	878056ce <number+0xba>
87805a44:	4638      	mov	r0, r7
87805a46:	e66e      	b.n	87805726 <number+0x112>
87805a48:	ab0f      	add	r3, sp, #60	; 0x3c
87805a4a:	9303      	str	r3, [sp, #12]
87805a4c:	469c      	mov	ip, r3
87805a4e:	e794      	b.n	8780597a <number+0x366>

87805a50 <simple_strtoul>:
87805a50:	b5f0      	push	{r4, r5, r6, r7, lr}
87805a52:	4606      	mov	r6, r0
87805a54:	b96a      	cbnz	r2, 87805a72 <simple_strtoul+0x22>
87805a56:	7804      	ldrb	r4, [r0, #0]
87805a58:	2c30      	cmp	r4, #48	; 0x30
87805a5a:	d043      	beq.n	87805ae4 <simple_strtoul+0x94>
87805a5c:	f64d 6790 	movw	r7, #56976	; 0xde90
87805a60:	4623      	mov	r3, r4
87805a62:	f2c8 7780 	movt	r7, #34688	; 0x8780
87805a66:	220a      	movs	r2, #10
87805a68:	f817 e004 	ldrb.w	lr, [r7, r4]
87805a6c:	f00e 0544 	and.w	r5, lr, #68	; 0x44
87805a70:	e00b      	b.n	87805a8a <simple_strtoul+0x3a>
87805a72:	2a10      	cmp	r2, #16
87805a74:	d029      	beq.n	87805aca <simple_strtoul+0x7a>
87805a76:	7803      	ldrb	r3, [r0, #0]
87805a78:	f64d 6790 	movw	r7, #56976	; 0xde90
87805a7c:	f2c8 7780 	movt	r7, #34688	; 0x8780
87805a80:	f817 e003 	ldrb.w	lr, [r7, r3]
87805a84:	461c      	mov	r4, r3
87805a86:	f00e 0544 	and.w	r5, lr, #68	; 0x44
87805a8a:	4628      	mov	r0, r5
87805a8c:	b1d5      	cbz	r5, 87805ac4 <simple_strtoul+0x74>
87805a8e:	2000      	movs	r0, #0
87805a90:	e009      	b.n	87805aa6 <simple_strtoul+0x56>
87805a92:	f816 3f01 	ldrb.w	r3, [r6, #1]!
87805a96:	fb02 4000 	mla	r0, r2, r0, r4
87805a9a:	f817 e003 	ldrb.w	lr, [r7, r3]
87805a9e:	461c      	mov	r4, r3
87805aa0:	f01e 0f44 	tst.w	lr, #68	; 0x44
87805aa4:	d00e      	beq.n	87805ac4 <simple_strtoul+0x74>
87805aa6:	f01e 0f04 	tst.w	lr, #4
87805aaa:	f1a4 0430 	sub.w	r4, r4, #48	; 0x30
87805aae:	d107      	bne.n	87805ac0 <simple_strtoul+0x70>
87805ab0:	f1a3 0420 	sub.w	r4, r3, #32
87805ab4:	f01e 0f02 	tst.w	lr, #2
87805ab8:	bf18      	it	ne
87805aba:	b2e3      	uxtbne	r3, r4
87805abc:	f1a3 0437 	sub.w	r4, r3, #55	; 0x37
87805ac0:	4294      	cmp	r4, r2
87805ac2:	d3e6      	bcc.n	87805a92 <simple_strtoul+0x42>
87805ac4:	b101      	cbz	r1, 87805ac8 <simple_strtoul+0x78>
87805ac6:	600e      	str	r6, [r1, #0]
87805ac8:	bdf0      	pop	{r4, r5, r6, r7, pc}
87805aca:	7804      	ldrb	r4, [r0, #0]
87805acc:	2c30      	cmp	r4, #48	; 0x30
87805ace:	d020      	beq.n	87805b12 <simple_strtoul+0xc2>
87805ad0:	f64d 6790 	movw	r7, #56976	; 0xde90
87805ad4:	4623      	mov	r3, r4
87805ad6:	f2c8 7780 	movt	r7, #34688	; 0x8780
87805ada:	f817 e004 	ldrb.w	lr, [r7, r4]
87805ade:	f00e 0544 	and.w	r5, lr, #68	; 0x44
87805ae2:	e7d2      	b.n	87805a8a <simple_strtoul+0x3a>
87805ae4:	7843      	ldrb	r3, [r0, #1]
87805ae6:	f64d 6790 	movw	r7, #56976	; 0xde90
87805aea:	f2c8 7780 	movt	r7, #34688	; 0x8780
87805aee:	3001      	adds	r0, #1
87805af0:	f817 e003 	ldrb.w	lr, [r7, r3]
87805af4:	461c      	mov	r4, r3
87805af6:	f01e 0f02 	tst.w	lr, #2
87805afa:	bf16      	itet	ne
87805afc:	f1a3 0220 	subne.w	r2, r3, #32
87805b00:	461a      	moveq	r2, r3
87805b02:	b2d2      	uxtbne	r2, r2
87805b04:	2a58      	cmp	r2, #88	; 0x58
87805b06:	d01d      	beq.n	87805b44 <simple_strtoul+0xf4>
87805b08:	4606      	mov	r6, r0
87805b0a:	f00e 0544 	and.w	r5, lr, #68	; 0x44
87805b0e:	2208      	movs	r2, #8
87805b10:	e7bb      	b.n	87805a8a <simple_strtoul+0x3a>
87805b12:	7843      	ldrb	r3, [r0, #1]
87805b14:	f64d 6790 	movw	r7, #56976	; 0xde90
87805b18:	f2c8 7780 	movt	r7, #34688	; 0x8780
87805b1c:	5cf8      	ldrb	r0, [r7, r3]
87805b1e:	0780      	lsls	r0, r0, #30
87805b20:	bf44      	itt	mi
87805b22:	3b20      	submi	r3, #32
87805b24:	b2db      	uxtbmi	r3, r3
87805b26:	2b58      	cmp	r3, #88	; 0x58
87805b28:	bf0f      	iteee	eq
87805b2a:	78b3      	ldrbeq	r3, [r6, #2]
87805b2c:	2430      	movne	r4, #48	; 0x30
87805b2e:	f897 e030 	ldrbne.w	lr, [r7, #48]	; 0x30
87805b32:	4623      	movne	r3, r4
87805b34:	bf02      	ittt	eq
87805b36:	3602      	addeq	r6, #2
87805b38:	f817 e003 	ldrbeq.w	lr, [r7, r3]
87805b3c:	461c      	moveq	r4, r3
87805b3e:	f00e 0544 	and.w	r5, lr, #68	; 0x44
87805b42:	e7a2      	b.n	87805a8a <simple_strtoul+0x3a>
87805b44:	78b5      	ldrb	r5, [r6, #2]
87805b46:	5d7a      	ldrb	r2, [r7, r5]
87805b48:	f012 0f44 	tst.w	r2, #68	; 0x44
87805b4c:	d0dc      	beq.n	87805b08 <simple_strtoul+0xb8>
87805b4e:	4696      	mov	lr, r2
87805b50:	462c      	mov	r4, r5
87805b52:	3602      	adds	r6, #2
87805b54:	462b      	mov	r3, r5
87805b56:	2210      	movs	r2, #16
87805b58:	e799      	b.n	87805a8e <simple_strtoul+0x3e>
87805b5a:	bf00      	nop

87805b5c <simple_strtol>:
87805b5c:	b510      	push	{r4, lr}
87805b5e:	7804      	ldrb	r4, [r0, #0]
87805b60:	2c2d      	cmp	r4, #45	; 0x2d
87805b62:	d003      	beq.n	87805b6c <simple_strtol+0x10>
87805b64:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
87805b68:	f7ff bf72 	b.w	87805a50 <simple_strtoul>
87805b6c:	3001      	adds	r0, #1
87805b6e:	f7ff ff6f 	bl	87805a50 <simple_strtoul>
87805b72:	4240      	negs	r0, r0
87805b74:	bd10      	pop	{r4, pc}
87805b76:	bf00      	nop

87805b78 <simple_strtoull>:
87805b78:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87805b7c:	4684      	mov	ip, r0
87805b7e:	468a      	mov	sl, r1
87805b80:	4616      	mov	r6, r2
87805b82:	2a00      	cmp	r2, #0
87805b84:	d13f      	bne.n	87805c06 <simple_strtoull+0x8e>
87805b86:	7802      	ldrb	r2, [r0, #0]
87805b88:	2a30      	cmp	r2, #48	; 0x30
87805b8a:	d056      	beq.n	87805c3a <simple_strtoull+0xc2>
87805b8c:	f64d 6b90 	movw	fp, #56976	; 0xde90
87805b90:	4613      	mov	r3, r2
87805b92:	f2c8 7b80 	movt	fp, #34688	; 0x8780
87805b96:	260a      	movs	r6, #10
87805b98:	f81b e002 	ldrb.w	lr, [fp, r2]
87805b9c:	f00e 0144 	and.w	r1, lr, #68	; 0x44
87805ba0:	2900      	cmp	r1, #0
87805ba2:	d061      	beq.n	87805c68 <simple_strtoull+0xf0>
87805ba4:	2700      	movs	r7, #0
87805ba6:	2000      	movs	r0, #0
87805ba8:	2100      	movs	r1, #0
87805baa:	e00e      	b.n	87805bca <simple_strtoull+0x52>
87805bac:	f81c 3f01 	ldrb.w	r3, [ip, #1]!
87805bb0:	fb06 2101 	mla	r1, r6, r1, r2
87805bb4:	eb14 0008 	adds.w	r0, r4, r8
87805bb8:	f81b e003 	ldrb.w	lr, [fp, r3]
87805bbc:	461a      	mov	r2, r3
87805bbe:	4489      	add	r9, r1
87805bc0:	eb45 0109 	adc.w	r1, r5, r9
87805bc4:	f01e 0f44 	tst.w	lr, #68	; 0x44
87805bc8:	d016      	beq.n	87805bf8 <simple_strtoull+0x80>
87805bca:	f01e 0f04 	tst.w	lr, #4
87805bce:	f1a2 0430 	sub.w	r4, r2, #48	; 0x30
87805bd2:	d108      	bne.n	87805be6 <simple_strtoull+0x6e>
87805bd4:	3b20      	subs	r3, #32
87805bd6:	f01e 0f02 	tst.w	lr, #2
87805bda:	b2db      	uxtb	r3, r3
87805bdc:	f1a2 0437 	sub.w	r4, r2, #55	; 0x37
87805be0:	bf18      	it	ne
87805be2:	f1a3 0437 	subne.w	r4, r3, #55	; 0x37
87805be6:	17e5      	asrs	r5, r4, #31
87805be8:	fb00 f207 	mul.w	r2, r0, r7
87805bec:	42bd      	cmp	r5, r7
87805bee:	fba0 8906 	umull	r8, r9, r0, r6
87805bf2:	bf08      	it	eq
87805bf4:	42b4      	cmpeq	r4, r6
87805bf6:	d3d9      	bcc.n	87805bac <simple_strtoull+0x34>
87805bf8:	f1ba 0f00 	cmp.w	sl, #0
87805bfc:	d001      	beq.n	87805c02 <simple_strtoull+0x8a>
87805bfe:	f8ca c000 	str.w	ip, [sl]
87805c02:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87805c06:	2a10      	cmp	r2, #16
87805c08:	d00a      	beq.n	87805c20 <simple_strtoull+0xa8>
87805c0a:	7803      	ldrb	r3, [r0, #0]
87805c0c:	f64d 6b90 	movw	fp, #56976	; 0xde90
87805c10:	f2c8 7b80 	movt	fp, #34688	; 0x8780
87805c14:	f81b e003 	ldrb.w	lr, [fp, r3]
87805c18:	461a      	mov	r2, r3
87805c1a:	f00e 0144 	and.w	r1, lr, #68	; 0x44
87805c1e:	e7bf      	b.n	87805ba0 <simple_strtoull+0x28>
87805c20:	7802      	ldrb	r2, [r0, #0]
87805c22:	2a30      	cmp	r2, #48	; 0x30
87805c24:	d031      	beq.n	87805c8a <simple_strtoull+0x112>
87805c26:	f64d 6b90 	movw	fp, #56976	; 0xde90
87805c2a:	4613      	mov	r3, r2
87805c2c:	f2c8 7b80 	movt	fp, #34688	; 0x8780
87805c30:	f81b e002 	ldrb.w	lr, [fp, r2]
87805c34:	f00e 0144 	and.w	r1, lr, #68	; 0x44
87805c38:	e7b2      	b.n	87805ba0 <simple_strtoull+0x28>
87805c3a:	7843      	ldrb	r3, [r0, #1]
87805c3c:	f64d 6b90 	movw	fp, #56976	; 0xde90
87805c40:	f2c8 7b80 	movt	fp, #34688	; 0x8780
87805c44:	3001      	adds	r0, #1
87805c46:	f81b e003 	ldrb.w	lr, [fp, r3]
87805c4a:	461a      	mov	r2, r3
87805c4c:	f01e 0f02 	tst.w	lr, #2
87805c50:	bf16      	itet	ne
87805c52:	f1a3 0120 	subne.w	r1, r3, #32
87805c56:	4619      	moveq	r1, r3
87805c58:	b2c9      	uxtbne	r1, r1
87805c5a:	2958      	cmp	r1, #88	; 0x58
87805c5c:	d007      	beq.n	87805c6e <simple_strtoull+0xf6>
87805c5e:	4684      	mov	ip, r0
87805c60:	f00e 0144 	and.w	r1, lr, #68	; 0x44
87805c64:	2608      	movs	r6, #8
87805c66:	e79b      	b.n	87805ba0 <simple_strtoull+0x28>
87805c68:	2000      	movs	r0, #0
87805c6a:	2100      	movs	r1, #0
87805c6c:	e7c4      	b.n	87805bf8 <simple_strtoull+0x80>
87805c6e:	f89c 4002 	ldrb.w	r4, [ip, #2]
87805c72:	f81b 1004 	ldrb.w	r1, [fp, r4]
87805c76:	f011 0f44 	tst.w	r1, #68	; 0x44
87805c7a:	d0f0      	beq.n	87805c5e <simple_strtoull+0xe6>
87805c7c:	4622      	mov	r2, r4
87805c7e:	f10c 0c02 	add.w	ip, ip, #2
87805c82:	468e      	mov	lr, r1
87805c84:	4623      	mov	r3, r4
87805c86:	2610      	movs	r6, #16
87805c88:	e78c      	b.n	87805ba4 <simple_strtoull+0x2c>
87805c8a:	7843      	ldrb	r3, [r0, #1]
87805c8c:	f64d 6b90 	movw	fp, #56976	; 0xde90
87805c90:	f2c8 7b80 	movt	fp, #34688	; 0x8780
87805c94:	f81b 2003 	ldrb.w	r2, [fp, r3]
87805c98:	0792      	lsls	r2, r2, #30
87805c9a:	bf44      	itt	mi
87805c9c:	3b20      	submi	r3, #32
87805c9e:	b2db      	uxtbmi	r3, r3
87805ca0:	2b58      	cmp	r3, #88	; 0x58
87805ca2:	bf0f      	iteee	eq
87805ca4:	7883      	ldrbeq	r3, [r0, #2]
87805ca6:	2230      	movne	r2, #48	; 0x30
87805ca8:	f89b e030 	ldrbne.w	lr, [fp, #48]	; 0x30
87805cac:	4613      	movne	r3, r2
87805cae:	bf02      	ittt	eq
87805cb0:	f100 0c02 	addeq.w	ip, r0, #2
87805cb4:	f81b e003 	ldrbeq.w	lr, [fp, r3]
87805cb8:	461a      	moveq	r2, r3
87805cba:	f00e 0144 	and.w	r1, lr, #68	; 0x44
87805cbe:	e76f      	b.n	87805ba0 <simple_strtoull+0x28>

87805cc0 <simple_strtoll>:
87805cc0:	b510      	push	{r4, lr}
87805cc2:	7804      	ldrb	r4, [r0, #0]
87805cc4:	2c2d      	cmp	r4, #45	; 0x2d
87805cc6:	d003      	beq.n	87805cd0 <simple_strtoll+0x10>
87805cc8:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
87805ccc:	f7ff bf54 	b.w	87805b78 <simple_strtoull>
87805cd0:	3001      	adds	r0, #1
87805cd2:	f7ff ff51 	bl	87805b78 <simple_strtoull>
87805cd6:	4240      	negs	r0, r0
87805cd8:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
87805cdc:	bd10      	pop	{r4, pc}
87805cde:	bf00      	nop

87805ce0 <vsnprintf>:
87805ce0:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87805ce4:	f1b1 0b00 	subs.w	fp, r1, #0
87805ce8:	b089      	sub	sp, #36	; 0x24
87805cea:	f2c0 82f6 	blt.w	878062da <vsnprintf+0x5fa>
87805cee:	f10b 38ff 	add.w	r8, fp, #4294967295	; 0xffffffff
87805cf2:	469a      	mov	sl, r3
87805cf4:	4480      	add	r8, r0
87805cf6:	1e43      	subs	r3, r0, #1
87805cf8:	4598      	cmp	r8, r3
87805cfa:	4605      	mov	r5, r0
87805cfc:	4614      	mov	r4, r2
87805cfe:	7810      	ldrb	r0, [r2, #0]
87805d00:	f0c0 81c7 	bcc.w	87806092 <vsnprintf+0x3b2>
87805d04:	2800      	cmp	r0, #0
87805d06:	f000 82f3 	beq.w	878062f0 <vsnprintf+0x610>
87805d0a:	9505      	str	r5, [sp, #20]
87805d0c:	f64d 6330 	movw	r3, #56880	; 0xde30
87805d10:	f64d 6990 	movw	r9, #56976	; 0xde90
87805d14:	f2c8 7380 	movt	r3, #34688	; 0x8780
87805d18:	f2c8 7980 	movt	r9, #34688	; 0x8780
87805d1c:	9306      	str	r3, [sp, #24]
87805d1e:	e008      	b.n	87805d32 <vsnprintf+0x52>
87805d20:	45a8      	cmp	r8, r5
87805d22:	bf28      	it	cs
87805d24:	7028      	strbcs	r0, [r5, #0]
87805d26:	7860      	ldrb	r0, [r4, #1]
87805d28:	3501      	adds	r5, #1
87805d2a:	3401      	adds	r4, #1
87805d2c:	2800      	cmp	r0, #0
87805d2e:	f000 8140 	beq.w	87805fb2 <vsnprintf+0x2d2>
87805d32:	2825      	cmp	r0, #37	; 0x25
87805d34:	bf08      	it	eq
87805d36:	2700      	moveq	r7, #0
87805d38:	d1f2      	bne.n	87805d20 <vsnprintf+0x40>
87805d3a:	7861      	ldrb	r1, [r4, #1]
87805d3c:	1c62      	adds	r2, r4, #1
87805d3e:	f1a1 0320 	sub.w	r3, r1, #32
87805d42:	2b10      	cmp	r3, #16
87805d44:	d81e      	bhi.n	87805d84 <vsnprintf+0xa4>
87805d46:	e8df f003 	tbb	[pc, r3]
87805d4a:	151d1d19 	ldrne	r1, [sp, #-3353]	; 0xd19
87805d4e:	1d1d1d1d 	ldcne	13, cr1, [sp, #-116]	; 0xffffff8c
87805d52:	111d1d1d 	tstne	sp, sp, lsl sp
87805d56:	1d1d0d1d 	ldcne	13, cr0, [sp, #-116]	; 0xffffff8c
87805d5a:	f0470009 			; <UNDEFINED> instruction: 0xf0470009
87805d5e:	0701      	lsls	r1, r0, #28
87805d60:	4614      	mov	r4, r2
87805d62:	e7ea      	b.n	87805d3a <vsnprintf+0x5a>
87805d64:	f047 0710 	orr.w	r7, r7, #16
87805d68:	4614      	mov	r4, r2
87805d6a:	e7e6      	b.n	87805d3a <vsnprintf+0x5a>
87805d6c:	f047 0704 	orr.w	r7, r7, #4
87805d70:	4614      	mov	r4, r2
87805d72:	e7e2      	b.n	87805d3a <vsnprintf+0x5a>
87805d74:	f047 0720 	orr.w	r7, r7, #32
87805d78:	4614      	mov	r4, r2
87805d7a:	e7de      	b.n	87805d3a <vsnprintf+0x5a>
87805d7c:	f047 0708 	orr.w	r7, r7, #8
87805d80:	4614      	mov	r4, r2
87805d82:	e7da      	b.n	87805d3a <vsnprintf+0x5a>
87805d84:	f819 3001 	ldrb.w	r3, [r9, r1]
87805d88:	4608      	mov	r0, r1
87805d8a:	075e      	lsls	r6, r3, #29
87805d8c:	f140 80dd 	bpl.w	87805f4a <vsnprintf+0x26a>
87805d90:	2600      	movs	r6, #0
87805d92:	00f3      	lsls	r3, r6, #3
87805d94:	eb03 0646 	add.w	r6, r3, r6, lsl #1
87805d98:	4406      	add	r6, r0
87805d9a:	f812 0f01 	ldrb.w	r0, [r2, #1]!
87805d9e:	3e30      	subs	r6, #48	; 0x30
87805da0:	f819 3000 	ldrb.w	r3, [r9, r0]
87805da4:	075c      	lsls	r4, r3, #29
87805da6:	d4f4      	bmi.n	87805d92 <vsnprintf+0xb2>
87805da8:	4603      	mov	r3, r0
87805daa:	2b2e      	cmp	r3, #46	; 0x2e
87805dac:	bf18      	it	ne
87805dae:	f04f 31ff 	movne.w	r1, #4294967295	; 0xffffffff
87805db2:	f000 80e2 	beq.w	87805f7a <vsnprintf+0x29a>
87805db6:	f003 00df 	and.w	r0, r3, #223	; 0xdf
87805dba:	285a      	cmp	r0, #90	; 0x5a
87805dbc:	bf18      	it	ne
87805dbe:	284c      	cmpne	r0, #76	; 0x4c
87805dc0:	bf0c      	ite	eq
87805dc2:	2001      	moveq	r0, #1
87805dc4:	2000      	movne	r0, #0
87805dc6:	2b68      	cmp	r3, #104	; 0x68
87805dc8:	bf08      	it	eq
87805dca:	f040 0001 	orreq.w	r0, r0, #1
87805dce:	2800      	cmp	r0, #0
87805dd0:	d15b      	bne.n	87805e8a <vsnprintf+0x1aa>
87805dd2:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
87805dd6:	3b25      	subs	r3, #37	; 0x25
87805dd8:	2b53      	cmp	r3, #83	; 0x53
87805dda:	f200 8234 	bhi.w	87806246 <vsnprintf+0x566>
87805dde:	e8df f013 	tbh	[pc, r3, lsl #1]
87805de2:	023201e7 	eorseq	r0, r2, #-1073741767	; 0xc0000039
87805de6:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805dea:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805dee:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805df2:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805df6:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805dfa:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805dfe:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e02:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e06:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e0a:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e0e:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e12:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e16:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e1a:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e1e:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e22:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e26:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e2a:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e2e:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e32:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e36:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e3a:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e3e:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e42:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e46:	016e0232 	cmneq	lr, r2, lsr r2
87805e4a:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e4e:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e52:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e56:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e5a:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e5e:	01b301b9 			; <UNDEFINED> instruction: 0x01b301b9
87805e62:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e66:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e6a:	023201b3 	eorseq	r0, r2, #-1073741780	; 0xc000002c
87805e6e:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e72:	025a0232 	subseq	r0, sl, #536870915	; 0x20000003
87805e76:	019701af 	orrseq	r0, r7, pc, lsr #3
87805e7a:	02320232 	eorseq	r0, r2, #536870915	; 0x20000003
87805e7e:	023201ed 	eorseq	r0, r2, #1073741883	; 0x4000003b
87805e82:	0232026c 	eorseq	r0, r2, #108, 4	; 0xc0000006
87805e86:	026a0232 	rsbeq	r0, sl, #536870915	; 0x20000003
87805e8a:	4618      	mov	r0, r3
87805e8c:	1c54      	adds	r4, r2, #1
87805e8e:	286c      	cmp	r0, #108	; 0x6c
87805e90:	7853      	ldrb	r3, [r2, #1]
87805e92:	f000 809c 	beq.w	87805fce <vsnprintf+0x2ee>
87805e96:	3b25      	subs	r3, #37	; 0x25
87805e98:	2b53      	cmp	r3, #83	; 0x53
87805e9a:	f200 81d5 	bhi.w	87806248 <vsnprintf+0x568>
87805e9e:	e8df f013 	tbh	[pc, r3, lsl #1]
87805ea2:	01d30188 	bicseq	r0, r3, r8, lsl #3
87805ea6:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805eaa:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805eae:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805eb2:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805eb6:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805eba:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ebe:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ec2:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ec6:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805eca:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ece:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ed2:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ed6:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805eda:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ede:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ee2:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ee6:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805eea:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805eee:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ef2:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805ef6:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805efa:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805efe:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f02:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f06:	010f01d3 	ldrdeq	r0, [pc, -r3]
87805f0a:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f0e:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f12:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f16:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f1a:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f1e:	0154015a 	cmpeq	r4, sl, asr r1
87805f22:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f26:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f2a:	01d30154 	bicseq	r0, r3, r4, asr r1
87805f2e:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f32:	01fb01d3 	ldrsbeq	r0, [fp, #19]!
87805f36:	01380150 	teqeq	r8, r0, asr r1
87805f3a:	01d301d3 	ldrsbeq	r0, [r3, #19]
87805f3e:	01d3018e 	bicseq	r0, r3, lr, lsl #3
87805f42:	01d3012e 	bicseq	r0, r3, lr, lsr #2
87805f46:	011101d3 			; <UNDEFINED> instruction: 0x011101d3
87805f4a:	292a      	cmp	r1, #42	; 0x2a
87805f4c:	bf1c      	itt	ne
87805f4e:	460b      	movne	r3, r1
87805f50:	f04f 36ff 	movne.w	r6, #4294967295	; 0xffffffff
87805f54:	f47f af29 	bne.w	87805daa <vsnprintf+0xca>
87805f58:	f8da 6000 	ldr.w	r6, [sl]
87805f5c:	1ca2      	adds	r2, r4, #2
87805f5e:	78a3      	ldrb	r3, [r4, #2]
87805f60:	f10a 0a04 	add.w	sl, sl, #4
87805f64:	2e00      	cmp	r6, #0
87805f66:	bfbc      	itt	lt
87805f68:	4276      	neglt	r6, r6
87805f6a:	f047 0710 	orrlt.w	r7, r7, #16
87805f6e:	2b2e      	cmp	r3, #46	; 0x2e
87805f70:	bf18      	it	ne
87805f72:	f04f 31ff 	movne.w	r1, #4294967295	; 0xffffffff
87805f76:	f47f af1e 	bne.w	87805db6 <vsnprintf+0xd6>
87805f7a:	7853      	ldrb	r3, [r2, #1]
87805f7c:	1c54      	adds	r4, r2, #1
87805f7e:	f819 0003 	ldrb.w	r0, [r9, r3]
87805f82:	f000 0004 	and.w	r0, r0, #4
87805f86:	f000 01ff 	and.w	r1, r0, #255	; 0xff
87805f8a:	2800      	cmp	r0, #0
87805f8c:	f000 808a 	beq.w	878060a4 <vsnprintf+0x3c4>
87805f90:	2000      	movs	r0, #0
87805f92:	4622      	mov	r2, r4
87805f94:	4601      	mov	r1, r0
87805f96:	00cc      	lsls	r4, r1, #3
87805f98:	eb04 0141 	add.w	r1, r4, r1, lsl #1
87805f9c:	4419      	add	r1, r3
87805f9e:	f812 3f01 	ldrb.w	r3, [r2, #1]!
87805fa2:	3930      	subs	r1, #48	; 0x30
87805fa4:	f819 0003 	ldrb.w	r0, [r9, r3]
87805fa8:	0740      	lsls	r0, r0, #29
87805faa:	d4f4      	bmi.n	87805f96 <vsnprintf+0x2b6>
87805fac:	ea21 71e1 	bic.w	r1, r1, r1, asr #31
87805fb0:	e701      	b.n	87805db6 <vsnprintf+0xd6>
87805fb2:	462b      	mov	r3, r5
87805fb4:	9a05      	ldr	r2, [sp, #20]
87805fb6:	1aa8      	subs	r0, r5, r2
87805fb8:	4543      	cmp	r3, r8
87805fba:	d965      	bls.n	87806088 <vsnprintf+0x3a8>
87805fbc:	f1bb 0f00 	cmp.w	fp, #0
87805fc0:	d002      	beq.n	87805fc8 <vsnprintf+0x2e8>
87805fc2:	2300      	movs	r3, #0
87805fc4:	f888 3000 	strb.w	r3, [r8]
87805fc8:	b009      	add	sp, #36	; 0x24
87805fca:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87805fce:	2b6c      	cmp	r3, #108	; 0x6c
87805fd0:	f000 816b 	beq.w	878062aa <vsnprintf+0x5ca>
87805fd4:	3b25      	subs	r3, #37	; 0x25
87805fd6:	2b53      	cmp	r3, #83	; 0x53
87805fd8:	f200 8136 	bhi.w	87806248 <vsnprintf+0x568>
87805fdc:	e8df f013 	tbh	[pc, r3, lsl #1]
87805fe0:	013400e9 	teqeq	r4, r9, ror #1
87805fe4:	01340134 	teqeq	r4, r4, lsr r1
87805fe8:	01340134 	teqeq	r4, r4, lsr r1
87805fec:	01340134 	teqeq	r4, r4, lsr r1
87805ff0:	01340134 	teqeq	r4, r4, lsr r1
87805ff4:	01340134 	teqeq	r4, r4, lsr r1
87805ff8:	01340134 	teqeq	r4, r4, lsr r1
87805ffc:	01340134 	teqeq	r4, r4, lsr r1
87806000:	01340134 	teqeq	r4, r4, lsr r1
87806004:	01340134 	teqeq	r4, r4, lsr r1
87806008:	01340134 	teqeq	r4, r4, lsr r1
8780600c:	01340134 	teqeq	r4, r4, lsr r1
87806010:	01340134 	teqeq	r4, r4, lsr r1
87806014:	01340134 	teqeq	r4, r4, lsr r1
87806018:	01340134 	teqeq	r4, r4, lsr r1
8780601c:	01340134 	teqeq	r4, r4, lsr r1
87806020:	01340134 	teqeq	r4, r4, lsr r1
87806024:	01340134 	teqeq	r4, r4, lsr r1
87806028:	01340134 	teqeq	r4, r4, lsr r1
8780602c:	01340134 	teqeq	r4, r4, lsr r1
87806030:	01340134 	teqeq	r4, r4, lsr r1
87806034:	01340134 	teqeq	r4, r4, lsr r1
87806038:	01340134 	teqeq	r4, r4, lsr r1
8780603c:	01340134 	teqeq	r4, r4, lsr r1
87806040:	01340134 	teqeq	r4, r4, lsr r1
87806044:	00700134 	rsbseq	r0, r0, r4, lsr r1
87806048:	01340134 	teqeq	r4, r4, lsr r1
8780604c:	01340134 	teqeq	r4, r4, lsr r1
87806050:	01340134 	teqeq	r4, r4, lsr r1
87806054:	01340134 	teqeq	r4, r4, lsr r1
87806058:	01340134 	teqeq	r4, r4, lsr r1
8780605c:	00b500bb 	ldrhteq	r0, [r5], fp
87806060:	01340134 	teqeq	r4, r4, lsr r1
87806064:	01340134 	teqeq	r4, r4, lsr r1
87806068:	013400b5 	ldrheq	r0, [r4, -r5]!
8780606c:	01340134 	teqeq	r4, r4, lsr r1
87806070:	01740134 	cmneq	r4, r4, lsr r1
87806074:	009900b1 	ldrheq	r0, [r9], r1
87806078:	01340134 	teqeq	r4, r4, lsr r1
8780607c:	013400ef 	teqeq	r4, pc, ror #1
87806080:	0134014f 	teqeq	r4, pc, asr #2
87806084:	00720134 	rsbseq	r0, r2, r4, lsr r1
87806088:	2200      	movs	r2, #0
8780608a:	701a      	strb	r2, [r3, #0]
8780608c:	b009      	add	sp, #36	; 0x24
8780608e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87806092:	9505      	str	r5, [sp, #20]
87806094:	f1c5 0b00 	rsb	fp, r5, #0
87806098:	462b      	mov	r3, r5
8780609a:	2800      	cmp	r0, #0
8780609c:	d0f4      	beq.n	87806088 <vsnprintf+0x3a8>
8780609e:	f04f 38ff 	mov.w	r8, #4294967295	; 0xffffffff
878060a2:	e633      	b.n	87805d0c <vsnprintf+0x2c>
878060a4:	2b2a      	cmp	r3, #42	; 0x2a
878060a6:	bf09      	itett	eq
878060a8:	f8da 1000 	ldreq.w	r1, [sl]
878060ac:	4622      	movne	r2, r4
878060ae:	7893      	ldrbeq	r3, [r2, #2]
878060b0:	f10a 0a04 	addeq.w	sl, sl, #4
878060b4:	bf04      	itt	eq
878060b6:	3202      	addeq	r2, #2
878060b8:	ea21 71e1 	biceq.w	r1, r1, r1, asr #31
878060bc:	e67b      	b.n	87805db6 <vsnprintf+0xd6>
878060be:	4614      	mov	r4, r2
878060c0:	f047 0740 	orr.w	r7, r7, #64	; 0x40
878060c4:	f04f 0e10 	mov.w	lr, #16
878060c8:	284c      	cmp	r0, #76	; 0x4c
878060ca:	d01c      	beq.n	87806106 <vsnprintf+0x426>
878060cc:	286c      	cmp	r0, #108	; 0x6c
878060ce:	f000 80d8 	beq.w	87806282 <vsnprintf+0x5a2>
878060d2:	f020 0320 	bic.w	r3, r0, #32
878060d6:	2b5a      	cmp	r3, #90	; 0x5a
878060d8:	f040 80c5 	bne.w	87806266 <vsnprintf+0x586>
878060dc:	f8da 2000 	ldr.w	r2, [sl]
878060e0:	f10a 0a04 	add.w	sl, sl, #4
878060e4:	2300      	movs	r3, #0
878060e6:	9102      	str	r1, [sp, #8]
878060e8:	4628      	mov	r0, r5
878060ea:	9703      	str	r7, [sp, #12]
878060ec:	4641      	mov	r1, r8
878060ee:	9601      	str	r6, [sp, #4]
878060f0:	f8cd e000 	str.w	lr, [sp]
878060f4:	f7ff fa8e 	bl	87805614 <number>
878060f8:	4605      	mov	r5, r0
878060fa:	7860      	ldrb	r0, [r4, #1]
878060fc:	e615      	b.n	87805d2a <vsnprintf+0x4a>
878060fe:	284c      	cmp	r0, #76	; 0x4c
87806100:	f04f 0e0a 	mov.w	lr, #10
87806104:	d1e2      	bne.n	878060cc <vsnprintf+0x3ec>
87806106:	e9da 2300 	ldrd	r2, r3, [sl]
8780610a:	f10a 0a08 	add.w	sl, sl, #8
8780610e:	e7ea      	b.n	878060e6 <vsnprintf+0x406>
87806110:	4614      	mov	r4, r2
87806112:	1c72      	adds	r2, r6, #1
87806114:	f04f 0310 	mov.w	r3, #16
87806118:	bf04      	itt	eq
8780611a:	f047 0701 	orreq.w	r7, r7, #1
8780611e:	2608      	moveq	r6, #8
87806120:	f8da 2000 	ldr.w	r2, [sl]
87806124:	4628      	mov	r0, r5
87806126:	9102      	str	r1, [sp, #8]
87806128:	4641      	mov	r1, r8
8780612a:	e88d 0048 	stmia.w	sp, {r3, r6}
8780612e:	2300      	movs	r3, #0
87806130:	9703      	str	r7, [sp, #12]
87806132:	f10a 0a04 	add.w	sl, sl, #4
87806136:	f7ff fa6d 	bl	87805614 <number>
8780613a:	4605      	mov	r5, r0
8780613c:	7860      	ldrb	r0, [r4, #1]
8780613e:	e5f4      	b.n	87805d2a <vsnprintf+0x4a>
87806140:	4614      	mov	r4, r2
87806142:	f04f 0e08 	mov.w	lr, #8
87806146:	e7bf      	b.n	878060c8 <vsnprintf+0x3e8>
87806148:	4614      	mov	r4, r2
8780614a:	f047 0702 	orr.w	r7, r7, #2
8780614e:	f04f 0e0a 	mov.w	lr, #10
87806152:	e7b9      	b.n	878060c8 <vsnprintf+0x3e8>
87806154:	4614      	mov	r4, r2
87806156:	06f9      	lsls	r1, r7, #27
87806158:	d40d      	bmi.n	87806176 <vsnprintf+0x496>
8780615a:	3e01      	subs	r6, #1
8780615c:	2e00      	cmp	r6, #0
8780615e:	dd0a      	ble.n	87806176 <vsnprintf+0x496>
87806160:	19ab      	adds	r3, r5, r6
87806162:	2220      	movs	r2, #32
87806164:	45a8      	cmp	r8, r5
87806166:	f105 0501 	add.w	r5, r5, #1
8780616a:	bf28      	it	cs
8780616c:	f805 2c01 	strbcs.w	r2, [r5, #-1]
87806170:	429d      	cmp	r5, r3
87806172:	d1f7      	bne.n	87806164 <vsnprintf+0x484>
87806174:	2600      	movs	r6, #0
87806176:	f8da 3000 	ldr.w	r3, [sl]
8780617a:	4545      	cmp	r5, r8
8780617c:	f105 0101 	add.w	r1, r5, #1
87806180:	f10a 0004 	add.w	r0, sl, #4
87806184:	bf98      	it	ls
87806186:	702b      	strbls	r3, [r5, #0]
87806188:	2e01      	cmp	r6, #1
8780618a:	bfc2      	ittt	gt
8780618c:	19ad      	addgt	r5, r5, r6
8780618e:	460b      	movgt	r3, r1
87806190:	2220      	movgt	r2, #32
87806192:	f340 80b1 	ble.w	878062f8 <vsnprintf+0x618>
87806196:	4598      	cmp	r8, r3
87806198:	f103 0301 	add.w	r3, r3, #1
8780619c:	bf28      	it	cs
8780619e:	f803 2c01 	strbcs.w	r2, [r3, #-1]
878061a2:	42ab      	cmp	r3, r5
878061a4:	d1f7      	bne.n	87806196 <vsnprintf+0x4b6>
878061a6:	1e75      	subs	r5, r6, #1
878061a8:	4682      	mov	sl, r0
878061aa:	440d      	add	r5, r1
878061ac:	7860      	ldrb	r0, [r4, #1]
878061ae:	e5bc      	b.n	87805d2a <vsnprintf+0x4a>
878061b0:	4614      	mov	r4, r2
878061b2:	45a8      	cmp	r8, r5
878061b4:	bf24      	itt	cs
878061b6:	2325      	movcs	r3, #37	; 0x25
878061b8:	702b      	strbcs	r3, [r5, #0]
878061ba:	e5b4      	b.n	87805d26 <vsnprintf+0x46>
878061bc:	4614      	mov	r4, r2
878061be:	f8da 3000 	ldr.w	r3, [sl]
878061c2:	f10a 0a04 	add.w	sl, sl, #4
878061c6:	9a06      	ldr	r2, [sp, #24]
878061c8:	f5b3 6f80 	cmp.w	r3, #1024	; 0x400
878061cc:	bf38      	it	cc
878061ce:	4613      	movcc	r3, r2
878061d0:	4618      	mov	r0, r3
878061d2:	9307      	str	r3, [sp, #28]
878061d4:	f7ff f85a 	bl	8780528c <strnlen>
878061d8:	f017 0f10 	tst.w	r7, #16
878061dc:	9b07      	ldr	r3, [sp, #28]
878061de:	d111      	bne.n	87806204 <vsnprintf+0x524>
878061e0:	4286      	cmp	r6, r0
878061e2:	f106 37ff 	add.w	r7, r6, #4294967295	; 0xffffffff
878061e6:	f340 808b 	ble.w	87806300 <vsnprintf+0x620>
878061ea:	1a32      	subs	r2, r6, r0
878061ec:	2120      	movs	r1, #32
878061ee:	442a      	add	r2, r5
878061f0:	45a8      	cmp	r8, r5
878061f2:	f105 0501 	add.w	r5, r5, #1
878061f6:	bf28      	it	cs
878061f8:	f805 1c01 	strbcs.w	r1, [r5, #-1]
878061fc:	4295      	cmp	r5, r2
878061fe:	d1f7      	bne.n	878061f0 <vsnprintf+0x510>
87806200:	1bbe      	subs	r6, r7, r6
87806202:	4406      	add	r6, r0
87806204:	2800      	cmp	r0, #0
87806206:	dd75      	ble.n	878062f4 <vsnprintf+0x614>
87806208:	182a      	adds	r2, r5, r0
8780620a:	45a8      	cmp	r8, r5
8780620c:	f105 0501 	add.w	r5, r5, #1
87806210:	bf28      	it	cs
87806212:	7819      	ldrbcs	r1, [r3, #0]
87806214:	f103 0301 	add.w	r3, r3, #1
87806218:	bf28      	it	cs
8780621a:	f805 1c01 	strbcs.w	r1, [r5, #-1]
8780621e:	4295      	cmp	r5, r2
87806220:	d1f3      	bne.n	8780620a <vsnprintf+0x52a>
87806222:	4286      	cmp	r6, r0
87806224:	bfdc      	itt	le
87806226:	4615      	movle	r5, r2
87806228:	7860      	ldrble	r0, [r4, #1]
8780622a:	f77f ad7e 	ble.w	87805d2a <vsnprintf+0x4a>
8780622e:	1a30      	subs	r0, r6, r0
87806230:	2320      	movs	r3, #32
87806232:	1815      	adds	r5, r2, r0
87806234:	4590      	cmp	r8, r2
87806236:	f102 0201 	add.w	r2, r2, #1
8780623a:	bf28      	it	cs
8780623c:	f802 3c01 	strbcs.w	r3, [r2, #-1]
87806240:	42aa      	cmp	r2, r5
87806242:	d1f7      	bne.n	87806234 <vsnprintf+0x554>
87806244:	e759      	b.n	878060fa <vsnprintf+0x41a>
87806246:	4614      	mov	r4, r2
87806248:	45a8      	cmp	r8, r5
8780624a:	bf24      	itt	cs
8780624c:	2325      	movcs	r3, #37	; 0x25
8780624e:	702b      	strbcs	r3, [r5, #0]
87806250:	1c6b      	adds	r3, r5, #1
87806252:	7822      	ldrb	r2, [r4, #0]
87806254:	b36a      	cbz	r2, 878062b2 <vsnprintf+0x5d2>
87806256:	4543      	cmp	r3, r8
87806258:	f105 0502 	add.w	r5, r5, #2
8780625c:	bf98      	it	ls
8780625e:	f805 2c01 	strbls.w	r2, [r5, #-1]
87806262:	7860      	ldrb	r0, [r4, #1]
87806264:	e561      	b.n	87805d2a <vsnprintf+0x4a>
87806266:	2868      	cmp	r0, #104	; 0x68
87806268:	f10a 0304 	add.w	r3, sl, #4
8780626c:	f8da 2000 	ldr.w	r2, [sl]
87806270:	469a      	mov	sl, r3
87806272:	d036      	beq.n	878062e2 <vsnprintf+0x602>
87806274:	07bb      	lsls	r3, r7, #30
87806276:	f57f af35 	bpl.w	878060e4 <vsnprintf+0x404>
8780627a:	17d3      	asrs	r3, r2, #31
8780627c:	e733      	b.n	878060e6 <vsnprintf+0x406>
8780627e:	f04f 0e0a 	mov.w	lr, #10
87806282:	f10a 0304 	add.w	r3, sl, #4
87806286:	f8da 2000 	ldr.w	r2, [sl]
8780628a:	469a      	mov	sl, r3
8780628c:	07bb      	lsls	r3, r7, #30
8780628e:	f57f af29 	bpl.w	878060e4 <vsnprintf+0x404>
87806292:	17d3      	asrs	r3, r2, #31
87806294:	e727      	b.n	878060e6 <vsnprintf+0x406>
87806296:	4614      	mov	r4, r2
87806298:	9a05      	ldr	r2, [sp, #20]
8780629a:	f10a 0a04 	add.w	sl, sl, #4
8780629e:	f85a 3c04 	ldr.w	r3, [sl, #-4]
878062a2:	1aaa      	subs	r2, r5, r2
878062a4:	601a      	str	r2, [r3, #0]
878062a6:	7860      	ldrb	r0, [r4, #1]
878062a8:	e53f      	b.n	87805d2a <vsnprintf+0x4a>
878062aa:	7893      	ldrb	r3, [r2, #2]
878062ac:	204c      	movs	r0, #76	; 0x4c
878062ae:	3202      	adds	r2, #2
878062b0:	e591      	b.n	87805dd6 <vsnprintf+0xf6>
878062b2:	461d      	mov	r5, r3
878062b4:	e67e      	b.n	87805fb4 <vsnprintf+0x2d4>
878062b6:	4614      	mov	r4, r2
878062b8:	e704      	b.n	878060c4 <vsnprintf+0x3e4>
878062ba:	284c      	cmp	r0, #76	; 0x4c
878062bc:	4614      	mov	r4, r2
878062be:	f04f 0e0a 	mov.w	lr, #10
878062c2:	f47f af06 	bne.w	878060d2 <vsnprintf+0x3f2>
878062c6:	e71e      	b.n	87806106 <vsnprintf+0x426>
878062c8:	9905      	ldr	r1, [sp, #20]
878062ca:	f10a 0a04 	add.w	sl, sl, #4
878062ce:	f85a 3c04 	ldr.w	r3, [sl, #-4]
878062d2:	1a69      	subs	r1, r5, r1
878062d4:	6019      	str	r1, [r3, #0]
878062d6:	7890      	ldrb	r0, [r2, #2]
878062d8:	e527      	b.n	87805d2a <vsnprintf+0x4a>
878062da:	2000      	movs	r0, #0
878062dc:	b009      	add	sp, #36	; 0x24
878062de:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
878062e2:	07b8      	lsls	r0, r7, #30
878062e4:	bf4d      	iteet	mi
878062e6:	b212      	sxthmi	r2, r2
878062e8:	b292      	uxthpl	r2, r2
878062ea:	2300      	movpl	r3, #0
878062ec:	17d3      	asrmi	r3, r2, #31
878062ee:	e6fa      	b.n	878060e6 <vsnprintf+0x406>
878062f0:	462b      	mov	r3, r5
878062f2:	e661      	b.n	87805fb8 <vsnprintf+0x2d8>
878062f4:	462a      	mov	r2, r5
878062f6:	e794      	b.n	87806222 <vsnprintf+0x542>
878062f8:	4682      	mov	sl, r0
878062fa:	460d      	mov	r5, r1
878062fc:	7860      	ldrb	r0, [r4, #1]
878062fe:	e514      	b.n	87805d2a <vsnprintf+0x4a>
87806300:	463e      	mov	r6, r7
87806302:	e77f      	b.n	87806204 <vsnprintf+0x524>

87806304 <vscnprintf>:
87806304:	b510      	push	{r4, lr}
87806306:	460c      	mov	r4, r1
87806308:	f7ff fcea 	bl	87805ce0 <vsnprintf>
8780630c:	4284      	cmp	r4, r0
8780630e:	bf98      	it	ls
87806310:	f104 30ff 	addls.w	r0, r4, #4294967295	; 0xffffffff
87806314:	bd10      	pop	{r4, pc}
87806316:	bf00      	nop

87806318 <snprintf>:
87806318:	b40c      	push	{r2, r3}
8780631a:	b508      	push	{r3, lr}
8780631c:	ab03      	add	r3, sp, #12
8780631e:	9a02      	ldr	r2, [sp, #8]
87806320:	f7ff fcde 	bl	87805ce0 <vsnprintf>
87806324:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
87806328:	b002      	add	sp, #8
8780632a:	4770      	bx	lr

8780632c <scnprintf>:
8780632c:	b40c      	push	{r2, r3}
8780632e:	b510      	push	{r4, lr}
87806330:	460c      	mov	r4, r1
87806332:	ab03      	add	r3, sp, #12
87806334:	9a02      	ldr	r2, [sp, #8]
87806336:	f7ff fcd3 	bl	87805ce0 <vsnprintf>
8780633a:	4284      	cmp	r4, r0
8780633c:	bf98      	it	ls
8780633e:	f104 30ff 	addls.w	r0, r4, #4294967295	; 0xffffffff
87806342:	e8bd 4010 	ldmia.w	sp!, {r4, lr}
87806346:	b002      	add	sp, #8
87806348:	4770      	bx	lr
8780634a:	bf00      	nop

8780634c <vsprintf>:
8780634c:	4613      	mov	r3, r2
8780634e:	460a      	mov	r2, r1
87806350:	f06f 4100 	mvn.w	r1, #2147483648	; 0x80000000
87806354:	f7ff bcc4 	b.w	87805ce0 <vsnprintf>

87806358 <sprintf>:
87806358:	b40e      	push	{r1, r2, r3}
8780635a:	f06f 4100 	mvn.w	r1, #2147483648	; 0x80000000
8780635e:	b500      	push	{lr}
87806360:	ab02      	add	r3, sp, #8
87806362:	9a01      	ldr	r2, [sp, #4]
87806364:	f7ff fcbc 	bl	87805ce0 <vsnprintf>
87806368:	f85d eb04 	ldr.w	lr, [sp], #4
8780636c:	b003      	add	sp, #12
8780636e:	4770      	bx	lr

87806370 <vsscanf>:
87806370:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87806374:	f64d 6490 	movw	r4, #56976	; 0xde90
87806378:	780b      	ldrb	r3, [r1, #0]
8780637a:	f2c8 7480 	movt	r4, #34688	; 0x8780
8780637e:	b083      	sub	sp, #12
87806380:	4607      	mov	r7, r0
87806382:	460d      	mov	r5, r1
87806384:	4696      	mov	lr, r2
87806386:	46a0      	mov	r8, r4
87806388:	2600      	movs	r6, #0
8780638a:	b143      	cbz	r3, 8780639e <vsscanf+0x2e>
8780638c:	7801      	ldrb	r1, [r0, #0]
8780638e:	b131      	cbz	r1, 8780639e <vsscanf+0x2e>
87806390:	5ce2      	ldrb	r2, [r4, r3]
87806392:	0692      	lsls	r2, r2, #26
87806394:	d407      	bmi.n	878063a6 <vsscanf+0x36>
87806396:	2b25      	cmp	r3, #37	; 0x25
87806398:	d020      	beq.n	878063dc <vsscanf+0x6c>
8780639a:	428b      	cmp	r3, r1
8780639c:	d018      	beq.n	878063d0 <vsscanf+0x60>
8780639e:	4630      	mov	r0, r6
878063a0:	b003      	add	sp, #12
878063a2:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
878063a6:	f815 3f01 	ldrb.w	r3, [r5, #1]!
878063aa:	5ce2      	ldrb	r2, [r4, r3]
878063ac:	0692      	lsls	r2, r2, #26
878063ae:	d4fa      	bmi.n	878063a6 <vsscanf+0x36>
878063b0:	f818 2001 	ldrb.w	r2, [r8, r1]
878063b4:	0691      	lsls	r1, r2, #26
878063b6:	d504      	bpl.n	878063c2 <vsscanf+0x52>
878063b8:	f810 2f01 	ldrb.w	r2, [r0, #1]!
878063bc:	5ca2      	ldrb	r2, [r4, r2]
878063be:	0692      	lsls	r2, r2, #26
878063c0:	d4fa      	bmi.n	878063b8 <vsscanf+0x48>
878063c2:	2b00      	cmp	r3, #0
878063c4:	bf18      	it	ne
878063c6:	2b25      	cmpne	r3, #37	; 0x25
878063c8:	d006      	beq.n	878063d8 <vsscanf+0x68>
878063ca:	7801      	ldrb	r1, [r0, #0]
878063cc:	428b      	cmp	r3, r1
878063ce:	d1e6      	bne.n	8780639e <vsscanf+0x2e>
878063d0:	786b      	ldrb	r3, [r5, #1]
878063d2:	3001      	adds	r0, #1
878063d4:	3501      	adds	r5, #1
878063d6:	e7d8      	b.n	8780638a <vsscanf+0x1a>
878063d8:	2b00      	cmp	r3, #0
878063da:	d0e0      	beq.n	8780639e <vsscanf+0x2e>
878063dc:	786b      	ldrb	r3, [r5, #1]
878063de:	3501      	adds	r5, #1
878063e0:	2b2a      	cmp	r3, #42	; 0x2a
878063e2:	f000 80f1 	beq.w	878065c8 <vsscanf+0x258>
878063e6:	5ce2      	ldrb	r2, [r4, r3]
878063e8:	4619      	mov	r1, r3
878063ea:	0752      	lsls	r2, r2, #29
878063ec:	bf5c      	itt	pl
878063ee:	4619      	movpl	r1, r3
878063f0:	f04f 33ff 	movpl.w	r3, #4294967295	; 0xffffffff
878063f4:	d50a      	bpl.n	8780640c <vsscanf+0x9c>
878063f6:	2300      	movs	r3, #0
878063f8:	00da      	lsls	r2, r3, #3
878063fa:	eb02 0343 	add.w	r3, r2, r3, lsl #1
878063fe:	440b      	add	r3, r1
87806400:	f815 1f01 	ldrb.w	r1, [r5, #1]!
87806404:	3b30      	subs	r3, #48	; 0x30
87806406:	5c62      	ldrb	r2, [r4, r1]
87806408:	0752      	lsls	r2, r2, #29
8780640a:	d4f5      	bmi.n	878063f8 <vsscanf+0x88>
8780640c:	f001 02df 	and.w	r2, r1, #223	; 0xdf
87806410:	46a9      	mov	r9, r5
87806412:	2a5a      	cmp	r2, #90	; 0x5a
87806414:	bf18      	it	ne
87806416:	2a4c      	cmpne	r2, #76	; 0x4c
87806418:	f04f 3cff 	mov.w	ip, #4294967295	; 0xffffffff
8780641c:	bf0c      	ite	eq
8780641e:	2201      	moveq	r2, #1
87806420:	2200      	movne	r2, #0
87806422:	2968      	cmp	r1, #104	; 0x68
87806424:	bf08      	it	eq
87806426:	f042 0201 	orreq.w	r2, r2, #1
8780642a:	2a00      	cmp	r2, #0
8780642c:	f040 80b8 	bne.w	878065a0 <vsscanf+0x230>
87806430:	2900      	cmp	r1, #0
87806432:	d0b4      	beq.n	8780639e <vsscanf+0x2e>
87806434:	f890 a000 	ldrb.w	sl, [r0]
87806438:	f1ba 0f00 	cmp.w	sl, #0
8780643c:	d0af      	beq.n	8780639e <vsscanf+0x2e>
8780643e:	3925      	subs	r1, #37	; 0x25
87806440:	f109 0501 	add.w	r5, r9, #1
87806444:	2953      	cmp	r1, #83	; 0x53
87806446:	d8aa      	bhi.n	8780639e <vsscanf+0x2e>
87806448:	a201      	add	r2, pc, #4	; (adr r2, 87806450 <vsscanf+0xe0>)
8780644a:	f852 f021 	ldr.w	pc, [r2, r1, lsl #2]
8780644e:	bf00      	nop
87806450:	878066cd 	strhi	r6, [r0, sp, asr #13]
87806454:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806458:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780645c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806460:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806464:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806468:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780646c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806470:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806474:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806478:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780647c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806480:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806484:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806488:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780648c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806490:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806494:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806498:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780649c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064a0:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064a4:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064a8:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064ac:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064b0:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064b4:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064b8:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064bc:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064c0:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064c4:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064c8:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064cc:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064d0:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064d4:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064d8:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064dc:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064e0:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064e4:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064e8:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064ec:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064f0:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064f4:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064f8:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
878064fc:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806500:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806504:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806508:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780650c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806510:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806514:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806518:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780651c:	878066dd 			; <UNDEFINED> instruction: 0x878066dd
87806520:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806524:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806528:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780652c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806530:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806534:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806538:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780653c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806540:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806544:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806548:	878067ad 	strhi	r6, [r0, sp, lsr #15]
8780654c:	878067df 			; <UNDEFINED> instruction: 0x878067df
87806550:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806554:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806558:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780655c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806560:	87806603 	strhi	r6, [r0, r3, lsl #12]
87806564:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806568:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780656c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806570:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806574:	8780679b 			; <UNDEFINED> instruction: 0x8780679b
87806578:	87806711 	usada8hi	r0, r1, r7, r6
8780657c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806580:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806584:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806588:	8780673f 			; <UNDEFINED> instruction: 0x8780673f
8780658c:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806590:	878066f7 			; <UNDEFINED> instruction: 0x878066f7
87806594:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
87806598:	8780639f 			; <UNDEFINED> instruction: 0x8780639f
8780659c:	878066dd 			; <UNDEFINED> instruction: 0x878066dd
878065a0:	786a      	ldrb	r2, [r5, #1]
878065a2:	468c      	mov	ip, r1
878065a4:	f105 0901 	add.w	r9, r5, #1
878065a8:	4291      	cmp	r1, r2
878065aa:	bf18      	it	ne
878065ac:	4611      	movne	r1, r2
878065ae:	f47f af3f 	bne.w	87806430 <vsscanf+0xc0>
878065b2:	2968      	cmp	r1, #104	; 0x68
878065b4:	f000 8199 	beq.w	878068ea <vsscanf+0x57a>
878065b8:	296c      	cmp	r1, #108	; 0x6c
878065ba:	bf02      	ittt	eq
878065bc:	f105 0902 	addeq.w	r9, r5, #2
878065c0:	78a9      	ldrbeq	r1, [r5, #2]
878065c2:	f04f 0c4c 	moveq.w	ip, #76	; 0x4c
878065c6:	e733      	b.n	87806430 <vsscanf+0xc0>
878065c8:	f894 202a 	ldrb.w	r2, [r4, #42]	; 0x2a
878065cc:	0691      	lsls	r1, r2, #26
878065ce:	d40a      	bmi.n	878065e6 <vsscanf+0x276>
878065d0:	f815 3f01 	ldrb.w	r3, [r5, #1]!
878065d4:	5ce2      	ldrb	r2, [r4, r3]
878065d6:	1c19      	adds	r1, r3, #0
878065d8:	bf18      	it	ne
878065da:	2101      	movne	r1, #1
878065dc:	f082 0220 	eor.w	r2, r2, #32
878065e0:	ea11 1252 	ands.w	r2, r1, r2, lsr #5
878065e4:	d1f4      	bne.n	878065d0 <vsscanf+0x260>
878065e6:	7802      	ldrb	r2, [r0, #0]
878065e8:	e001      	b.n	878065ee <vsscanf+0x27e>
878065ea:	f810 2f01 	ldrb.w	r2, [r0, #1]!
878065ee:	5ca1      	ldrb	r1, [r4, r2]
878065f0:	3200      	adds	r2, #0
878065f2:	bf18      	it	ne
878065f4:	2201      	movne	r2, #1
878065f6:	f081 0120 	eor.w	r1, r1, #32
878065fa:	ea12 1251 	ands.w	r2, r2, r1, lsr #5
878065fe:	d1f4      	bne.n	878065ea <vsscanf+0x27a>
87806600:	e6c3      	b.n	8780638a <vsscanf+0x1a>
87806602:	2200      	movs	r2, #0
87806604:	f814 300a 	ldrb.w	r3, [r4, sl]
87806608:	0699      	lsls	r1, r3, #26
8780660a:	bf44      	itt	mi
8780660c:	2101      	movmi	r1, #1
8780660e:	468b      	movmi	fp, r1
87806610:	f140 8171 	bpl.w	878068f6 <vsscanf+0x586>
87806614:	f810 3f01 	ldrb.w	r3, [r0, #1]!
87806618:	f814 a003 	ldrb.w	sl, [r4, r3]
8780661c:	f01a 0f20 	tst.w	sl, #32
87806620:	d1f8      	bne.n	87806614 <vsscanf+0x2a4>
87806622:	2b2d      	cmp	r3, #45	; 0x2d
87806624:	bf14      	ite	ne
87806626:	2100      	movne	r1, #0
87806628:	f001 0101 	andeq.w	r1, r1, #1
8780662c:	469a      	mov	sl, r3
8780662e:	b109      	cbz	r1, 87806634 <vsscanf+0x2c4>
87806630:	f890 a001 	ldrb.w	sl, [r0, #1]
87806634:	f1ba 0f00 	cmp.w	sl, #0
87806638:	f43f aeb1 	beq.w	8780639e <vsscanf+0x2e>
8780663c:	2a10      	cmp	r2, #16
8780663e:	f000 8120 	beq.w	87806882 <vsscanf+0x512>
87806642:	2a0a      	cmp	r2, #10
87806644:	d002      	beq.n	8780664c <vsscanf+0x2dc>
87806646:	2a08      	cmp	r2, #8
87806648:	d06b      	beq.n	87806722 <vsscanf+0x3b2>
8780664a:	b922      	cbnz	r2, 87806656 <vsscanf+0x2e6>
8780664c:	f814 300a 	ldrb.w	r3, [r4, sl]
87806650:	0759      	lsls	r1, r3, #29
87806652:	f57f aea4 	bpl.w	8780639e <vsscanf+0x2e>
87806656:	f1ac 0c48 	sub.w	ip, ip, #72	; 0x48
8780665a:	f1bc 0f32 	cmp.w	ip, #50	; 0x32
8780665e:	f200 810a 	bhi.w	87806876 <vsscanf+0x506>
87806662:	e8df f01c 	tbh	[pc, ip, lsl #1]
87806666:	010800ec 	smlatteq	r8, ip, r0, r0
8780666a:	01080108 	tsteq	r8, r8, lsl #2
8780666e:	010800fa 	strdeq	r0, [r8, -sl]
87806672:	01080108 	tsteq	r8, r8, lsl #2
87806676:	01080108 	tsteq	r8, r8, lsl #2
8780667a:	01080108 	tsteq	r8, r8, lsl #2
8780667e:	01080108 	tsteq	r8, r8, lsl #2
87806682:	01080108 	tsteq	r8, r8, lsl #2
87806686:	01080108 	tsteq	r8, r8, lsl #2
8780668a:	010800da 	ldrdeq	r0, [r8, -sl]
8780668e:	01080108 	tsteq	r8, r8, lsl #2
87806692:	01080108 	tsteq	r8, r8, lsl #2
87806696:	01080108 	tsteq	r8, r8, lsl #2
8780669a:	01080108 	tsteq	r8, r8, lsl #2
8780669e:	01080108 	tsteq	r8, r8, lsl #2
878066a2:	01080108 	tsteq	r8, r8, lsl #2
878066a6:	010800cc 	smlabteq	r8, ip, r0, r0
878066aa:	01080108 	tsteq	r8, r8, lsl #2
878066ae:	010800be 	strheq	r0, [r8, -lr]
878066b2:	01080108 	tsteq	r8, r8, lsl #2
878066b6:	01080108 	tsteq	r8, r8, lsl #2
878066ba:	01080108 	tsteq	r8, r8, lsl #2
878066be:	01080108 	tsteq	r8, r8, lsl #2
878066c2:	01080108 	tsteq	r8, r8, lsl #2
878066c6:	01080108 	tsteq	r8, r8, lsl #2
878066ca:	f1ba00da 			; <UNDEFINED> instruction: 0xf1ba00da
878066ce:	0f25      	lsrs	r5, r4, #28
878066d0:	f47f ae65 	bne.w	8780639e <vsscanf+0x2e>
878066d4:	3001      	adds	r0, #1
878066d6:	f899 3001 	ldrb.w	r3, [r9, #1]
878066da:	e656      	b.n	8780638a <vsscanf+0x1a>
878066dc:	f814 300a 	ldrb.w	r3, [r4, sl]
878066e0:	2210      	movs	r2, #16
878066e2:	f003 0320 	and.w	r3, r3, #32
878066e6:	f003 0bff 	and.w	fp, r3, #255	; 0xff
878066ea:	2b00      	cmp	r3, #0
878066ec:	f000 80c9 	beq.w	87806882 <vsscanf+0x512>
878066f0:	2100      	movs	r1, #0
878066f2:	468b      	mov	fp, r1
878066f4:	e78e      	b.n	87806614 <vsscanf+0x2a4>
878066f6:	f814 300a 	ldrb.w	r3, [r4, sl]
878066fa:	220a      	movs	r2, #10
878066fc:	f003 0320 	and.w	r3, r3, #32
87806700:	f003 0bff 	and.w	fp, r3, #255	; 0xff
87806704:	2b00      	cmp	r3, #0
87806706:	d0a1      	beq.n	8780664c <vsscanf+0x2dc>
87806708:	2100      	movs	r1, #0
8780670a:	220a      	movs	r2, #10
8780670c:	468b      	mov	fp, r1
8780670e:	e781      	b.n	87806614 <vsscanf+0x2a4>
87806710:	f814 300a 	ldrb.w	r3, [r4, sl]
87806714:	2208      	movs	r2, #8
87806716:	f003 0320 	and.w	r3, r3, #32
8780671a:	f003 0bff 	and.w	fp, r3, #255	; 0xff
8780671e:	2b00      	cmp	r3, #0
87806720:	d1e6      	bne.n	878066f0 <vsscanf+0x380>
87806722:	f814 300a 	ldrb.w	r3, [r4, sl]
87806726:	f083 0304 	eor.w	r3, r3, #4
8780672a:	f3c3 0380 	ubfx	r3, r3, #2, #1
8780672e:	f1ba 0f37 	cmp.w	sl, #55	; 0x37
87806732:	bf88      	it	hi
87806734:	f043 0301 	orrhi.w	r3, r3, #1
87806738:	2b00      	cmp	r3, #0
8780673a:	d08c      	beq.n	87806656 <vsscanf+0x2e6>
8780673c:	e62f      	b.n	8780639e <vsscanf+0x2e>
8780673e:	1c5a      	adds	r2, r3, #1
87806740:	f10e 0c04 	add.w	ip, lr, #4
87806744:	f8de 1000 	ldr.w	r1, [lr]
87806748:	f000 80a2 	beq.w	87806890 <vsscanf+0x520>
8780674c:	f814 200a 	ldrb.w	r2, [r4, sl]
87806750:	0692      	lsls	r2, r2, #26
87806752:	bf58      	it	pl
87806754:	4652      	movpl	r2, sl
87806756:	d507      	bpl.n	87806768 <vsscanf+0x3f8>
87806758:	f810 af01 	ldrb.w	sl, [r0, #1]!
8780675c:	f814 200a 	ldrb.w	r2, [r4, sl]
87806760:	0692      	lsls	r2, r2, #26
87806762:	d4f9      	bmi.n	87806758 <vsscanf+0x3e8>
87806764:	4652      	mov	r2, sl
87806766:	b18a      	cbz	r2, 8780678c <vsscanf+0x41c>
87806768:	b183      	cbz	r3, 8780678c <vsscanf+0x41c>
8780676a:	f100 0e01 	add.w	lr, r0, #1
8780676e:	e006      	b.n	8780677e <vsscanf+0x40e>
87806770:	f814 a002 	ldrb.w	sl, [r4, r2]
87806774:	f01a 0f20 	tst.w	sl, #32
87806778:	d108      	bne.n	8780678c <vsscanf+0x41c>
8780677a:	3b01      	subs	r3, #1
8780677c:	d006      	beq.n	8780678c <vsscanf+0x41c>
8780677e:	f801 2b01 	strb.w	r2, [r1], #1
87806782:	4670      	mov	r0, lr
87806784:	f81e 2b01 	ldrb.w	r2, [lr], #1
87806788:	2a00      	cmp	r2, #0
8780678a:	d1f1      	bne.n	87806770 <vsscanf+0x400>
8780678c:	2300      	movs	r3, #0
8780678e:	3601      	adds	r6, #1
87806790:	700b      	strb	r3, [r1, #0]
87806792:	46e6      	mov	lr, ip
87806794:	f899 3001 	ldrb.w	r3, [r9, #1]
87806798:	e5f7      	b.n	8780638a <vsscanf+0x1a>
8780679a:	f8de 3000 	ldr.w	r3, [lr]
8780679e:	1bc2      	subs	r2, r0, r7
878067a0:	f10e 0e04 	add.w	lr, lr, #4
878067a4:	601a      	str	r2, [r3, #0]
878067a6:	f899 3001 	ldrb.w	r3, [r9, #1]
878067aa:	e5ee      	b.n	8780638a <vsscanf+0x1a>
878067ac:	f8de 1000 	ldr.w	r1, [lr]
878067b0:	1c5a      	adds	r2, r3, #1
878067b2:	bf08      	it	eq
878067b4:	2301      	moveq	r3, #1
878067b6:	1c42      	adds	r2, r0, #1
878067b8:	f10e 0e04 	add.w	lr, lr, #4
878067bc:	f101 3cff 	add.w	ip, r1, #4294967295	; 0xffffffff
878067c0:	e003      	b.n	878067ca <vsscanf+0x45a>
878067c2:	f812 1b01 	ldrb.w	r1, [r2], #1
878067c6:	468a      	mov	sl, r1
878067c8:	b129      	cbz	r1, 878067d6 <vsscanf+0x466>
878067ca:	3b01      	subs	r3, #1
878067cc:	f80c af01 	strb.w	sl, [ip, #1]!
878067d0:	2b00      	cmp	r3, #0
878067d2:	4610      	mov	r0, r2
878067d4:	dcf5      	bgt.n	878067c2 <vsscanf+0x452>
878067d6:	3601      	adds	r6, #1
878067d8:	f899 3001 	ldrb.w	r3, [r9, #1]
878067dc:	e5d5      	b.n	8780638a <vsscanf+0x1a>
878067de:	220a      	movs	r2, #10
878067e0:	e710      	b.n	87806604 <vsscanf+0x294>
878067e2:	f8de a000 	ldr.w	sl, [lr]
878067e6:	f1bb 0f00 	cmp.w	fp, #0
878067ea:	d018      	beq.n	8780681e <vsscanf+0x4ae>
878067ec:	f10e 0b04 	add.w	fp, lr, #4
878067f0:	a901      	add	r1, sp, #4
878067f2:	f7ff f9b3 	bl	87805b5c <simple_strtol>
878067f6:	46de      	mov	lr, fp
878067f8:	f8ca 0000 	str.w	r0, [sl]
878067fc:	e017      	b.n	8780682e <vsscanf+0x4be>
878067fe:	f8de a000 	ldr.w	sl, [lr]
87806802:	f1bb 0f00 	cmp.w	fp, #0
87806806:	d05c      	beq.n	878068c2 <vsscanf+0x552>
87806808:	f10e 0b04 	add.w	fp, lr, #4
8780680c:	a901      	add	r1, sp, #4
8780680e:	f7ff f9a5 	bl	87805b5c <simple_strtol>
87806812:	46de      	mov	lr, fp
87806814:	f8aa 0000 	strh.w	r0, [sl]
87806818:	e009      	b.n	8780682e <vsscanf+0x4be>
8780681a:	f8de a000 	ldr.w	sl, [lr]
8780681e:	f10e 0b04 	add.w	fp, lr, #4
87806822:	a901      	add	r1, sp, #4
87806824:	f7ff f914 	bl	87805a50 <simple_strtoul>
87806828:	46de      	mov	lr, fp
8780682a:	f8ca 0000 	str.w	r0, [sl]
8780682e:	9801      	ldr	r0, [sp, #4]
87806830:	3601      	adds	r6, #1
87806832:	2800      	cmp	r0, #0
87806834:	f43f adb3 	beq.w	8780639e <vsscanf+0x2e>
87806838:	f899 3001 	ldrb.w	r3, [r9, #1]
8780683c:	e5a5      	b.n	8780638a <vsscanf+0x1a>
8780683e:	f8de a000 	ldr.w	sl, [lr]
87806842:	f1bb 0f00 	cmp.w	fp, #0
87806846:	d033      	beq.n	878068b0 <vsscanf+0x540>
87806848:	f10e 0b04 	add.w	fp, lr, #4
8780684c:	a901      	add	r1, sp, #4
8780684e:	f7ff f985 	bl	87805b5c <simple_strtol>
87806852:	46de      	mov	lr, fp
87806854:	f88a 0000 	strb.w	r0, [sl]
87806858:	e7e9      	b.n	8780682e <vsscanf+0x4be>
8780685a:	f8de a000 	ldr.w	sl, [lr]
8780685e:	f1bb 0f00 	cmp.w	fp, #0
87806862:	d01c      	beq.n	8780689e <vsscanf+0x52e>
87806864:	f10e 0b04 	add.w	fp, lr, #4
87806868:	a901      	add	r1, sp, #4
8780686a:	f7ff fa29 	bl	87805cc0 <simple_strtoll>
8780686e:	46de      	mov	lr, fp
87806870:	e9ca 0100 	strd	r0, r1, [sl]
87806874:	e7db      	b.n	8780682e <vsscanf+0x4be>
87806876:	f1bb 0f00 	cmp.w	fp, #0
8780687a:	d02b      	beq.n	878068d4 <vsscanf+0x564>
8780687c:	f8de a000 	ldr.w	sl, [lr]
87806880:	e7b4      	b.n	878067ec <vsscanf+0x47c>
87806882:	f814 300a 	ldrb.w	r3, [r4, sl]
87806886:	f013 0f44 	tst.w	r3, #68	; 0x44
8780688a:	f47f aee4 	bne.w	87806656 <vsscanf+0x2e6>
8780688e:	e586      	b.n	8780639e <vsscanf+0x2e>
87806890:	f814 300a 	ldrb.w	r3, [r4, sl]
87806894:	069b      	lsls	r3, r3, #26
87806896:	d535      	bpl.n	87806904 <vsscanf+0x594>
87806898:	f06f 4300 	mvn.w	r3, #2147483648	; 0x80000000
8780689c:	e75c      	b.n	87806758 <vsscanf+0x3e8>
8780689e:	f10e 0b04 	add.w	fp, lr, #4
878068a2:	a901      	add	r1, sp, #4
878068a4:	f7ff f968 	bl	87805b78 <simple_strtoull>
878068a8:	46de      	mov	lr, fp
878068aa:	e9ca 0100 	strd	r0, r1, [sl]
878068ae:	e7be      	b.n	8780682e <vsscanf+0x4be>
878068b0:	f10e 0b04 	add.w	fp, lr, #4
878068b4:	a901      	add	r1, sp, #4
878068b6:	f7ff f8cb 	bl	87805a50 <simple_strtoul>
878068ba:	46de      	mov	lr, fp
878068bc:	f88a 0000 	strb.w	r0, [sl]
878068c0:	e7b5      	b.n	8780682e <vsscanf+0x4be>
878068c2:	f10e 0b04 	add.w	fp, lr, #4
878068c6:	a901      	add	r1, sp, #4
878068c8:	f7ff f8c2 	bl	87805a50 <simple_strtoul>
878068cc:	46de      	mov	lr, fp
878068ce:	f8aa 0000 	strh.w	r0, [sl]
878068d2:	e7ac      	b.n	8780682e <vsscanf+0x4be>
878068d4:	f8de a000 	ldr.w	sl, [lr]
878068d8:	f10e 0b04 	add.w	fp, lr, #4
878068dc:	a901      	add	r1, sp, #4
878068de:	f7ff f8b7 	bl	87805a50 <simple_strtoul>
878068e2:	46de      	mov	lr, fp
878068e4:	f8ca 0000 	str.w	r0, [sl]
878068e8:	e7a1      	b.n	8780682e <vsscanf+0x4be>
878068ea:	f105 0902 	add.w	r9, r5, #2
878068ee:	78a9      	ldrb	r1, [r5, #2]
878068f0:	f04f 0c48 	mov.w	ip, #72	; 0x48
878068f4:	e59c      	b.n	87806430 <vsscanf+0xc0>
878068f6:	f1ba 0f2d 	cmp.w	sl, #45	; 0x2d
878068fa:	f04f 0b01 	mov.w	fp, #1
878068fe:	f43f ae97 	beq.w	87806630 <vsscanf+0x2c0>
87806902:	e69e      	b.n	87806642 <vsscanf+0x2d2>
87806904:	4652      	mov	r2, sl
87806906:	f06f 4300 	mvn.w	r3, #2147483648	; 0x80000000
8780690a:	e72e      	b.n	8780676a <vsscanf+0x3fa>

8780690c <sscanf>:
8780690c:	b40e      	push	{r1, r2, r3}
8780690e:	b500      	push	{lr}
87806910:	aa02      	add	r2, sp, #8
87806912:	9901      	ldr	r1, [sp, #4]
87806914:	f7ff fd2c 	bl	87806370 <vsscanf>
87806918:	f85d eb04 	ldr.w	lr, [sp], #4
8780691c:	b003      	add	sp, #12
8780691e:	4770      	bx	lr

87806920 <clk_enable>:
87806920:	f44f 4380 	mov.w	r3, #16384	; 0x4000
87806924:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
87806928:	f2c0 230c 	movt	r3, #524	; 0x20c
8780692c:	669a      	str	r2, [r3, #104]	; 0x68
8780692e:	66da      	str	r2, [r3, #108]	; 0x6c
87806930:	671a      	str	r2, [r3, #112]	; 0x70
87806932:	675a      	str	r2, [r3, #116]	; 0x74
87806934:	679a      	str	r2, [r3, #120]	; 0x78
87806936:	67da      	str	r2, [r3, #124]	; 0x7c
87806938:	f8c3 2080 	str.w	r2, [r3, #128]	; 0x80
8780693c:	4770      	bx	lr
8780693e:	bf00      	nop

87806940 <imx6u_clkinit>:
87806940:	f44f 4380 	mov.w	r3, #16384	; 0x4000
87806944:	b470      	push	{r4, r5, r6}
87806946:	f2c0 230c 	movt	r3, #524	; 0x20c
8780694a:	68da      	ldr	r2, [r3, #12]
8780694c:	0751      	lsls	r1, r2, #29
8780694e:	d407      	bmi.n	87806960 <imx6u_clkinit+0x20>
87806950:	68da      	ldr	r2, [r3, #12]
87806952:	f422 7280 	bic.w	r2, r2, #256	; 0x100
87806956:	60da      	str	r2, [r3, #12]
87806958:	68da      	ldr	r2, [r3, #12]
8780695a:	f042 0204 	orr.w	r2, r2, #4
8780695e:	60da      	str	r2, [r3, #12]
87806960:	f44f 4200 	mov.w	r2, #32768	; 0x8000
87806964:	f44f 4380 	mov.w	r3, #16384	; 0x4000
87806968:	f2c0 230c 	movt	r3, #524	; 0x20c
8780696c:	f2c0 220c 	movt	r2, #524	; 0x20c
87806970:	f242 0158 	movw	r1, #8280	; 0x2058
87806974:	2601      	movs	r6, #1
87806976:	6011      	str	r1, [r2, #0]
87806978:	f241 041b 	movw	r4, #4123	; 0x101b
8780697c:	68dd      	ldr	r5, [r3, #12]
8780697e:	f2c2 0418 	movt	r4, #8216	; 0x2018
87806982:	f241 000c 	movw	r0, #4108	; 0x100c
87806986:	4619      	mov	r1, r3
87806988:	f2c1 3011 	movt	r0, #4881	; 0x1311
8780698c:	f025 0504 	bic.w	r5, r5, #4
87806990:	60dd      	str	r5, [r3, #12]
87806992:	611e      	str	r6, [r3, #16]
87806994:	f8d2 5100 	ldr.w	r5, [r2, #256]	; 0x100
87806998:	f005 35c0 	and.w	r5, r5, #3233857728	; 0xc0c0c0c0
8780699c:	432c      	orrs	r4, r5
8780699e:	f8c2 4100 	str.w	r4, [r2, #256]	; 0x100
878069a2:	f8d2 40f0 	ldr.w	r4, [r2, #240]	; 0xf0
878069a6:	f004 34c0 	and.w	r4, r4, #3233857728	; 0xc0c0c0c0
878069aa:	4320      	orrs	r0, r4
878069ac:	f8c2 00f0 	str.w	r0, [r2, #240]	; 0xf0
878069b0:	699a      	ldr	r2, [r3, #24]
878069b2:	f422 2240 	bic.w	r2, r2, #786432	; 0xc0000
878069b6:	619a      	str	r2, [r3, #24]
878069b8:	699a      	ldr	r2, [r3, #24]
878069ba:	f442 2280 	orr.w	r2, r2, #262144	; 0x40000
878069be:	619a      	str	r2, [r3, #24]
878069c0:	695a      	ldr	r2, [r3, #20]
878069c2:	f022 7200 	bic.w	r2, r2, #33554432	; 0x2000000
878069c6:	615a      	str	r2, [r3, #20]
878069c8:	6c8a      	ldr	r2, [r1, #72]	; 0x48
878069ca:	f44f 4380 	mov.w	r3, #16384	; 0x4000
878069ce:	f2c0 230c 	movt	r3, #524	; 0x20c
878069d2:	0692      	lsls	r2, r2, #26
878069d4:	d4f8      	bmi.n	878069c8 <imx6u_clkinit+0x88>
878069d6:	695a      	ldr	r2, [r3, #20]
878069d8:	bc70      	pop	{r4, r5, r6}
878069da:	f422 7240 	bic.w	r2, r2, #768	; 0x300
878069de:	615a      	str	r2, [r3, #20]
878069e0:	695a      	ldr	r2, [r3, #20]
878069e2:	f442 7280 	orr.w	r2, r2, #256	; 0x100
878069e6:	615a      	str	r2, [r3, #20]
878069e8:	69da      	ldr	r2, [r3, #28]
878069ea:	f022 0240 	bic.w	r2, r2, #64	; 0x40
878069ee:	61da      	str	r2, [r3, #28]
878069f0:	69da      	ldr	r2, [r3, #28]
878069f2:	f022 0207 	bic.w	r2, r2, #7
878069f6:	61da      	str	r2, [r3, #28]
878069f8:	6a5a      	ldr	r2, [r3, #36]	; 0x24
878069fa:	f022 0240 	bic.w	r2, r2, #64	; 0x40
878069fe:	625a      	str	r2, [r3, #36]	; 0x24
87806a00:	6a5a      	ldr	r2, [r3, #36]	; 0x24
87806a02:	f022 023f 	bic.w	r2, r2, #63	; 0x3f
87806a06:	625a      	str	r2, [r3, #36]	; 0x24
87806a08:	4770      	bx	lr
87806a0a:	bf00      	nop

87806a0c <led_init>:
87806a0c:	2168      	movs	r1, #104	; 0x68
87806a0e:	f44f 723d 	mov.w	r2, #756	; 0x2f4
87806a12:	f44f 4340 	mov.w	r3, #49152	; 0xc000
87806a16:	b410      	push	{r4}
87806a18:	f2c0 2309 	movt	r3, #521	; 0x209
87806a1c:	2405      	movs	r4, #5
87806a1e:	f2c0 220e 	movt	r2, #526	; 0x20e
87806a22:	f2c0 210e 	movt	r1, #526	; 0x20e
87806a26:	f241 00b0 	movw	r0, #4272	; 0x10b0
87806a2a:	600c      	str	r4, [r1, #0]
87806a2c:	6010      	str	r0, [r2, #0]
87806a2e:	685a      	ldr	r2, [r3, #4]
87806a30:	f85d 4b04 	ldr.w	r4, [sp], #4
87806a34:	f042 0208 	orr.w	r2, r2, #8
87806a38:	605a      	str	r2, [r3, #4]
87806a3a:	681a      	ldr	r2, [r3, #0]
87806a3c:	f022 0208 	bic.w	r2, r2, #8
87806a40:	601a      	str	r2, [r3, #0]
87806a42:	4770      	bx	lr

87806a44 <led_switch>:
87806a44:	b950      	cbnz	r0, 87806a5c <led_switch+0x18>
87806a46:	2901      	cmp	r1, #1
87806a48:	d009      	beq.n	87806a5e <led_switch+0x1a>
87806a4a:	b939      	cbnz	r1, 87806a5c <led_switch+0x18>
87806a4c:	f44f 4340 	mov.w	r3, #49152	; 0xc000
87806a50:	f2c0 2309 	movt	r3, #521	; 0x209
87806a54:	681a      	ldr	r2, [r3, #0]
87806a56:	f042 0208 	orr.w	r2, r2, #8
87806a5a:	601a      	str	r2, [r3, #0]
87806a5c:	4770      	bx	lr
87806a5e:	f44f 4340 	mov.w	r3, #49152	; 0xc000
87806a62:	f2c0 2309 	movt	r3, #521	; 0x209
87806a66:	681a      	ldr	r2, [r3, #0]
87806a68:	f022 0208 	bic.w	r2, r2, #8
87806a6c:	601a      	str	r2, [r3, #0]
87806a6e:	4770      	bx	lr

87806a70 <delay_init>:
87806a70:	f44f 4300 	mov.w	r3, #32768	; 0x8000
87806a74:	2000      	movs	r0, #0
87806a76:	f2c0 2309 	movt	r3, #521	; 0x209
87806a7a:	f44f 4200 	mov.w	r2, #32768	; 0x8000
87806a7e:	4619      	mov	r1, r3
87806a80:	6018      	str	r0, [r3, #0]
87806a82:	601a      	str	r2, [r3, #0]
87806a84:	680a      	ldr	r2, [r1, #0]
87806a86:	f44f 4300 	mov.w	r3, #32768	; 0x8000
87806a8a:	f2c0 2309 	movt	r3, #521	; 0x209
87806a8e:	0412      	lsls	r2, r2, #16
87806a90:	d4f8      	bmi.n	87806a84 <delay_init+0x14>
87806a92:	f04f 32ff 	mov.w	r2, #4294967295	; 0xffffffff
87806a96:	2040      	movs	r0, #64	; 0x40
87806a98:	2141      	movs	r1, #65	; 0x41
87806a9a:	6018      	str	r0, [r3, #0]
87806a9c:	6059      	str	r1, [r3, #4]
87806a9e:	611a      	str	r2, [r3, #16]
87806aa0:	681a      	ldr	r2, [r3, #0]
87806aa2:	f042 0201 	orr.w	r2, r2, #1
87806aa6:	601a      	str	r2, [r3, #0]
87806aa8:	4770      	bx	lr
87806aaa:	bf00      	nop

87806aac <delayus>:
87806aac:	f44f 4100 	mov.w	r1, #32768	; 0x8000
87806ab0:	b430      	push	{r4, r5}
87806ab2:	f2c0 2109 	movt	r1, #521	; 0x209
87806ab6:	2400      	movs	r4, #0
87806ab8:	6a4a      	ldr	r2, [r1, #36]	; 0x24
87806aba:	6a4b      	ldr	r3, [r1, #36]	; 0x24
87806abc:	4293      	cmp	r3, r2
87806abe:	d0fc      	beq.n	87806aba <delayus+0xe>
87806ac0:	1e65      	subs	r5, r4, #1
87806ac2:	429a      	cmp	r2, r3
87806ac4:	eba4 0402 	sub.w	r4, r4, r2
87806ac8:	eba5 0502 	sub.w	r5, r5, r2
87806acc:	bf34      	ite	cc
87806ace:	18e4      	addcc	r4, r4, r3
87806ad0:	18ec      	addcs	r4, r5, r3
87806ad2:	4284      	cmp	r4, r0
87806ad4:	461a      	mov	r2, r3
87806ad6:	d3f0      	bcc.n	87806aba <delayus+0xe>
87806ad8:	bc30      	pop	{r4, r5}
87806ada:	4770      	bx	lr

87806adc <delayms>:
87806adc:	b1d8      	cbz	r0, 87806b16 <delayms+0x3a>
87806ade:	f44f 4100 	mov.w	r1, #32768	; 0x8000
87806ae2:	b470      	push	{r4, r5, r6}
87806ae4:	f2c0 2109 	movt	r1, #521	; 0x209
87806ae8:	2600      	movs	r6, #0
87806aea:	2400      	movs	r4, #0
87806aec:	6a4a      	ldr	r2, [r1, #36]	; 0x24
87806aee:	6a4b      	ldr	r3, [r1, #36]	; 0x24
87806af0:	4293      	cmp	r3, r2
87806af2:	d0fc      	beq.n	87806aee <delayms+0x12>
87806af4:	1e65      	subs	r5, r4, #1
87806af6:	429a      	cmp	r2, r3
87806af8:	eba4 0402 	sub.w	r4, r4, r2
87806afc:	eba5 0502 	sub.w	r5, r5, r2
87806b00:	bf34      	ite	cc
87806b02:	18e4      	addcc	r4, r4, r3
87806b04:	18ec      	addcs	r4, r5, r3
87806b06:	f5b4 7f7a 	cmp.w	r4, #1000	; 0x3e8
87806b0a:	461a      	mov	r2, r3
87806b0c:	d3ef      	bcc.n	87806aee <delayms+0x12>
87806b0e:	3601      	adds	r6, #1
87806b10:	4286      	cmp	r6, r0
87806b12:	d1ea      	bne.n	87806aea <delayms+0xe>
87806b14:	bc70      	pop	{r4, r5, r6}
87806b16:	4770      	bx	lr

87806b18 <delay_short>:
87806b18:	b082      	sub	sp, #8
87806b1a:	9001      	str	r0, [sp, #4]
87806b1c:	9b01      	ldr	r3, [sp, #4]
87806b1e:	1e5a      	subs	r2, r3, #1
87806b20:	9201      	str	r2, [sp, #4]
87806b22:	2b00      	cmp	r3, #0
87806b24:	d1fa      	bne.n	87806b1c <delay_short+0x4>
87806b26:	b002      	add	sp, #8
87806b28:	4770      	bx	lr
87806b2a:	bf00      	nop

87806b2c <delay>:
87806b2c:	b082      	sub	sp, #8
87806b2e:	9001      	str	r0, [sp, #4]
87806b30:	9b01      	ldr	r3, [sp, #4]
87806b32:	1e5a      	subs	r2, r3, #1
87806b34:	9201      	str	r2, [sp, #4]
87806b36:	b153      	cbz	r3, 87806b4e <delay+0x22>
87806b38:	f240 73ff 	movw	r3, #2047	; 0x7ff
87806b3c:	461a      	mov	r2, r3
87806b3e:	3b01      	subs	r3, #1
87806b40:	2a00      	cmp	r2, #0
87806b42:	d1fb      	bne.n	87806b3c <delay+0x10>
87806b44:	9b01      	ldr	r3, [sp, #4]
87806b46:	1e5a      	subs	r2, r3, #1
87806b48:	9201      	str	r2, [sp, #4]
87806b4a:	2b00      	cmp	r3, #0
87806b4c:	d1f4      	bne.n	87806b38 <delay+0xc>
87806b4e:	b002      	add	sp, #8
87806b50:	4770      	bx	lr
87806b52:	bf00      	nop

87806b54 <beep_init>:
87806b54:	210c      	movs	r1, #12
87806b56:	2250      	movs	r2, #80	; 0x50
87806b58:	f44f 4340 	mov.w	r3, #49152	; 0xc000
87806b5c:	b410      	push	{r4}
87806b5e:	f2c0 230a 	movt	r3, #522	; 0x20a
87806b62:	2405      	movs	r4, #5
87806b64:	f2c0 2229 	movt	r2, #553	; 0x229
87806b68:	f2c0 2129 	movt	r1, #553	; 0x229
87806b6c:	f241 00b0 	movw	r0, #4272	; 0x10b0
87806b70:	600c      	str	r4, [r1, #0]
87806b72:	6010      	str	r0, [r2, #0]
87806b74:	685a      	ldr	r2, [r3, #4]
87806b76:	f85d 4b04 	ldr.w	r4, [sp], #4
87806b7a:	f042 0202 	orr.w	r2, r2, #2
87806b7e:	605a      	str	r2, [r3, #4]
87806b80:	681a      	ldr	r2, [r3, #0]
87806b82:	f042 0202 	orr.w	r2, r2, #2
87806b86:	601a      	str	r2, [r3, #0]
87806b88:	4770      	bx	lr
87806b8a:	bf00      	nop

87806b8c <beep_switch>:
87806b8c:	2801      	cmp	r0, #1
87806b8e:	d009      	beq.n	87806ba4 <beep_switch+0x18>
87806b90:	b938      	cbnz	r0, 87806ba2 <beep_switch+0x16>
87806b92:	f44f 4340 	mov.w	r3, #49152	; 0xc000
87806b96:	f2c0 230a 	movt	r3, #522	; 0x20a
87806b9a:	681a      	ldr	r2, [r3, #0]
87806b9c:	f042 0202 	orr.w	r2, r2, #2
87806ba0:	601a      	str	r2, [r3, #0]
87806ba2:	4770      	bx	lr
87806ba4:	f44f 4340 	mov.w	r3, #49152	; 0xc000
87806ba8:	f2c0 230a 	movt	r3, #522	; 0x20a
87806bac:	681a      	ldr	r2, [r3, #0]
87806bae:	f022 0202 	bic.w	r2, r2, #2
87806bb2:	601a      	str	r2, [r3, #0]
87806bb4:	4770      	bx	lr
87806bb6:	bf00      	nop

87806bb8 <gpio_pinread>:
87806bb8:	6800      	ldr	r0, [r0, #0]
87806bba:	40c8      	lsrs	r0, r1
87806bbc:	f000 0001 	and.w	r0, r0, #1
87806bc0:	4770      	bx	lr
87806bc2:	bf00      	nop

87806bc4 <gpio_pinwrite>:
87806bc4:	2301      	movs	r3, #1
87806bc6:	b122      	cbz	r2, 87806bd2 <gpio_pinwrite+0xe>
87806bc8:	6802      	ldr	r2, [r0, #0]
87806bca:	408b      	lsls	r3, r1
87806bcc:	4313      	orrs	r3, r2
87806bce:	6003      	str	r3, [r0, #0]
87806bd0:	4770      	bx	lr
87806bd2:	6802      	ldr	r2, [r0, #0]
87806bd4:	408b      	lsls	r3, r1
87806bd6:	ea22 0303 	bic.w	r3, r2, r3
87806bda:	6003      	str	r3, [r0, #0]
87806bdc:	4770      	bx	lr
87806bde:	bf00      	nop

87806be0 <gpio_intconfig>:
87806be0:	b430      	push	{r4, r5}
87806be2:	2301      	movs	r3, #1
87806be4:	69c4      	ldr	r4, [r0, #28]
87806be6:	408b      	lsls	r3, r1
87806be8:	290f      	cmp	r1, #15
87806bea:	f102 32ff 	add.w	r2, r2, #4294967295	; 0xffffffff
87806bee:	bf88      	it	hi
87806bf0:	3910      	subhi	r1, #16
87806bf2:	ea24 0403 	bic.w	r4, r4, r3
87806bf6:	61c4      	str	r4, [r0, #28]
87806bf8:	bf94      	ite	ls
87806bfa:	f100 040c 	addls.w	r4, r0, #12
87806bfe:	f100 0410 	addhi.w	r4, r0, #16
87806c02:	2a04      	cmp	r2, #4
87806c04:	d80f      	bhi.n	87806c26 <gpio_intconfig+0x46>
87806c06:	e8df f002 	tbb	[pc, r2]
87806c0a:	231e0315 	tstcs	lr, #1409286144	; 0x54000000
87806c0e:	68200010 	stmdavs	r0!, {r4}
87806c12:	0049      	lsls	r1, r1, #1
87806c14:	2503      	movs	r5, #3
87806c16:	2301      	movs	r3, #1
87806c18:	408d      	lsls	r5, r1
87806c1a:	fa03 f201 	lsl.w	r2, r3, r1
87806c1e:	ea20 0305 	bic.w	r3, r0, r5
87806c22:	4313      	orrs	r3, r2
87806c24:	6023      	str	r3, [r4, #0]
87806c26:	bc30      	pop	{r4, r5}
87806c28:	4770      	bx	lr
87806c2a:	69c2      	ldr	r2, [r0, #28]
87806c2c:	bc30      	pop	{r4, r5}
87806c2e:	4313      	orrs	r3, r2
87806c30:	61c3      	str	r3, [r0, #28]
87806c32:	4770      	bx	lr
87806c34:	0049      	lsls	r1, r1, #1
87806c36:	6822      	ldr	r2, [r4, #0]
87806c38:	2303      	movs	r3, #3
87806c3a:	408b      	lsls	r3, r1
87806c3c:	ea22 0303 	bic.w	r3, r2, r3
87806c40:	6023      	str	r3, [r4, #0]
87806c42:	bc30      	pop	{r4, r5}
87806c44:	4770      	bx	lr
87806c46:	0049      	lsls	r1, r1, #1
87806c48:	2503      	movs	r5, #3
87806c4a:	6820      	ldr	r0, [r4, #0]
87806c4c:	2302      	movs	r3, #2
87806c4e:	e7e3      	b.n	87806c18 <gpio_intconfig+0x38>
87806c50:	0049      	lsls	r1, r1, #1
87806c52:	6822      	ldr	r2, [r4, #0]
87806c54:	2303      	movs	r3, #3
87806c56:	408b      	lsls	r3, r1
87806c58:	4313      	orrs	r3, r2
87806c5a:	6023      	str	r3, [r4, #0]
87806c5c:	bc30      	pop	{r4, r5}
87806c5e:	4770      	bx	lr

87806c60 <gpio_init>:
87806c60:	2301      	movs	r3, #1
87806c62:	b470      	push	{r4, r5, r6}
87806c64:	408b      	lsls	r3, r1
87806c66:	6944      	ldr	r4, [r0, #20]
87806c68:	6816      	ldr	r6, [r2, #0]
87806c6a:	43dd      	mvns	r5, r3
87806c6c:	402c      	ands	r4, r5
87806c6e:	6144      	str	r4, [r0, #20]
87806c70:	b15e      	cbz	r6, 87806c8a <gpio_init+0x2a>
87806c72:	6844      	ldr	r4, [r0, #4]
87806c74:	431c      	orrs	r4, r3
87806c76:	6044      	str	r4, [r0, #4]
87806c78:	7914      	ldrb	r4, [r2, #4]
87806c7a:	b16c      	cbz	r4, 87806c98 <gpio_init+0x38>
87806c7c:	6804      	ldr	r4, [r0, #0]
87806c7e:	6892      	ldr	r2, [r2, #8]
87806c80:	4323      	orrs	r3, r4
87806c82:	bc70      	pop	{r4, r5, r6}
87806c84:	6003      	str	r3, [r0, #0]
87806c86:	f7ff bfab 	b.w	87806be0 <gpio_intconfig>
87806c8a:	6843      	ldr	r3, [r0, #4]
87806c8c:	6892      	ldr	r2, [r2, #8]
87806c8e:	401d      	ands	r5, r3
87806c90:	6045      	str	r5, [r0, #4]
87806c92:	bc70      	pop	{r4, r5, r6}
87806c94:	f7ff bfa4 	b.w	87806be0 <gpio_intconfig>
87806c98:	6803      	ldr	r3, [r0, #0]
87806c9a:	6892      	ldr	r2, [r2, #8]
87806c9c:	401d      	ands	r5, r3
87806c9e:	6005      	str	r5, [r0, #0]
87806ca0:	bc70      	pop	{r4, r5, r6}
87806ca2:	f7ff bf9d 	b.w	87806be0 <gpio_intconfig>
87806ca6:	bf00      	nop

87806ca8 <gpio_enableint>:
87806ca8:	6943      	ldr	r3, [r0, #20]
87806caa:	2201      	movs	r2, #1
87806cac:	408a      	lsls	r2, r1
87806cae:	4313      	orrs	r3, r2
87806cb0:	6143      	str	r3, [r0, #20]
87806cb2:	4770      	bx	lr

87806cb4 <gpio_disableint>:
87806cb4:	6943      	ldr	r3, [r0, #20]
87806cb6:	2201      	movs	r2, #1
87806cb8:	408a      	lsls	r2, r1
87806cba:	ea23 0302 	bic.w	r3, r3, r2
87806cbe:	6143      	str	r3, [r0, #20]
87806cc0:	4770      	bx	lr
87806cc2:	bf00      	nop

87806cc4 <gpio_clearintflags>:
87806cc4:	6983      	ldr	r3, [r0, #24]
87806cc6:	2201      	movs	r2, #1
87806cc8:	408a      	lsls	r2, r1
87806cca:	4313      	orrs	r3, r2
87806ccc:	6183      	str	r3, [r0, #24]
87806cce:	4770      	bx	lr

87806cd0 <key_init>:
87806cd0:	b570      	push	{r4, r5, r6, lr}
87806cd2:	218c      	movs	r1, #140	; 0x8c
87806cd4:	b084      	sub	sp, #16
87806cd6:	f44f 7346 	mov.w	r3, #792	; 0x318
87806cda:	f2c0 230e 	movt	r3, #526	; 0x20e
87806cde:	aa04      	add	r2, sp, #16
87806ce0:	f2c0 210e 	movt	r1, #526	; 0x20e
87806ce4:	2605      	movs	r6, #5
87806ce6:	f24f 0580 	movw	r5, #61568	; 0xf080
87806cea:	2400      	movs	r4, #0
87806cec:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87806cf0:	600e      	str	r6, [r1, #0]
87806cf2:	f2c0 2009 	movt	r0, #521	; 0x209
87806cf6:	601d      	str	r5, [r3, #0]
87806cf8:	2112      	movs	r1, #18
87806cfa:	f842 4d0c 	str.w	r4, [r2, #-12]!
87806cfe:	f7ff ffaf 	bl	87806c60 <gpio_init>
87806d02:	b004      	add	sp, #16
87806d04:	bd70      	pop	{r4, r5, r6, pc}
87806d06:	bf00      	nop

87806d08 <key_getvalue>:
87806d08:	b538      	push	{r3, r4, r5, lr}
87806d0a:	f64d 7490 	movw	r4, #57232	; 0xdf90
87806d0e:	f2c8 7480 	movt	r4, #34688	; 0x8780
87806d12:	7823      	ldrb	r3, [r4, #0]
87806d14:	2b01      	cmp	r3, #1
87806d16:	d00b      	beq.n	87806d30 <key_getvalue+0x28>
87806d18:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87806d1c:	2112      	movs	r1, #18
87806d1e:	f2c0 2009 	movt	r0, #521	; 0x209
87806d22:	f7ff ff49 	bl	87806bb8 <gpio_pinread>
87806d26:	2801      	cmp	r0, #1
87806d28:	bf08      	it	eq
87806d2a:	7020      	strbeq	r0, [r4, #0]
87806d2c:	2000      	movs	r0, #0
87806d2e:	bd38      	pop	{r3, r4, r5, pc}
87806d30:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87806d34:	2112      	movs	r1, #18
87806d36:	f2c0 2009 	movt	r0, #521	; 0x209
87806d3a:	f7ff ff3d 	bl	87806bb8 <gpio_pinread>
87806d3e:	4605      	mov	r5, r0
87806d40:	2800      	cmp	r0, #0
87806d42:	d1e9      	bne.n	87806d18 <key_getvalue+0x10>
87806d44:	200a      	movs	r0, #10
87806d46:	f7ff fef1 	bl	87806b2c <delay>
87806d4a:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87806d4e:	2112      	movs	r1, #18
87806d50:	f2c0 2009 	movt	r0, #521	; 0x209
87806d54:	7025      	strb	r5, [r4, #0]
87806d56:	f7ff ff2f 	bl	87806bb8 <gpio_pinread>
87806d5a:	fab0 f080 	clz	r0, r0
87806d5e:	0940      	lsrs	r0, r0, #5
87806d60:	bd38      	pop	{r3, r4, r5, pc}
87806d62:	bf00      	nop

87806d64 <gpio1_io18_irqhandler>:
87806d64:	b508      	push	{r3, lr}
87806d66:	200a      	movs	r0, #10
87806d68:	f7ff fee0 	bl	87806b2c <delay>
87806d6c:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87806d70:	2112      	movs	r1, #18
87806d72:	f2c0 2009 	movt	r0, #521	; 0x209
87806d76:	f7ff ff1f 	bl	87806bb8 <gpio_pinread>
87806d7a:	b958      	cbnz	r0, 87806d94 <gpio1_io18_irqhandler+0x30>
87806d7c:	f241 72c0 	movw	r2, #6080	; 0x17c0
87806d80:	f2c8 7281 	movt	r2, #34689	; 0x8781
87806d84:	7813      	ldrb	r3, [r2, #0]
87806d86:	fab3 f383 	clz	r3, r3
87806d8a:	095b      	lsrs	r3, r3, #5
87806d8c:	4618      	mov	r0, r3
87806d8e:	7013      	strb	r3, [r2, #0]
87806d90:	f7ff fefc 	bl	87806b8c <beep_switch>
87806d94:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87806d98:	2112      	movs	r1, #18
87806d9a:	f2c0 2009 	movt	r0, #521	; 0x209
87806d9e:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
87806da2:	f7ff bf8f 	b.w	87806cc4 <gpio_clearintflags>
87806da6:	bf00      	nop

87806da8 <exit_init>:
87806da8:	b570      	push	{r4, r5, r6, lr}
87806daa:	228c      	movs	r2, #140	; 0x8c
87806dac:	f44f 7346 	mov.w	r3, #792	; 0x318
87806db0:	f44f 4440 	mov.w	r4, #49152	; 0xc000
87806db4:	b084      	sub	sp, #16
87806db6:	2604      	movs	r6, #4
87806db8:	f2c0 220e 	movt	r2, #526	; 0x20e
87806dbc:	2005      	movs	r0, #5
87806dbe:	f2c0 230e 	movt	r3, #526	; 0x20e
87806dc2:	f24f 0180 	movw	r1, #61568	; 0xf080
87806dc6:	f2c0 2409 	movt	r4, #521	; 0x209
87806dca:	6010      	str	r0, [r2, #0]
87806dcc:	2500      	movs	r5, #0
87806dce:	6019      	str	r1, [r3, #0]
87806dd0:	4620      	mov	r0, r4
87806dd2:	2301      	movs	r3, #1
87806dd4:	eb0d 0206 	add.w	r2, sp, r6
87806dd8:	2112      	movs	r1, #18
87806dda:	9501      	str	r5, [sp, #4]
87806ddc:	9603      	str	r6, [sp, #12]
87806dde:	f88d 3008 	strb.w	r3, [sp, #8]
87806de2:	f7ff ff3d 	bl	87806c60 <gpio_init>
87806de6:	ee9f 3f10 	mrc	15, 4, r3, cr15, cr0, {0}
87806dea:	0c1b      	lsrs	r3, r3, #16
87806dec:	f241 100c 	movw	r0, #4364	; 0x110c
87806df0:	2608      	movs	r6, #8
87806df2:	041b      	lsls	r3, r3, #16
87806df4:	f646 5165 	movw	r1, #28005	; 0x6d65
87806df8:	462a      	mov	r2, r5
87806dfa:	501e      	str	r6, [r3, r0]
87806dfc:	f2c8 7180 	movt	r1, #34688	; 0x8780
87806e00:	2063      	movs	r0, #99	; 0x63
87806e02:	f7fd fe61 	bl	87804ac8 <tos_interrupt_handler_register>
87806e06:	4620      	mov	r0, r4
87806e08:	2112      	movs	r1, #18
87806e0a:	f7ff ff4d 	bl	87806ca8 <gpio_enableint>
87806e0e:	b004      	add	sp, #16
87806e10:	bd70      	pop	{r4, r5, r6, pc}
87806e12:	bf00      	nop

87806e14 <default_irqhandler>:
87806e14:	e7fe      	b.n	87806e14 <default_irqhandler>
87806e16:	bf00      	nop

87806e18 <int_init>:
87806e18:	b5f0      	push	{r4, r5, r6, r7, lr}
87806e1a:	ee9f 4f10 	mrc	15, 4, r4, cr15, cr0, {0}
87806e1e:	0c24      	lsrs	r4, r4, #16
87806e20:	f241 0204 	movw	r2, #4100	; 0x1004
87806e24:	2300      	movs	r3, #0
87806e26:	0424      	lsls	r4, r4, #16
87806e28:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
87806e2c:	58a1      	ldr	r1, [r4, r2]
87806e2e:	f001 011f 	and.w	r1, r1, #31
87806e32:	3101      	adds	r1, #1
87806e34:	f503 628c 	add.w	r2, r3, #1120	; 0x460
87806e38:	3301      	adds	r3, #1
87806e3a:	4299      	cmp	r1, r3
87806e3c:	f844 0022 	str.w	r0, [r4, r2, lsl #2]
87806e40:	d1f8      	bne.n	87806e34 <int_init+0x1c>
87806e42:	f242 0604 	movw	r6, #8196	; 0x2004
87806e46:	27f8      	movs	r7, #248	; 0xf8
87806e48:	f241 73c4 	movw	r3, #6084	; 0x17c4
87806e4c:	f646 6215 	movw	r2, #28181	; 0x6e15
87806e50:	51a7      	str	r7, [r4, r6]
87806e52:	2500      	movs	r5, #0
87806e54:	f2c8 7381 	movt	r3, #34689	; 0x8781
87806e58:	f242 0708 	movw	r7, #8200	; 0x2008
87806e5c:	f04f 0e02 	mov.w	lr, #2
87806e60:	f2c8 7280 	movt	r2, #34688	; 0x8780
87806e64:	f844 e007 	str.w	lr, [r4, r7]
87806e68:	f504 5680 	add.w	r6, r4, #4096	; 0x1000
87806e6c:	f203 5004 	addw	r0, r3, #1284	; 0x504
87806e70:	4629      	mov	r1, r5
87806e72:	f843 5b04 	str.w	r5, [r3], #4
87806e76:	f504 5400 	add.w	r4, r4, #8192	; 0x2000
87806e7a:	2501      	movs	r5, #1
87806e7c:	6035      	str	r5, [r6, #0]
87806e7e:	6025      	str	r5, [r4, #0]
87806e80:	601a      	str	r2, [r3, #0]
87806e82:	3308      	adds	r3, #8
87806e84:	f843 1c04 	str.w	r1, [r3, #-4]
87806e88:	4283      	cmp	r3, r0
87806e8a:	d1f9      	bne.n	87806e80 <int_init+0x68>
87806e8c:	2300      	movs	r3, #0
87806e8e:	f2c8 7380 	movt	r3, #34688	; 0x8780
87806e92:	ee0c 3f10 	mcr	15, 0, r3, cr12, cr0, {0}
87806e96:	bdf0      	pop	{r4, r5, r6, r7, pc}

87806e98 <system_irqtable_init>:
87806e98:	b410      	push	{r4}
87806e9a:	f646 6215 	movw	r2, #28181	; 0x6e15
87806e9e:	2400      	movs	r4, #0
87806ea0:	f241 73c4 	movw	r3, #6084	; 0x17c4
87806ea4:	4621      	mov	r1, r4
87806ea6:	f2c8 7280 	movt	r2, #34688	; 0x8780
87806eaa:	f2c8 7381 	movt	r3, #34689	; 0x8781
87806eae:	f203 5004 	addw	r0, r3, #1284	; 0x504
87806eb2:	f843 4b04 	str.w	r4, [r3], #4
87806eb6:	601a      	str	r2, [r3, #0]
87806eb8:	3308      	adds	r3, #8
87806eba:	f843 1c04 	str.w	r1, [r3, #-4]
87806ebe:	4283      	cmp	r3, r0
87806ec0:	d1f9      	bne.n	87806eb6 <system_irqtable_init+0x1e>
87806ec2:	f85d 4b04 	ldr.w	r4, [sp], #4
87806ec6:	4770      	bx	lr

87806ec8 <system_register_irqhandler>:
87806ec8:	f241 73c4 	movw	r3, #6084	; 0x17c4
87806ecc:	f2c8 7381 	movt	r3, #34689	; 0x8781
87806ed0:	eb03 00c0 	add.w	r0, r3, r0, lsl #3
87806ed4:	6041      	str	r1, [r0, #4]
87806ed6:	6082      	str	r2, [r0, #8]
87806ed8:	4770      	bx	lr
87806eda:	bf00      	nop

87806edc <system_irqhandler>:
87806edc:	b538      	push	{r3, r4, r5, lr}
87806ede:	f3c0 0309 	ubfx	r3, r0, #0, #10
87806ee2:	2b9f      	cmp	r3, #159	; 0x9f
87806ee4:	d80f      	bhi.n	87806f06 <system_irqhandler+0x2a>
87806ee6:	f241 74c4 	movw	r4, #6084	; 0x17c4
87806eea:	4618      	mov	r0, r3
87806eec:	f2c8 7481 	movt	r4, #34689	; 0x8781
87806ef0:	6822      	ldr	r2, [r4, #0]
87806ef2:	eb04 03c3 	add.w	r3, r4, r3, lsl #3
87806ef6:	685d      	ldr	r5, [r3, #4]
87806ef8:	3201      	adds	r2, #1
87806efa:	6899      	ldr	r1, [r3, #8]
87806efc:	6022      	str	r2, [r4, #0]
87806efe:	47a8      	blx	r5
87806f00:	6823      	ldr	r3, [r4, #0]
87806f02:	3b01      	subs	r3, #1
87806f04:	6023      	str	r3, [r4, #0]
87806f06:	bd38      	pop	{r3, r4, r5, pc}

87806f08 <epit1_irqhandler>:
87806f08:	f64d 6038 	movw	r0, #56888	; 0xde38
87806f0c:	b508      	push	{r3, lr}
87806f0e:	f2c8 7080 	movt	r0, #34688	; 0x8780
87806f12:	f7fe f89b 	bl	8780504c <printf>
87806f16:	f641 43c8 	movw	r3, #7368	; 0x1cc8
87806f1a:	2200      	movs	r2, #0
87806f1c:	f2c8 7381 	movt	r3, #34689	; 0x8781
87806f20:	f2c0 220d 	movt	r2, #525	; 0x20d
87806f24:	7819      	ldrb	r1, [r3, #0]
87806f26:	fab1 f181 	clz	r1, r1
87806f2a:	0949      	lsrs	r1, r1, #5
87806f2c:	7019      	strb	r1, [r3, #0]
87806f2e:	6853      	ldr	r3, [r2, #4]
87806f30:	07db      	lsls	r3, r3, #31
87806f32:	d502      	bpl.n	87806f3a <epit1_irqhandler+0x32>
87806f34:	2000      	movs	r0, #0
87806f36:	f7ff fd85 	bl	87806a44 <led_switch>
87806f3a:	2300      	movs	r3, #0
87806f3c:	f2c0 230d 	movt	r3, #525	; 0x20d
87806f40:	685a      	ldr	r2, [r3, #4]
87806f42:	f042 0201 	orr.w	r2, r2, #1
87806f46:	605a      	str	r2, [r3, #4]
87806f48:	bd08      	pop	{r3, pc}
87806f4a:	bf00      	nop

87806f4c <epit1_init>:
87806f4c:	b538      	push	{r3, r4, r5, lr}
87806f4e:	f640 73ff 	movw	r3, #4095	; 0xfff
87806f52:	4298      	cmp	r0, r3
87806f54:	f04f 0400 	mov.w	r4, #0
87806f58:	bf28      	it	cs
87806f5a:	4618      	movcs	r0, r3
87806f5c:	f2c0 240d 	movt	r4, #525	; 0x20d
87806f60:	0100      	lsls	r0, r0, #4
87806f62:	2200      	movs	r2, #0
87806f64:	6022      	str	r2, [r4, #0]
87806f66:	f040 7080 	orr.w	r0, r0, #16777216	; 0x1000000
87806f6a:	f040 000e 	orr.w	r0, r0, #14
87806f6e:	6020      	str	r0, [r4, #0]
87806f70:	60a1      	str	r1, [r4, #8]
87806f72:	60e2      	str	r2, [r4, #12]
87806f74:	ee9f 3f10 	mrc	15, 4, r3, cr15, cr0, {0}
87806f78:	0c1b      	lsrs	r3, r3, #16
87806f7a:	f241 1008 	movw	r0, #4360	; 0x1108
87806f7e:	f04f 7580 	mov.w	r5, #16777216	; 0x1000000
87806f82:	041b      	lsls	r3, r3, #16
87806f84:	f646 7109 	movw	r1, #28425	; 0x6f09
87806f88:	f2c8 7180 	movt	r1, #34688	; 0x8780
87806f8c:	501d      	str	r5, [r3, r0]
87806f8e:	2058      	movs	r0, #88	; 0x58
87806f90:	f7fd fd9a 	bl	87804ac8 <tos_interrupt_handler_register>
87806f94:	6823      	ldr	r3, [r4, #0]
87806f96:	f043 0301 	orr.w	r3, r3, #1
87806f9a:	6023      	str	r3, [r4, #0]
87806f9c:	bd38      	pop	{r3, r4, r5, pc}
87806f9e:	bf00      	nop

87806fa0 <filtertimer_irqhandler>:
87806fa0:	b508      	push	{r3, lr}
87806fa2:	2300      	movs	r3, #0
87806fa4:	f2c0 230d 	movt	r3, #525	; 0x20d
87806fa8:	685a      	ldr	r2, [r3, #4]
87806faa:	07d2      	lsls	r2, r2, #31
87806fac:	d407      	bmi.n	87806fbe <filtertimer_irqhandler+0x1e>
87806fae:	2300      	movs	r3, #0
87806fb0:	f2c0 230d 	movt	r3, #525	; 0x20d
87806fb4:	685a      	ldr	r2, [r3, #4]
87806fb6:	f042 0201 	orr.w	r2, r2, #1
87806fba:	605a      	str	r2, [r3, #4]
87806fbc:	bd08      	pop	{r3, pc}
87806fbe:	681a      	ldr	r2, [r3, #0]
87806fc0:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87806fc4:	f2c0 2009 	movt	r0, #521	; 0x209
87806fc8:	2112      	movs	r1, #18
87806fca:	f022 0201 	bic.w	r2, r2, #1
87806fce:	601a      	str	r2, [r3, #0]
87806fd0:	f7ff fdf2 	bl	87806bb8 <gpio_pinread>
87806fd4:	2800      	cmp	r0, #0
87806fd6:	d1ea      	bne.n	87806fae <filtertimer_irqhandler+0xe>
87806fd8:	f641 42c9 	movw	r2, #7369	; 0x1cc9
87806fdc:	f2c8 7281 	movt	r2, #34689	; 0x8781
87806fe0:	7813      	ldrb	r3, [r2, #0]
87806fe2:	fab3 f383 	clz	r3, r3
87806fe6:	095b      	lsrs	r3, r3, #5
87806fe8:	4618      	mov	r0, r3
87806fea:	7013      	strb	r3, [r2, #0]
87806fec:	f7ff fdce 	bl	87806b8c <beep_switch>
87806ff0:	e7dd      	b.n	87806fae <filtertimer_irqhandler+0xe>
87806ff2:	bf00      	nop

87806ff4 <gpio1_16_31_irqhandler>:
87806ff4:	2300      	movs	r3, #0
87806ff6:	b410      	push	{r4}
87806ff8:	f2c0 230d 	movt	r3, #525	; 0x20d
87806ffc:	f44f 5491 	mov.w	r4, #4640	; 0x1220
87807000:	f2c0 040a 	movt	r4, #10
87807004:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87807008:	681a      	ldr	r2, [r3, #0]
8780700a:	f2c0 2009 	movt	r0, #521	; 0x209
8780700e:	2112      	movs	r1, #18
87807010:	f022 0201 	bic.w	r2, r2, #1
87807014:	601a      	str	r2, [r3, #0]
87807016:	609c      	str	r4, [r3, #8]
87807018:	681a      	ldr	r2, [r3, #0]
8780701a:	f85d 4b04 	ldr.w	r4, [sp], #4
8780701e:	f042 0201 	orr.w	r2, r2, #1
87807022:	601a      	str	r2, [r3, #0]
87807024:	f7ff be4e 	b.w	87806cc4 <gpio_clearintflags>

87807028 <filtertimer_init>:
87807028:	2300      	movs	r3, #0
8780702a:	210e      	movs	r1, #14
8780702c:	f2c0 230d 	movt	r3, #525	; 0x20d
87807030:	2200      	movs	r2, #0
87807032:	f2c0 1100 	movt	r1, #256	; 0x100
87807036:	b410      	push	{r4}
87807038:	601a      	str	r2, [r3, #0]
8780703a:	6019      	str	r1, [r3, #0]
8780703c:	6098      	str	r0, [r3, #8]
8780703e:	60da      	str	r2, [r3, #12]
87807040:	ee9f 3f10 	mrc	15, 4, r3, cr15, cr0, {0}
87807044:	0c1b      	lsrs	r3, r3, #16
87807046:	f241 1008 	movw	r0, #4360	; 0x1108
8780704a:	f04f 7480 	mov.w	r4, #16777216	; 0x1000000
8780704e:	041b      	lsls	r3, r3, #16
87807050:	f646 71a1 	movw	r1, #28577	; 0x6fa1
87807054:	f2c8 7180 	movt	r1, #34688	; 0x8780
87807058:	501c      	str	r4, [r3, r0]
8780705a:	2058      	movs	r0, #88	; 0x58
8780705c:	f85d 4b04 	ldr.w	r4, [sp], #4
87807060:	f7ff bf32 	b.w	87806ec8 <system_register_irqhandler>

87807064 <filterkey_init>:
87807064:	b570      	push	{r4, r5, r6, lr}
87807066:	228c      	movs	r2, #140	; 0x8c
87807068:	f44f 7346 	mov.w	r3, #792	; 0x318
8780706c:	f44f 4440 	mov.w	r4, #49152	; 0xc000
87807070:	b084      	sub	sp, #16
87807072:	2604      	movs	r6, #4
87807074:	f2c0 220e 	movt	r2, #526	; 0x20e
87807078:	2005      	movs	r0, #5
8780707a:	f2c0 230e 	movt	r3, #526	; 0x20e
8780707e:	f24f 0180 	movw	r1, #61568	; 0xf080
87807082:	f2c0 2409 	movt	r4, #521	; 0x209
87807086:	6010      	str	r0, [r2, #0]
87807088:	2500      	movs	r5, #0
8780708a:	6019      	str	r1, [r3, #0]
8780708c:	4620      	mov	r0, r4
8780708e:	2301      	movs	r3, #1
87807090:	eb0d 0206 	add.w	r2, sp, r6
87807094:	2112      	movs	r1, #18
87807096:	9501      	str	r5, [sp, #4]
87807098:	9603      	str	r6, [sp, #12]
8780709a:	f88d 3008 	strb.w	r3, [sp, #8]
8780709e:	f7ff fddf 	bl	87806c60 <gpio_init>
878070a2:	ee9f 3f10 	mrc	15, 4, r3, cr15, cr0, {0}
878070a6:	0c1b      	lsrs	r3, r3, #16
878070a8:	f241 100c 	movw	r0, #4364	; 0x110c
878070ac:	2608      	movs	r6, #8
878070ae:	041b      	lsls	r3, r3, #16
878070b0:	f646 71f5 	movw	r1, #28661	; 0x6ff5
878070b4:	462a      	mov	r2, r5
878070b6:	501e      	str	r6, [r3, r0]
878070b8:	f2c8 7180 	movt	r1, #34688	; 0x8780
878070bc:	2063      	movs	r0, #99	; 0x63
878070be:	f7ff ff03 	bl	87806ec8 <system_register_irqhandler>
878070c2:	4620      	mov	r0, r4
878070c4:	2112      	movs	r1, #18
878070c6:	f7ff fdef 	bl	87806ca8 <gpio_enableint>
878070ca:	f44f 5091 	mov.w	r0, #4640	; 0x1220
878070ce:	f2c0 000a 	movt	r0, #10
878070d2:	f7ff ffa9 	bl	87807028 <filtertimer_init>
878070d6:	b004      	add	sp, #16
878070d8:	bd70      	pop	{r4, r5, r6, pc}
878070da:	bf00      	nop

878070dc <filtertimer_stop>:
878070dc:	2300      	movs	r3, #0
878070de:	f2c0 230d 	movt	r3, #525	; 0x20d
878070e2:	681a      	ldr	r2, [r3, #0]
878070e4:	f022 0201 	bic.w	r2, r2, #1
878070e8:	601a      	str	r2, [r3, #0]
878070ea:	4770      	bx	lr

878070ec <filtertimer_restart>:
878070ec:	2300      	movs	r3, #0
878070ee:	f2c0 230d 	movt	r3, #525	; 0x20d
878070f2:	681a      	ldr	r2, [r3, #0]
878070f4:	f022 0201 	bic.w	r2, r2, #1
878070f8:	601a      	str	r2, [r3, #0]
878070fa:	6098      	str	r0, [r3, #8]
878070fc:	681a      	ldr	r2, [r3, #0]
878070fe:	f042 0201 	orr.w	r2, r2, #1
87807102:	601a      	str	r2, [r3, #0]
87807104:	4770      	bx	lr
87807106:	bf00      	nop

87807108 <uart_init>:
87807108:	2300      	movs	r3, #0
8780710a:	2084      	movs	r0, #132	; 0x84
8780710c:	b4f0      	push	{r4, r5, r6, r7}
8780710e:	f2c0 200e 	movt	r0, #526	; 0x20e
87807112:	461e      	mov	r6, r3
87807114:	2188      	movs	r1, #136	; 0x88
87807116:	f240 6524 	movw	r5, #1572	; 0x624
8780711a:	f44f 7444 	mov.w	r4, #784	; 0x310
8780711e:	f44f 7245 	mov.w	r2, #788	; 0x314
87807122:	6006      	str	r6, [r0, #0]
87807124:	f2c0 2302 	movt	r3, #514	; 0x202
87807128:	f2c0 210e 	movt	r1, #526	; 0x20e
8780712c:	f2c0 220e 	movt	r2, #526	; 0x20e
87807130:	f241 00b0 	movw	r0, #4272	; 0x10b0
87807134:	f2c0 250e 	movt	r5, #526	; 0x20e
87807138:	f2c0 240e 	movt	r4, #526	; 0x20e
8780713c:	2703      	movs	r7, #3
8780713e:	600e      	str	r6, [r1, #0]
87807140:	602f      	str	r7, [r5, #0]
87807142:	4619      	mov	r1, r3
87807144:	6020      	str	r0, [r4, #0]
87807146:	6010      	str	r0, [r2, #0]
87807148:	f8d3 2080 	ldr.w	r2, [r3, #128]	; 0x80
8780714c:	f022 0201 	bic.w	r2, r2, #1
87807150:	f8c3 2080 	str.w	r2, [r3, #128]	; 0x80
87807154:	f8d3 2084 	ldr.w	r2, [r3, #132]	; 0x84
87807158:	f022 0201 	bic.w	r2, r2, #1
8780715c:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
87807160:	f8d1 2084 	ldr.w	r2, [r1, #132]	; 0x84
87807164:	2300      	movs	r3, #0
87807166:	f2c0 2302 	movt	r3, #514	; 0x202
8780716a:	07d2      	lsls	r2, r2, #31
8780716c:	d5f8      	bpl.n	87807160 <uart_init+0x58>
8780716e:	2200      	movs	r2, #0
87807170:	f44f 7420 	mov.w	r4, #640	; 0x280
87807174:	f8c3 2080 	str.w	r2, [r3, #128]	; 0x80
87807178:	2047      	movs	r0, #71	; 0x47
8780717a:	f8d3 2084 	ldr.w	r2, [r3, #132]	; 0x84
8780717e:	f640 4134 	movw	r1, #3124	; 0xc34
87807182:	f442 4280 	orr.w	r2, r2, #16384	; 0x4000
87807186:	f042 0226 	orr.w	r2, r2, #38	; 0x26
8780718a:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
8780718e:	f8d3 2088 	ldr.w	r2, [r3, #136]	; 0x88
87807192:	f042 0204 	orr.w	r2, r2, #4
87807196:	f8c3 2088 	str.w	r2, [r3, #136]	; 0x88
8780719a:	f8d3 2080 	ldr.w	r2, [r3, #128]	; 0x80
8780719e:	f422 4280 	bic.w	r2, r2, #16384	; 0x4000
878071a2:	f8c3 2080 	str.w	r2, [r3, #128]	; 0x80
878071a6:	f8c3 4090 	str.w	r4, [r3, #144]	; 0x90
878071aa:	f8c3 00a4 	str.w	r0, [r3, #164]	; 0xa4
878071ae:	f8c3 10a8 	str.w	r1, [r3, #168]	; 0xa8
878071b2:	f8d3 2080 	ldr.w	r2, [r3, #128]	; 0x80
878071b6:	bcf0      	pop	{r4, r5, r6, r7}
878071b8:	f042 0201 	orr.w	r2, r2, #1
878071bc:	f8c3 2080 	str.w	r2, [r3, #128]	; 0x80
878071c0:	4770      	bx	lr
878071c2:	bf00      	nop

878071c4 <uart_io_init>:
878071c4:	b4f0      	push	{r4, r5, r6, r7}
878071c6:	f240 6024 	movw	r0, #1572	; 0x624
878071ca:	2684      	movs	r6, #132	; 0x84
878071cc:	2488      	movs	r4, #136	; 0x88
878071ce:	f44f 7144 	mov.w	r1, #784	; 0x310
878071d2:	f44f 7345 	mov.w	r3, #788	; 0x314
878071d6:	2500      	movs	r5, #0
878071d8:	f2c0 260e 	movt	r6, #526	; 0x20e
878071dc:	f2c0 240e 	movt	r4, #526	; 0x20e
878071e0:	2703      	movs	r7, #3
878071e2:	f241 02b0 	movw	r2, #4272	; 0x10b0
878071e6:	f2c0 200e 	movt	r0, #526	; 0x20e
878071ea:	f2c0 210e 	movt	r1, #526	; 0x20e
878071ee:	f2c0 230e 	movt	r3, #526	; 0x20e
878071f2:	6035      	str	r5, [r6, #0]
878071f4:	6025      	str	r5, [r4, #0]
878071f6:	6007      	str	r7, [r0, #0]
878071f8:	600a      	str	r2, [r1, #0]
878071fa:	bcf0      	pop	{r4, r5, r6, r7}
878071fc:	601a      	str	r2, [r3, #0]
878071fe:	4770      	bx	lr

87807200 <uart_setbaudrate>:
87807200:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87807204:	010d      	lsls	r5, r1, #4
87807206:	b085      	sub	sp, #20
87807208:	460f      	mov	r7, r1
8780720a:	4682      	mov	sl, r0
8780720c:	4691      	mov	r9, r2
8780720e:	f000 80f1 	beq.w	878073f4 <uart_setbaudrate+0x1f4>
87807212:	462c      	mov	r4, r5
87807214:	4610      	mov	r0, r2
87807216:	e000      	b.n	8780721a <uart_setbaudrate+0x1a>
87807218:	460c      	mov	r4, r1
8780721a:	4621      	mov	r1, r4
8780721c:	f001 ffe6 	bl	878091ec <__aeabi_uidivmod>
87807220:	4620      	mov	r0, r4
87807222:	2900      	cmp	r1, #0
87807224:	d1f8      	bne.n	87807218 <uart_setbaudrate+0x18>
87807226:	4621      	mov	r1, r4
87807228:	4648      	mov	r0, r9
8780722a:	f001 feb1 	bl	87808f90 <__aeabi_uidiv>
8780722e:	4621      	mov	r1, r4
87807230:	4606      	mov	r6, r0
87807232:	4628      	mov	r0, r5
87807234:	f001 feac 	bl	87808f90 <__aeabi_uidiv>
87807238:	f64f 73f9 	movw	r3, #65529	; 0xfff9
8780723c:	4605      	mov	r5, r0
8780723e:	f2c0 0306 	movt	r3, #6
87807242:	429e      	cmp	r6, r3
87807244:	bf98      	it	ls
87807246:	f5b0 3f80 	cmpls.w	r0, #65536	; 0x10000
8780724a:	bf2c      	ite	cs
8780724c:	2301      	movcs	r3, #1
8780724e:	2300      	movcc	r3, #0
87807250:	d277      	bcs.n	87807342 <uart_setbaudrate+0x142>
87807252:	f248 0101 	movw	r1, #32769	; 0x8001
87807256:	1e72      	subs	r2, r6, #1
87807258:	f2c8 0100 	movt	r1, #32768	; 0x8000
8780725c:	9301      	str	r3, [sp, #4]
8780725e:	fba2 2301 	umull	r2, r3, r2, r1
87807262:	ea4f 1b06 	mov.w	fp, r6, lsl #4
87807266:	4680      	mov	r8, r0
87807268:	0bda      	lsrs	r2, r3, #15
8780726a:	1e53      	subs	r3, r2, #1
8780726c:	3201      	adds	r2, #1
8780726e:	9203      	str	r2, [sp, #12]
87807270:	2b05      	cmp	r3, #5
87807272:	4648      	mov	r0, r9
87807274:	bf96      	itet	ls
87807276:	f249 428c 	movwls	r2, #38028	; 0x948c
8780727a:	2305      	movhi	r3, #5
8780727c:	f2c8 7280 	movtls	r2, #34688	; 0x8780
87807280:	f04f 0100 	mov.w	r1, #0
87807284:	bf98      	it	ls
87807286:	f852 3023 	ldrls.w	r3, [r2, r3, lsl #2]
8780728a:	9302      	str	r3, [sp, #8]
8780728c:	f1bb 0f00 	cmp.w	fp, #0
87807290:	f000 80ae 	beq.w	878073f0 <uart_setbaudrate+0x1f0>
87807294:	465c      	mov	r4, fp
87807296:	e000      	b.n	8780729a <uart_setbaudrate+0x9a>
87807298:	4614      	mov	r4, r2
8780729a:	4622      	mov	r2, r4
8780729c:	2300      	movs	r3, #0
8780729e:	f001 ffb3 	bl	87809208 <__aeabi_uldivmod>
878072a2:	4620      	mov	r0, r4
878072a4:	2100      	movs	r1, #0
878072a6:	2a00      	cmp	r2, #0
878072a8:	d1f6      	bne.n	87807298 <uart_setbaudrate+0x98>
878072aa:	4648      	mov	r0, r9
878072ac:	4621      	mov	r1, r4
878072ae:	f001 fe6f 	bl	87808f90 <__aeabi_uidiv>
878072b2:	4603      	mov	r3, r0
878072b4:	9a01      	ldr	r2, [sp, #4]
878072b6:	fba3 8908 	umull	r8, r9, r3, r8
878072ba:	4621      	mov	r1, r4
878072bc:	4658      	mov	r0, fp
878072be:	fb03 9902 	mla	r9, r3, r2, r9
878072c2:	f001 fe65 	bl	87808f90 <__aeabi_uidiv>
878072c6:	2300      	movs	r3, #0
878072c8:	4602      	mov	r2, r0
878072ca:	4649      	mov	r1, r9
878072cc:	4640      	mov	r0, r8
878072ce:	f001 ff9b 	bl	87809208 <__aeabi_uldivmod>
878072d2:	2300      	movs	r3, #0
878072d4:	4299      	cmp	r1, r3
878072d6:	bf08      	it	eq
878072d8:	42b8      	cmpeq	r0, r7
878072da:	d32e      	bcc.n	8780733a <uart_setbaudrate+0x13a>
878072dc:	1bc0      	subs	r0, r0, r7
878072de:	eb61 0103 	sbc.w	r1, r1, r3
878072e2:	f248 521f 	movw	r2, #34079	; 0x851f
878072e6:	f2c5 12eb 	movt	r2, #20971	; 0x51eb
878072ea:	fba7 2302 	umull	r2, r3, r7, r2
878072ee:	095a      	lsrs	r2, r3, #5
878072f0:	2300      	movs	r3, #0
878072f2:	4299      	cmp	r1, r3
878072f4:	eb02 0242 	add.w	r2, r2, r2, lsl #1
878072f8:	bf08      	it	eq
878072fa:	4290      	cmpeq	r0, r2
878072fc:	d21a      	bcs.n	87807334 <uart_setbaudrate+0x134>
878072fe:	9b02      	ldr	r3, [sp, #8]
87807300:	3d01      	subs	r5, #1
87807302:	f8da 1090 	ldr.w	r1, [sl, #144]	; 0x90
87807306:	b2ad      	uxth	r5, r5
87807308:	4630      	mov	r0, r6
8780730a:	01db      	lsls	r3, r3, #7
8780730c:	f403 7260 	and.w	r2, r3, #896	; 0x380
87807310:	f421 7360 	bic.w	r3, r1, #896	; 0x380
87807314:	f8ca 3090 	str.w	r3, [sl, #144]	; 0x90
87807318:	f8da 3090 	ldr.w	r3, [sl, #144]	; 0x90
8780731c:	9903      	ldr	r1, [sp, #12]
8780731e:	4313      	orrs	r3, r2
87807320:	f8ca 3090 	str.w	r3, [sl, #144]	; 0x90
87807324:	f8ca 50a4 	str.w	r5, [sl, #164]	; 0xa4
87807328:	f001 fe32 	bl	87808f90 <__aeabi_uidiv>
8780732c:	3801      	subs	r0, #1
8780732e:	b280      	uxth	r0, r0
87807330:	f8ca 00a8 	str.w	r0, [sl, #168]	; 0xa8
87807334:	b005      	add	sp, #20
87807336:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
8780733a:	1a38      	subs	r0, r7, r0
8780733c:	eb63 0101 	sbc.w	r1, r3, r1
87807340:	e7cf      	b.n	878072e2 <uart_setbaudrate+0xe2>
87807342:	f646 52b9 	movw	r2, #28089	; 0x6db9
87807346:	1e71      	subs	r1, r6, #1
87807348:	f2c2 4293 	movt	r2, #9363	; 0x2493
8780734c:	f248 0401 	movw	r4, #32769	; 0x8001
87807350:	fba1 2302 	umull	r2, r3, r1, r2
87807354:	f100 3bff 	add.w	fp, r0, #4294967295	; 0xffffffff
87807358:	f2c8 0400 	movt	r4, #32768	; 0x8000
8780735c:	4630      	mov	r0, r6
8780735e:	fbab bc04 	umull	fp, ip, fp, r4
87807362:	1ac9      	subs	r1, r1, r3
87807364:	eb03 0351 	add.w	r3, r3, r1, lsr #1
87807368:	ea4f 38dc 	mov.w	r8, ip, lsr #15
8780736c:	0c9b      	lsrs	r3, r3, #18
8780736e:	f108 0801 	add.w	r8, r8, #1
87807372:	3301      	adds	r3, #1
87807374:	4598      	cmp	r8, r3
87807376:	bf38      	it	cc
87807378:	4698      	movcc	r8, r3
8780737a:	4641      	mov	r1, r8
8780737c:	f001 fe08 	bl	87808f90 <__aeabi_uidiv>
87807380:	4606      	mov	r6, r0
87807382:	4641      	mov	r1, r8
87807384:	4628      	mov	r0, r5
87807386:	f001 fe03 	bl	87808f90 <__aeabi_uidiv>
8780738a:	4605      	mov	r5, r0
8780738c:	b976      	cbnz	r6, 878073ac <uart_setbaudrate+0x1ac>
8780738e:	bb40      	cbnz	r0, 878073e2 <uart_setbaudrate+0x1e2>
87807390:	f04f 0801 	mov.w	r8, #1
87807394:	2305      	movs	r3, #5
87807396:	4601      	mov	r1, r0
87807398:	9001      	str	r0, [sp, #4]
8780739a:	4646      	mov	r6, r8
8780739c:	f8cd 800c 	str.w	r8, [sp, #12]
878073a0:	4645      	mov	r5, r8
878073a2:	4648      	mov	r0, r9
878073a4:	f04f 0b10 	mov.w	fp, #16
878073a8:	9302      	str	r3, [sp, #8]
878073aa:	e773      	b.n	87807294 <uart_setbaudrate+0x94>
878073ac:	1e72      	subs	r2, r6, #1
878073ae:	b158      	cbz	r0, 878073c8 <uart_setbaudrate+0x1c8>
878073b0:	fba2 2304 	umull	r2, r3, r2, r4
878073b4:	ea4f 1b06 	mov.w	fp, r6, lsl #4
878073b8:	0bda      	lsrs	r2, r3, #15
878073ba:	1e53      	subs	r3, r2, #1
878073bc:	3201      	adds	r2, #1
878073be:	9203      	str	r2, [sp, #12]
878073c0:	2200      	movs	r2, #0
878073c2:	46a8      	mov	r8, r5
878073c4:	9201      	str	r2, [sp, #4]
878073c6:	e753      	b.n	87807270 <uart_setbaudrate+0x70>
878073c8:	fba2 2304 	umull	r2, r3, r2, r4
878073cc:	f04f 0801 	mov.w	r8, #1
878073d0:	9001      	str	r0, [sp, #4]
878073d2:	ea4f 1b06 	mov.w	fp, r6, lsl #4
878073d6:	4645      	mov	r5, r8
878073d8:	0bda      	lsrs	r2, r3, #15
878073da:	1e53      	subs	r3, r2, #1
878073dc:	4442      	add	r2, r8
878073de:	9203      	str	r2, [sp, #12]
878073e0:	e746      	b.n	87807270 <uart_setbaudrate+0x70>
878073e2:	2201      	movs	r2, #1
878073e4:	f04f 0b10 	mov.w	fp, #16
878073e8:	4616      	mov	r6, r2
878073ea:	f04f 33ff 	mov.w	r3, #4294967295	; 0xffffffff
878073ee:	e7e6      	b.n	878073be <uart_setbaudrate+0x1be>
878073f0:	2401      	movs	r4, #1
878073f2:	e75a      	b.n	878072aa <uart_setbaudrate+0xaa>
878073f4:	2401      	movs	r4, #1
878073f6:	e716      	b.n	87807226 <uart_setbaudrate+0x26>

878073f8 <uart_disable>:
878073f8:	f8d0 3080 	ldr.w	r3, [r0, #128]	; 0x80
878073fc:	f023 0301 	bic.w	r3, r3, #1
87807400:	f8c0 3080 	str.w	r3, [r0, #128]	; 0x80
87807404:	4770      	bx	lr
87807406:	bf00      	nop

87807408 <uart_enable>:
87807408:	f8d0 3080 	ldr.w	r3, [r0, #128]	; 0x80
8780740c:	f043 0301 	orr.w	r3, r3, #1
87807410:	f8c0 3080 	str.w	r3, [r0, #128]	; 0x80
87807414:	4770      	bx	lr
87807416:	bf00      	nop

87807418 <uart_softreset>:
87807418:	f8d0 3084 	ldr.w	r3, [r0, #132]	; 0x84
8780741c:	f023 0301 	bic.w	r3, r3, #1
87807420:	f8c0 3084 	str.w	r3, [r0, #132]	; 0x84
87807424:	f8d0 3084 	ldr.w	r3, [r0, #132]	; 0x84
87807428:	07db      	lsls	r3, r3, #31
8780742a:	d5fb      	bpl.n	87807424 <uart_softreset+0xc>
8780742c:	4770      	bx	lr
8780742e:	bf00      	nop

87807430 <uart_putc>:
87807430:	2100      	movs	r1, #0
87807432:	f2c0 2102 	movt	r1, #514	; 0x202
87807436:	f8d1 2098 	ldr.w	r2, [r1, #152]	; 0x98
8780743a:	2300      	movs	r3, #0
8780743c:	f2c0 2302 	movt	r3, #514	; 0x202
87807440:	0712      	lsls	r2, r2, #28
87807442:	d5f8      	bpl.n	87807436 <uart_putc+0x6>
87807444:	6418      	str	r0, [r3, #64]	; 0x40
87807446:	4770      	bx	lr

87807448 <uart_puts>:
87807448:	7801      	ldrb	r1, [r0, #0]
8780744a:	b179      	cbz	r1, 8780746c <uart_puts+0x24>
8780744c:	2200      	movs	r2, #0
8780744e:	b410      	push	{r4}
87807450:	f2c0 2202 	movt	r2, #514	; 0x202
87807454:	4614      	mov	r4, r2
87807456:	f8d2 3098 	ldr.w	r3, [r2, #152]	; 0x98
8780745a:	071b      	lsls	r3, r3, #28
8780745c:	d5fb      	bpl.n	87807456 <uart_puts+0xe>
8780745e:	6421      	str	r1, [r4, #64]	; 0x40
87807460:	f810 1f01 	ldrb.w	r1, [r0, #1]!
87807464:	2900      	cmp	r1, #0
87807466:	d1f6      	bne.n	87807456 <uart_puts+0xe>
87807468:	f85d 4b04 	ldr.w	r4, [sp], #4
8780746c:	4770      	bx	lr
8780746e:	bf00      	nop

87807470 <uart_getc>:
87807470:	2100      	movs	r1, #0
87807472:	f2c0 2102 	movt	r1, #514	; 0x202
87807476:	f8d1 2098 	ldr.w	r2, [r1, #152]	; 0x98
8780747a:	2300      	movs	r3, #0
8780747c:	f2c0 2302 	movt	r3, #514	; 0x202
87807480:	07d2      	lsls	r2, r2, #31
87807482:	d5f8      	bpl.n	87807476 <uart_getc+0x6>
87807484:	6818      	ldr	r0, [r3, #0]
87807486:	b2c0      	uxtb	r0, r0
87807488:	4770      	bx	lr
8780748a:	bf00      	nop

8780748c <raise>:
8780748c:	4770      	bx	lr
8780748e:	bf00      	nop

87807490 <lcdgpio_init>:
87807490:	f44f 728c 	mov.w	r2, #280	; 0x118
87807494:	2300      	movs	r3, #0
87807496:	f2c0 220e 	movt	r2, #526	; 0x20e
8780749a:	f44f 718e 	mov.w	r1, #284	; 0x11c
8780749e:	b5f0      	push	{r4, r5, r6, r7, lr}
878074a0:	f2c0 210e 	movt	r1, #526	; 0x20e
878074a4:	6013      	str	r3, [r2, #0]
878074a6:	f44f 7690 	mov.w	r6, #288	; 0x120
878074aa:	f44f 7492 	mov.w	r4, #292	; 0x124
878074ae:	f44f 7794 	mov.w	r7, #296	; 0x128
878074b2:	f44f 7096 	mov.w	r0, #300	; 0x12c
878074b6:	f44f 7598 	mov.w	r5, #304	; 0x130
878074ba:	f44f 729a 	mov.w	r2, #308	; 0x134
878074be:	600b      	str	r3, [r1, #0]
878074c0:	f2c0 260e 	movt	r6, #526	; 0x20e
878074c4:	f2c0 240e 	movt	r4, #526	; 0x20e
878074c8:	f2c0 270e 	movt	r7, #526	; 0x20e
878074cc:	f2c0 200e 	movt	r0, #526	; 0x20e
878074d0:	f2c0 250e 	movt	r5, #526	; 0x20e
878074d4:	f2c0 220e 	movt	r2, #526	; 0x20e
878074d8:	f44f 719c 	mov.w	r1, #312	; 0x138
878074dc:	6033      	str	r3, [r6, #0]
878074de:	f2c0 210e 	movt	r1, #526	; 0x20e
878074e2:	6023      	str	r3, [r4, #0]
878074e4:	f44f 769e 	mov.w	r6, #316	; 0x13c
878074e8:	603b      	str	r3, [r7, #0]
878074ea:	f44f 74a0 	mov.w	r4, #320	; 0x140
878074ee:	6003      	str	r3, [r0, #0]
878074f0:	f44f 77a2 	mov.w	r7, #324	; 0x144
878074f4:	602b      	str	r3, [r5, #0]
878074f6:	f44f 70a4 	mov.w	r0, #328	; 0x148
878074fa:	6013      	str	r3, [r2, #0]
878074fc:	f44f 75a6 	mov.w	r5, #332	; 0x14c
87807500:	f44f 72a8 	mov.w	r2, #336	; 0x150
87807504:	600b      	str	r3, [r1, #0]
87807506:	f2c0 260e 	movt	r6, #526	; 0x20e
8780750a:	f2c0 240e 	movt	r4, #526	; 0x20e
8780750e:	f2c0 270e 	movt	r7, #526	; 0x20e
87807512:	f2c0 200e 	movt	r0, #526	; 0x20e
87807516:	f2c0 250e 	movt	r5, #526	; 0x20e
8780751a:	f2c0 220e 	movt	r2, #526	; 0x20e
8780751e:	f44f 71aa 	mov.w	r1, #340	; 0x154
87807522:	6033      	str	r3, [r6, #0]
87807524:	f2c0 210e 	movt	r1, #526	; 0x20e
87807528:	6023      	str	r3, [r4, #0]
8780752a:	f44f 76ac 	mov.w	r6, #344	; 0x158
8780752e:	603b      	str	r3, [r7, #0]
87807530:	f44f 74ae 	mov.w	r4, #348	; 0x15c
87807534:	6003      	str	r3, [r0, #0]
87807536:	f44f 77b0 	mov.w	r7, #352	; 0x160
8780753a:	602b      	str	r3, [r5, #0]
8780753c:	f44f 70b2 	mov.w	r0, #356	; 0x164
87807540:	6013      	str	r3, [r2, #0]
87807542:	f44f 75b4 	mov.w	r5, #360	; 0x168
87807546:	f44f 72b6 	mov.w	r2, #364	; 0x16c
8780754a:	600b      	str	r3, [r1, #0]
8780754c:	f2c0 260e 	movt	r6, #526	; 0x20e
87807550:	f2c0 240e 	movt	r4, #526	; 0x20e
87807554:	f2c0 270e 	movt	r7, #526	; 0x20e
87807558:	f2c0 200e 	movt	r0, #526	; 0x20e
8780755c:	f2c0 250e 	movt	r5, #526	; 0x20e
87807560:	f2c0 220e 	movt	r2, #526	; 0x20e
87807564:	f44f 71b8 	mov.w	r1, #368	; 0x170
87807568:	6033      	str	r3, [r6, #0]
8780756a:	f2c0 210e 	movt	r1, #526	; 0x20e
8780756e:	6023      	str	r3, [r4, #0]
87807570:	f44f 76ba 	mov.w	r6, #372	; 0x174
87807574:	603b      	str	r3, [r7, #0]
87807576:	f44f 7482 	mov.w	r4, #260	; 0x104
8780757a:	6003      	str	r3, [r0, #0]
8780757c:	f44f 7784 	mov.w	r7, #264	; 0x108
87807580:	602b      	str	r3, [r5, #0]
87807582:	f44f 7086 	mov.w	r0, #268	; 0x10c
87807586:	6013      	str	r3, [r2, #0]
87807588:	f240 55dc 	movw	r5, #1500	; 0x5dc
8780758c:	f44f 7288 	mov.w	r2, #272	; 0x110
87807590:	600b      	str	r3, [r1, #0]
87807592:	f2c0 260e 	movt	r6, #526	; 0x20e
87807596:	f2c0 240e 	movt	r4, #526	; 0x20e
8780759a:	f2c0 270e 	movt	r7, #526	; 0x20e
8780759e:	f2c0 200e 	movt	r0, #526	; 0x20e
878075a2:	f2c0 250e 	movt	r5, #526	; 0x20e
878075a6:	f2c0 220e 	movt	r2, #526	; 0x20e
878075aa:	217c      	movs	r1, #124	; 0x7c
878075ac:	6033      	str	r3, [r6, #0]
878075ae:	f2c0 210e 	movt	r1, #526	; 0x20e
878075b2:	6023      	str	r3, [r4, #0]
878075b4:	f44f 7669 	mov.w	r6, #932	; 0x3a4
878075b8:	603b      	str	r3, [r7, #0]
878075ba:	f44f 746a 	mov.w	r4, #936	; 0x3a8
878075be:	6003      	str	r3, [r0, #0]
878075c0:	f2c0 260e 	movt	r6, #526	; 0x20e
878075c4:	602b      	str	r3, [r5, #0]
878075c6:	f2c0 240e 	movt	r4, #526	; 0x20e
878075ca:	6013      	str	r3, [r2, #0]
878075cc:	2305      	movs	r3, #5
878075ce:	600b      	str	r3, [r1, #0]
878075d0:	23b9      	movs	r3, #185	; 0xb9
878075d2:	6033      	str	r3, [r6, #0]
878075d4:	f44f 776b 	mov.w	r7, #940	; 0x3ac
878075d8:	6023      	str	r3, [r4, #0]
878075da:	f44f 706c 	mov.w	r0, #944	; 0x3b0
878075de:	f44f 756d 	mov.w	r5, #948	; 0x3b4
878075e2:	f44f 726e 	mov.w	r2, #952	; 0x3b8
878075e6:	f44f 716f 	mov.w	r1, #956	; 0x3bc
878075ea:	f44f 7670 	mov.w	r6, #960	; 0x3c0
878075ee:	f44f 7471 	mov.w	r4, #964	; 0x3c4
878075f2:	f2c0 270e 	movt	r7, #526	; 0x20e
878075f6:	f2c0 200e 	movt	r0, #526	; 0x20e
878075fa:	f2c0 250e 	movt	r5, #526	; 0x20e
878075fe:	f2c0 220e 	movt	r2, #526	; 0x20e
87807602:	f2c0 210e 	movt	r1, #526	; 0x20e
87807606:	f2c0 260e 	movt	r6, #526	; 0x20e
8780760a:	f2c0 240e 	movt	r4, #526	; 0x20e
8780760e:	603b      	str	r3, [r7, #0]
87807610:	f44f 7772 	mov.w	r7, #968	; 0x3c8
87807614:	6003      	str	r3, [r0, #0]
87807616:	f44f 7073 	mov.w	r0, #972	; 0x3cc
8780761a:	602b      	str	r3, [r5, #0]
8780761c:	f44f 7574 	mov.w	r5, #976	; 0x3d0
87807620:	6013      	str	r3, [r2, #0]
87807622:	f44f 7275 	mov.w	r2, #980	; 0x3d4
87807626:	600b      	str	r3, [r1, #0]
87807628:	f44f 7176 	mov.w	r1, #984	; 0x3d8
8780762c:	6033      	str	r3, [r6, #0]
8780762e:	f44f 7677 	mov.w	r6, #988	; 0x3dc
87807632:	6023      	str	r3, [r4, #0]
87807634:	f44f 7478 	mov.w	r4, #992	; 0x3e0
87807638:	f2c0 270e 	movt	r7, #526	; 0x20e
8780763c:	f2c0 200e 	movt	r0, #526	; 0x20e
87807640:	f2c0 250e 	movt	r5, #526	; 0x20e
87807644:	f2c0 220e 	movt	r2, #526	; 0x20e
87807648:	f2c0 210e 	movt	r1, #526	; 0x20e
8780764c:	f2c0 260e 	movt	r6, #526	; 0x20e
87807650:	f2c0 240e 	movt	r4, #526	; 0x20e
87807654:	603b      	str	r3, [r7, #0]
87807656:	6003      	str	r3, [r0, #0]
87807658:	f44f 7779 	mov.w	r7, #996	; 0x3e4
8780765c:	602b      	str	r3, [r5, #0]
8780765e:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
87807662:	6013      	str	r3, [r2, #0]
87807664:	f44f 757b 	mov.w	r5, #1004	; 0x3ec
87807668:	600b      	str	r3, [r1, #0]
8780766a:	f44f 727c 	mov.w	r2, #1008	; 0x3f0
8780766e:	6033      	str	r3, [r6, #0]
87807670:	f44f 717d 	mov.w	r1, #1012	; 0x3f4
87807674:	6023      	str	r3, [r4, #0]
87807676:	f44f 767e 	mov.w	r6, #1016	; 0x3f8
8780767a:	f44f 747f 	mov.w	r4, #1020	; 0x3fc
8780767e:	f2c0 270e 	movt	r7, #526	; 0x20e
87807682:	f2c0 200e 	movt	r0, #526	; 0x20e
87807686:	f2c0 250e 	movt	r5, #526	; 0x20e
8780768a:	f2c0 220e 	movt	r2, #526	; 0x20e
8780768e:	f2c0 210e 	movt	r1, #526	; 0x20e
87807692:	f2c0 260e 	movt	r6, #526	; 0x20e
87807696:	f2c0 240e 	movt	r4, #526	; 0x20e
8780769a:	603b      	str	r3, [r7, #0]
8780769c:	f44f 6780 	mov.w	r7, #1024	; 0x400
878076a0:	6003      	str	r3, [r0, #0]
878076a2:	f44f 7064 	mov.w	r0, #912	; 0x390
878076a6:	602b      	str	r3, [r5, #0]
878076a8:	f44f 7565 	mov.w	r5, #916	; 0x394
878076ac:	6013      	str	r3, [r2, #0]
878076ae:	f44f 7266 	mov.w	r2, #920	; 0x398
878076b2:	600b      	str	r3, [r1, #0]
878076b4:	f44f 7167 	mov.w	r1, #924	; 0x39c
878076b8:	6033      	str	r3, [r6, #0]
878076ba:	f44f 7642 	mov.w	r6, #776	; 0x308
878076be:	6023      	str	r3, [r4, #0]
878076c0:	f44f 4440 	mov.w	r4, #49152	; 0xc000
878076c4:	b085      	sub	sp, #20
878076c6:	f2c0 2409 	movt	r4, #521	; 0x209
878076ca:	f2c0 270e 	movt	r7, #526	; 0x20e
878076ce:	f2c0 200e 	movt	r0, #526	; 0x20e
878076d2:	f2c0 250e 	movt	r5, #526	; 0x20e
878076d6:	f2c0 220e 	movt	r2, #526	; 0x20e
878076da:	f2c0 210e 	movt	r1, #526	; 0x20e
878076de:	f2c0 260e 	movt	r6, #526	; 0x20e
878076e2:	603b      	str	r3, [r7, #0]
878076e4:	6003      	str	r3, [r0, #0]
878076e6:	4620      	mov	r0, r4
878076e8:	602b      	str	r3, [r5, #0]
878076ea:	2501      	movs	r5, #1
878076ec:	6013      	str	r3, [r2, #0]
878076ee:	aa01      	add	r2, sp, #4
878076f0:	600b      	str	r3, [r1, #0]
878076f2:	2108      	movs	r1, #8
878076f4:	6033      	str	r3, [r6, #0]
878076f6:	9501      	str	r5, [sp, #4]
878076f8:	f88d 5008 	strb.w	r5, [sp, #8]
878076fc:	f7ff fab0 	bl	87806c60 <gpio_init>
87807700:	462a      	mov	r2, r5
87807702:	4620      	mov	r0, r4
87807704:	2108      	movs	r1, #8
87807706:	f7ff fa5d 	bl	87806bc4 <gpio_pinwrite>
8780770a:	b005      	add	sp, #20
8780770c:	bdf0      	pop	{r4, r5, r6, r7, pc}
8780770e:	bf00      	nop

87807710 <lcdclk_init>:
87807710:	b430      	push	{r4, r5}
87807712:	f44f 4400 	mov.w	r4, #32768	; 0x8000
87807716:	f2c0 240c 	movt	r4, #524	; 0x20c
8780771a:	2500      	movs	r5, #0
8780771c:	f440 1081 	orr.w	r0, r0, #1056768	; 0x102000
87807720:	f44f 4380 	mov.w	r3, #16384	; 0x4000
87807724:	f8c4 50b0 	str.w	r5, [r4, #176]	; 0xb0
87807728:	f2c0 230c 	movt	r3, #524	; 0x20c
8780772c:	f8c4 50c0 	str.w	r5, [r4, #192]	; 0xc0
87807730:	3901      	subs	r1, #1
87807732:	f8c4 00a0 	str.w	r0, [r4, #160]	; 0xa0
87807736:	3a01      	subs	r2, #1
87807738:	f8d4 0170 	ldr.w	r0, [r4, #368]	; 0x170
8780773c:	f020 4040 	bic.w	r0, r0, #3221225472	; 0xc0000000
87807740:	f8c4 0170 	str.w	r0, [r4, #368]	; 0x170
87807744:	f8c4 5170 	str.w	r5, [r4, #368]	; 0x170
87807748:	6b98      	ldr	r0, [r3, #56]	; 0x38
8780774a:	bc30      	pop	{r4, r5}
8780774c:	f420 3060 	bic.w	r0, r0, #229376	; 0x38000
87807750:	6398      	str	r0, [r3, #56]	; 0x38
87807752:	6b98      	ldr	r0, [r3, #56]	; 0x38
87807754:	f440 3080 	orr.w	r0, r0, #65536	; 0x10000
87807758:	6398      	str	r0, [r3, #56]	; 0x38
8780775a:	6b98      	ldr	r0, [r3, #56]	; 0x38
8780775c:	f420 40e0 	bic.w	r0, r0, #28672	; 0x7000
87807760:	6398      	str	r0, [r3, #56]	; 0x38
87807762:	6b98      	ldr	r0, [r3, #56]	; 0x38
87807764:	ea40 3101 	orr.w	r1, r0, r1, lsl #12
87807768:	6399      	str	r1, [r3, #56]	; 0x38
8780776a:	6999      	ldr	r1, [r3, #24]
8780776c:	f021 7160 	bic.w	r1, r1, #58720256	; 0x3800000
87807770:	6199      	str	r1, [r3, #24]
87807772:	6999      	ldr	r1, [r3, #24]
87807774:	ea41 52c2 	orr.w	r2, r1, r2, lsl #23
87807778:	619a      	str	r2, [r3, #24]
8780777a:	6b9a      	ldr	r2, [r3, #56]	; 0x38
8780777c:	f422 6260 	bic.w	r2, r2, #3584	; 0xe00
87807780:	639a      	str	r2, [r3, #56]	; 0x38
87807782:	6b9a      	ldr	r2, [r3, #56]	; 0x38
87807784:	639a      	str	r2, [r3, #56]	; 0x38
87807786:	4770      	bx	lr

87807788 <lcd_init>:
87807788:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
8780778c:	f44f 4500 	mov.w	r5, #32768	; 0x8000
87807790:	f7ff fe7e 	bl	87807490 <lcdgpio_init>
87807794:	2205      	movs	r2, #5
87807796:	2103      	movs	r1, #3
87807798:	2020      	movs	r0, #32
8780779a:	f2c0 251c 	movt	r5, #540	; 0x21c
8780779e:	f7ff ffb7 	bl	87807710 <lcdclk_init>
878077a2:	f04f 4300 	mov.w	r3, #2147483648	; 0x80000000
878077a6:	200a      	movs	r0, #10
878077a8:	602b      	str	r3, [r5, #0]
878077aa:	f24b 1434 	movw	r4, #45364	; 0xb134
878077ae:	f7ff f995 	bl	87806adc <delayms>
878077b2:	f2c8 7481 	movt	r4, #34689	; 0x8781
878077b6:	2100      	movs	r1, #0
878077b8:	f04f 4209 	mov.w	r2, #2298478592	; 0x89000000
878077bc:	f04f 0c04 	mov.w	ip, #4
878077c0:	f06f 467f 	mvn.w	r6, #4278190080	; 0xff000000
878077c4:	6029      	str	r1, [r5, #0]
878077c6:	61a1      	str	r1, [r4, #24]
878077c8:	f44f 6080 	mov.w	r0, #1024	; 0x400
878077cc:	f884 c004 	strb.w	ip, [r4, #4]
878077d0:	f44f 7716 	mov.w	r7, #600	; 0x258
878077d4:	6162      	str	r2, [r4, #20]
878077d6:	f04f 0e03 	mov.w	lr, #3
878077da:	61e6      	str	r6, [r4, #28]
878077dc:	4681      	mov	r9, r0
878077de:	682b      	ldr	r3, [r5, #0]
878077e0:	f367 491f 	bfi	r9, r7, #16, #16
878077e4:	f8a4 e006 	strh.w	lr, [r4, #6]
878077e8:	f04f 0e0c 	mov.w	lr, #12
878077ec:	8027      	strh	r7, [r4, #0]
878077ee:	278c      	movs	r7, #140	; 0x8c
878077f0:	f443 2320 	orr.w	r3, r3, #655360	; 0xa0000
878077f4:	f8a4 e00a 	strh.w	lr, [r4, #10]
878077f8:	81e7      	strh	r7, [r4, #14]
878077fa:	f04f 0e14 	mov.w	lr, #20
878077fe:	f04f 0aa0 	mov.w	sl, #160	; 0xa0
87807802:	f443 6372 	orr.w	r3, r3, #3872	; 0xf20
87807806:	4607      	mov	r7, r0
87807808:	8060      	strh	r0, [r4, #2]
8780780a:	f8a4 e008 	strh.w	lr, [r4, #8]
8780780e:	f36c 471f 	bfi	r7, ip, #16, #16
87807812:	f8a4 e00c 	strh.w	lr, [r4, #12]
87807816:	f04f 0803 	mov.w	r8, #3
8780781a:	f8a4 a010 	strh.w	sl, [r4, #16]
8780781e:	f44f 6ca8 	mov.w	ip, #1344	; 0x540
87807822:	602b      	str	r3, [r5, #0]
87807824:	f04f 0e17 	mov.w	lr, #23
87807828:	f44f 23e0 	mov.w	r3, #458752	; 0x70000
8780782c:	f2c0 0c50 	movt	ip, #80	; 0x50
87807830:	612b      	str	r3, [r5, #16]
87807832:	f36a 4e1f 	bfi	lr, sl, #16, #16
87807836:	f240 237b 	movw	r3, #635	; 0x27b
8780783a:	f2c1 1830 	movt	r8, #4400	; 0x1130
8780783e:	f8c5 9030 	str.w	r9, [r5, #48]	; 0x30
87807842:	200a      	movs	r0, #10
87807844:	6729      	str	r1, [r5, #112]	; 0x70
87807846:	f8c5 8070 	str.w	r8, [r5, #112]	; 0x70
8780784a:	f8c5 3080 	str.w	r3, [r5, #128]	; 0x80
8780784e:	f8c5 c090 	str.w	ip, [r5, #144]	; 0x90
87807852:	f8c5 e0a0 	str.w	lr, [r5, #160]	; 0xa0
87807856:	f8c5 70b0 	str.w	r7, [r5, #176]	; 0xb0
8780785a:	642a      	str	r2, [r5, #64]	; 0x40
8780785c:	652a      	str	r2, [r5, #80]	; 0x50
8780785e:	682b      	ldr	r3, [r5, #0]
87807860:	f043 0301 	orr.w	r3, r3, #1
87807864:	602b      	str	r3, [r5, #0]
87807866:	f7ff f939 	bl	87806adc <delayms>
8780786a:	8821      	ldrh	r1, [r4, #0]
8780786c:	8862      	ldrh	r2, [r4, #2]
8780786e:	6963      	ldr	r3, [r4, #20]
87807870:	fb02 f201 	mul.w	r2, r2, r1
87807874:	b132      	cbz	r2, 87807884 <lcd_init+0xfc>
87807876:	eb03 0282 	add.w	r2, r3, r2, lsl #2
8780787a:	4631      	mov	r1, r6
8780787c:	f843 1b04 	str.w	r1, [r3], #4
87807880:	4293      	cmp	r3, r2
87807882:	d1fb      	bne.n	8780787c <lcd_init+0xf4>
87807884:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}

87807888 <lcd_reset>:
87807888:	f44f 4300 	mov.w	r3, #32768	; 0x8000
8780788c:	f04f 4200 	mov.w	r2, #2147483648	; 0x80000000
87807890:	f2c0 231c 	movt	r3, #540	; 0x21c
87807894:	601a      	str	r2, [r3, #0]
87807896:	4770      	bx	lr

87807898 <lcd_noreset>:
87807898:	f44f 4300 	mov.w	r3, #32768	; 0x8000
8780789c:	2200      	movs	r2, #0
8780789e:	f2c0 231c 	movt	r3, #540	; 0x21c
878078a2:	601a      	str	r2, [r3, #0]
878078a4:	4770      	bx	lr
878078a6:	bf00      	nop

878078a8 <lcd_enable>:
878078a8:	f44f 4300 	mov.w	r3, #32768	; 0x8000
878078ac:	f2c0 231c 	movt	r3, #540	; 0x21c
878078b0:	681a      	ldr	r2, [r3, #0]
878078b2:	f042 0201 	orr.w	r2, r2, #1
878078b6:	601a      	str	r2, [r3, #0]
878078b8:	4770      	bx	lr
878078ba:	bf00      	nop

878078bc <lcd_drawpoint>:
878078bc:	b430      	push	{r4, r5}
878078be:	f24b 1434 	movw	r4, #45364	; 0xb134
878078c2:	f2c8 7481 	movt	r4, #34689	; 0x8781
878078c6:	8863      	ldrh	r3, [r4, #2]
878078c8:	7925      	ldrb	r5, [r4, #4]
878078ca:	6964      	ldr	r4, [r4, #20]
878078cc:	fb01 0103 	mla	r1, r1, r3, r0
878078d0:	fb01 f105 	mul.w	r1, r1, r5
878078d4:	510a      	str	r2, [r1, r4]
878078d6:	bc30      	pop	{r4, r5}
878078d8:	4770      	bx	lr
878078da:	bf00      	nop

878078dc <lcd_readpoint>:
878078dc:	f24b 1234 	movw	r2, #45364	; 0xb134
878078e0:	b410      	push	{r4}
878078e2:	f2c8 7281 	movt	r2, #34689	; 0x8781
878078e6:	8853      	ldrh	r3, [r2, #2]
878078e8:	7914      	ldrb	r4, [r2, #4]
878078ea:	6952      	ldr	r2, [r2, #20]
878078ec:	fb01 0103 	mla	r1, r1, r3, r0
878078f0:	fb01 f104 	mul.w	r1, r1, r4
878078f4:	f85d 4b04 	ldr.w	r4, [sp], #4
878078f8:	5888      	ldr	r0, [r1, r2]
878078fa:	4770      	bx	lr

878078fc <lcd_clear>:
878078fc:	f24b 1334 	movw	r3, #45364	; 0xb134
87807900:	f2c8 7381 	movt	r3, #34689	; 0x8781
87807904:	8819      	ldrh	r1, [r3, #0]
87807906:	885a      	ldrh	r2, [r3, #2]
87807908:	695b      	ldr	r3, [r3, #20]
8780790a:	fb02 f201 	mul.w	r2, r2, r1
8780790e:	b12a      	cbz	r2, 8780791c <lcd_clear+0x20>
87807910:	eb03 0282 	add.w	r2, r3, r2, lsl #2
87807914:	f843 0b04 	str.w	r0, [r3], #4
87807918:	4293      	cmp	r3, r2
8780791a:	d1fb      	bne.n	87807914 <lcd_clear+0x18>
8780791c:	4770      	bx	lr
8780791e:	bf00      	nop

87807920 <lcd_fill>:
87807920:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
87807924:	f24b 1634 	movw	r6, #45364	; 0xb134
87807928:	f2c8 7681 	movt	r6, #34689	; 0x8781
8780792c:	4699      	mov	r9, r3
8780792e:	f8b6 8002 	ldrh.w	r8, [r6, #2]
87807932:	8834      	ldrh	r4, [r6, #0]
87807934:	9b07      	ldr	r3, [sp, #28]
87807936:	4590      	cmp	r8, r2
87807938:	bf9c      	itt	ls
8780793a:	f108 32ff 	addls.w	r2, r8, #4294967295	; 0xffffffff
8780793e:	b292      	uxthls	r2, r2
87807940:	454c      	cmp	r4, r9
87807942:	bf9c      	itt	ls
87807944:	f104 34ff 	addls.w	r4, r4, #4294967295	; 0xffffffff
87807948:	fa1f f984 	uxthls.w	r9, r4
8780794c:	4589      	cmp	r9, r1
8780794e:	d316      	bcc.n	8780797e <lcd_fill+0x5e>
87807950:	4282      	cmp	r2, r0
87807952:	d30e      	bcc.n	87807972 <lcd_fill+0x52>
87807954:	fb01 fc08 	mul.w	ip, r1, r8
87807958:	4604      	mov	r4, r0
8780795a:	f896 e004 	ldrb.w	lr, [r6, #4]
8780795e:	eb04 050c 	add.w	r5, r4, ip
87807962:	3401      	adds	r4, #1
87807964:	6977      	ldr	r7, [r6, #20]
87807966:	b2a4      	uxth	r4, r4
87807968:	fb05 f50e 	mul.w	r5, r5, lr
8780796c:	42a2      	cmp	r2, r4
8780796e:	51eb      	str	r3, [r5, r7]
87807970:	d2f3      	bcs.n	8780795a <lcd_fill+0x3a>
87807972:	3101      	adds	r1, #1
87807974:	b289      	uxth	r1, r1
87807976:	4589      	cmp	r9, r1
87807978:	d2ea      	bcs.n	87807950 <lcd_fill+0x30>
8780797a:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
8780797e:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
87807982:	bf00      	nop

87807984 <lcd_drawline>:
87807984:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87807988:	ebc0 0902 	rsb	r9, r0, r2
8780798c:	f1b9 0f00 	cmp.w	r9, #0
87807990:	b085      	sub	sp, #20
87807992:	ebc1 0a03 	rsb	sl, r1, r3
87807996:	bfc8      	it	gt
87807998:	2301      	movgt	r3, #1
8780799a:	4607      	mov	r7, r0
8780799c:	4688      	mov	r8, r1
8780799e:	bfc8      	it	gt
878079a0:	9302      	strgt	r3, [sp, #8]
878079a2:	dd33      	ble.n	87807a0c <lcd_drawline+0x88>
878079a4:	f1ba 0f00 	cmp.w	sl, #0
878079a8:	bfc4      	itt	gt
878079aa:	2301      	movgt	r3, #1
878079ac:	9303      	strgt	r3, [sp, #12]
878079ae:	dd36      	ble.n	87807a1e <lcd_drawline+0x9a>
878079b0:	2500      	movs	r5, #0
878079b2:	45ca      	cmp	sl, r9
878079b4:	46d3      	mov	fp, sl
878079b6:	f24b 1334 	movw	r3, #45364	; 0xb134
878079ba:	bfb8      	it	lt
878079bc:	46cb      	movlt	fp, r9
878079be:	462e      	mov	r6, r5
878079c0:	462c      	mov	r4, r5
878079c2:	f10b 0201 	add.w	r2, fp, #1
878079c6:	f2c8 7381 	movt	r3, #34689	; 0x8781
878079ca:	9200      	str	r2, [sp, #0]
878079cc:	9301      	str	r3, [sp, #4]
878079ce:	9b01      	ldr	r3, [sp, #4]
878079d0:	444e      	add	r6, r9
878079d2:	fa1f f188 	uxth.w	r1, r8
878079d6:	b2b8      	uxth	r0, r7
878079d8:	699a      	ldr	r2, [r3, #24]
878079da:	4455      	add	r5, sl
878079dc:	f7ff ff6e 	bl	878078bc <lcd_drawpoint>
878079e0:	455e      	cmp	r6, fp
878079e2:	f104 0401 	add.w	r4, r4, #1
878079e6:	bfc4      	itt	gt
878079e8:	9b02      	ldrgt	r3, [sp, #8]
878079ea:	ebcb 0606 	rsbgt	r6, fp, r6
878079ee:	b2a4      	uxth	r4, r4
878079f0:	bfc8      	it	gt
878079f2:	18ff      	addgt	r7, r7, r3
878079f4:	455d      	cmp	r5, fp
878079f6:	bfc2      	ittt	gt
878079f8:	9b03      	ldrgt	r3, [sp, #12]
878079fa:	ebcb 0505 	rsbgt	r5, fp, r5
878079fe:	4498      	addgt	r8, r3
87807a00:	9b00      	ldr	r3, [sp, #0]
87807a02:	429c      	cmp	r4, r3
87807a04:	dde3      	ble.n	878079ce <lcd_drawline+0x4a>
87807a06:	b005      	add	sp, #20
87807a08:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87807a0c:	bf1d      	ittte	ne
87807a0e:	f04f 33ff 	movne.w	r3, #4294967295	; 0xffffffff
87807a12:	f1c9 0900 	rsbne	r9, r9, #0
87807a16:	9302      	strne	r3, [sp, #8]
87807a18:	f8cd 9008 	streq.w	r9, [sp, #8]
87807a1c:	e7c2      	b.n	878079a4 <lcd_drawline+0x20>
87807a1e:	bf1d      	ittte	ne
87807a20:	f04f 33ff 	movne.w	r3, #4294967295	; 0xffffffff
87807a24:	f1ca 0a00 	rsbne	sl, sl, #0
87807a28:	9303      	strne	r3, [sp, #12]
87807a2a:	f8cd a00c 	streq.w	sl, [sp, #12]
87807a2e:	e7bf      	b.n	878079b0 <lcd_drawline+0x2c>

87807a30 <lcd_draw_rectangle>:
87807a30:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87807a34:	461c      	mov	r4, r3
87807a36:	4606      	mov	r6, r0
87807a38:	460d      	mov	r5, r1
87807a3a:	460b      	mov	r3, r1
87807a3c:	4617      	mov	r7, r2
87807a3e:	f7ff ffa1 	bl	87807984 <lcd_drawline>
87807a42:	4623      	mov	r3, r4
87807a44:	4632      	mov	r2, r6
87807a46:	4629      	mov	r1, r5
87807a48:	4630      	mov	r0, r6
87807a4a:	f7ff ff9b 	bl	87807984 <lcd_drawline>
87807a4e:	4623      	mov	r3, r4
87807a50:	463a      	mov	r2, r7
87807a52:	4621      	mov	r1, r4
87807a54:	4630      	mov	r0, r6
87807a56:	f7ff ff95 	bl	87807984 <lcd_drawline>
87807a5a:	4623      	mov	r3, r4
87807a5c:	463a      	mov	r2, r7
87807a5e:	4629      	mov	r1, r5
87807a60:	4638      	mov	r0, r7
87807a62:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
87807a66:	f7ff bf8d 	b.w	87807984 <lcd_drawline>
87807a6a:	bf00      	nop

87807a6c <lcd_draw_Circle>:
87807a6c:	4613      	mov	r3, r2
87807a6e:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87807a72:	f1c2 0201 	rsb	r2, r2, #1
87807a76:	b085      	sub	sp, #20
87807a78:	9000      	str	r0, [sp, #0]
87807a7a:	9201      	str	r2, [sp, #4]
87807a7c:	2b00      	cmp	r3, #0
87807a7e:	d066      	beq.n	87807b4e <lcd_draw_Circle+0xe2>
87807a80:	f24b 1634 	movw	r6, #45364	; 0xb134
87807a84:	2700      	movs	r7, #0
87807a86:	f2c8 7681 	movt	r6, #34689	; 0x8781
87807a8a:	468b      	mov	fp, r1
87807a8c:	b29d      	uxth	r5, r3
87807a8e:	9303      	str	r3, [sp, #12]
87807a90:	9b00      	ldr	r3, [sp, #0]
87807a92:	b2bc      	uxth	r4, r7
87807a94:	eb05 0a0b 	add.w	sl, r5, fp
87807a98:	69b2      	ldr	r2, [r6, #24]
87807a9a:	fa1f fa8a 	uxth.w	sl, sl
87807a9e:	eb0b 0904 	add.w	r9, fp, r4
87807aa2:	18e1      	adds	r1, r4, r3
87807aa4:	fa1f f989 	uxth.w	r9, r9
87807aa8:	b289      	uxth	r1, r1
87807aaa:	4608      	mov	r0, r1
87807aac:	9102      	str	r1, [sp, #8]
87807aae:	4651      	mov	r1, sl
87807ab0:	f7ff ff04 	bl	878078bc <lcd_drawpoint>
87807ab4:	9a00      	ldr	r2, [sp, #0]
87807ab6:	4649      	mov	r1, r9
87807ab8:	eb05 0802 	add.w	r8, r5, r2
87807abc:	69b2      	ldr	r2, [r6, #24]
87807abe:	fa1f f888 	uxth.w	r8, r8
87807ac2:	4640      	mov	r0, r8
87807ac4:	f7ff fefa 	bl	878078bc <lcd_drawpoint>
87807ac8:	9a00      	ldr	r2, [sp, #0]
87807aca:	4651      	mov	r1, sl
87807acc:	1b12      	subs	r2, r2, r4
87807ace:	ebc4 040b 	rsb	r4, r4, fp
87807ad2:	fa1f fa82 	uxth.w	sl, r2
87807ad6:	69b2      	ldr	r2, [r6, #24]
87807ad8:	b2a4      	uxth	r4, r4
87807ada:	4650      	mov	r0, sl
87807adc:	f7ff feee 	bl	878078bc <lcd_drawpoint>
87807ae0:	9a00      	ldr	r2, [sp, #0]
87807ae2:	4649      	mov	r1, r9
87807ae4:	1b52      	subs	r2, r2, r5
87807ae6:	ebc5 050b 	rsb	r5, r5, fp
87807aea:	fa1f f982 	uxth.w	r9, r2
87807aee:	b2ad      	uxth	r5, r5
87807af0:	69b2      	ldr	r2, [r6, #24]
87807af2:	4648      	mov	r0, r9
87807af4:	f7ff fee2 	bl	878078bc <lcd_drawpoint>
87807af8:	4650      	mov	r0, sl
87807afa:	69b2      	ldr	r2, [r6, #24]
87807afc:	4629      	mov	r1, r5
87807afe:	f7ff fedd 	bl	878078bc <lcd_drawpoint>
87807b02:	4648      	mov	r0, r9
87807b04:	69b2      	ldr	r2, [r6, #24]
87807b06:	4621      	mov	r1, r4
87807b08:	f7ff fed8 	bl	878078bc <lcd_drawpoint>
87807b0c:	4629      	mov	r1, r5
87807b0e:	9802      	ldr	r0, [sp, #8]
87807b10:	69b2      	ldr	r2, [r6, #24]
87807b12:	f7ff fed3 	bl	878078bc <lcd_drawpoint>
87807b16:	4621      	mov	r1, r4
87807b18:	69b2      	ldr	r2, [r6, #24]
87807b1a:	4640      	mov	r0, r8
87807b1c:	f7ff fece 	bl	878078bc <lcd_drawpoint>
87807b20:	9901      	ldr	r1, [sp, #4]
87807b22:	9b03      	ldr	r3, [sp, #12]
87807b24:	2900      	cmp	r1, #0
87807b26:	eba7 0203 	sub.w	r2, r7, r3
87807b2a:	bfa8      	it	ge
87807b2c:	f103 33ff 	addge.w	r3, r3, #4294967295	; 0xffffffff
87807b30:	eb01 0242 	add.w	r2, r1, r2, lsl #1
87807b34:	bfba      	itte	lt
87807b36:	460a      	movlt	r2, r1
87807b38:	eb02 0247 	addlt.w	r2, r2, r7, lsl #1
87807b3c:	3205      	addge	r2, #5
87807b3e:	f107 0701 	add.w	r7, r7, #1
87807b42:	bfae      	itee	ge
87807b44:	9201      	strge	r2, [sp, #4]
87807b46:	3203      	addlt	r2, #3
87807b48:	9201      	strlt	r2, [sp, #4]
87807b4a:	42bb      	cmp	r3, r7
87807b4c:	dc9e      	bgt.n	87807a8c <lcd_draw_Circle+0x20>
87807b4e:	b005      	add	sp, #20
87807b50:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}

87807b54 <lcd_showchar>:
87807b54:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87807b58:	461e      	mov	r6, r3
87807b5a:	f013 0307 	ands.w	r3, r3, #7
87807b5e:	ea4f 0456 	mov.w	r4, r6, lsr #1
87807b62:	bf18      	it	ne
87807b64:	2301      	movne	r3, #1
87807b66:	eb03 05d6 	add.w	r5, r3, r6, lsr #3
87807b6a:	4689      	mov	r9, r1
87807b6c:	b087      	sub	sp, #28
87807b6e:	fb15 f104 	smulbb	r1, r5, r4
87807b72:	3a20      	subs	r2, #32
87807b74:	b2d2      	uxtb	r2, r2
87807b76:	9001      	str	r0, [sp, #4]
87807b78:	f89d b040 	ldrb.w	fp, [sp, #64]	; 0x40
87807b7c:	f011 01ff 	ands.w	r1, r1, #255	; 0xff
87807b80:	d06c      	beq.n	87807c5c <lcd_showchar+0x108>
87807b82:	b217      	sxth	r7, r2
87807b84:	483d      	ldr	r0, [pc, #244]	; (87807c7c <lcd_showchar+0x128>)
87807b86:	3901      	subs	r1, #1
87807b88:	ea4f 0887 	mov.w	r8, r7, lsl #2
87807b8c:	b2c9      	uxtb	r1, r1
87807b8e:	013c      	lsls	r4, r7, #4
87807b90:	eb08 1e47 	add.w	lr, r8, r7, lsl #5
87807b94:	f249 45a4 	movw	r5, #38052	; 0x94a4
87807b98:	f500 6cbe 	add.w	ip, r0, #1520	; 0x5f0
87807b9c:	eb0c 13c7 	add.w	r3, ip, r7, lsl #7
87807ba0:	f2c8 7580 	movt	r5, #34688	; 0x8780
87807ba4:	f10e 0201 	add.w	r2, lr, #1
87807ba8:	440a      	add	r2, r1
87807baa:	f205 4a74 	addw	sl, r5, #1140	; 0x474
87807bae:	ebc8 0104 	rsb	r1, r8, r4
87807bb2:	9302      	str	r3, [sp, #8]
87807bb4:	eb0a 0302 	add.w	r3, sl, r2
87807bb8:	f24b 1734 	movw	r7, #45364	; 0xb134
87807bbc:	9305      	str	r3, [sp, #20]
87807bbe:	186b      	adds	r3, r5, r1
87807bc0:	9303      	str	r3, [sp, #12]
87807bc2:	1903      	adds	r3, r0, r4
87807bc4:	44f2      	add	sl, lr
87807bc6:	9304      	str	r3, [sp, #16]
87807bc8:	465b      	mov	r3, fp
87807bca:	f2c8 7781 	movt	r7, #34689	; 0x8781
87807bce:	46d3      	mov	fp, sl
87807bd0:	464c      	mov	r4, r9
87807bd2:	469a      	mov	sl, r3
87807bd4:	2e0c      	cmp	r6, #12
87807bd6:	d04a      	beq.n	87807c6e <lcd_showchar+0x11a>
87807bd8:	2e10      	cmp	r6, #16
87807bda:	d04b      	beq.n	87807c74 <lcd_showchar+0x120>
87807bdc:	2e18      	cmp	r6, #24
87807bde:	bf08      	it	eq
87807be0:	f89b 5000 	ldrbeq.w	r5, [fp]
87807be4:	d003      	beq.n	87807bee <lcd_showchar+0x9a>
87807be6:	2e20      	cmp	r6, #32
87807be8:	d138      	bne.n	87807c5c <lcd_showchar+0x108>
87807bea:	9b02      	ldr	r3, [sp, #8]
87807bec:	781d      	ldrb	r5, [r3, #0]
87807bee:	f104 0808 	add.w	r8, r4, #8
87807bf2:	fa1f f888 	uxth.w	r8, r8
87807bf6:	e00c      	b.n	87807c12 <lcd_showchar+0xbe>
87807bf8:	3401      	adds	r4, #1
87807bfa:	883b      	ldrh	r3, [r7, #0]
87807bfc:	b2a4      	uxth	r4, r4
87807bfe:	006d      	lsls	r5, r5, #1
87807c00:	42a3      	cmp	r3, r4
87807c02:	ebc9 0204 	rsb	r2, r9, r4
87807c06:	b2ed      	uxtb	r5, r5
87807c08:	d928      	bls.n	87807c5c <lcd_showchar+0x108>
87807c0a:	42b2      	cmp	r2, r6
87807c0c:	d00c      	beq.n	87807c28 <lcd_showchar+0xd4>
87807c0e:	4544      	cmp	r4, r8
87807c10:	d016      	beq.n	87807c40 <lcd_showchar+0xec>
87807c12:	062b      	lsls	r3, r5, #24
87807c14:	d425      	bmi.n	87807c62 <lcd_showchar+0x10e>
87807c16:	f1ba 0f00 	cmp.w	sl, #0
87807c1a:	d1ed      	bne.n	87807bf8 <lcd_showchar+0xa4>
87807c1c:	69fa      	ldr	r2, [r7, #28]
87807c1e:	4621      	mov	r1, r4
87807c20:	9801      	ldr	r0, [sp, #4]
87807c22:	f7ff fe4b 	bl	878078bc <lcd_drawpoint>
87807c26:	e7e7      	b.n	87807bf8 <lcd_showchar+0xa4>
87807c28:	9b01      	ldr	r3, [sp, #4]
87807c2a:	f24b 1234 	movw	r2, #45364	; 0xb134
87807c2e:	f2c8 7281 	movt	r2, #34689	; 0x8781
87807c32:	8852      	ldrh	r2, [r2, #2]
87807c34:	3301      	adds	r3, #1
87807c36:	b29b      	uxth	r3, r3
87807c38:	429a      	cmp	r2, r3
87807c3a:	9301      	str	r3, [sp, #4]
87807c3c:	d90e      	bls.n	87807c5c <lcd_showchar+0x108>
87807c3e:	464c      	mov	r4, r9
87807c40:	9b03      	ldr	r3, [sp, #12]
87807c42:	f10b 0b01 	add.w	fp, fp, #1
87807c46:	3301      	adds	r3, #1
87807c48:	9303      	str	r3, [sp, #12]
87807c4a:	9b04      	ldr	r3, [sp, #16]
87807c4c:	3301      	adds	r3, #1
87807c4e:	9304      	str	r3, [sp, #16]
87807c50:	9b02      	ldr	r3, [sp, #8]
87807c52:	3301      	adds	r3, #1
87807c54:	9302      	str	r3, [sp, #8]
87807c56:	9b05      	ldr	r3, [sp, #20]
87807c58:	459b      	cmp	fp, r3
87807c5a:	d1bb      	bne.n	87807bd4 <lcd_showchar+0x80>
87807c5c:	b007      	add	sp, #28
87807c5e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87807c62:	69ba      	ldr	r2, [r7, #24]
87807c64:	4621      	mov	r1, r4
87807c66:	9801      	ldr	r0, [sp, #4]
87807c68:	f7ff fe28 	bl	878078bc <lcd_drawpoint>
87807c6c:	e7c4      	b.n	87807bf8 <lcd_showchar+0xa4>
87807c6e:	9b03      	ldr	r3, [sp, #12]
87807c70:	781d      	ldrb	r5, [r3, #0]
87807c72:	e7bc      	b.n	87807bee <lcd_showchar+0x9a>
87807c74:	9b04      	ldr	r3, [sp, #16]
87807c76:	781d      	ldrb	r5, [r3, #0]
87807c78:	e7b9      	b.n	87807bee <lcd_showchar+0x9a>
87807c7a:	bf00      	nop
87807c7c:	8780a674 			; <UNDEFINED> instruction: 0x8780a674

87807c80 <lcd_pow>:
87807c80:	1e4b      	subs	r3, r1, #1
87807c82:	2201      	movs	r2, #1
87807c84:	b2db      	uxtb	r3, r3
87807c86:	b129      	cbz	r1, 87807c94 <lcd_pow+0x14>
87807c88:	3b01      	subs	r3, #1
87807c8a:	fb00 f202 	mul.w	r2, r0, r2
87807c8e:	b2db      	uxtb	r3, r3
87807c90:	2bff      	cmp	r3, #255	; 0xff
87807c92:	d1f9      	bne.n	87807c88 <lcd_pow+0x8>
87807c94:	4610      	mov	r0, r2
87807c96:	4770      	bx	lr

87807c98 <lcd_shownum>:
87807c98:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87807c9c:	b087      	sub	sp, #28
87807c9e:	f89d a040 	ldrb.w	sl, [sp, #64]	; 0x40
87807ca2:	2b00      	cmp	r3, #0
87807ca4:	d042      	beq.n	87807d2c <lcd_shownum+0x94>
87807ca6:	4617      	mov	r7, r2
87807ca8:	1e5a      	subs	r2, r3, #1
87807caa:	2500      	movs	r5, #0
87807cac:	fa5f f982 	uxtb.w	r9, r2
87807cb0:	f64c 48cd 	movw	r8, #52429	; 0xcccd
87807cb4:	ea4f 0b5a 	mov.w	fp, sl, lsr #1
87807cb8:	3b02      	subs	r3, #2
87807cba:	9104      	str	r1, [sp, #16]
87807cbc:	4604      	mov	r4, r0
87807cbe:	462e      	mov	r6, r5
87807cc0:	f6cc 48cc 	movt	r8, #52428	; 0xcccc
87807cc4:	9303      	str	r3, [sp, #12]
87807cc6:	f109 0301 	add.w	r3, r9, #1
87807cca:	9205      	str	r2, [sp, #20]
87807ccc:	9302      	str	r3, [sp, #8]
87807cce:	b2ea      	uxtb	r2, r5
87807cd0:	9b03      	ldr	r3, [sp, #12]
87807cd2:	2101      	movs	r1, #1
87807cd4:	454a      	cmp	r2, r9
87807cd6:	eba3 0302 	sub.w	r3, r3, r2
87807cda:	b2db      	uxtb	r3, r3
87807cdc:	d006      	beq.n	87807cec <lcd_shownum+0x54>
87807cde:	3b01      	subs	r3, #1
87807ce0:	00ca      	lsls	r2, r1, #3
87807ce2:	b2db      	uxtb	r3, r3
87807ce4:	eb02 0141 	add.w	r1, r2, r1, lsl #1
87807ce8:	2bff      	cmp	r3, #255	; 0xff
87807cea:	d1f8      	bne.n	87807cde <lcd_shownum+0x46>
87807cec:	4638      	mov	r0, r7
87807cee:	f001 f94f 	bl	87808f90 <__aeabi_uidiv>
87807cf2:	fba0 2308 	umull	r2, r3, r0, r8
87807cf6:	08db      	lsrs	r3, r3, #3
87807cf8:	00da      	lsls	r2, r3, #3
87807cfa:	eb02 0343 	add.w	r3, r2, r3, lsl #1
87807cfe:	1ac3      	subs	r3, r0, r3
87807d00:	b2da      	uxtb	r2, r3
87807d02:	b91e      	cbnz	r6, 87807d0c <lcd_shownum+0x74>
87807d04:	9905      	ldr	r1, [sp, #20]
87807d06:	42a9      	cmp	r1, r5
87807d08:	dd01      	ble.n	87807d0e <lcd_shownum+0x76>
87807d0a:	b193      	cbz	r3, 87807d32 <lcd_shownum+0x9a>
87807d0c:	2601      	movs	r6, #1
87807d0e:	2300      	movs	r3, #0
87807d10:	3230      	adds	r2, #48	; 0x30
87807d12:	9300      	str	r3, [sp, #0]
87807d14:	b2d2      	uxtb	r2, r2
87807d16:	4653      	mov	r3, sl
87807d18:	9904      	ldr	r1, [sp, #16]
87807d1a:	4620      	mov	r0, r4
87807d1c:	f7ff ff1a 	bl	87807b54 <lcd_showchar>
87807d20:	9b02      	ldr	r3, [sp, #8]
87807d22:	3501      	adds	r5, #1
87807d24:	445c      	add	r4, fp
87807d26:	b2a4      	uxth	r4, r4
87807d28:	429d      	cmp	r5, r3
87807d2a:	d1d0      	bne.n	87807cce <lcd_shownum+0x36>
87807d2c:	b007      	add	sp, #28
87807d2e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87807d32:	9600      	str	r6, [sp, #0]
87807d34:	4653      	mov	r3, sl
87807d36:	2220      	movs	r2, #32
87807d38:	9904      	ldr	r1, [sp, #16]
87807d3a:	4620      	mov	r0, r4
87807d3c:	f7ff ff0a 	bl	87807b54 <lcd_showchar>
87807d40:	e7ee      	b.n	87807d20 <lcd_shownum+0x88>
87807d42:	bf00      	nop

87807d44 <lcd_showxnum>:
87807d44:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87807d48:	b089      	sub	sp, #36	; 0x24
87807d4a:	f89d a048 	ldrb.w	sl, [sp, #72]	; 0x48
87807d4e:	f89d e04c 	ldrb.w	lr, [sp, #76]	; 0x4c
87807d52:	2b00      	cmp	r3, #0
87807d54:	d04a      	beq.n	87807dec <lcd_showxnum+0xa8>
87807d56:	4617      	mov	r7, r2
87807d58:	1e5a      	subs	r2, r3, #1
87807d5a:	fa5f f982 	uxtb.w	r9, r2
87807d5e:	3b02      	subs	r3, #2
87807d60:	2500      	movs	r5, #0
87807d62:	ea4f 0b5a 	mov.w	fp, sl, lsr #1
87807d66:	f64c 48cd 	movw	r8, #52429	; 0xcccd
87807d6a:	9206      	str	r2, [sp, #24]
87807d6c:	9303      	str	r3, [sp, #12]
87807d6e:	fa4f f28e 	sxtb.w	r2, lr
87807d72:	f109 0301 	add.w	r3, r9, #1
87807d76:	9105      	str	r1, [sp, #20]
87807d78:	4604      	mov	r4, r0
87807d7a:	462e      	mov	r6, r5
87807d7c:	f6cc 48cc 	movt	r8, #52428	; 0xcccc
87807d80:	9302      	str	r3, [sp, #8]
87807d82:	f00e 0301 	and.w	r3, lr, #1
87807d86:	9207      	str	r2, [sp, #28]
87807d88:	9304      	str	r3, [sp, #16]
87807d8a:	b2ea      	uxtb	r2, r5
87807d8c:	9b03      	ldr	r3, [sp, #12]
87807d8e:	2101      	movs	r1, #1
87807d90:	454a      	cmp	r2, r9
87807d92:	eba3 0302 	sub.w	r3, r3, r2
87807d96:	b2db      	uxtb	r3, r3
87807d98:	d006      	beq.n	87807da8 <lcd_showxnum+0x64>
87807d9a:	3b01      	subs	r3, #1
87807d9c:	00ca      	lsls	r2, r1, #3
87807d9e:	b2db      	uxtb	r3, r3
87807da0:	eb02 0141 	add.w	r1, r2, r1, lsl #1
87807da4:	2bff      	cmp	r3, #255	; 0xff
87807da6:	d1f8      	bne.n	87807d9a <lcd_showxnum+0x56>
87807da8:	4638      	mov	r0, r7
87807daa:	f001 f8f1 	bl	87808f90 <__aeabi_uidiv>
87807dae:	fba0 2308 	umull	r2, r3, r0, r8
87807db2:	08db      	lsrs	r3, r3, #3
87807db4:	00da      	lsls	r2, r3, #3
87807db6:	eb02 0343 	add.w	r3, r2, r3, lsl #1
87807dba:	1ac3      	subs	r3, r0, r3
87807dbc:	b2da      	uxtb	r2, r3
87807dbe:	b9c6      	cbnz	r6, 87807df2 <lcd_showxnum+0xae>
87807dc0:	9906      	ldr	r1, [sp, #24]
87807dc2:	42a9      	cmp	r1, r5
87807dc4:	dd16      	ble.n	87807df4 <lcd_showxnum+0xb0>
87807dc6:	b9a3      	cbnz	r3, 87807df2 <lcd_showxnum+0xae>
87807dc8:	9b07      	ldr	r3, [sp, #28]
87807dca:	4620      	mov	r0, r4
87807dcc:	9905      	ldr	r1, [sp, #20]
87807dce:	2b00      	cmp	r3, #0
87807dd0:	9b04      	ldr	r3, [sp, #16]
87807dd2:	bfb4      	ite	lt
87807dd4:	2230      	movlt	r2, #48	; 0x30
87807dd6:	2220      	movge	r2, #32
87807dd8:	9300      	str	r3, [sp, #0]
87807dda:	4653      	mov	r3, sl
87807ddc:	f7ff feba 	bl	87807b54 <lcd_showchar>
87807de0:	9b02      	ldr	r3, [sp, #8]
87807de2:	3501      	adds	r5, #1
87807de4:	445c      	add	r4, fp
87807de6:	b2a4      	uxth	r4, r4
87807de8:	429d      	cmp	r5, r3
87807dea:	d1ce      	bne.n	87807d8a <lcd_showxnum+0x46>
87807dec:	b009      	add	sp, #36	; 0x24
87807dee:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
87807df2:	2601      	movs	r6, #1
87807df4:	9b04      	ldr	r3, [sp, #16]
87807df6:	3230      	adds	r2, #48	; 0x30
87807df8:	b2d2      	uxtb	r2, r2
87807dfa:	9905      	ldr	r1, [sp, #20]
87807dfc:	4620      	mov	r0, r4
87807dfe:	9300      	str	r3, [sp, #0]
87807e00:	4653      	mov	r3, sl
87807e02:	f7ff fea7 	bl	87807b54 <lcd_showchar>
87807e06:	e7eb      	b.n	87807de0 <lcd_showxnum+0x9c>

87807e08 <lcd_show_string>:
87807e08:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
87807e0c:	b085      	sub	sp, #20
87807e0e:	9f0f      	ldr	r7, [sp, #60]	; 0x3c
87807e10:	4402      	add	r2, r0
87807e12:	fa1f f882 	uxth.w	r8, r2
87807e16:	440b      	add	r3, r1
87807e18:	fa1f f983 	uxth.w	r9, r3
87807e1c:	460d      	mov	r5, r1
87807e1e:	783a      	ldrb	r2, [r7, #0]
87807e20:	f89d 6038 	ldrb.w	r6, [sp, #56]	; 0x38
87807e24:	f1a2 0320 	sub.w	r3, r2, #32
87807e28:	2b5e      	cmp	r3, #94	; 0x5e
87807e2a:	d81e      	bhi.n	87807e6a <lcd_show_string+0x62>
87807e2c:	b2c3      	uxtb	r3, r0
87807e2e:	4604      	mov	r4, r0
87807e30:	ea4f 0b56 	mov.w	fp, r6, lsr #1
87807e34:	f04f 0a00 	mov.w	sl, #0
87807e38:	9303      	str	r3, [sp, #12]
87807e3a:	e00b      	b.n	87807e54 <lcd_show_string+0x4c>
87807e3c:	f8cd a000 	str.w	sl, [sp]
87807e40:	445c      	add	r4, fp
87807e42:	f7ff fe87 	bl	87807b54 <lcd_showchar>
87807e46:	f817 2f01 	ldrb.w	r2, [r7, #1]!
87807e4a:	b2a4      	uxth	r4, r4
87807e4c:	f1a2 0320 	sub.w	r3, r2, #32
87807e50:	2b5e      	cmp	r3, #94	; 0x5e
87807e52:	d80a      	bhi.n	87807e6a <lcd_show_string+0x62>
87807e54:	45a0      	cmp	r8, r4
87807e56:	eb05 0106 	add.w	r1, r5, r6
87807e5a:	bf9c      	itt	ls
87807e5c:	b28d      	uxthls	r5, r1
87807e5e:	9c03      	ldrls	r4, [sp, #12]
87807e60:	4633      	mov	r3, r6
87807e62:	454d      	cmp	r5, r9
87807e64:	4629      	mov	r1, r5
87807e66:	4620      	mov	r0, r4
87807e68:	d3e8      	bcc.n	87807e3c <lcd_show_string+0x34>
87807e6a:	b005      	add	sp, #20
87807e6c:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}

87807e70 <rtc_isleapyear.part.0>:
87807e70:	0783      	lsls	r3, r0, #30
87807e72:	d111      	bne.n	87807e98 <rtc_isleapyear.part.0+0x28>
87807e74:	f248 521f 	movw	r2, #34079	; 0x851f
87807e78:	f2c5 12eb 	movt	r2, #20971	; 0x51eb
87807e7c:	fba0 2302 	umull	r2, r3, r0, r2
87807e80:	095a      	lsrs	r2, r3, #5
87807e82:	0113      	lsls	r3, r2, #4
87807e84:	eb03 0382 	add.w	r3, r3, r2, lsl #2
87807e88:	eb03 0383 	add.w	r3, r3, r3, lsl #2
87807e8c:	1ac0      	subs	r0, r0, r3
87807e8e:	b280      	uxth	r0, r0
87807e90:	3000      	adds	r0, #0
87807e92:	bf18      	it	ne
87807e94:	2001      	movne	r0, #1
87807e96:	4770      	bx	lr
87807e98:	2000      	movs	r0, #0
87807e9a:	4770      	bx	lr

87807e9c <rtc_init>:
87807e9c:	f44f 4340 	mov.w	r3, #49152	; 0xc000
87807ea0:	f2c0 230c 	movt	r3, #524	; 0x20c
87807ea4:	6859      	ldr	r1, [r3, #4]
87807ea6:	461a      	mov	r2, r3
87807ea8:	f041 4100 	orr.w	r1, r1, #2147483648	; 0x80000000
87807eac:	f441 7180 	orr.w	r1, r1, #256	; 0x100
87807eb0:	6059      	str	r1, [r3, #4]
87807eb2:	6b99      	ldr	r1, [r3, #56]	; 0x38
87807eb4:	f041 0101 	orr.w	r1, r1, #1
87807eb8:	6399      	str	r1, [r3, #56]	; 0x38
87807eba:	6b93      	ldr	r3, [r2, #56]	; 0x38
87807ebc:	07db      	lsls	r3, r3, #31
87807ebe:	d5fc      	bpl.n	87807eba <rtc_init+0x1e>
87807ec0:	4770      	bx	lr
87807ec2:	bf00      	nop

87807ec4 <rtc_enable>:
87807ec4:	f44f 4340 	mov.w	r3, #49152	; 0xc000
87807ec8:	f2c0 230c 	movt	r3, #524	; 0x20c
87807ecc:	6b99      	ldr	r1, [r3, #56]	; 0x38
87807ece:	461a      	mov	r2, r3
87807ed0:	f041 0101 	orr.w	r1, r1, #1
87807ed4:	6399      	str	r1, [r3, #56]	; 0x38
87807ed6:	6b93      	ldr	r3, [r2, #56]	; 0x38
87807ed8:	07db      	lsls	r3, r3, #31
87807eda:	d5fc      	bpl.n	87807ed6 <rtc_enable+0x12>
87807edc:	4770      	bx	lr
87807ede:	bf00      	nop

87807ee0 <rtc_disable>:
87807ee0:	f44f 4340 	mov.w	r3, #49152	; 0xc000
87807ee4:	f2c0 230c 	movt	r3, #524	; 0x20c
87807ee8:	6b99      	ldr	r1, [r3, #56]	; 0x38
87807eea:	461a      	mov	r2, r3
87807eec:	f021 0101 	bic.w	r1, r1, #1
87807ef0:	6399      	str	r1, [r3, #56]	; 0x38
87807ef2:	6b93      	ldr	r3, [r2, #56]	; 0x38
87807ef4:	07db      	lsls	r3, r3, #31
87807ef6:	d4fc      	bmi.n	87807ef2 <rtc_disable+0x12>
87807ef8:	4770      	bx	lr
87807efa:	bf00      	nop

87807efc <rtc_isleapyear>:
87807efc:	b430      	push	{r4, r5}
87807efe:	f248 541f 	movw	r4, #34079	; 0x851f
87807f02:	f2c5 14eb 	movt	r4, #20971	; 0x51eb
87807f06:	fba0 4504 	umull	r4, r5, r0, r4
87807f0a:	09e9      	lsrs	r1, r5, #7
87807f0c:	018a      	lsls	r2, r1, #6
87807f0e:	eb02 1201 	add.w	r2, r2, r1, lsl #4
87807f12:	eb02 0282 	add.w	r2, r2, r2, lsl #2
87807f16:	1a83      	subs	r3, r0, r2
87807f18:	b29b      	uxth	r3, r3
87807f1a:	b10b      	cbz	r3, 87807f20 <rtc_isleapyear+0x24>
87807f1c:	bc30      	pop	{r4, r5}
87807f1e:	e7a7      	b.n	87807e70 <rtc_isleapyear.part.0>
87807f20:	2001      	movs	r0, #1
87807f22:	bc30      	pop	{r4, r5}
87807f24:	4770      	bx	lr
87807f26:	bf00      	nop

87807f28 <rtc_coverdate_to_seconds>:
87807f28:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
87807f2c:	f64d 3ee4 	movw	lr, #56292	; 0xdbe4
87807f30:	f2c8 7e80 	movt	lr, #34688	; 0x8780
87807f34:	4605      	mov	r5, r0
87807f36:	b088      	sub	sp, #32
87807f38:	f240 74b2 	movw	r4, #1970	; 0x7b2
87807f3c:	f8de 0000 	ldr.w	r0, [lr]
87807f40:	ae01      	add	r6, sp, #4
87807f42:	f8de 1004 	ldr.w	r1, [lr, #4]
87807f46:	f8de 2008 	ldr.w	r2, [lr, #8]
87807f4a:	f8de 300c 	ldr.w	r3, [lr, #12]
87807f4e:	882f      	ldrh	r7, [r5, #0]
87807f50:	f8be c018 	ldrh.w	ip, [lr, #24]
87807f54:	c60f      	stmia	r6!, {r0, r1, r2, r3}
87807f56:	f8de 0010 	ldr.w	r0, [lr, #16]
87807f5a:	42a7      	cmp	r7, r4
87807f5c:	f8de 1014 	ldr.w	r1, [lr, #20]
87807f60:	c603      	stmia	r6!, {r0, r1}
87807f62:	f8a6 c000 	strh.w	ip, [r6]
87807f66:	d957      	bls.n	87808018 <rtc_coverdate_to_seconds+0xf0>
87807f68:	f248 581f 	movw	r8, #34079	; 0x851f
87807f6c:	2600      	movs	r6, #0
87807f6e:	f2c5 18eb 	movt	r8, #20971	; 0x51eb
87807f72:	e008      	b.n	87807f86 <rtc_coverdate_to_seconds+0x5e>
87807f74:	f7ff ff7c 	bl	87807e70 <rtc_isleapyear.part.0>
87807f78:	b990      	cbnz	r0, 87807fa0 <rtc_coverdate_to_seconds+0x78>
87807f7a:	3401      	adds	r4, #1
87807f7c:	f206 166d 	addw	r6, r6, #365	; 0x16d
87807f80:	b2a4      	uxth	r4, r4
87807f82:	42bc      	cmp	r4, r7
87807f84:	d012      	beq.n	87807fac <rtc_coverdate_to_seconds+0x84>
87807f86:	fba4 2308 	umull	r2, r3, r4, r8
87807f8a:	4620      	mov	r0, r4
87807f8c:	09da      	lsrs	r2, r3, #7
87807f8e:	0193      	lsls	r3, r2, #6
87807f90:	eb03 1302 	add.w	r3, r3, r2, lsl #4
87807f94:	eb03 0383 	add.w	r3, r3, r3, lsl #2
87807f98:	1ae3      	subs	r3, r4, r3
87807f9a:	b29b      	uxth	r3, r3
87807f9c:	2b00      	cmp	r3, #0
87807f9e:	d1e9      	bne.n	87807f74 <rtc_coverdate_to_seconds+0x4c>
87807fa0:	3401      	adds	r4, #1
87807fa2:	f506 76b7 	add.w	r6, r6, #366	; 0x16e
87807fa6:	b2a4      	uxth	r4, r4
87807fa8:	42bc      	cmp	r4, r7
87807faa:	d1ec      	bne.n	87807f86 <rtc_coverdate_to_seconds+0x5e>
87807fac:	f248 521f 	movw	r2, #34079	; 0x851f
87807fb0:	f895 8002 	ldrb.w	r8, [r5, #2]
87807fb4:	f2c5 12eb 	movt	r2, #20971	; 0x51eb
87807fb8:	a908      	add	r1, sp, #32
87807fba:	fba7 2302 	umull	r2, r3, r7, r2
87807fbe:	eb01 0248 	add.w	r2, r1, r8, lsl #1
87807fc2:	f832 0c1c 	ldrh.w	r0, [r2, #-28]
87807fc6:	09da      	lsrs	r2, r3, #7
87807fc8:	0193      	lsls	r3, r2, #6
87807fca:	4406      	add	r6, r0
87807fcc:	eb03 1302 	add.w	r3, r3, r2, lsl #4
87807fd0:	eb03 0383 	add.w	r3, r3, r3, lsl #2
87807fd4:	1afb      	subs	r3, r7, r3
87807fd6:	b29b      	uxth	r3, r3
87807fd8:	bb33      	cbnz	r3, 87808028 <rtc_coverdate_to_seconds+0x100>
87807fda:	f1b8 0f02 	cmp.w	r8, #2
87807fde:	bf88      	it	hi
87807fe0:	3601      	addhi	r6, #1
87807fe2:	792a      	ldrb	r2, [r5, #4]
87807fe4:	f44f 47a3 	mov.w	r7, #20864	; 0x5180
87807fe8:	f895 e005 	ldrb.w	lr, [r5, #5]
87807fec:	f2c0 0701 	movt	r7, #1
87807ff0:	78eb      	ldrb	r3, [r5, #3]
87807ff2:	79a8      	ldrb	r0, [r5, #6]
87807ff4:	0211      	lsls	r1, r2, #8
87807ff6:	ea4f 148e 	mov.w	r4, lr, lsl #6
87807ffa:	3b01      	subs	r3, #1
87807ffc:	eba1 1202 	sub.w	r2, r1, r2, lsl #4
87808000:	441e      	add	r6, r3
87808002:	eba4 038e 	sub.w	r3, r4, lr, lsl #2
87808006:	ebc2 1202 	rsb	r2, r2, r2, lsl #4
8780800a:	441a      	add	r2, r3
8780800c:	4402      	add	r2, r0
8780800e:	fb07 2006 	mla	r0, r7, r6, r2
87808012:	b008      	add	sp, #32
87808014:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
87808018:	f895 8002 	ldrb.w	r8, [r5, #2]
8780801c:	ab08      	add	r3, sp, #32
8780801e:	4627      	mov	r7, r4
87808020:	eb03 0348 	add.w	r3, r3, r8, lsl #1
87808024:	f833 6c1c 	ldrh.w	r6, [r3, #-28]
87808028:	4638      	mov	r0, r7
8780802a:	f7ff ff21 	bl	87807e70 <rtc_isleapyear.part.0>
8780802e:	2800      	cmp	r0, #0
87808030:	d0d7      	beq.n	87807fe2 <rtc_coverdate_to_seconds+0xba>
87808032:	e7d2      	b.n	87807fda <rtc_coverdate_to_seconds+0xb2>

87808034 <rtc_setdatetime>:
87808034:	b538      	push	{r3, r4, r5, lr}
87808036:	f44f 4340 	mov.w	r3, #49152	; 0xc000
8780803a:	f2c0 230c 	movt	r3, #524	; 0x20c
8780803e:	461a      	mov	r2, r3
87808040:	6b9d      	ldr	r5, [r3, #56]	; 0x38
87808042:	6b99      	ldr	r1, [r3, #56]	; 0x38
87808044:	f021 0101 	bic.w	r1, r1, #1
87808048:	6399      	str	r1, [r3, #56]	; 0x38
8780804a:	6b93      	ldr	r3, [r2, #56]	; 0x38
8780804c:	f44f 4440 	mov.w	r4, #49152	; 0xc000
87808050:	f2c0 240c 	movt	r4, #524	; 0x20c
87808054:	07d9      	lsls	r1, r3, #31
87808056:	d4f8      	bmi.n	8780804a <rtc_setdatetime+0x16>
87808058:	f7ff ff66 	bl	87807f28 <rtc_coverdate_to_seconds>
8780805c:	07ea      	lsls	r2, r5, #31
8780805e:	ea4f 4350 	mov.w	r3, r0, lsr #17
87808062:	ea4f 30c0 	mov.w	r0, r0, lsl #15
87808066:	6523      	str	r3, [r4, #80]	; 0x50
87808068:	6560      	str	r0, [r4, #84]	; 0x54
8780806a:	d507      	bpl.n	8780807c <rtc_setdatetime+0x48>
8780806c:	6ba3      	ldr	r3, [r4, #56]	; 0x38
8780806e:	4622      	mov	r2, r4
87808070:	f043 0301 	orr.w	r3, r3, #1
87808074:	63a3      	str	r3, [r4, #56]	; 0x38
87808076:	6b93      	ldr	r3, [r2, #56]	; 0x38
87808078:	07db      	lsls	r3, r3, #31
8780807a:	d5fc      	bpl.n	87808076 <rtc_setdatetime+0x42>
8780807c:	bd38      	pop	{r3, r4, r5, pc}
8780807e:	bf00      	nop

87808080 <rtc_convertseconds_to_datetime>:
87808080:	f244 5207 	movw	r2, #17671	; 0x4507
87808084:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
87808088:	f2cc 222e 	movt	r2, #49710	; 0xc22e
8780808c:	f44f 4ca3 	mov.w	ip, #20864	; 0x5180
87808090:	fba0 2302 	umull	r2, r3, r0, r2
87808094:	f2c0 0c01 	movt	ip, #1
87808098:	f24b 38c5 	movw	r8, #46021	; 0xb3c5
8780809c:	f8df e104 	ldr.w	lr, [pc, #260]	; 878081a4 <rtc_convertseconds_to_datetime+0x124>
878080a0:	f2c9 18a2 	movt	r8, #37282	; 0x91a2
878080a4:	460f      	mov	r7, r1
878080a6:	b085      	sub	sp, #20
878080a8:	f8de 1004 	ldr.w	r1, [lr, #4]
878080ac:	466c      	mov	r4, sp
878080ae:	0c1d      	lsrs	r5, r3, #16
878080b0:	f8de 2008 	ldr.w	r2, [lr, #8]
878080b4:	f648 0689 	movw	r6, #34953	; 0x8889
878080b8:	fb0c 0315 	mls	r3, ip, r5, r0
878080bc:	f8de 0000 	ldr.w	r0, [lr]
878080c0:	f89e e00c 	ldrb.w	lr, [lr, #12]
878080c4:	f6c8 0688 	movt	r6, #34952	; 0x8888
878080c8:	c407      	stmia	r4!, {r0, r1, r2}
878080ca:	f240 116d 	movw	r1, #365	; 0x16d
878080ce:	fba3 8908 	umull	r8, r9, r3, r8
878080d2:	f248 581f 	movw	r8, #34079	; 0x851f
878080d6:	3501      	adds	r5, #1
878080d8:	f2c5 18eb 	movt	r8, #20971	; 0x51eb
878080dc:	f884 e000 	strb.w	lr, [r4]
878080e0:	f240 74b2 	movw	r4, #1970	; 0x7b2
878080e4:	803c      	strh	r4, [r7, #0]
878080e6:	ea4f 20d9 	mov.w	r0, r9, lsr #11
878080ea:	0202      	lsls	r2, r0, #8
878080ec:	7138      	strb	r0, [r7, #4]
878080ee:	eba2 1000 	sub.w	r0, r2, r0, lsl #4
878080f2:	ebc0 1000 	rsb	r0, r0, r0, lsl #4
878080f6:	1a18      	subs	r0, r3, r0
878080f8:	fba0 2306 	umull	r2, r3, r0, r6
878080fc:	460e      	mov	r6, r1
878080fe:	095a      	lsrs	r2, r3, #5
87808100:	0193      	lsls	r3, r2, #6
87808102:	717a      	strb	r2, [r7, #5]
87808104:	eba3 0382 	sub.w	r3, r3, r2, lsl #2
87808108:	1ac3      	subs	r3, r0, r3
8780810a:	71bb      	strb	r3, [r7, #6]
8780810c:	428d      	cmp	r5, r1
8780810e:	f104 0301 	add.w	r3, r4, #1
87808112:	d91c      	bls.n	8780814e <rtc_convertseconds_to_datetime+0xce>
87808114:	b29c      	uxth	r4, r3
87808116:	1a6d      	subs	r5, r5, r1
87808118:	f44f 71b7 	mov.w	r1, #366	; 0x16e
8780811c:	fba4 2308 	umull	r2, r3, r4, r8
87808120:	4620      	mov	r0, r4
87808122:	803c      	strh	r4, [r7, #0]
87808124:	09da      	lsrs	r2, r3, #7
87808126:	0193      	lsls	r3, r2, #6
87808128:	eb03 1302 	add.w	r3, r3, r2, lsl #4
8780812c:	eb03 0383 	add.w	r3, r3, r3, lsl #2
87808130:	1ae3      	subs	r3, r4, r3
87808132:	b29b      	uxth	r3, r3
87808134:	2b00      	cmp	r3, #0
87808136:	d0e9      	beq.n	8780810c <rtc_convertseconds_to_datetime+0x8c>
87808138:	f7ff fe9a 	bl	87807e70 <rtc_isleapyear.part.0>
8780813c:	2800      	cmp	r0, #0
8780813e:	f104 0301 	add.w	r3, r4, #1
87808142:	bf0c      	ite	eq
87808144:	4631      	moveq	r1, r6
87808146:	f44f 71b7 	movne.w	r1, #366	; 0x16e
8780814a:	428d      	cmp	r5, r1
8780814c:	d8e2      	bhi.n	87808114 <rtc_convertseconds_to_datetime+0x94>
8780814e:	f248 521f 	movw	r2, #34079	; 0x851f
87808152:	f2c5 12eb 	movt	r2, #20971	; 0x51eb
87808156:	fba4 2302 	umull	r2, r3, r4, r2
8780815a:	09da      	lsrs	r2, r3, #7
8780815c:	0193      	lsls	r3, r2, #6
8780815e:	eb03 1302 	add.w	r3, r3, r2, lsl #4
87808162:	eb03 0383 	add.w	r3, r3, r3, lsl #2
87808166:	1ae3      	subs	r3, r4, r3
87808168:	b29b      	uxth	r3, r3
8780816a:	b18b      	cbz	r3, 87808190 <rtc_convertseconds_to_datetime+0x110>
8780816c:	4620      	mov	r0, r4
8780816e:	f7ff fe7f 	bl	87807e70 <rtc_isleapyear.part.0>
87808172:	b968      	cbnz	r0, 87808190 <rtc_convertseconds_to_datetime+0x110>
87808174:	4669      	mov	r1, sp
87808176:	2301      	movs	r3, #1
87808178:	f811 2f01 	ldrb.w	r2, [r1, #1]!
8780817c:	42aa      	cmp	r2, r5
8780817e:	d20b      	bcs.n	87808198 <rtc_convertseconds_to_datetime+0x118>
87808180:	3301      	adds	r3, #1
87808182:	1aad      	subs	r5, r5, r2
87808184:	2b0d      	cmp	r3, #13
87808186:	d1f7      	bne.n	87808178 <rtc_convertseconds_to_datetime+0xf8>
87808188:	70fd      	strb	r5, [r7, #3]
8780818a:	b005      	add	sp, #20
8780818c:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
87808190:	231d      	movs	r3, #29
87808192:	f88d 3002 	strb.w	r3, [sp, #2]
87808196:	e7ed      	b.n	87808174 <rtc_convertseconds_to_datetime+0xf4>
87808198:	70bb      	strb	r3, [r7, #2]
8780819a:	70fd      	strb	r5, [r7, #3]
8780819c:	b005      	add	sp, #20
8780819e:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
878081a2:	bf00      	nop
878081a4:	8780dc00 	strhi	sp, [r0, r0, lsl #24]

878081a8 <rtc_getseconds>:
878081a8:	f44f 4240 	mov.w	r2, #49152	; 0xc000
878081ac:	f2c0 220c 	movt	r2, #524	; 0x20c
878081b0:	6d13      	ldr	r3, [r2, #80]	; 0x50
878081b2:	6d50      	ldr	r0, [r2, #84]	; 0x54
878081b4:	045b      	lsls	r3, r3, #17
878081b6:	ea43 30d0 	orr.w	r0, r3, r0, lsr #15
878081ba:	4770      	bx	lr

878081bc <rtc_getdatetime>:
878081bc:	f44f 4240 	mov.w	r2, #49152	; 0xc000
878081c0:	4601      	mov	r1, r0
878081c2:	f2c0 220c 	movt	r2, #524	; 0x20c
878081c6:	6d13      	ldr	r3, [r2, #80]	; 0x50
878081c8:	6d50      	ldr	r0, [r2, #84]	; 0x54
878081ca:	045b      	lsls	r3, r3, #17
878081cc:	ea43 30d0 	orr.w	r0, r3, r0, lsr #15
878081d0:	f7ff bf56 	b.w	87808080 <rtc_convertseconds_to_datetime>

878081d4 <i2c_check_and_clear_error.part.1>:
878081d4:	4602      	mov	r2, r0
878081d6:	2004      	movs	r0, #4
878081d8:	8993      	ldrh	r3, [r2, #12]
878081da:	f023 0310 	bic.w	r3, r3, #16
878081de:	041b      	lsls	r3, r3, #16
878081e0:	0c1b      	lsrs	r3, r3, #16
878081e2:	8193      	strh	r3, [r2, #12]
878081e4:	8913      	ldrh	r3, [r2, #8]
878081e6:	f023 0380 	bic.w	r3, r3, #128	; 0x80
878081ea:	041b      	lsls	r3, r3, #16
878081ec:	0c1b      	lsrs	r3, r3, #16
878081ee:	8113      	strh	r3, [r2, #8]
878081f0:	8913      	ldrh	r3, [r2, #8]
878081f2:	b29b      	uxth	r3, r3
878081f4:	f043 0380 	orr.w	r3, r3, #128	; 0x80
878081f8:	8113      	strh	r3, [r2, #8]
878081fa:	4770      	bx	lr

878081fc <i2c_init>:
878081fc:	8903      	ldrh	r3, [r0, #8]
878081fe:	2215      	movs	r2, #21
87808200:	f023 0380 	bic.w	r3, r3, #128	; 0x80
87808204:	041b      	lsls	r3, r3, #16
87808206:	0c1b      	lsrs	r3, r3, #16
87808208:	8103      	strh	r3, [r0, #8]
8780820a:	8082      	strh	r2, [r0, #4]
8780820c:	8903      	ldrh	r3, [r0, #8]
8780820e:	b29b      	uxth	r3, r3
87808210:	f043 0380 	orr.w	r3, r3, #128	; 0x80
87808214:	8103      	strh	r3, [r0, #8]
87808216:	4770      	bx	lr

87808218 <i2c_master_repeated_start>:
87808218:	4603      	mov	r3, r0
8780821a:	8980      	ldrh	r0, [r0, #12]
8780821c:	b410      	push	{r4}
8780821e:	0684      	lsls	r4, r0, #26
87808220:	d504      	bpl.n	8780822c <i2c_master_repeated_start+0x14>
87808222:	8918      	ldrh	r0, [r3, #8]
87808224:	0680      	lsls	r0, r0, #26
87808226:	bf58      	it	pl
87808228:	2001      	movpl	r0, #1
8780822a:	d50d      	bpl.n	87808248 <i2c_master_repeated_start+0x30>
8780822c:	891c      	ldrh	r4, [r3, #8]
8780822e:	f1a2 0201 	sub.w	r2, r2, #1
87808232:	fab2 f282 	clz	r2, r2
87808236:	2000      	movs	r0, #0
87808238:	0952      	lsrs	r2, r2, #5
8780823a:	ea42 0141 	orr.w	r1, r2, r1, lsl #1
8780823e:	b2a2      	uxth	r2, r4
87808240:	f042 0214 	orr.w	r2, r2, #20
87808244:	811a      	strh	r2, [r3, #8]
87808246:	8219      	strh	r1, [r3, #16]
87808248:	f85d 4b04 	ldr.w	r4, [sp], #4
8780824c:	4770      	bx	lr
8780824e:	bf00      	nop

87808250 <i2c_master_start>:
87808250:	8983      	ldrh	r3, [r0, #12]
87808252:	f003 0320 	and.w	r3, r3, #32
87808256:	b29b      	uxth	r3, r3
87808258:	b98b      	cbnz	r3, 8780827e <i2c_master_start+0x2e>
8780825a:	b410      	push	{r4}
8780825c:	f1a2 0201 	sub.w	r2, r2, #1
87808260:	8904      	ldrh	r4, [r0, #8]
87808262:	fab2 f282 	clz	r2, r2
87808266:	0952      	lsrs	r2, r2, #5
87808268:	ea42 0141 	orr.w	r1, r2, r1, lsl #1
8780826c:	b2a2      	uxth	r2, r4
8780826e:	f85d 4b04 	ldr.w	r4, [sp], #4
87808272:	f042 0230 	orr.w	r2, r2, #48	; 0x30
87808276:	8102      	strh	r2, [r0, #8]
87808278:	8201      	strh	r1, [r0, #16]
8780827a:	4618      	mov	r0, r3
8780827c:	4770      	bx	lr
8780827e:	2301      	movs	r3, #1
87808280:	4618      	mov	r0, r3
87808282:	4770      	bx	lr

87808284 <i2c_check_and_clear_error>:
87808284:	06cb      	lsls	r3, r1, #27
87808286:	d500      	bpl.n	8780828a <i2c_check_and_clear_error+0x6>
87808288:	e7a4      	b.n	878081d4 <i2c_check_and_clear_error.part.1>
8780828a:	f011 0f01 	tst.w	r1, #1
8780828e:	bf14      	ite	ne
87808290:	2003      	movne	r0, #3
87808292:	2000      	moveq	r0, #0
87808294:	4770      	bx	lr
87808296:	bf00      	nop

87808298 <i2c_master_stop>:
87808298:	8903      	ldrh	r3, [r0, #8]
8780829a:	f64f 72ff 	movw	r2, #65535	; 0xffff
8780829e:	f023 0338 	bic.w	r3, r3, #56	; 0x38
878082a2:	041b      	lsls	r3, r3, #16
878082a4:	0c1b      	lsrs	r3, r3, #16
878082a6:	8103      	strh	r3, [r0, #8]
878082a8:	e000      	b.n	878082ac <i2c_master_stop+0x14>
878082aa:	b14a      	cbz	r2, 878082c0 <i2c_master_stop+0x28>
878082ac:	8983      	ldrh	r3, [r0, #12]
878082ae:	3a01      	subs	r2, #1
878082b0:	b292      	uxth	r2, r2
878082b2:	f003 0320 	and.w	r3, r3, #32
878082b6:	b29b      	uxth	r3, r3
878082b8:	2b00      	cmp	r3, #0
878082ba:	d1f6      	bne.n	878082aa <i2c_master_stop+0x12>
878082bc:	4618      	mov	r0, r3
878082be:	4770      	bx	lr
878082c0:	2005      	movs	r0, #5
878082c2:	4770      	bx	lr

878082c4 <i2c_master_write>:
878082c4:	b570      	push	{r4, r5, r6, lr}
878082c6:	4604      	mov	r4, r0
878082c8:	460d      	mov	r5, r1
878082ca:	89a3      	ldrh	r3, [r4, #12]
878082cc:	0619      	lsls	r1, r3, #24
878082ce:	d5fc      	bpl.n	878082ca <i2c_master_write+0x6>
878082d0:	89a3      	ldrh	r3, [r4, #12]
878082d2:	18ae      	adds	r6, r5, r2
878082d4:	f023 0302 	bic.w	r3, r3, #2
878082d8:	041b      	lsls	r3, r3, #16
878082da:	0c1b      	lsrs	r3, r3, #16
878082dc:	81a3      	strh	r3, [r4, #12]
878082de:	8923      	ldrh	r3, [r4, #8]
878082e0:	b29b      	uxth	r3, r3
878082e2:	f043 0310 	orr.w	r3, r3, #16
878082e6:	8123      	strh	r3, [r4, #8]
878082e8:	42b5      	cmp	r5, r6
878082ea:	d011      	beq.n	87808310 <i2c_master_write+0x4c>
878082ec:	f815 3b01 	ldrb.w	r3, [r5], #1
878082f0:	8223      	strh	r3, [r4, #16]
878082f2:	89a3      	ldrh	r3, [r4, #12]
878082f4:	079b      	lsls	r3, r3, #30
878082f6:	d5fc      	bpl.n	878082f2 <i2c_master_write+0x2e>
878082f8:	89a3      	ldrh	r3, [r4, #12]
878082fa:	f023 0302 	bic.w	r3, r3, #2
878082fe:	041b      	lsls	r3, r3, #16
87808300:	0c1b      	lsrs	r3, r3, #16
87808302:	81a3      	strh	r3, [r4, #12]
87808304:	89a3      	ldrh	r3, [r4, #12]
87808306:	b29b      	uxth	r3, r3
87808308:	06d8      	lsls	r0, r3, #27
8780830a:	d417      	bmi.n	8780833c <i2c_master_write+0x78>
8780830c:	07d9      	lsls	r1, r3, #31
8780830e:	d5eb      	bpl.n	878082e8 <i2c_master_write+0x24>
87808310:	89a2      	ldrh	r2, [r4, #12]
87808312:	f64f 73ff 	movw	r3, #65535	; 0xffff
87808316:	f022 0202 	bic.w	r2, r2, #2
8780831a:	0412      	lsls	r2, r2, #16
8780831c:	0c12      	lsrs	r2, r2, #16
8780831e:	81a2      	strh	r2, [r4, #12]
87808320:	8922      	ldrh	r2, [r4, #8]
87808322:	f022 0238 	bic.w	r2, r2, #56	; 0x38
87808326:	0412      	lsls	r2, r2, #16
87808328:	0c12      	lsrs	r2, r2, #16
8780832a:	8122      	strh	r2, [r4, #8]
8780832c:	e000      	b.n	87808330 <i2c_master_write+0x6c>
8780832e:	b123      	cbz	r3, 8780833a <i2c_master_write+0x76>
87808330:	89a2      	ldrh	r2, [r4, #12]
87808332:	3b01      	subs	r3, #1
87808334:	b29b      	uxth	r3, r3
87808336:	0692      	lsls	r2, r2, #26
87808338:	d4f9      	bmi.n	8780832e <i2c_master_write+0x6a>
8780833a:	bd70      	pop	{r4, r5, r6, pc}
8780833c:	4620      	mov	r0, r4
8780833e:	f7ff ff49 	bl	878081d4 <i2c_check_and_clear_error.part.1>
87808342:	2800      	cmp	r0, #0
87808344:	d0d0      	beq.n	878082e8 <i2c_master_write+0x24>
87808346:	e7e3      	b.n	87808310 <i2c_master_write+0x4c>

87808348 <i2c_master_read>:
87808348:	b430      	push	{r4, r5}
8780834a:	2300      	movs	r3, #0
8780834c:	b082      	sub	sp, #8
8780834e:	f88d 3007 	strb.w	r3, [sp, #7]
87808352:	f89d 3007 	ldrb.w	r3, [sp, #7]
87808356:	3301      	adds	r3, #1
87808358:	b2db      	uxtb	r3, r3
8780835a:	f88d 3007 	strb.w	r3, [sp, #7]
8780835e:	8983      	ldrh	r3, [r0, #12]
87808360:	061c      	lsls	r4, r3, #24
87808362:	d5fc      	bpl.n	8780835e <i2c_master_read+0x16>
87808364:	8983      	ldrh	r3, [r0, #12]
87808366:	2a01      	cmp	r2, #1
87808368:	f023 0302 	bic.w	r3, r3, #2
8780836c:	ea4f 4303 	mov.w	r3, r3, lsl #16
87808370:	ea4f 4313 	mov.w	r3, r3, lsr #16
87808374:	8183      	strh	r3, [r0, #12]
87808376:	8903      	ldrh	r3, [r0, #8]
87808378:	f023 0318 	bic.w	r3, r3, #24
8780837c:	ea4f 4303 	mov.w	r3, r3, lsl #16
87808380:	ea4f 4313 	mov.w	r3, r3, lsr #16
87808384:	8103      	strh	r3, [r0, #8]
87808386:	d02d      	beq.n	878083e4 <i2c_master_read+0x9c>
87808388:	8a03      	ldrh	r3, [r0, #16]
8780838a:	1e55      	subs	r5, r2, #1
8780838c:	b2db      	uxtb	r3, r3
8780838e:	f88d 3007 	strb.w	r3, [sp, #7]
87808392:	b322      	cbz	r2, 878083de <i2c_master_read+0x96>
87808394:	462a      	mov	r2, r5
87808396:	460c      	mov	r4, r1
87808398:	8983      	ldrh	r3, [r0, #12]
8780839a:	079b      	lsls	r3, r3, #30
8780839c:	d5fc      	bpl.n	87808398 <i2c_master_read+0x50>
8780839e:	8983      	ldrh	r3, [r0, #12]
878083a0:	f023 0302 	bic.w	r3, r3, #2
878083a4:	041b      	lsls	r3, r3, #16
878083a6:	0c1b      	lsrs	r3, r3, #16
878083a8:	8183      	strh	r3, [r0, #12]
878083aa:	b132      	cbz	r2, 878083ba <i2c_master_read+0x72>
878083ac:	2a01      	cmp	r2, #1
878083ae:	d024      	beq.n	878083fa <i2c_master_read+0xb2>
878083b0:	8a03      	ldrh	r3, [r0, #16]
878083b2:	3a01      	subs	r2, #1
878083b4:	f804 3b01 	strb.w	r3, [r4], #1
878083b8:	e7ee      	b.n	87808398 <i2c_master_read+0x50>
878083ba:	8903      	ldrh	r3, [r0, #8]
878083bc:	4429      	add	r1, r5
878083be:	f64f 72ff 	movw	r2, #65535	; 0xffff
878083c2:	f023 0338 	bic.w	r3, r3, #56	; 0x38
878083c6:	041b      	lsls	r3, r3, #16
878083c8:	0c1b      	lsrs	r3, r3, #16
878083ca:	8103      	strh	r3, [r0, #8]
878083cc:	e000      	b.n	878083d0 <i2c_master_read+0x88>
878083ce:	b122      	cbz	r2, 878083da <i2c_master_read+0x92>
878083d0:	8983      	ldrh	r3, [r0, #12]
878083d2:	3a01      	subs	r2, #1
878083d4:	b292      	uxth	r2, r2
878083d6:	069b      	lsls	r3, r3, #26
878083d8:	d4f9      	bmi.n	878083ce <i2c_master_read+0x86>
878083da:	8a03      	ldrh	r3, [r0, #16]
878083dc:	700b      	strb	r3, [r1, #0]
878083de:	b002      	add	sp, #8
878083e0:	bc30      	pop	{r4, r5}
878083e2:	4770      	bx	lr
878083e4:	8903      	ldrh	r3, [r0, #8]
878083e6:	2500      	movs	r5, #0
878083e8:	b29b      	uxth	r3, r3
878083ea:	f043 0308 	orr.w	r3, r3, #8
878083ee:	8103      	strh	r3, [r0, #8]
878083f0:	8a03      	ldrh	r3, [r0, #16]
878083f2:	b2db      	uxtb	r3, r3
878083f4:	f88d 3007 	strb.w	r3, [sp, #7]
878083f8:	e7cc      	b.n	87808394 <i2c_master_read+0x4c>
878083fa:	8903      	ldrh	r3, [r0, #8]
878083fc:	b29b      	uxth	r3, r3
878083fe:	f043 0308 	orr.w	r3, r3, #8
87808402:	8103      	strh	r3, [r0, #8]
87808404:	e7d4      	b.n	878083b0 <i2c_master_read+0x68>
87808406:	bf00      	nop

87808408 <i2c_master_transfer>:
87808408:	b538      	push	{r3, r4, r5, lr}
8780840a:	8983      	ldrh	r3, [r0, #12]
8780840c:	684a      	ldr	r2, [r1, #4]
8780840e:	f023 0312 	bic.w	r3, r3, #18
87808412:	041b      	lsls	r3, r3, #16
87808414:	0c1b      	lsrs	r3, r3, #16
87808416:	8183      	strh	r3, [r0, #12]
87808418:	8983      	ldrh	r3, [r0, #12]
8780841a:	061b      	lsls	r3, r3, #24
8780841c:	d5fc      	bpl.n	87808418 <i2c_master_transfer+0x10>
8780841e:	7b0b      	ldrb	r3, [r1, #12]
87808420:	b14b      	cbz	r3, 87808436 <i2c_master_transfer+0x2e>
87808422:	8983      	ldrh	r3, [r0, #12]
87808424:	780c      	ldrb	r4, [r1, #0]
87808426:	f003 0320 	and.w	r3, r3, #32
8780842a:	b29b      	uxth	r3, r3
8780842c:	2b00      	cmp	r3, #0
8780842e:	f000 8087 	beq.w	87808540 <i2c_master_transfer+0x138>
87808432:	2001      	movs	r0, #1
87808434:	bd38      	pop	{r3, r4, r5, pc}
87808436:	8984      	ldrh	r4, [r0, #12]
87808438:	780b      	ldrb	r3, [r1, #0]
8780843a:	06a5      	lsls	r5, r4, #26
8780843c:	d4f9      	bmi.n	87808432 <i2c_master_transfer+0x2a>
8780843e:	8905      	ldrh	r5, [r0, #8]
87808440:	005c      	lsls	r4, r3, #1
87808442:	f1a2 0301 	sub.w	r3, r2, #1
87808446:	fab3 f383 	clz	r3, r3
8780844a:	095b      	lsrs	r3, r3, #5
8780844c:	b2aa      	uxth	r2, r5
8780844e:	f042 0230 	orr.w	r2, r2, #48	; 0x30
87808452:	8102      	strh	r2, [r0, #8]
87808454:	4323      	orrs	r3, r4
87808456:	b29b      	uxth	r3, r3
87808458:	8203      	strh	r3, [r0, #16]
8780845a:	8983      	ldrh	r3, [r0, #12]
8780845c:	079c      	lsls	r4, r3, #30
8780845e:	d5fc      	bpl.n	8780845a <i2c_master_transfer+0x52>
87808460:	8983      	ldrh	r3, [r0, #12]
87808462:	460d      	mov	r5, r1
87808464:	4604      	mov	r4, r0
87808466:	b29b      	uxth	r3, r3
87808468:	06da      	lsls	r2, r3, #27
8780846a:	d413      	bmi.n	87808494 <i2c_master_transfer+0x8c>
8780846c:	07db      	lsls	r3, r3, #31
8780846e:	d515      	bpl.n	8780849c <i2c_master_transfer+0x94>
87808470:	2003      	movs	r0, #3
87808472:	8922      	ldrh	r2, [r4, #8]
87808474:	f64f 73ff 	movw	r3, #65535	; 0xffff
87808478:	f022 0238 	bic.w	r2, r2, #56	; 0x38
8780847c:	0412      	lsls	r2, r2, #16
8780847e:	0c12      	lsrs	r2, r2, #16
87808480:	8122      	strh	r2, [r4, #8]
87808482:	e000      	b.n	87808486 <i2c_master_transfer+0x7e>
87808484:	b12b      	cbz	r3, 87808492 <i2c_master_transfer+0x8a>
87808486:	89a2      	ldrh	r2, [r4, #12]
87808488:	3b01      	subs	r3, #1
8780848a:	b29b      	uxth	r3, r3
8780848c:	0695      	lsls	r5, r2, #26
8780848e:	d4f9      	bmi.n	87808484 <i2c_master_transfer+0x7c>
87808490:	bd38      	pop	{r3, r4, r5, pc}
87808492:	bd38      	pop	{r3, r4, r5, pc}
87808494:	f7ff fe9e 	bl	878081d4 <i2c_check_and_clear_error.part.1>
87808498:	2800      	cmp	r0, #0
8780849a:	d1ea      	bne.n	87808472 <i2c_master_transfer+0x6a>
8780849c:	7b2b      	ldrb	r3, [r5, #12]
8780849e:	b96b      	cbnz	r3, 878084bc <i2c_master_transfer+0xb4>
878084a0:	686b      	ldr	r3, [r5, #4]
878084a2:	2b00      	cmp	r3, #0
878084a4:	d141      	bne.n	8780852a <i2c_master_transfer+0x122>
878084a6:	696b      	ldr	r3, [r5, #20]
878084a8:	2b00      	cmp	r3, #0
878084aa:	d138      	bne.n	8780851e <i2c_master_transfer+0x116>
878084ac:	2000      	movs	r0, #0
878084ae:	bd38      	pop	{r3, r4, r5, pc}
878084b0:	4620      	mov	r0, r4
878084b2:	f7ff fe8f 	bl	878081d4 <i2c_check_and_clear_error.part.1>
878084b6:	b9d8      	cbnz	r0, 878084f0 <i2c_master_transfer+0xe8>
878084b8:	7b2b      	ldrb	r3, [r5, #12]
878084ba:	b353      	cbz	r3, 87808512 <i2c_master_transfer+0x10a>
878084bc:	89a3      	ldrh	r3, [r4, #12]
878084be:	68aa      	ldr	r2, [r5, #8]
878084c0:	f023 0302 	bic.w	r3, r3, #2
878084c4:	041b      	lsls	r3, r3, #16
878084c6:	0c1b      	lsrs	r3, r3, #16
878084c8:	81a3      	strh	r3, [r4, #12]
878084ca:	7b2b      	ldrb	r3, [r5, #12]
878084cc:	3b01      	subs	r3, #1
878084ce:	b2db      	uxtb	r3, r3
878084d0:	00d9      	lsls	r1, r3, #3
878084d2:	732b      	strb	r3, [r5, #12]
878084d4:	fa22 f301 	lsr.w	r3, r2, r1
878084d8:	b29b      	uxth	r3, r3
878084da:	8223      	strh	r3, [r4, #16]
878084dc:	89a3      	ldrh	r3, [r4, #12]
878084de:	0799      	lsls	r1, r3, #30
878084e0:	d5fc      	bpl.n	878084dc <i2c_master_transfer+0xd4>
878084e2:	89a3      	ldrh	r3, [r4, #12]
878084e4:	b29b      	uxth	r3, r3
878084e6:	06da      	lsls	r2, r3, #27
878084e8:	d4e2      	bmi.n	878084b0 <i2c_master_transfer+0xa8>
878084ea:	07db      	lsls	r3, r3, #31
878084ec:	d5e4      	bpl.n	878084b8 <i2c_master_transfer+0xb0>
878084ee:	2003      	movs	r0, #3
878084f0:	8922      	ldrh	r2, [r4, #8]
878084f2:	f64f 73ff 	movw	r3, #65535	; 0xffff
878084f6:	f022 0238 	bic.w	r2, r2, #56	; 0x38
878084fa:	0412      	lsls	r2, r2, #16
878084fc:	0c12      	lsrs	r2, r2, #16
878084fe:	8122      	strh	r2, [r4, #8]
87808500:	e001      	b.n	87808506 <i2c_master_transfer+0xfe>
87808502:	2b00      	cmp	r3, #0
87808504:	d0c5      	beq.n	87808492 <i2c_master_transfer+0x8a>
87808506:	89a2      	ldrh	r2, [r4, #12]
87808508:	3b01      	subs	r3, #1
8780850a:	b29b      	uxth	r3, r3
8780850c:	0695      	lsls	r5, r2, #26
8780850e:	d4f8      	bmi.n	87808502 <i2c_master_transfer+0xfa>
87808510:	bd38      	pop	{r3, r4, r5, pc}
87808512:	686b      	ldr	r3, [r5, #4]
87808514:	2b01      	cmp	r3, #1
87808516:	d01a      	beq.n	8780854e <i2c_master_transfer+0x146>
87808518:	2b00      	cmp	r3, #0
8780851a:	d0c4      	beq.n	878084a6 <i2c_master_transfer+0x9e>
8780851c:	e7c6      	b.n	878084ac <i2c_master_transfer+0xa4>
8780851e:	6929      	ldr	r1, [r5, #16]
87808520:	4620      	mov	r0, r4
87808522:	696a      	ldr	r2, [r5, #20]
87808524:	f7ff fece 	bl	878082c4 <i2c_master_write>
87808528:	686b      	ldr	r3, [r5, #4]
8780852a:	2b01      	cmp	r3, #1
8780852c:	d1be      	bne.n	878084ac <i2c_master_transfer+0xa4>
8780852e:	696b      	ldr	r3, [r5, #20]
87808530:	2b00      	cmp	r3, #0
87808532:	d0bb      	beq.n	878084ac <i2c_master_transfer+0xa4>
87808534:	6929      	ldr	r1, [r5, #16]
87808536:	4620      	mov	r0, r4
87808538:	696a      	ldr	r2, [r5, #20]
8780853a:	f7ff ff05 	bl	87808348 <i2c_master_read>
8780853e:	e7b5      	b.n	878084ac <i2c_master_transfer+0xa4>
87808540:	8902      	ldrh	r2, [r0, #8]
87808542:	0064      	lsls	r4, r4, #1
87808544:	b292      	uxth	r2, r2
87808546:	f042 0230 	orr.w	r2, r2, #48	; 0x30
8780854a:	8102      	strh	r2, [r0, #8]
8780854c:	e782      	b.n	87808454 <i2c_master_transfer+0x4c>
8780854e:	89a3      	ldrh	r3, [r4, #12]
87808550:	f023 0302 	bic.w	r3, r3, #2
87808554:	041b      	lsls	r3, r3, #16
87808556:	0c1b      	lsrs	r3, r3, #16
87808558:	81a3      	strh	r3, [r4, #12]
8780855a:	89a2      	ldrh	r2, [r4, #12]
8780855c:	782b      	ldrb	r3, [r5, #0]
8780855e:	0691      	lsls	r1, r2, #26
87808560:	d502      	bpl.n	87808568 <i2c_master_transfer+0x160>
87808562:	8922      	ldrh	r2, [r4, #8]
87808564:	0692      	lsls	r2, r2, #26
87808566:	d508      	bpl.n	8780857a <i2c_master_transfer+0x172>
87808568:	8922      	ldrh	r2, [r4, #8]
8780856a:	005b      	lsls	r3, r3, #1
8780856c:	f043 0301 	orr.w	r3, r3, #1
87808570:	b292      	uxth	r2, r2
87808572:	f042 0214 	orr.w	r2, r2, #20
87808576:	8122      	strh	r2, [r4, #8]
87808578:	8223      	strh	r3, [r4, #16]
8780857a:	89a3      	ldrh	r3, [r4, #12]
8780857c:	079b      	lsls	r3, r3, #30
8780857e:	d5fc      	bpl.n	8780857a <i2c_master_transfer+0x172>
87808580:	89a3      	ldrh	r3, [r4, #12]
87808582:	b29b      	uxth	r3, r3
87808584:	06d8      	lsls	r0, r3, #27
87808586:	d412      	bmi.n	878085ae <i2c_master_transfer+0x1a6>
87808588:	07d9      	lsls	r1, r3, #31
8780858a:	d5d0      	bpl.n	8780852e <i2c_master_transfer+0x126>
8780858c:	8922      	ldrh	r2, [r4, #8]
8780858e:	f64f 73ff 	movw	r3, #65535	; 0xffff
87808592:	f022 0238 	bic.w	r2, r2, #56	; 0x38
87808596:	0412      	lsls	r2, r2, #16
87808598:	0c12      	lsrs	r2, r2, #16
8780859a:	8122      	strh	r2, [r4, #8]
8780859c:	e000      	b.n	878085a0 <i2c_master_transfer+0x198>
8780859e:	b123      	cbz	r3, 878085aa <i2c_master_transfer+0x1a2>
878085a0:	89a2      	ldrh	r2, [r4, #12]
878085a2:	3b01      	subs	r3, #1
878085a4:	b29b      	uxth	r3, r3
878085a6:	0692      	lsls	r2, r2, #26
878085a8:	d4f9      	bmi.n	8780859e <i2c_master_transfer+0x196>
878085aa:	2006      	movs	r0, #6
878085ac:	bd38      	pop	{r3, r4, r5, pc}
878085ae:	4620      	mov	r0, r4
878085b0:	f7ff fe10 	bl	878081d4 <i2c_check_and_clear_error.part.1>
878085b4:	2800      	cmp	r0, #0
878085b6:	d1e9      	bne.n	8780858c <i2c_master_transfer+0x184>
878085b8:	e772      	b.n	878084a0 <i2c_master_transfer+0x98>
878085ba:	bf00      	nop

878085bc <ap3216c_writeonebyte>:
878085bc:	b530      	push	{r4, r5, lr}
878085be:	b089      	sub	sp, #36	; 0x24
878085c0:	f88d 0008 	strb.w	r0, [sp, #8]
878085c4:	2000      	movs	r0, #0
878085c6:	2301      	movs	r3, #1
878085c8:	2400      	movs	r4, #0
878085ca:	9104      	str	r1, [sp, #16]
878085cc:	f10d 0507 	add.w	r5, sp, #7
878085d0:	a902      	add	r1, sp, #8
878085d2:	f2c0 201a 	movt	r0, #538	; 0x21a
878085d6:	9506      	str	r5, [sp, #24]
878085d8:	f88d 2007 	strb.w	r2, [sp, #7]
878085dc:	9403      	str	r4, [sp, #12]
878085de:	f88d 3014 	strb.w	r3, [sp, #20]
878085e2:	9307      	str	r3, [sp, #28]
878085e4:	f7ff ff10 	bl	87808408 <i2c_master_transfer>
878085e8:	1b00      	subs	r0, r0, r4
878085ea:	bf18      	it	ne
878085ec:	2001      	movne	r0, #1
878085ee:	b009      	add	sp, #36	; 0x24
878085f0:	bd30      	pop	{r4, r5, pc}
878085f2:	bf00      	nop

878085f4 <ap3216c_readonebyte>:
878085f4:	b510      	push	{r4, lr}
878085f6:	b088      	sub	sp, #32
878085f8:	f88d 0008 	strb.w	r0, [sp, #8]
878085fc:	2000      	movs	r0, #0
878085fe:	2301      	movs	r3, #1
87808600:	2200      	movs	r2, #0
87808602:	9104      	str	r1, [sp, #16]
87808604:	f10d 0407 	add.w	r4, sp, #7
87808608:	a902      	add	r1, sp, #8
8780860a:	f2c0 201a 	movt	r0, #538	; 0x21a
8780860e:	9406      	str	r4, [sp, #24]
87808610:	f88d 2007 	strb.w	r2, [sp, #7]
87808614:	9303      	str	r3, [sp, #12]
87808616:	f88d 3014 	strb.w	r3, [sp, #20]
8780861a:	9307      	str	r3, [sp, #28]
8780861c:	f7ff fef4 	bl	87808408 <i2c_master_transfer>
87808620:	f89d 0007 	ldrb.w	r0, [sp, #7]
87808624:	b008      	add	sp, #32
87808626:	bd10      	pop	{r4, pc}

87808628 <ap3216c_init>:
87808628:	20b4      	movs	r0, #180	; 0xb4
8780862a:	f240 52a4 	movw	r2, #1444	; 0x5a4
8780862e:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87808630:	f2c0 200e 	movt	r0, #526	; 0x20e
87808634:	f04f 0e01 	mov.w	lr, #1
87808638:	2612      	movs	r6, #18
8780863a:	f2c0 220e 	movt	r2, #526	; 0x20e
8780863e:	25b8      	movs	r5, #184	; 0xb8
87808640:	f44f 64b5 	mov.w	r4, #1448	; 0x5a8
87808644:	f44f 7150 	mov.w	r1, #832	; 0x340
87808648:	f44f 7351 	mov.w	r3, #836	; 0x344
8780864c:	6006      	str	r6, [r0, #0]
8780864e:	f2c0 230e 	movt	r3, #526	; 0x20e
87808652:	f8c2 e000 	str.w	lr, [r2]
87808656:	f2c0 210e 	movt	r1, #526	; 0x20e
8780865a:	f247 02b0 	movw	r2, #28848	; 0x70b0
8780865e:	f2c0 250e 	movt	r5, #526	; 0x20e
87808662:	f2c0 240e 	movt	r4, #526	; 0x20e
87808666:	2702      	movs	r7, #2
87808668:	2000      	movs	r0, #0
8780866a:	602e      	str	r6, [r5, #0]
8780866c:	f2c0 201a 	movt	r0, #538	; 0x21a
87808670:	6027      	str	r7, [r4, #0]
87808672:	600a      	str	r2, [r1, #0]
87808674:	601a      	str	r2, [r3, #0]
87808676:	f7ff fdc1 	bl	878081fc <i2c_init>
8780867a:	2204      	movs	r2, #4
8780867c:	2100      	movs	r1, #0
8780867e:	201e      	movs	r0, #30
87808680:	f7ff ff9c 	bl	878085bc <ap3216c_writeonebyte>
87808684:	2032      	movs	r0, #50	; 0x32
87808686:	f7fe fa29 	bl	87806adc <delayms>
8780868a:	2203      	movs	r2, #3
8780868c:	2100      	movs	r1, #0
8780868e:	201e      	movs	r0, #30
87808690:	f7ff ff94 	bl	878085bc <ap3216c_writeonebyte>
87808694:	2100      	movs	r1, #0
87808696:	201e      	movs	r0, #30
87808698:	f7ff ffac 	bl	878085f4 <ap3216c_readonebyte>
8780869c:	3803      	subs	r0, #3
8780869e:	bf18      	it	ne
878086a0:	2001      	movne	r0, #1
878086a2:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}

878086a4 <ap3216c_readdata>:
878086a4:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
878086a8:	b082      	sub	sp, #8
878086aa:	4680      	mov	r8, r0
878086ac:	460e      	mov	r6, r1
878086ae:	4617      	mov	r7, r2
878086b0:	466d      	mov	r5, sp
878086b2:	240a      	movs	r4, #10
878086b4:	4621      	mov	r1, r4
878086b6:	3401      	adds	r4, #1
878086b8:	b2e4      	uxtb	r4, r4
878086ba:	201e      	movs	r0, #30
878086bc:	f7ff ff9a 	bl	878085f4 <ap3216c_readonebyte>
878086c0:	2c10      	cmp	r4, #16
878086c2:	f805 0b01 	strb.w	r0, [r5], #1
878086c6:	d1f5      	bne.n	878086b4 <ap3216c_readdata+0x10>
878086c8:	f89d 3000 	ldrb.w	r3, [sp]
878086cc:	0619      	lsls	r1, r3, #24
878086ce:	f89d 1003 	ldrb.w	r1, [sp, #3]
878086d2:	bf5b      	ittet	pl
878086d4:	f89d 2001 	ldrbpl.w	r2, [sp, #1]
878086d8:	f003 0303 	andpl.w	r3, r3, #3
878086dc:	2300      	movmi	r3, #0
878086de:	ea43 0382 	orrpl.w	r3, r3, r2, lsl #2
878086e2:	f89d 2002 	ldrb.w	r2, [sp, #2]
878086e6:	f8a8 3000 	strh.w	r3, [r8]
878086ea:	f89d 3004 	ldrb.w	r3, [sp, #4]
878086ee:	ea42 2201 	orr.w	r2, r2, r1, lsl #8
878086f2:	803a      	strh	r2, [r7, #0]
878086f4:	065a      	lsls	r2, r3, #25
878086f6:	bf5b      	ittet	pl
878086f8:	f89d 2005 	ldrbpl.w	r2, [sp, #5]
878086fc:	f003 030f 	andpl.w	r3, r3, #15
87808700:	2300      	movmi	r3, #0
87808702:	f002 023f 	andpl.w	r2, r2, #63	; 0x3f
87808706:	bf58      	it	pl
87808708:	ea43 1302 	orrpl.w	r3, r3, r2, lsl #4
8780870c:	8033      	strh	r3, [r6, #0]
8780870e:	b002      	add	sp, #8
87808710:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}

87808714 <spi_init>:
87808714:	2200      	movs	r2, #0
87808716:	6082      	str	r2, [r0, #8]
87808718:	6883      	ldr	r3, [r0, #8]
8780871a:	f443 03e0 	orr.w	r3, r3, #7340032	; 0x700000
8780871e:	f043 0319 	orr.w	r3, r3, #25
87808722:	6083      	str	r3, [r0, #8]
87808724:	60c2      	str	r2, [r0, #12]
87808726:	61c2      	str	r2, [r0, #28]
87808728:	6883      	ldr	r3, [r0, #8]
8780872a:	f423 437f 	bic.w	r3, r3, #65280	; 0xff00
8780872e:	6083      	str	r3, [r0, #8]
87808730:	6883      	ldr	r3, [r0, #8]
87808732:	f443 4310 	orr.w	r3, r3, #36864	; 0x9000
87808736:	6083      	str	r3, [r0, #8]
87808738:	4770      	bx	lr
8780873a:	bf00      	nop

8780873c <spich0_readwrite_byte>:
8780873c:	6883      	ldr	r3, [r0, #8]
8780873e:	f423 2340 	bic.w	r3, r3, #786432	; 0xc0000
87808742:	6083      	str	r3, [r0, #8]
87808744:	6883      	ldr	r3, [r0, #8]
87808746:	6083      	str	r3, [r0, #8]
87808748:	6983      	ldr	r3, [r0, #24]
8780874a:	07da      	lsls	r2, r3, #31
8780874c:	d5fc      	bpl.n	87808748 <spich0_readwrite_byte+0xc>
8780874e:	6041      	str	r1, [r0, #4]
87808750:	6983      	ldr	r3, [r0, #24]
87808752:	071b      	lsls	r3, r3, #28
87808754:	d5fc      	bpl.n	87808750 <spich0_readwrite_byte+0x14>
87808756:	6800      	ldr	r0, [r0, #0]
87808758:	b2c0      	uxtb	r0, r0
8780875a:	4770      	bx	lr
8780875c:	0000      	movs	r0, r0
	...

87808760 <icm20608_write_reg>:
87808760:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
87808762:	f44f 4440 	mov.w	r4, #49152	; 0xc000
87808766:	f2c0 2409 	movt	r4, #521	; 0x209
8780876a:	2500      	movs	r5, #0
8780876c:	462a      	mov	r2, r5
8780876e:	4607      	mov	r7, r0
87808770:	460e      	mov	r6, r1
87808772:	4620      	mov	r0, r4
87808774:	f2c0 2501 	movt	r5, #513	; 0x201
87808778:	2114      	movs	r1, #20
8780877a:	f7fe fa23 	bl	87806bc4 <gpio_pinwrite>
8780877e:	f007 017f 	and.w	r1, r7, #127	; 0x7f
87808782:	4628      	mov	r0, r5
87808784:	f7ff ffda 	bl	8780873c <spich0_readwrite_byte>
87808788:	4631      	mov	r1, r6
8780878a:	4628      	mov	r0, r5
8780878c:	f7ff ffd6 	bl	8780873c <spich0_readwrite_byte>
87808790:	4620      	mov	r0, r4
87808792:	2201      	movs	r2, #1
87808794:	2114      	movs	r1, #20
87808796:	e8bd 40f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, lr}
8780879a:	f7fe ba13 	b.w	87806bc4 <gpio_pinwrite>
8780879e:	bf00      	nop

878087a0 <icm20608_read_reg>:
878087a0:	b570      	push	{r4, r5, r6, lr}
878087a2:	f44f 4440 	mov.w	r4, #49152	; 0xc000
878087a6:	f2c0 2409 	movt	r4, #521	; 0x209
878087aa:	2500      	movs	r5, #0
878087ac:	462a      	mov	r2, r5
878087ae:	4606      	mov	r6, r0
878087b0:	f2c0 2501 	movt	r5, #513	; 0x201
878087b4:	4620      	mov	r0, r4
878087b6:	2114      	movs	r1, #20
878087b8:	f7fe fa04 	bl	87806bc4 <gpio_pinwrite>
878087bc:	f046 0180 	orr.w	r1, r6, #128	; 0x80
878087c0:	4628      	mov	r0, r5
878087c2:	f7ff ffbb 	bl	8780873c <spich0_readwrite_byte>
878087c6:	4628      	mov	r0, r5
878087c8:	21ff      	movs	r1, #255	; 0xff
878087ca:	f7ff ffb7 	bl	8780873c <spich0_readwrite_byte>
878087ce:	2201      	movs	r2, #1
878087d0:	4605      	mov	r5, r0
878087d2:	2114      	movs	r1, #20
878087d4:	4620      	mov	r0, r4
878087d6:	f7fe f9f5 	bl	87806bc4 <gpio_pinwrite>
878087da:	4628      	mov	r0, r5
878087dc:	bd70      	pop	{r4, r5, r6, pc}
878087de:	bf00      	nop

878087e0 <icm20608_init>:
878087e0:	2098      	movs	r0, #152	; 0x98
878087e2:	b5f0      	push	{r4, r5, r6, r7, lr}
878087e4:	2308      	movs	r3, #8
878087e6:	f2c0 200e 	movt	r0, #526	; 0x20e
878087ea:	f240 5e54 	movw	lr, #1364	; 0x554
878087ee:	249c      	movs	r4, #156	; 0x9c
878087f0:	f240 515c 	movw	r1, #1372	; 0x55c
878087f4:	22a0      	movs	r2, #160	; 0xa0
878087f6:	6003      	str	r3, [r0, #0]
878087f8:	2500      	movs	r5, #0
878087fa:	f2c0 2e0e 	movt	lr, #526	; 0x20e
878087fe:	f2c0 240e 	movt	r4, #526	; 0x20e
87808802:	f2c0 210e 	movt	r1, #526	; 0x20e
87808806:	f2c0 220e 	movt	r2, #526	; 0x20e
8780880a:	f44f 67ab 	mov.w	r7, #1368	; 0x558
8780880e:	f44f 7649 	mov.w	r6, #804	; 0x324
87808812:	f44f 704a 	mov.w	r0, #808	; 0x328
87808816:	f8ce 5000 	str.w	r5, [lr]
8780881a:	f2c0 270e 	movt	r7, #526	; 0x20e
8780881e:	6023      	str	r3, [r4, #0]
87808820:	f2c0 260e 	movt	r6, #526	; 0x20e
87808824:	600d      	str	r5, [r1, #0]
87808826:	f2c0 200e 	movt	r0, #526	; 0x20e
8780882a:	6013      	str	r3, [r2, #0]
8780882c:	f44f 744b 	mov.w	r4, #812	; 0x32c
87808830:	f241 03b1 	movw	r3, #4273	; 0x10b1
87808834:	2194      	movs	r1, #148	; 0x94
87808836:	f44f 7248 	mov.w	r2, #800	; 0x320
8780883a:	b085      	sub	sp, #20
8780883c:	603d      	str	r5, [r7, #0]
8780883e:	f04f 0e05 	mov.w	lr, #5
87808842:	6033      	str	r3, [r6, #0]
87808844:	f2c0 240e 	movt	r4, #526	; 0x20e
87808848:	6003      	str	r3, [r0, #0]
8780884a:	f2c0 210e 	movt	r1, #526	; 0x20e
8780884e:	f2c0 220e 	movt	r2, #526	; 0x20e
87808852:	f241 07b0 	movw	r7, #4272	; 0x10b0
87808856:	f44f 4040 	mov.w	r0, #49152	; 0xc000
8780885a:	6023      	str	r3, [r4, #0]
8780885c:	2601      	movs	r6, #1
8780885e:	f8c1 e000 	str.w	lr, [r1]
87808862:	f2c0 2009 	movt	r0, #521	; 0x209
87808866:	6017      	str	r7, [r2, #0]
87808868:	2114      	movs	r1, #20
8780886a:	aa01      	add	r2, sp, #4
8780886c:	f88d 5008 	strb.w	r5, [sp, #8]
87808870:	9601      	str	r6, [sp, #4]
87808872:	f7fe f9f5 	bl	87806c60 <gpio_init>
87808876:	4628      	mov	r0, r5
87808878:	f2c0 2001 	movt	r0, #513	; 0x201
8780887c:	f7ff ff4a 	bl	87808714 <spi_init>
87808880:	2180      	movs	r1, #128	; 0x80
87808882:	206b      	movs	r0, #107	; 0x6b
87808884:	f7ff ff6c 	bl	87808760 <icm20608_write_reg>
87808888:	2032      	movs	r0, #50	; 0x32
8780888a:	f7fe f927 	bl	87806adc <delayms>
8780888e:	4631      	mov	r1, r6
87808890:	206b      	movs	r0, #107	; 0x6b
87808892:	f7ff ff65 	bl	87808760 <icm20608_write_reg>
87808896:	2032      	movs	r0, #50	; 0x32
87808898:	f7fe f920 	bl	87806adc <delayms>
8780889c:	2075      	movs	r0, #117	; 0x75
8780889e:	f7ff ff7f 	bl	878087a0 <icm20608_read_reg>
878088a2:	4604      	mov	r4, r0
878088a4:	4601      	mov	r1, r0
878088a6:	3452      	adds	r4, #82	; 0x52
878088a8:	f64d 6044 	movw	r0, #56900	; 0xde44
878088ac:	b2e4      	uxtb	r4, r4
878088ae:	f2c8 7080 	movt	r0, #34688	; 0x8780
878088b2:	f7fc fbcb 	bl	8780504c <printf>
878088b6:	42b4      	cmp	r4, r6
878088b8:	bf88      	it	hi
878088ba:	4630      	movhi	r0, r6
878088bc:	d901      	bls.n	878088c2 <icm20608_init+0xe2>
878088be:	b005      	add	sp, #20
878088c0:	bdf0      	pop	{r4, r5, r6, r7, pc}
878088c2:	4629      	mov	r1, r5
878088c4:	2019      	movs	r0, #25
878088c6:	f7ff ff4b 	bl	87808760 <icm20608_write_reg>
878088ca:	2118      	movs	r1, #24
878088cc:	201b      	movs	r0, #27
878088ce:	f7ff ff47 	bl	87808760 <icm20608_write_reg>
878088d2:	2118      	movs	r1, #24
878088d4:	201c      	movs	r0, #28
878088d6:	f7ff ff43 	bl	87808760 <icm20608_write_reg>
878088da:	2104      	movs	r1, #4
878088dc:	201a      	movs	r0, #26
878088de:	f7ff ff3f 	bl	87808760 <icm20608_write_reg>
878088e2:	2104      	movs	r1, #4
878088e4:	201d      	movs	r0, #29
878088e6:	f7ff ff3b 	bl	87808760 <icm20608_write_reg>
878088ea:	4629      	mov	r1, r5
878088ec:	206c      	movs	r0, #108	; 0x6c
878088ee:	f7ff ff37 	bl	87808760 <icm20608_write_reg>
878088f2:	4629      	mov	r1, r5
878088f4:	201e      	movs	r0, #30
878088f6:	f7ff ff33 	bl	87808760 <icm20608_write_reg>
878088fa:	4629      	mov	r1, r5
878088fc:	2023      	movs	r0, #35	; 0x23
878088fe:	f7ff ff2f 	bl	87808760 <icm20608_write_reg>
87808902:	4628      	mov	r0, r5
87808904:	b005      	add	sp, #20
87808906:	bdf0      	pop	{r4, r5, r6, r7, pc}

87808908 <icm20608_read_len>:
87808908:	b570      	push	{r4, r5, r6, lr}
8780890a:	4606      	mov	r6, r0
8780890c:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87808910:	4614      	mov	r4, r2
87808912:	f2c0 2009 	movt	r0, #521	; 0x209
87808916:	460d      	mov	r5, r1
87808918:	2200      	movs	r2, #0
8780891a:	2114      	movs	r1, #20
8780891c:	f7fe f952 	bl	87806bc4 <gpio_pinwrite>
87808920:	2000      	movs	r0, #0
87808922:	f046 0180 	orr.w	r1, r6, #128	; 0x80
87808926:	f2c0 2001 	movt	r0, #513	; 0x201
8780892a:	f7ff ff07 	bl	8780873c <spich0_readwrite_byte>
8780892e:	b16c      	cbz	r4, 8780894c <icm20608_read_len+0x44>
87808930:	1e63      	subs	r3, r4, #1
87808932:	1e6c      	subs	r4, r5, #1
87808934:	b2db      	uxtb	r3, r3
87808936:	441d      	add	r5, r3
87808938:	2000      	movs	r0, #0
8780893a:	21ff      	movs	r1, #255	; 0xff
8780893c:	f2c0 2001 	movt	r0, #513	; 0x201
87808940:	f7ff fefc 	bl	8780873c <spich0_readwrite_byte>
87808944:	f804 0f01 	strb.w	r0, [r4, #1]!
87808948:	42ac      	cmp	r4, r5
8780894a:	d1f5      	bne.n	87808938 <icm20608_read_len+0x30>
8780894c:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87808950:	2201      	movs	r2, #1
87808952:	f2c0 2009 	movt	r0, #521	; 0x209
87808956:	2114      	movs	r1, #20
87808958:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
8780895c:	f7fe b932 	b.w	87806bc4 <gpio_pinwrite>

87808960 <icm20608_gyro_scaleget>:
87808960:	b508      	push	{r3, lr}
87808962:	201b      	movs	r0, #27
87808964:	f7ff ff1c 	bl	878087a0 <icm20608_read_reg>
87808968:	f3c0 00c1 	ubfx	r0, r0, #3, #2
8780896c:	2802      	cmp	r0, #2
8780896e:	d00a      	beq.n	87808986 <icm20608_gyro_scaleget+0x26>
87808970:	2803      	cmp	r0, #3
87808972:	d00b      	beq.n	8780898c <icm20608_gyro_scaleget+0x2c>
87808974:	eddf 7a07 	vldr	s15, [pc, #28]	; 87808994 <icm20608_gyro_scaleget+0x34>
87808978:	2801      	cmp	r0, #1
8780897a:	ed9f 0a07 	vldr	s0, [pc, #28]	; 87808998 <icm20608_gyro_scaleget+0x38>
8780897e:	bf08      	it	eq
87808980:	eeb0 0a67 	vmoveq.f32	s0, s15
87808984:	bd08      	pop	{r3, pc}
87808986:	ed9f 0a05 	vldr	s0, [pc, #20]	; 8780899c <icm20608_gyro_scaleget+0x3c>
8780898a:	bd08      	pop	{r3, pc}
8780898c:	ed9f 0a04 	vldr	s0, [pc, #16]	; 878089a0 <icm20608_gyro_scaleget+0x40>
87808990:	bd08      	pop	{r3, pc}
87808992:	bf00      	nop
87808994:	42830000 	addmi	r0, r3, #0
87808998:	43030000 	movwmi	r0, #12288	; 0x3000
8780899c:	42033333 	andmi	r3, r3, #-872415232	; 0xcc000000
878089a0:	41833333 	orrmi	r3, r3, r3, lsr r3

878089a4 <icm20608_accel_scaleget>:
878089a4:	b508      	push	{r3, lr}
878089a6:	201c      	movs	r0, #28
878089a8:	f7ff fefa 	bl	878087a0 <icm20608_read_reg>
878089ac:	f3c0 00c1 	ubfx	r0, r0, #3, #2
878089b0:	2802      	cmp	r0, #2
878089b2:	d008      	beq.n	878089c6 <icm20608_accel_scaleget+0x22>
878089b4:	2803      	cmp	r0, #3
878089b6:	d009      	beq.n	878089cc <icm20608_accel_scaleget+0x28>
878089b8:	2801      	cmp	r0, #1
878089ba:	bf0c      	ite	eq
878089bc:	f44f 5000 	moveq.w	r0, #8192	; 0x2000
878089c0:	f44f 4080 	movne.w	r0, #16384	; 0x4000
878089c4:	bd08      	pop	{r3, pc}
878089c6:	f44f 5080 	mov.w	r0, #4096	; 0x1000
878089ca:	bd08      	pop	{r3, pc}
878089cc:	f44f 6000 	mov.w	r0, #2048	; 0x800
878089d0:	bd08      	pop	{r3, pc}
878089d2:	bf00      	nop

878089d4 <icm20608_getdata>:
878089d4:	b5f0      	push	{r4, r5, r6, r7, lr}
878089d6:	220e      	movs	r2, #14
878089d8:	ed2d 8b02 	vpush	{d8}
878089dc:	b085      	sub	sp, #20
878089de:	4669      	mov	r1, sp
878089e0:	203b      	movs	r0, #59	; 0x3b
878089e2:	f7ff ff91 	bl	87808908 <icm20608_read_len>
878089e6:	f7ff ffbb 	bl	87808960 <icm20608_gyro_scaleget>
878089ea:	eeb0 8a40 	vmov.f32	s16, s0
878089ee:	f7ff ffd9 	bl	878089a4 <icm20608_accel_scaleget>
878089f2:	f89d 1007 	ldrb.w	r1, [sp, #7]
878089f6:	f24b 1354 	movw	r3, #45396	; 0xb154
878089fa:	f89d 6006 	ldrb.w	r6, [sp, #6]
878089fe:	f2c8 7381 	movt	r3, #34689	; 0x8781
87808a02:	f89d 2009 	ldrb.w	r2, [sp, #9]
87808a06:	f89d c008 	ldrb.w	ip, [sp, #8]
87808a0a:	ee07 0a90 	vmov	s15, r0
87808a0e:	ea41 2606 	orr.w	r6, r1, r6, lsl #8
87808a12:	eef8 5ae7 	vcvt.f32.s32	s11, s15
87808a16:	eeb3 6a09 	vmov.f32	s12, #57	; 0x39
87808a1a:	b236      	sxth	r6, r6
87808a1c:	ea42 2c0c 	orr.w	ip, r2, ip, lsl #8
87808a20:	f89d 100b 	ldrb.w	r1, [sp, #11]
87808a24:	fa0f fc8c 	sxth.w	ip, ip
87808a28:	f89d e00a 	ldrb.w	lr, [sp, #10]
87808a2c:	ee07 6a90 	vmov	s15, r6
87808a30:	f89d 200d 	ldrb.w	r2, [sp, #13]
87808a34:	eef8 7ae7 	vcvt.f32.s32	s15, s15
87808a38:	f89d 500c 	ldrb.w	r5, [sp, #12]
87808a3c:	ea41 2e0e 	orr.w	lr, r1, lr, lsl #8
87808a40:	f89d 7000 	ldrb.w	r7, [sp]
87808a44:	fa0f fe8e 	sxth.w	lr, lr
87808a48:	f89d 1003 	ldrb.w	r1, [sp, #3]
87808a4c:	ee37 6ac6 	vsub.f32	s12, s15, s12
87808a50:	ee07 ca90 	vmov	s15, ip
87808a54:	ea42 2505 	orr.w	r5, r2, r5, lsl #8
87808a58:	eeb8 4ae7 	vcvt.f32.s32	s8, s15
87808a5c:	f89d 2001 	ldrb.w	r2, [sp, #1]
87808a60:	b22d      	sxth	r5, r5
87808a62:	ee07 ea90 	vmov	s15, lr
87808a66:	f89d 4002 	ldrb.w	r4, [sp, #2]
87808a6a:	ee84 4a08 	vdiv.f32	s8, s8, s16
87808a6e:	ed9f 3b38 	vldr	d3, [pc, #224]	; 87808b50 <icm20608_getdata+0x17c>
87808a72:	ea42 2707 	orr.w	r7, r2, r7, lsl #8
87808a76:	f89d 0004 	ldrb.w	r0, [sp, #4]
87808a7a:	f89d 2005 	ldrb.w	r2, [sp, #5]
87808a7e:	ea41 2404 	orr.w	r4, r1, r4, lsl #8
87808a82:	eeb7 6ac6 	vcvt.f64.f32	d6, s12
87808a86:	b23f      	sxth	r7, r7
87808a88:	eef8 7ae7 	vcvt.f32.s32	s15, s15
87808a8c:	b224      	sxth	r4, r4
87808a8e:	ed9f 1a34 	vldr	s2, [pc, #208]	; 87808b60 <icm20608_getdata+0x18c>
87808a92:	ea42 2000 	orr.w	r0, r2, r0, lsl #8
87808a96:	ee07 5a10 	vmov	s14, r5
87808a9a:	ee86 6b03 	vdiv.f64	d6, d6, d3
87808a9e:	eec7 4a88 	vdiv.f32	s9, s15, s16
87808aa2:	b200      	sxth	r0, r0
87808aa4:	ee07 4a90 	vmov	s15, r4
87808aa8:	eeb8 0ac7 	vcvt.f32.s32	s0, s14
87808aac:	ee07 7a10 	vmov	s14, r7
87808ab0:	eef8 1ae7 	vcvt.f32.s32	s3, s15
87808ab4:	ee07 0a90 	vmov	s15, r0
87808ab8:	eeb8 5ac7 	vcvt.f32.s32	s10, s14
87808abc:	eef8 7ae7 	vcvt.f32.s32	s15, s15
87808ac0:	ee24 7a01 	vmul.f32	s14, s8, s2
87808ac4:	eec1 1aa5 	vdiv.f32	s3, s3, s11
87808ac8:	ee85 5a25 	vdiv.f32	s10, s10, s11
87808acc:	eec7 5aa5 	vdiv.f32	s11, s15, s11
87808ad0:	eefd 7ac7 	vcvt.s32.f32	s15, s14
87808ad4:	ee80 0a08 	vdiv.f32	s0, s0, s16
87808ad8:	eeb3 2b09 	vmov.f64	d2, #57	; 0x39
87808adc:	ed9f 3b1e 	vldr	d3, [pc, #120]	; 87808b58 <icm20608_getdata+0x184>
87808ae0:	ee17 1a90 	vmov	r1, s15
87808ae4:	ee64 7a81 	vmul.f32	s15, s9, s2
87808ae8:	619e      	str	r6, [r3, #24]
87808aea:	e883 5000 	stmia.w	r3, {ip, lr}
87808aee:	609d      	str	r5, [r3, #8]
87808af0:	eefd 7ae7 	vcvt.s32.f32	s15, s15
87808af4:	61d9      	str	r1, [r3, #28]
87808af6:	60df      	str	r7, [r3, #12]
87808af8:	611c      	str	r4, [r3, #16]
87808afa:	ee17 2a90 	vmov	r2, s15
87808afe:	ee36 7b02 	vadd.f64	d7, d6, d2
87808b02:	ee25 5a01 	vmul.f32	s10, s10, s2
87808b06:	ee20 0a01 	vmul.f32	s0, s0, s2
87808b0a:	ee21 6a81 	vmul.f32	s12, s3, s2
87808b0e:	ee27 7b03 	vmul.f64	d7, d7, d3
87808b12:	eebd 5ac5 	vcvt.s32.f32	s10, s10
87808b16:	ee65 6a81 	vmul.f32	s13, s11, s2
87808b1a:	eebd 0ac0 	vcvt.s32.f32	s0, s0
87808b1e:	eebd 6ac6 	vcvt.s32.f32	s12, s12
87808b22:	eebd 7bc7 	vcvt.s32.f64	s14, d7
87808b26:	eefd 6ae6 	vcvt.s32.f32	s13, s13
87808b2a:	621a      	str	r2, [r3, #32]
87808b2c:	ed83 0a09 	vstr	s0, [r3, #36]	; 0x24
87808b30:	ed83 5a0a 	vstr	s10, [r3, #40]	; 0x28
87808b34:	ed83 6a0b 	vstr	s12, [r3, #44]	; 0x2c
87808b38:	6158      	str	r0, [r3, #20]
87808b3a:	edc3 6a0c 	vstr	s13, [r3, #48]	; 0x30
87808b3e:	ed83 7a0d 	vstr	s14, [r3, #52]	; 0x34
87808b42:	b005      	add	sp, #20
87808b44:	ecbd 8b02 	vpop	{d8}
87808b48:	bdf0      	pop	{r4, r5, r6, r7, pc}
87808b4a:	bf00      	nop
87808b4c:	f3af 8000 	nop.w
87808b50:	cccccccd 	stclgt	12, cr12, [ip], {205}	; 0xcd
87808b54:	40746ccc 	rsbsmi	r6, r4, ip, asr #25
87808b58:	00000000 	andeq	r0, r0, r0
87808b5c:	40590000 	subsmi	r0, r9, r0
87808b60:	42c80000 	sbcmi	r0, r8, #0
87808b64:	f3af 8000 	nop.w

87808b68 <ft5426_write_byte>:
87808b68:	b530      	push	{r4, r5, lr}
87808b6a:	b089      	sub	sp, #36	; 0x24
87808b6c:	f88d 0008 	strb.w	r0, [sp, #8]
87808b70:	f44f 4080 	mov.w	r0, #16384	; 0x4000
87808b74:	2301      	movs	r3, #1
87808b76:	2400      	movs	r4, #0
87808b78:	9104      	str	r1, [sp, #16]
87808b7a:	f10d 0507 	add.w	r5, sp, #7
87808b7e:	a902      	add	r1, sp, #8
87808b80:	f2c0 201a 	movt	r0, #538	; 0x21a
87808b84:	9506      	str	r5, [sp, #24]
87808b86:	f88d 2007 	strb.w	r2, [sp, #7]
87808b8a:	9403      	str	r4, [sp, #12]
87808b8c:	f88d 3014 	strb.w	r3, [sp, #20]
87808b90:	9307      	str	r3, [sp, #28]
87808b92:	f7ff fc39 	bl	87808408 <i2c_master_transfer>
87808b96:	1b00      	subs	r0, r0, r4
87808b98:	bf18      	it	ne
87808b9a:	2001      	movne	r0, #1
87808b9c:	b009      	add	sp, #36	; 0x24
87808b9e:	bd30      	pop	{r4, r5, pc}

87808ba0 <touchscreen_init>:
87808ba0:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
87808ba4:	21bc      	movs	r1, #188	; 0xbc
87808ba6:	f24b 168c 	movw	r6, #45452	; 0xb18c
87808baa:	f240 59ac 	movw	r9, #1452	; 0x5ac
87808bae:	22c0      	movs	r2, #192	; 0xc0
87808bb0:	2400      	movs	r4, #0
87808bb2:	f04f 0e12 	mov.w	lr, #18
87808bb6:	f04f 0802 	mov.w	r8, #2
87808bba:	f2c8 7681 	movt	r6, #34689	; 0x8781
87808bbe:	f2c0 210e 	movt	r1, #526	; 0x20e
87808bc2:	f2c0 290e 	movt	r9, #526	; 0x20e
87808bc6:	f2c0 220e 	movt	r2, #526	; 0x20e
87808bca:	f44f 65b6 	mov.w	r5, #1456	; 0x5b0
87808bce:	f44f 7c52 	mov.w	ip, #840	; 0x348
87808bd2:	f44f 7353 	mov.w	r3, #844	; 0x34c
87808bd6:	7034      	strb	r4, [r6, #0]
87808bd8:	f2c0 2c0e 	movt	ip, #526	; 0x20e
87808bdc:	f8c1 e000 	str.w	lr, [r1]
87808be0:	f2c0 250e 	movt	r5, #526	; 0x20e
87808be4:	f8c9 8000 	str.w	r8, [r9]
87808be8:	f2c0 230e 	movt	r3, #526	; 0x20e
87808bec:	f8c2 e000 	str.w	lr, [r2]
87808bf0:	2780      	movs	r7, #128	; 0x80
87808bf2:	f247 0eb0 	movw	lr, #28848	; 0x70b0
87808bf6:	f8c5 8000 	str.w	r8, [r5]
87808bfa:	f2c0 270e 	movt	r7, #526	; 0x20e
87808bfe:	f8cc e000 	str.w	lr, [ip]
87808c02:	202c      	movs	r0, #44	; 0x2c
87808c04:	f8c3 e000 	str.w	lr, [r3]
87808c08:	f44f 7143 	mov.w	r1, #780	; 0x30c
87808c0c:	2305      	movs	r3, #5
87808c0e:	2270      	movs	r2, #112	; 0x70
87808c10:	f44f 4540 	mov.w	r5, #49152	; 0xc000
87808c14:	603b      	str	r3, [r7, #0]
87808c16:	b08b      	sub	sp, #44	; 0x2c
87808c18:	f24f 0e80 	movw	lr, #61568	; 0xf080
87808c1c:	f2c0 2029 	movt	r0, #553	; 0x229
87808c20:	f2c0 210e 	movt	r1, #526	; 0x20e
87808c24:	f2c0 2229 	movt	r2, #553	; 0x229
87808c28:	f2c0 2509 	movt	r5, #521	; 0x209
87808c2c:	f241 07b0 	movw	r7, #4272	; 0x10b0
87808c30:	6003      	str	r3, [r0, #0]
87808c32:	f8c1 e000 	str.w	lr, [r1]
87808c36:	4628      	mov	r0, r5
87808c38:	6017      	str	r7, [r2, #0]
87808c3a:	2109      	movs	r1, #9
87808c3c:	aa01      	add	r2, sp, #4
87808c3e:	9303      	str	r3, [sp, #12]
87808c40:	9401      	str	r4, [sp, #4]
87808c42:	f7fe f80d 	bl	87806c60 <gpio_init>
87808c46:	ee9f 3f10 	mrc	15, 4, r3, cr15, cr0, {0}
87808c4a:	0c1b      	lsrs	r3, r3, #16
87808c4c:	f241 120c 	movw	r2, #4364	; 0x110c
87808c50:	2004      	movs	r0, #4
87808c52:	041b      	lsls	r3, r3, #16
87808c54:	f648 610d 	movw	r1, #36365	; 0x8e0d
87808c58:	f2c8 7180 	movt	r1, #34688	; 0x8780
87808c5c:	5098      	str	r0, [r3, r2]
87808c5e:	f44f 4740 	mov.w	r7, #49152	; 0xc000
87808c62:	4622      	mov	r2, r4
87808c64:	2062      	movs	r0, #98	; 0x62
87808c66:	f7fe f92f 	bl	87806ec8 <system_register_irqhandler>
87808c6a:	f2c0 270a 	movt	r7, #522	; 0x20a
87808c6e:	4628      	mov	r0, r5
87808c70:	2109      	movs	r1, #9
87808c72:	f44f 4980 	mov.w	r9, #16384	; 0x4000
87808c76:	f7fe f817 	bl	87806ca8 <gpio_enableint>
87808c7a:	2501      	movs	r5, #1
87808c7c:	aa01      	add	r2, sp, #4
87808c7e:	4638      	mov	r0, r7
87808c80:	2109      	movs	r1, #9
87808c82:	f2c0 291a 	movt	r9, #538	; 0x21a
87808c86:	9403      	str	r4, [sp, #12]
87808c88:	9501      	str	r5, [sp, #4]
87808c8a:	f88d 5008 	strb.w	r5, [sp, #8]
87808c8e:	f7fd ffe7 	bl	87806c60 <gpio_init>
87808c92:	4648      	mov	r0, r9
87808c94:	f7ff fab2 	bl	878081fc <i2c_init>
87808c98:	4622      	mov	r2, r4
87808c9a:	4638      	mov	r0, r7
87808c9c:	2109      	movs	r1, #9
87808c9e:	f7fd ff91 	bl	87806bc4 <gpio_pinwrite>
87808ca2:	2014      	movs	r0, #20
87808ca4:	f7fd ff1a 	bl	87806adc <delayms>
87808ca8:	462a      	mov	r2, r5
87808caa:	4638      	mov	r0, r7
87808cac:	2109      	movs	r1, #9
87808cae:	f7fd ff89 	bl	87806bc4 <gpio_pinwrite>
87808cb2:	2014      	movs	r0, #20
87808cb4:	f7fd ff12 	bl	87806adc <delayms>
87808cb8:	4622      	mov	r2, r4
87808cba:	4621      	mov	r1, r4
87808cbc:	2038      	movs	r0, #56	; 0x38
87808cbe:	f7ff ff53 	bl	87808b68 <ft5426_write_byte>
87808cc2:	462a      	mov	r2, r5
87808cc4:	21a4      	movs	r1, #164	; 0xa4
87808cc6:	2038      	movs	r0, #56	; 0x38
87808cc8:	f7ff ff4e 	bl	87808b68 <ft5426_write_byte>
87808ccc:	2238      	movs	r2, #56	; 0x38
87808cce:	23a1      	movs	r3, #161	; 0xa1
87808cd0:	a904      	add	r1, sp, #16
87808cd2:	4648      	mov	r0, r9
87808cd4:	f8cd d020 	str.w	sp, [sp, #32]
87808cd8:	f88d 2010 	strb.w	r2, [sp, #16]
87808cdc:	9306      	str	r3, [sp, #24]
87808cde:	9505      	str	r5, [sp, #20]
87808ce0:	f88d 501c 	strb.w	r5, [sp, #28]
87808ce4:	f8cd 8024 	str.w	r8, [sp, #36]	; 0x24
87808ce8:	f7ff fb8e 	bl	87808408 <i2c_master_transfer>
87808cec:	f89d 1000 	ldrb.w	r1, [sp]
87808cf0:	f64d 6058 	movw	r0, #56920	; 0xde58
87808cf4:	f89d 3001 	ldrb.w	r3, [sp, #1]
87808cf8:	f2c8 7080 	movt	r0, #34688	; 0x8780
87808cfc:	eb03 2101 	add.w	r1, r3, r1, lsl #8
87808d00:	f7fc f9a4 	bl	8780504c <printf>
87808d04:	7035      	strb	r5, [r6, #0]
87808d06:	7074      	strb	r4, [r6, #1]
87808d08:	b00b      	add	sp, #44	; 0x2c
87808d0a:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
87808d0e:	bf00      	nop

87808d10 <ft5426_read_byte>:
87808d10:	b510      	push	{r4, lr}
87808d12:	b088      	sub	sp, #32
87808d14:	f88d 0008 	strb.w	r0, [sp, #8]
87808d18:	f44f 4080 	mov.w	r0, #16384	; 0x4000
87808d1c:	2301      	movs	r3, #1
87808d1e:	2200      	movs	r2, #0
87808d20:	9104      	str	r1, [sp, #16]
87808d22:	f10d 0407 	add.w	r4, sp, #7
87808d26:	a902      	add	r1, sp, #8
87808d28:	f2c0 201a 	movt	r0, #538	; 0x21a
87808d2c:	9406      	str	r4, [sp, #24]
87808d2e:	f88d 2007 	strb.w	r2, [sp, #7]
87808d32:	9303      	str	r3, [sp, #12]
87808d34:	f88d 3014 	strb.w	r3, [sp, #20]
87808d38:	9307      	str	r3, [sp, #28]
87808d3a:	f7ff fb65 	bl	87808408 <i2c_master_transfer>
87808d3e:	f89d 0007 	ldrb.w	r0, [sp, #7]
87808d42:	b008      	add	sp, #32
87808d44:	bd10      	pop	{r4, pc}
87808d46:	bf00      	nop

87808d48 <ft426_read_len>:
87808d48:	b510      	push	{r4, lr}
87808d4a:	b086      	sub	sp, #24
87808d4c:	f88d 0000 	strb.w	r0, [sp]
87808d50:	f44f 4080 	mov.w	r0, #16384	; 0x4000
87808d54:	2401      	movs	r4, #1
87808d56:	f2c0 201a 	movt	r0, #538	; 0x21a
87808d5a:	9102      	str	r1, [sp, #8]
87808d5c:	4669      	mov	r1, sp
87808d5e:	9304      	str	r3, [sp, #16]
87808d60:	9401      	str	r4, [sp, #4]
87808d62:	9205      	str	r2, [sp, #20]
87808d64:	f88d 400c 	strb.w	r4, [sp, #12]
87808d68:	f7ff fb4e 	bl	87808408 <i2c_master_transfer>
87808d6c:	b006      	add	sp, #24
87808d6e:	bd10      	pop	{r4, pc}

87808d70 <ft5426_read_tpnum>:
87808d70:	b508      	push	{r3, lr}
87808d72:	2102      	movs	r1, #2
87808d74:	2038      	movs	r0, #56	; 0x38
87808d76:	f7ff ffcb 	bl	87808d10 <ft5426_read_byte>
87808d7a:	f24b 138c 	movw	r3, #45452	; 0xb18c
87808d7e:	f2c8 7381 	movt	r3, #34689	; 0x8781
87808d82:	7098      	strb	r0, [r3, #2]
87808d84:	bd08      	pop	{r3, pc}
87808d86:	bf00      	nop

87808d88 <ft5426_read_tpcoord>:
87808d88:	b5f0      	push	{r4, r5, r6, r7, lr}
87808d8a:	f24b 148c 	movw	r4, #45452	; 0xb18c
87808d8e:	b08f      	sub	sp, #60	; 0x3c
87808d90:	2102      	movs	r1, #2
87808d92:	f2c8 7481 	movt	r4, #34689	; 0x8781
87808d96:	2038      	movs	r0, #56	; 0x38
87808d98:	f7ff ffba 	bl	87808d10 <ft5426_read_byte>
87808d9c:	70a0      	strb	r0, [r4, #2]
87808d9e:	f44f 4080 	mov.w	r0, #16384	; 0x4000
87808da2:	2201      	movs	r2, #1
87808da4:	231e      	movs	r3, #30
87808da6:	ad06      	add	r5, sp, #24
87808da8:	f2c0 201a 	movt	r0, #538	; 0x21a
87808dac:	2738      	movs	r7, #56	; 0x38
87808dae:	2603      	movs	r6, #3
87808db0:	4669      	mov	r1, sp
87808db2:	9504      	str	r5, [sp, #16]
87808db4:	9201      	str	r2, [sp, #4]
87808db6:	f88d 200c 	strb.w	r2, [sp, #12]
87808dba:	f88d 7000 	strb.w	r7, [sp]
87808dbe:	9602      	str	r6, [sp, #8]
87808dc0:	9305      	str	r3, [sp, #20]
87808dc2:	f7ff fb21 	bl	87808408 <i2c_master_transfer>
87808dc6:	78a2      	ldrb	r2, [r4, #2]
87808dc8:	b1ea      	cbz	r2, 87808e06 <ft5426_read_tpcoord+0x7e>
87808dca:	1e56      	subs	r6, r2, #1
87808dcc:	462b      	mov	r3, r5
87808dce:	b2f6      	uxtb	r6, r6
87808dd0:	1ca0      	adds	r0, r4, #2
87808dd2:	3601      	adds	r6, #1
87808dd4:	00f2      	lsls	r2, r6, #3
87808dd6:	eba2 0646 	sub.w	r6, r2, r6, lsl #1
87808dda:	442e      	add	r6, r5
87808ddc:	789d      	ldrb	r5, [r3, #2]
87808dde:	3306      	adds	r3, #6
87808de0:	f813 1c03 	ldrb.w	r1, [r3, #-3]
87808de4:	f813 4c06 	ldrb.w	r4, [r3, #-6]
87808de8:	f813 2c05 	ldrb.w	r2, [r3, #-5]
87808dec:	42b3      	cmp	r3, r6
87808dee:	ea41 2105 	orr.w	r1, r1, r5, lsl #8
87808df2:	ea42 2204 	orr.w	r2, r2, r4, lsl #8
87808df6:	f3c1 010b 	ubfx	r1, r1, #0, #12
87808dfa:	f820 1f02 	strh.w	r1, [r0, #2]!
87808dfe:	f3c2 020b 	ubfx	r2, r2, #0, #12
87808e02:	8142      	strh	r2, [r0, #10]
87808e04:	d1ea      	bne.n	87808ddc <ft5426_read_tpcoord+0x54>
87808e06:	b00f      	add	sp, #60	; 0x3c
87808e08:	bdf0      	pop	{r4, r5, r6, r7, pc}
87808e0a:	bf00      	nop

87808e0c <gpio1_io9_irqhandler>:
87808e0c:	b508      	push	{r3, lr}
87808e0e:	f24b 138c 	movw	r3, #45452	; 0xb18c
87808e12:	f2c8 7381 	movt	r3, #34689	; 0x8781
87808e16:	781b      	ldrb	r3, [r3, #0]
87808e18:	2b01      	cmp	r3, #1
87808e1a:	d008      	beq.n	87808e2e <gpio1_io9_irqhandler+0x22>
87808e1c:	f44f 4040 	mov.w	r0, #49152	; 0xc000
87808e20:	2109      	movs	r1, #9
87808e22:	f2c0 2009 	movt	r0, #521	; 0x209
87808e26:	e8bd 4008 	ldmia.w	sp!, {r3, lr}
87808e2a:	f7fd bf4b 	b.w	87806cc4 <gpio_clearintflags>
87808e2e:	f7ff ffab 	bl	87808d88 <ft5426_read_tpcoord>
87808e32:	e7f3      	b.n	87808e1c <gpio1_io9_irqhandler+0x10>

87808e34 <pwm1_irqhandler>:
87808e34:	f04f 7102 	mov.w	r1, #34078720	; 0x2080000
87808e38:	684b      	ldr	r3, [r1, #4]
87808e3a:	071b      	lsls	r3, r3, #28
87808e3c:	d51a      	bpl.n	87808e74 <pwm1_irqhandler+0x40>
87808e3e:	690b      	ldr	r3, [r1, #16]
87808e40:	f24b 12a4 	movw	r2, #45476	; 0xb1a4
87808e44:	f2c8 7281 	movt	r2, #34689	; 0x8781
87808e48:	b430      	push	{r4, r5}
87808e4a:	7812      	ldrb	r2, [r2, #0]
87808e4c:	f248 541f 	movw	r4, #34079	; 0x851f
87808e50:	3302      	adds	r3, #2
87808e52:	f2c5 14eb 	movt	r4, #20971	; 0x51eb
87808e56:	b29b      	uxth	r3, r3
87808e58:	fb02 f303 	mul.w	r3, r2, r3
87808e5c:	fb83 4504 	smull	r4, r5, r3, r4
87808e60:	17db      	asrs	r3, r3, #31
87808e62:	ebc3 1365 	rsb	r3, r3, r5, asr #5
87808e66:	bc30      	pop	{r4, r5}
87808e68:	b29b      	uxth	r3, r3
87808e6a:	60cb      	str	r3, [r1, #12]
87808e6c:	684b      	ldr	r3, [r1, #4]
87808e6e:	f043 0308 	orr.w	r3, r3, #8
87808e72:	604b      	str	r3, [r1, #4]
87808e74:	4770      	bx	lr
87808e76:	bf00      	nop

87808e78 <backlight_init>:
87808e78:	207c      	movs	r0, #124	; 0x7c
87808e7a:	f44f 7242 	mov.w	r2, #776	; 0x308
87808e7e:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
87808e82:	2100      	movs	r1, #0
87808e84:	f04f 7302 	mov.w	r3, #34078720	; 0x2080000
87808e88:	f2c0 200e 	movt	r0, #526	; 0x20e
87808e8c:	f2c0 220e 	movt	r2, #526	; 0x20e
87808e90:	f24b 0490 	movw	r4, #45200	; 0xb090
87808e94:	6001      	str	r1, [r0, #0]
87808e96:	f248 561f 	movw	r6, #34079	; 0x851f
87808e9a:	6014      	str	r4, [r2, #0]
87808e9c:	f44f 6282 	mov.w	r2, #1040	; 0x410
87808ea0:	6019      	str	r1, [r3, #0]
87808ea2:	f2c0 4201 	movt	r2, #1025	; 0x401
87808ea6:	681c      	ldr	r4, [r3, #0]
87808ea8:	f24b 11a4 	movw	r1, #45476	; 0xb1a4
87808eac:	4618      	mov	r0, r3
87808eae:	f2c5 16eb 	movt	r6, #20971	; 0x51eb
87808eb2:	2504      	movs	r5, #4
87808eb4:	f240 37e6 	movw	r7, #998	; 0x3e6
87808eb8:	4322      	orrs	r2, r4
87808eba:	f2c8 7181 	movt	r1, #34689	; 0x8781
87808ebe:	2432      	movs	r4, #50	; 0x32
87808ec0:	601a      	str	r2, [r3, #0]
87808ec2:	611f      	str	r7, [r3, #16]
87808ec4:	700c      	strb	r4, [r1, #0]
87808ec6:	6903      	ldr	r3, [r0, #16]
87808ec8:	1e6a      	subs	r2, r5, #1
87808eca:	f012 05ff 	ands.w	r5, r2, #255	; 0xff
87808ece:	f04f 7402 	mov.w	r4, #34078720	; 0x2080000
87808ed2:	f103 0302 	add.w	r3, r3, #2
87808ed6:	b29b      	uxth	r3, r3
87808ed8:	ea4f 01c3 	mov.w	r1, r3, lsl #3
87808edc:	eb01 0343 	add.w	r3, r1, r3, lsl #1
87808ee0:	eb03 0383 	add.w	r3, r3, r3, lsl #2
87808ee4:	fb83 8906 	smull	r8, r9, r3, r6
87808ee8:	ea4f 73e3 	mov.w	r3, r3, asr #31
87808eec:	ebc3 1369 	rsb	r3, r3, r9, asr #5
87808ef0:	60c3      	str	r3, [r0, #12]
87808ef2:	d1e8      	bne.n	87808ec6 <backlight_init+0x4e>
87808ef4:	68a3      	ldr	r3, [r4, #8]
87808ef6:	f648 6135 	movw	r1, #36405	; 0x8e35
87808efa:	f2c8 7180 	movt	r1, #34688	; 0x8780
87808efe:	462a      	mov	r2, r5
87808f00:	2073      	movs	r0, #115	; 0x73
87808f02:	f043 0301 	orr.w	r3, r3, #1
87808f06:	60a3      	str	r3, [r4, #8]
87808f08:	f7fd ffde 	bl	87806ec8 <system_register_irqhandler>
87808f0c:	ee9f 3f10 	mrc	15, 4, r3, cr15, cr0, {0}
87808f10:	0c1b      	lsrs	r3, r3, #16
87808f12:	f241 120c 	movw	r2, #4364	; 0x110c
87808f16:	f44f 2100 	mov.w	r1, #524288	; 0x80000
87808f1a:	041b      	lsls	r3, r3, #16
87808f1c:	5099      	str	r1, [r3, r2]
87808f1e:	6065      	str	r5, [r4, #4]
87808f20:	6823      	ldr	r3, [r4, #0]
87808f22:	f043 0301 	orr.w	r3, r3, #1
87808f26:	6023      	str	r3, [r4, #0]
87808f28:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}

87808f2c <pwm1_enable>:
87808f2c:	f04f 7202 	mov.w	r2, #34078720	; 0x2080000
87808f30:	6813      	ldr	r3, [r2, #0]
87808f32:	f043 0301 	orr.w	r3, r3, #1
87808f36:	6013      	str	r3, [r2, #0]
87808f38:	4770      	bx	lr
87808f3a:	bf00      	nop

87808f3c <pwm1_setsample_value>:
87808f3c:	b280      	uxth	r0, r0
87808f3e:	f04f 7302 	mov.w	r3, #34078720	; 0x2080000
87808f42:	60d8      	str	r0, [r3, #12]
87808f44:	4770      	bx	lr
87808f46:	bf00      	nop

87808f48 <pwm1_setperiod_value>:
87808f48:	2801      	cmp	r0, #1
87808f4a:	f04f 7302 	mov.w	r3, #34078720	; 0x2080000
87808f4e:	bf8a      	itet	hi
87808f50:	3802      	subhi	r0, #2
87808f52:	2002      	movls	r0, #2
87808f54:	b280      	uxthhi	r0, r0
87808f56:	6118      	str	r0, [r3, #16]
87808f58:	4770      	bx	lr
87808f5a:	bf00      	nop

87808f5c <pwm1_setduty>:
87808f5c:	f24b 13a4 	movw	r3, #45476	; 0xb1a4
87808f60:	f04f 7102 	mov.w	r1, #34078720	; 0x2080000
87808f64:	f2c8 7381 	movt	r3, #34689	; 0x8781
87808f68:	f248 521f 	movw	r2, #34079	; 0x851f
87808f6c:	7018      	strb	r0, [r3, #0]
87808f6e:	f2c5 12eb 	movt	r2, #20971	; 0x51eb
87808f72:	690b      	ldr	r3, [r1, #16]
87808f74:	3302      	adds	r3, #2
87808f76:	b29b      	uxth	r3, r3
87808f78:	fb00 f003 	mul.w	r0, r0, r3
87808f7c:	fb80 2302 	smull	r2, r3, r0, r2
87808f80:	17c0      	asrs	r0, r0, #31
87808f82:	ebc0 1063 	rsb	r0, r0, r3, asr #5
87808f86:	b280      	uxth	r0, r0
87808f88:	60c8      	str	r0, [r1, #12]
87808f8a:	4770      	bx	lr
87808f8c:	0000      	movs	r0, r0
	...

87808f90 <__aeabi_uidiv>:
87808f90:	1e4a      	subs	r2, r1, #1
87808f92:	bf08      	it	eq
87808f94:	4770      	bxeq	lr
87808f96:	f0c0 8124 	bcc.w	878091e2 <__aeabi_uidiv+0x252>
87808f9a:	4288      	cmp	r0, r1
87808f9c:	f240 8116 	bls.w	878091cc <__aeabi_uidiv+0x23c>
87808fa0:	4211      	tst	r1, r2
87808fa2:	f000 8117 	beq.w	878091d4 <__aeabi_uidiv+0x244>
87808fa6:	fab0 f380 	clz	r3, r0
87808faa:	fab1 f281 	clz	r2, r1
87808fae:	eba2 0303 	sub.w	r3, r2, r3
87808fb2:	f1c3 031f 	rsb	r3, r3, #31
87808fb6:	a204      	add	r2, pc, #16	; (adr r2, 87808fc8 <__aeabi_uidiv+0x38>)
87808fb8:	eb02 1303 	add.w	r3, r2, r3, lsl #4
87808fbc:	f04f 0200 	mov.w	r2, #0
87808fc0:	469f      	mov	pc, r3
87808fc2:	bf00      	nop
87808fc4:	f3af 8000 	nop.w
87808fc8:	ebb0 7fc1 	cmp.w	r0, r1, lsl #31
87808fcc:	bf00      	nop
87808fce:	eb42 0202 	adc.w	r2, r2, r2
87808fd2:	bf28      	it	cs
87808fd4:	eba0 70c1 	subcs.w	r0, r0, r1, lsl #31
87808fd8:	ebb0 7f81 	cmp.w	r0, r1, lsl #30
87808fdc:	bf00      	nop
87808fde:	eb42 0202 	adc.w	r2, r2, r2
87808fe2:	bf28      	it	cs
87808fe4:	eba0 7081 	subcs.w	r0, r0, r1, lsl #30
87808fe8:	ebb0 7f41 	cmp.w	r0, r1, lsl #29
87808fec:	bf00      	nop
87808fee:	eb42 0202 	adc.w	r2, r2, r2
87808ff2:	bf28      	it	cs
87808ff4:	eba0 7041 	subcs.w	r0, r0, r1, lsl #29
87808ff8:	ebb0 7f01 	cmp.w	r0, r1, lsl #28
87808ffc:	bf00      	nop
87808ffe:	eb42 0202 	adc.w	r2, r2, r2
87809002:	bf28      	it	cs
87809004:	eba0 7001 	subcs.w	r0, r0, r1, lsl #28
87809008:	ebb0 6fc1 	cmp.w	r0, r1, lsl #27
8780900c:	bf00      	nop
8780900e:	eb42 0202 	adc.w	r2, r2, r2
87809012:	bf28      	it	cs
87809014:	eba0 60c1 	subcs.w	r0, r0, r1, lsl #27
87809018:	ebb0 6f81 	cmp.w	r0, r1, lsl #26
8780901c:	bf00      	nop
8780901e:	eb42 0202 	adc.w	r2, r2, r2
87809022:	bf28      	it	cs
87809024:	eba0 6081 	subcs.w	r0, r0, r1, lsl #26
87809028:	ebb0 6f41 	cmp.w	r0, r1, lsl #25
8780902c:	bf00      	nop
8780902e:	eb42 0202 	adc.w	r2, r2, r2
87809032:	bf28      	it	cs
87809034:	eba0 6041 	subcs.w	r0, r0, r1, lsl #25
87809038:	ebb0 6f01 	cmp.w	r0, r1, lsl #24
8780903c:	bf00      	nop
8780903e:	eb42 0202 	adc.w	r2, r2, r2
87809042:	bf28      	it	cs
87809044:	eba0 6001 	subcs.w	r0, r0, r1, lsl #24
87809048:	ebb0 5fc1 	cmp.w	r0, r1, lsl #23
8780904c:	bf00      	nop
8780904e:	eb42 0202 	adc.w	r2, r2, r2
87809052:	bf28      	it	cs
87809054:	eba0 50c1 	subcs.w	r0, r0, r1, lsl #23
87809058:	ebb0 5f81 	cmp.w	r0, r1, lsl #22
8780905c:	bf00      	nop
8780905e:	eb42 0202 	adc.w	r2, r2, r2
87809062:	bf28      	it	cs
87809064:	eba0 5081 	subcs.w	r0, r0, r1, lsl #22
87809068:	ebb0 5f41 	cmp.w	r0, r1, lsl #21
8780906c:	bf00      	nop
8780906e:	eb42 0202 	adc.w	r2, r2, r2
87809072:	bf28      	it	cs
87809074:	eba0 5041 	subcs.w	r0, r0, r1, lsl #21
87809078:	ebb0 5f01 	cmp.w	r0, r1, lsl #20
8780907c:	bf00      	nop
8780907e:	eb42 0202 	adc.w	r2, r2, r2
87809082:	bf28      	it	cs
87809084:	eba0 5001 	subcs.w	r0, r0, r1, lsl #20
87809088:	ebb0 4fc1 	cmp.w	r0, r1, lsl #19
8780908c:	bf00      	nop
8780908e:	eb42 0202 	adc.w	r2, r2, r2
87809092:	bf28      	it	cs
87809094:	eba0 40c1 	subcs.w	r0, r0, r1, lsl #19
87809098:	ebb0 4f81 	cmp.w	r0, r1, lsl #18
8780909c:	bf00      	nop
8780909e:	eb42 0202 	adc.w	r2, r2, r2
878090a2:	bf28      	it	cs
878090a4:	eba0 4081 	subcs.w	r0, r0, r1, lsl #18
878090a8:	ebb0 4f41 	cmp.w	r0, r1, lsl #17
878090ac:	bf00      	nop
878090ae:	eb42 0202 	adc.w	r2, r2, r2
878090b2:	bf28      	it	cs
878090b4:	eba0 4041 	subcs.w	r0, r0, r1, lsl #17
878090b8:	ebb0 4f01 	cmp.w	r0, r1, lsl #16
878090bc:	bf00      	nop
878090be:	eb42 0202 	adc.w	r2, r2, r2
878090c2:	bf28      	it	cs
878090c4:	eba0 4001 	subcs.w	r0, r0, r1, lsl #16
878090c8:	ebb0 3fc1 	cmp.w	r0, r1, lsl #15
878090cc:	bf00      	nop
878090ce:	eb42 0202 	adc.w	r2, r2, r2
878090d2:	bf28      	it	cs
878090d4:	eba0 30c1 	subcs.w	r0, r0, r1, lsl #15
878090d8:	ebb0 3f81 	cmp.w	r0, r1, lsl #14
878090dc:	bf00      	nop
878090de:	eb42 0202 	adc.w	r2, r2, r2
878090e2:	bf28      	it	cs
878090e4:	eba0 3081 	subcs.w	r0, r0, r1, lsl #14
878090e8:	ebb0 3f41 	cmp.w	r0, r1, lsl #13
878090ec:	bf00      	nop
878090ee:	eb42 0202 	adc.w	r2, r2, r2
878090f2:	bf28      	it	cs
878090f4:	eba0 3041 	subcs.w	r0, r0, r1, lsl #13
878090f8:	ebb0 3f01 	cmp.w	r0, r1, lsl #12
878090fc:	bf00      	nop
878090fe:	eb42 0202 	adc.w	r2, r2, r2
87809102:	bf28      	it	cs
87809104:	eba0 3001 	subcs.w	r0, r0, r1, lsl #12
87809108:	ebb0 2fc1 	cmp.w	r0, r1, lsl #11
8780910c:	bf00      	nop
8780910e:	eb42 0202 	adc.w	r2, r2, r2
87809112:	bf28      	it	cs
87809114:	eba0 20c1 	subcs.w	r0, r0, r1, lsl #11
87809118:	ebb0 2f81 	cmp.w	r0, r1, lsl #10
8780911c:	bf00      	nop
8780911e:	eb42 0202 	adc.w	r2, r2, r2
87809122:	bf28      	it	cs
87809124:	eba0 2081 	subcs.w	r0, r0, r1, lsl #10
87809128:	ebb0 2f41 	cmp.w	r0, r1, lsl #9
8780912c:	bf00      	nop
8780912e:	eb42 0202 	adc.w	r2, r2, r2
87809132:	bf28      	it	cs
87809134:	eba0 2041 	subcs.w	r0, r0, r1, lsl #9
87809138:	ebb0 2f01 	cmp.w	r0, r1, lsl #8
8780913c:	bf00      	nop
8780913e:	eb42 0202 	adc.w	r2, r2, r2
87809142:	bf28      	it	cs
87809144:	eba0 2001 	subcs.w	r0, r0, r1, lsl #8
87809148:	ebb0 1fc1 	cmp.w	r0, r1, lsl #7
8780914c:	bf00      	nop
8780914e:	eb42 0202 	adc.w	r2, r2, r2
87809152:	bf28      	it	cs
87809154:	eba0 10c1 	subcs.w	r0, r0, r1, lsl #7
87809158:	ebb0 1f81 	cmp.w	r0, r1, lsl #6
8780915c:	bf00      	nop
8780915e:	eb42 0202 	adc.w	r2, r2, r2
87809162:	bf28      	it	cs
87809164:	eba0 1081 	subcs.w	r0, r0, r1, lsl #6
87809168:	ebb0 1f41 	cmp.w	r0, r1, lsl #5
8780916c:	bf00      	nop
8780916e:	eb42 0202 	adc.w	r2, r2, r2
87809172:	bf28      	it	cs
87809174:	eba0 1041 	subcs.w	r0, r0, r1, lsl #5
87809178:	ebb0 1f01 	cmp.w	r0, r1, lsl #4
8780917c:	bf00      	nop
8780917e:	eb42 0202 	adc.w	r2, r2, r2
87809182:	bf28      	it	cs
87809184:	eba0 1001 	subcs.w	r0, r0, r1, lsl #4
87809188:	ebb0 0fc1 	cmp.w	r0, r1, lsl #3
8780918c:	bf00      	nop
8780918e:	eb42 0202 	adc.w	r2, r2, r2
87809192:	bf28      	it	cs
87809194:	eba0 00c1 	subcs.w	r0, r0, r1, lsl #3
87809198:	ebb0 0f81 	cmp.w	r0, r1, lsl #2
8780919c:	bf00      	nop
8780919e:	eb42 0202 	adc.w	r2, r2, r2
878091a2:	bf28      	it	cs
878091a4:	eba0 0081 	subcs.w	r0, r0, r1, lsl #2
878091a8:	ebb0 0f41 	cmp.w	r0, r1, lsl #1
878091ac:	bf00      	nop
878091ae:	eb42 0202 	adc.w	r2, r2, r2
878091b2:	bf28      	it	cs
878091b4:	eba0 0041 	subcs.w	r0, r0, r1, lsl #1
878091b8:	ebb0 0f01 	cmp.w	r0, r1
878091bc:	bf00      	nop
878091be:	eb42 0202 	adc.w	r2, r2, r2
878091c2:	bf28      	it	cs
878091c4:	eba0 0001 	subcs.w	r0, r0, r1
878091c8:	4610      	mov	r0, r2
878091ca:	4770      	bx	lr
878091cc:	bf0c      	ite	eq
878091ce:	2001      	moveq	r0, #1
878091d0:	2000      	movne	r0, #0
878091d2:	4770      	bx	lr
878091d4:	fab1 f281 	clz	r2, r1
878091d8:	f1c2 021f 	rsb	r2, r2, #31
878091dc:	fa20 f002 	lsr.w	r0, r0, r2
878091e0:	4770      	bx	lr
878091e2:	b108      	cbz	r0, 878091e8 <__aeabi_uidiv+0x258>
878091e4:	f04f 30ff 	mov.w	r0, #4294967295	; 0xffffffff
878091e8:	f000 b826 	b.w	87809238 <__aeabi_idiv0>

878091ec <__aeabi_uidivmod>:
878091ec:	2900      	cmp	r1, #0
878091ee:	d0f8      	beq.n	878091e2 <__aeabi_uidiv+0x252>
878091f0:	e92d 4003 	stmdb	sp!, {r0, r1, lr}
878091f4:	f7ff fecc 	bl	87808f90 <__aeabi_uidiv>
878091f8:	e8bd 4006 	ldmia.w	sp!, {r1, r2, lr}
878091fc:	fb02 f300 	mul.w	r3, r2, r0
87809200:	eba1 0103 	sub.w	r1, r1, r3
87809204:	4770      	bx	lr
87809206:	bf00      	nop

87809208 <__aeabi_uldivmod>:
87809208:	b953      	cbnz	r3, 87809220 <__aeabi_uldivmod+0x18>
8780920a:	b94a      	cbnz	r2, 87809220 <__aeabi_uldivmod+0x18>
8780920c:	2900      	cmp	r1, #0
8780920e:	bf08      	it	eq
87809210:	2800      	cmpeq	r0, #0
87809212:	bf1c      	itt	ne
87809214:	f04f 31ff 	movne.w	r1, #4294967295	; 0xffffffff
87809218:	f04f 30ff 	movne.w	r0, #4294967295	; 0xffffffff
8780921c:	f000 b80c 	b.w	87809238 <__aeabi_idiv0>
87809220:	f1ad 0c08 	sub.w	ip, sp, #8
87809224:	e96d ce04 	strd	ip, lr, [sp, #-16]!
87809228:	f000 f80c 	bl	87809244 <__udivmoddi4>
8780922c:	f8dd e004 	ldr.w	lr, [sp, #4]
87809230:	e9dd 2302 	ldrd	r2, r3, [sp, #8]
87809234:	b004      	add	sp, #16
87809236:	4770      	bx	lr

87809238 <__aeabi_idiv0>:
87809238:	b502      	push	{r1, lr}
8780923a:	f04f 0008 	mov.w	r0, #8
8780923e:	f7fe f925 	bl	8780748c <raise>
87809242:	bd02      	pop	{r1, pc}

87809244 <__udivmoddi4>:
87809244:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
87809248:	460d      	mov	r5, r1
8780924a:	429d      	cmp	r5, r3
8780924c:	4604      	mov	r4, r0
8780924e:	4610      	mov	r0, r2
87809250:	4619      	mov	r1, r3
87809252:	bf08      	it	eq
87809254:	4284      	cmpeq	r4, r0
87809256:	9a07      	ldr	r2, [sp, #28]
87809258:	bf3c      	itt	cc
8780925a:	2000      	movcc	r0, #0
8780925c:	2100      	movcc	r1, #0
8780925e:	d36a      	bcc.n	87809336 <__udivmoddi4+0xf2>
87809260:	fab1 f681 	clz	r6, r1
87809264:	2900      	cmp	r1, #0
87809266:	d06f      	beq.n	87809348 <__udivmoddi4+0x104>
87809268:	fab5 f385 	clz	r3, r5
8780926c:	2d00      	cmp	r5, #0
8780926e:	d067      	beq.n	87809340 <__udivmoddi4+0xfc>
87809270:	1af3      	subs	r3, r6, r3
87809272:	f1a3 0c20 	sub.w	ip, r3, #32
87809276:	fa01 f703 	lsl.w	r7, r1, r3
8780927a:	fa00 fe0c 	lsl.w	lr, r0, ip
8780927e:	f1c3 0820 	rsb	r8, r3, #32
87809282:	fa20 f108 	lsr.w	r1, r0, r8
87809286:	fa00 f603 	lsl.w	r6, r0, r3
8780928a:	ea47 070e 	orr.w	r7, r7, lr
8780928e:	469e      	mov	lr, r3
87809290:	430f      	orrs	r7, r1
87809292:	42bd      	cmp	r5, r7
87809294:	bf08      	it	eq
87809296:	42b4      	cmpeq	r4, r6
87809298:	bf3c      	itt	cc
8780929a:	2000      	movcc	r0, #0
8780929c:	2100      	movcc	r1, #0
8780929e:	d30c      	bcc.n	878092ba <__udivmoddi4+0x76>
878092a0:	f04f 0901 	mov.w	r9, #1
878092a4:	1ba4      	subs	r4, r4, r6
878092a6:	fa29 f808 	lsr.w	r8, r9, r8
878092aa:	fa09 f10c 	lsl.w	r1, r9, ip
878092ae:	fa09 f003 	lsl.w	r0, r9, r3
878092b2:	ea41 0108 	orr.w	r1, r1, r8
878092b6:	eb65 0507 	sbc.w	r5, r5, r7
878092ba:	2b00      	cmp	r3, #0
878092bc:	d03b      	beq.n	87809336 <__udivmoddi4+0xf2>
878092be:	087f      	lsrs	r7, r7, #1
878092c0:	ea4f 0636 	mov.w	r6, r6, rrx
878092c4:	e009      	b.n	878092da <__udivmoddi4+0x96>
878092c6:	1ba4      	subs	r4, r4, r6
878092c8:	eb65 0507 	sbc.w	r5, r5, r7
878092cc:	1924      	adds	r4, r4, r4
878092ce:	416d      	adcs	r5, r5
878092d0:	3401      	adds	r4, #1
878092d2:	f145 0500 	adc.w	r5, r5, #0
878092d6:	3b01      	subs	r3, #1
878092d8:	d007      	beq.n	878092ea <__udivmoddi4+0xa6>
878092da:	42bd      	cmp	r5, r7
878092dc:	bf08      	it	eq
878092de:	42b4      	cmpeq	r4, r6
878092e0:	d2f1      	bcs.n	878092c6 <__udivmoddi4+0x82>
878092e2:	1924      	adds	r4, r4, r4
878092e4:	416d      	adcs	r5, r5
878092e6:	3b01      	subs	r3, #1
878092e8:	d1f7      	bne.n	878092da <__udivmoddi4+0x96>
878092ea:	f1ce 0720 	rsb	r7, lr, #32
878092ee:	fa24 f60e 	lsr.w	r6, r4, lr
878092f2:	fa05 f707 	lsl.w	r7, r5, r7
878092f6:	f1ae 0320 	sub.w	r3, lr, #32
878092fa:	fa25 f303 	lsr.w	r3, r5, r3
878092fe:	fa25 fc0e 	lsr.w	ip, r5, lr
87809302:	1900      	adds	r0, r0, r4
87809304:	ea46 0607 	orr.w	r6, r6, r7
87809308:	ea46 0403 	orr.w	r4, r6, r3
8780930c:	4169      	adcs	r1, r5
8780930e:	4665      	mov	r5, ip
87809310:	f1ae 0c20 	sub.w	ip, lr, #32
87809314:	fa05 f70e 	lsl.w	r7, r5, lr
87809318:	fa04 fc0c 	lsl.w	ip, r4, ip
8780931c:	fa04 f60e 	lsl.w	r6, r4, lr
87809320:	f1ce 0320 	rsb	r3, lr, #32
87809324:	fa24 f303 	lsr.w	r3, r4, r3
87809328:	ea47 070c 	orr.w	r7, r7, ip
8780932c:	1b80      	subs	r0, r0, r6
8780932e:	ea47 0703 	orr.w	r7, r7, r3
87809332:	eb61 0107 	sbc.w	r1, r1, r7
87809336:	b10a      	cbz	r2, 8780933c <__udivmoddi4+0xf8>
87809338:	e9c2 4500 	strd	r4, r5, [r2]
8780933c:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
87809340:	fab4 f384 	clz	r3, r4
87809344:	3320      	adds	r3, #32
87809346:	e793      	b.n	87809270 <__udivmoddi4+0x2c>
87809348:	fab0 f380 	clz	r3, r0
8780934c:	f103 0620 	add.w	r6, r3, #32
87809350:	fab5 f385 	clz	r3, r5
87809354:	2d00      	cmp	r5, #0
87809356:	d18b      	bne.n	87809270 <__udivmoddi4+0x2c>
87809358:	e7f2      	b.n	87809340 <__udivmoddi4+0xfc>
8780935a:	bf00      	nop

8780935c <main>:
8780935c:	b5f0      	push	{r4, r5, r6, r7, lr}
8780935e:	b085      	sub	sp, #20
87809360:	f7fd fed2 	bl	87807108 <uart_init>
87809364:	2400      	movs	r4, #0
87809366:	f44f 7700 	mov.w	r7, #512	; 0x200
8780936a:	f7fd fb4f 	bl	87806a0c <led_init>
8780936e:	f7fd fbf1 	bl	87806b54 <beep_init>
87809372:	f7fd fcad 	bl	87806cd0 <key_init>
87809376:	f7fa ff75 	bl	87804264 <tos_knl_init>
8780937a:	f641 73d0 	movw	r3, #8144	; 0x1fd0
8780937e:	2604      	movs	r6, #4
87809380:	f2c8 7381 	movt	r3, #34689	; 0x8781
87809384:	f240 32d1 	movw	r2, #977	; 0x3d1
87809388:	f64d 4120 	movw	r1, #56352	; 0xdc20
8780938c:	f641 60d0 	movw	r0, #7888	; 0x1ed0
87809390:	9301      	str	r3, [sp, #4]
87809392:	f2c8 7280 	movt	r2, #34688	; 0x8780
87809396:	4623      	mov	r3, r4
87809398:	9403      	str	r4, [sp, #12]
8780939a:	9702      	str	r7, [sp, #8]
8780939c:	f2c8 7180 	movt	r1, #34688	; 0x8780
878093a0:	9600      	str	r6, [sp, #0]
878093a2:	f2c8 7081 	movt	r0, #34689	; 0x8781
878093a6:	f641 45d0 	movw	r5, #7376	; 0x1cd0
878093aa:	f7f9 fe33 	bl	87803014 <tos_task_create>
878093ae:	f2c8 7581 	movt	r5, #34689	; 0x8781
878093b2:	f240 32ed 	movw	r2, #1005	; 0x3ed
878093b6:	f64d 4128 	movw	r1, #56360	; 0xdc28
878093ba:	f641 7050 	movw	r0, #8016	; 0x1f50
878093be:	4623      	mov	r3, r4
878093c0:	f2c8 7280 	movt	r2, #34688	; 0x8780
878093c4:	f2c8 7180 	movt	r1, #34688	; 0x8780
878093c8:	9403      	str	r4, [sp, #12]
878093ca:	9702      	str	r7, [sp, #8]
878093cc:	f2c8 7081 	movt	r0, #34689	; 0x8781
878093d0:	9600      	str	r6, [sp, #0]
878093d2:	9501      	str	r5, [sp, #4]
878093d4:	f7f9 fe1e 	bl	87803014 <tos_task_create>
878093d8:	f7fa fe78 	bl	878040cc <tos_knl_start>
878093dc:	4620      	mov	r0, r4
878093de:	b005      	add	sp, #20
878093e0:	bdf0      	pop	{r4, r5, r6, r7, pc}
878093e2:	bf00      	nop
878093e4:	0000      	movs	r0, r0
	...

878093e8 <__port_cpsr_restore_from_thumb>:
878093e8:	4778      	bx	pc
878093ea:	46c0      	nop			; (mov r8, r8)
878093ec:	eaffdb77 	b	878001d0 <port_cpsr_restore>

878093f0 <__port_irq_context_switch_from_thumb>:
878093f0:	4778      	bx	pc
878093f2:	46c0      	nop			; (mov r8, r8)
878093f4:	eaffdb8a 	b	87800224 <port_irq_context_switch>

878093f8 <__port_context_switch_from_thumb>:
878093f8:	4778      	bx	pc
878093fa:	46c0      	nop			; (mov r8, r8)
878093fc:	eaffdb76 	b	878001dc <port_context_switch>

87809400 <__port_int_disable_from_thumb>:
87809400:	4778      	bx	pc
87809402:	46c0      	nop			; (mov r8, r8)
87809404:	eaffdb6c 	b	878001bc <port_int_disable>

87809408 <__port_int_enable_from_thumb>:
87809408:	4778      	bx	pc
8780940a:	46c0      	nop			; (mov r8, r8)
8780940c:	eaffdb6b 	b	878001c0 <port_int_enable>

87809410 <__port_cpsr_save_from_thumb>:
87809410:	4778      	bx	pc
87809412:	46c0      	nop			; (mov r8, r8)
87809414:	eaffdb6a 	b	878001c4 <port_cpsr_save>

Disassembly of section .ARM.exidx:

87809418 <__exidx_end-0x8>:
87809418:	7ffffe2c 	svcvc	0x00fffe2c
8780941c:	00000001 	andeq	r0, r0, r1

Disassembly of section .rodata:

87809420 <__rodata_start>:
87809420:	6f697270 	svcvs	0x00697270
87809424:	705f715f 	subsvc	r7, pc, pc, asr r1	; <UNPREDICTABLE>
87809428:	5f6f6972 	svcpl	0x006f6972
8780942c:	5f72676d 	svcpl	0x0072676d
87809430:	746f6c73 	strbtvc	r6, [pc], #-3187	; 87809438 <__rodata_start+0x18>
87809434:	716e655f 	cmnvc	lr, pc, asr r5
87809438:	65756575 	ldrbvs	r6, [r5, #-1397]!	; 0x575
8780943c:	00000000 	andeq	r0, r0, r0

87809440 <__FUNCTION__.6225>:
87809440:	5f736f74 	svcpl	0x00736f74
87809444:	6f697270 	svcvs	0x00697270
87809448:	655f715f 	ldrbvs	r7, [pc, #-351]	; 878092f1 <__udivmoddi4+0xad>
8780944c:	6575716e 	ldrbvs	r7, [r5, #-366]!	; 0x16e
87809450:	00006575 	andeq	r6, r0, r5, ror r5

87809454 <__FUNCTION__.6185>:
87809454:	6f697270 	svcvs	0x00697270
87809458:	705f715f 	subsvc	r7, pc, pc, asr r1	; <UNPREDICTABLE>
8780945c:	5f6f6972 	svcpl	0x006f6972
87809460:	5f72676d 	svcpl	0x0072676d
87809464:	746f6c73 	strbtvc	r6, [pc], #-3187	; 8780946c <__FUNCTION__.6185+0x18>
87809468:	7165645f 	cmnvc	r5, pc, asr r4
8780946c:	65756575 	ldrbvs	r6, [r5, #-1397]!	; 0x575
87809470:	00000000 	andeq	r0, r0, r0

87809474 <k_idle_task_stk_size>:
87809474:	00000200 	andeq	r0, r0, r0, lsl #4

87809478 <k_idle_task_stk_addr>:
87809478:	87812228 	strhi	r2, [r1, r8, lsr #4]

8780947c <digits.7616>:
8780947c:	33323130 	teqcc	r2, #48, 2
87809480:	37363534 			; <UNDEFINED> instruction: 0x37363534
87809484:	42413938 	submi	r3, r1, #56, 18	; 0xe0000
87809488:	46454443 	strbmi	r4, [r5], -r3, asr #8

8780948c <CSWTCH.9>:
8780948c:	00000004 	andeq	r0, r0, r4
87809490:	00000003 	andeq	r0, r0, r3
87809494:	00000002 	andeq	r0, r0, r2
87809498:	00000001 	andeq	r0, r0, r1
8780949c:	00000000 	andeq	r0, r0, r0
878094a0:	00000006 	andeq	r0, r0, r6

878094a4 <asc2_1206>:
	...
878094b4:	0000403f 	andeq	r4, r0, pc, lsr r0
878094b8:	00000000 	andeq	r0, r0, r0
878094bc:	00300000 	eorseq	r0, r0, r0
878094c0:	00300040 	eorseq	r0, r0, r0, asr #32
878094c4:	00000040 	andeq	r0, r0, r0, asr #32
878094c8:	c00b0009 	andgt	r0, fp, r9
878094cc:	c00b003d 	andgt	r0, fp, sp, lsr r0
878094d0:	0009003d 	andeq	r0, r9, sp, lsr r0
878094d4:	4024c018 	eormi	ip, r4, r8, lsl r0
878094d8:	4022e07f 	eormi	lr, r2, pc, ror r0
878094dc:	00008031 	andeq	r8, r0, r1, lsr r0
878094e0:	c0240018 	eorgt	r0, r4, r8, lsl r0
878094e4:	800d001b 	andhi	r0, sp, fp, lsl r0
878094e8:	80014032 	andhi	r4, r1, r2, lsr r0
878094ec:	401c8003 	andsmi	r8, ip, r3
878094f0:	801c4027 	andshi	r4, ip, r7, lsr #32
878094f4:	40004007 	andmi	r4, r0, r7
878094f8:	00600010 	rsbeq	r0, r0, r0, lsl r0
	...
87809508:	801f0000 	andshi	r0, pc, r0
8780950c:	20404020 	subcs	r4, r0, r0, lsr #32
87809510:	20400000 	subcs	r0, r0, r0
87809514:	801f4020 	andshi	r4, pc, r0, lsr #32
87809518:	00000000 	andeq	r0, r0, r0
8780951c:	00060009 	andeq	r0, r6, r9
87809520:	0006801f 	andeq	r8, r6, pc, lsl r0
87809524:	00000009 	andeq	r0, r0, r9
87809528:	00040004 	andeq	r0, r4, r4
8780952c:	0004803f 	andeq	r8, r4, pc, lsr r0
87809530:	00000004 	andeq	r0, r0, r4
87809534:	60001000 	andvs	r1, r0, r0
	...
87809540:	00040004 	andeq	r0, r4, r4
87809544:	00040004 	andeq	r0, r4, r4
87809548:	00000004 	andeq	r0, r0, r4
8780954c:	40000000 	andmi	r0, r0, r0
	...
87809558:	c0012000 	andgt	r2, r1, r0
8780955c:	00380006 	eorseq	r0, r8, r6
87809560:	00000040 	andeq	r0, r0, r0, asr #32
87809564:	4020801f 	eormi	r8, r0, pc, lsl r0
87809568:	40204020 	eormi	r4, r0, r0, lsr #32
8780956c:	0000801f 	andeq	r8, r0, pc, lsl r0
87809570:	40100000 	andsmi	r0, r0, r0
87809574:	4000c03f 	andmi	ip, r0, pc, lsr r0
87809578:	00000000 	andeq	r0, r0, r0
8780957c:	4021c018 	eormi	ip, r1, r8, lsl r0
87809580:	40244022 	eormi	r4, r4, r2, lsr #32
87809584:	00004018 	andeq	r4, r0, r8, lsl r0
87809588:	40208010 	eormi	r8, r0, r0, lsl r0
8780958c:	40244024 	eormi	r4, r4, r4, lsr #32
87809590:	0000801b 	andeq	r8, r0, fp, lsl r0
87809594:	000d0002 	andeq	r0, sp, r2
87809598:	c03f0011 	eorsgt	r0, pc, r1, lsl r0	; <UNPREDICTABLE>
8780959c:	00004001 	andeq	r4, r0, r1
878095a0:	4024803c 	eormi	r8, r4, ip, lsr r0
878095a4:	40244024 	eormi	r4, r4, r4, lsr #32
878095a8:	00008023 	andeq	r8, r0, r3, lsr #32
878095ac:	4024801f 	eormi	r8, r4, pc, lsl r0
878095b0:	40344024 	eorsmi	r4, r4, r4, lsr #32
878095b4:	00008003 	andeq	r8, r0, r3
878095b8:	00200030 	eoreq	r0, r0, r0, lsr r0
878095bc:	0038c027 	eorseq	ip, r8, r7, lsr #32
878095c0:	00000020 	andeq	r0, r0, r0, lsr #32
878095c4:	4024801b 	eormi	r8, r4, fp, lsl r0
878095c8:	40244024 	eormi	r4, r4, r4, lsr #32
878095cc:	0000801b 	andeq	r8, r0, fp, lsl r0
878095d0:	c022001c 	eorgt	r0, r2, ip, lsl r0
878095d4:	40224022 	eormi	r4, r2, r2, lsr #32
878095d8:	0000801f 	andeq	r8, r0, pc, lsl r0
878095dc:	00000000 	andeq	r0, r0, r0
878095e0:	00004008 	andeq	r4, r0, r8
	...
878095ec:	00006004 	andeq	r6, r0, r4
878095f0:	00000000 	andeq	r0, r0, r0
878095f4:	00040000 	andeq	r0, r4, r0
878095f8:	0011000a 	andseq	r0, r1, sl
878095fc:	40408020 	submi	r8, r0, r0, lsr #32
87809600:	00090009 	andeq	r0, r9, r9
87809604:	00090009 	andeq	r0, r9, r9
87809608:	00000009 	andeq	r0, r0, r9
8780960c:	40400000 	submi	r0, r0, r0
87809610:	00118020 	andseq	r8, r1, r0, lsr #32
87809614:	0004000a 	andeq	r0, r4, sl
87809618:	00200018 	eoreq	r0, r0, r8, lsl r0
8780961c:	00244023 	eoreq	r4, r4, r3, lsr #32
87809620:	00000018 	andeq	r0, r0, r8, lsl r0
87809624:	4020801f 	eormi	r8, r0, pc, lsl r0
87809628:	40294027 	eormi	r4, r9, r7, lsr #32
8780962c:	0000401f 	andeq	r4, r0, pc, lsl r0
87809630:	c0074000 	andgt	r4, r7, r0
87809634:	000f0039 	andeq	r0, pc, r9, lsr r0	; <UNPREDICTABLE>
87809638:	4000c001 	andmi	ip, r0, r1
8780963c:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
87809640:	40244024 	eormi	r4, r4, r4, lsr #32
87809644:	0000801b 	andeq	r8, r0, fp, lsl r0
87809648:	4020801f 	eormi	r8, r0, pc, lsl r0
8780964c:	40204020 	eormi	r4, r0, r0, lsr #32
87809650:	00008030 	andeq	r8, r0, r0, lsr r0
87809654:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
87809658:	40204020 	eormi	r4, r0, r0, lsr #32
8780965c:	0000801f 	andeq	r8, r0, pc, lsl r0
87809660:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
87809664:	402e4024 	eormi	r4, lr, r4, lsr #32
87809668:	0000c030 	andeq	ip, r0, r0, lsr r0
8780966c:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
87809670:	002e4024 	eoreq	r4, lr, r4, lsr #32
87809674:	00000030 	andeq	r0, r0, r0, lsr r0
87809678:	8010000f 	andshi	r0, r0, pc
8780967c:	40224020 	eormi	r4, r2, r0, lsr #32
87809680:	00028033 	andeq	r8, r2, r3, lsr r0
87809684:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
87809688:	00040004 	andeq	r0, r4, r4
8780968c:	4020c03f 	eormi	ip, r0, pc, lsr r0
87809690:	40204020 	eormi	r4, r0, r0, lsr #32
87809694:	4020c03f 	eormi	ip, r0, pc, lsr r0
87809698:	00004020 	andeq	r4, r0, r0, lsr #32
8780969c:	20206000 	eorcs	r6, r0, r0
878096a0:	c03f2020 	eorsgt	r2, pc, r0, lsr #32
878096a4:	00200020 	eoreq	r0, r0, r0, lsr #32
878096a8:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
878096ac:	000b4024 	andeq	r4, fp, r4, lsr #32
878096b0:	4020c030 	eormi	ip, r0, r0, lsr r0
878096b4:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
878096b8:	40004020 	andmi	r4, r0, r0, lsr #32
878096bc:	c0004000 	andgt	r4, r0, r0
878096c0:	003cc03f 	eorseq	ip, ip, pc, lsr r0
878096c4:	003cc003 	eorseq	ip, ip, r3
878096c8:	0000c03f 	andeq	ip, r0, pc, lsr r0
878096cc:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
878096d0:	0023400c 	eoreq	r4, r3, ip
878096d4:	0020c03f 	eoreq	ip, r0, pc, lsr r0
878096d8:	4020801f 	eormi	r8, r0, pc, lsl r0
878096dc:	40204020 	eormi	r4, r0, r0, lsr #32
878096e0:	0000801f 	andeq	r8, r0, pc, lsl r0
878096e4:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
878096e8:	00244024 	eoreq	r4, r4, r4, lsr #32
878096ec:	00000018 	andeq	r0, r0, r8, lsl r0
878096f0:	4021801f 	eormi	r8, r1, pc, lsl r0
878096f4:	e0204021 	eor	r4, r0, r1, lsr #32
878096f8:	0000a01f 	andeq	sl, r0, pc, lsl r0
878096fc:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
87809700:	00264024 	eoreq	r4, r6, r4, lsr #32
87809704:	4000c019 	andmi	ip, r0, r9, lsl r0
87809708:	4024c018 	eormi	ip, r4, r8, lsl r0
8780970c:	40224024 	eormi	r4, r2, r4, lsr #32
87809710:	00008031 	andeq	r8, r0, r1, lsr r0
87809714:	40200030 	eormi	r0, r0, r0, lsr r0
87809718:	4020c03f 	eormi	ip, r0, pc, lsr r0
8780971c:	00000030 	andeq	r0, r0, r0, lsr r0
87809720:	803f0020 	eorshi	r0, pc, r0, lsr #32
87809724:	40004000 	andmi	r4, r0, r0
87809728:	0020803f 	eoreq	r8, r0, pc, lsr r0
8780972c:	003e0020 	eorseq	r0, lr, r0, lsr #32
87809730:	0007c001 	andeq	ip, r7, r1
87809734:	00200038 	eoreq	r0, r0, r8, lsr r0
87809738:	c0070038 	andgt	r0, r7, r8, lsr r0
8780973c:	c007003c 	andgt	r0, r7, ip, lsr r0
87809740:	00000038 	andeq	r0, r0, r8, lsr r0
87809744:	c0394020 	eorsgt	r4, r9, r0, lsr #32
87809748:	c0390006 	eorsgt	r0, r9, r6
8780974c:	00004020 	andeq	r4, r0, r0, lsr #32
87809750:	40380020 	eorsmi	r0, r8, r0, lsr #32
87809754:	4038c007 	eorsmi	ip, r8, r7
87809758:	00000020 	andeq	r0, r0, r0, lsr #32
8780975c:	c0214030 	eorgt	r4, r1, r0, lsr r0
87809760:	40384026 	eorsmi	r4, r8, r6, lsr #32
87809764:	0000c020 	andeq	ip, r0, r0, lsr #32
87809768:	00000000 	andeq	r0, r0, r0
8780976c:	2040e07f 	subcs	lr, r0, pc, ror r0
87809770:	00002040 	andeq	r2, r0, r0, asr #32
87809774:	00700000 	rsbseq	r0, r0, r0
87809778:	8003000c 	andhi	r0, r3, ip
8780977c:	00004000 	andeq	r4, r0, r0
87809780:	20400000 	subcs	r0, r0, r0
87809784:	e07f2040 	rsbs	r2, pc, r0, asr #32
87809788:	00000000 	andeq	r0, r0, r0
8780978c:	00200000 	eoreq	r0, r0, r0
87809790:	00200040 	eoreq	r0, r0, r0, asr #32
87809794:	00000000 	andeq	r0, r0, r0
87809798:	10001000 	andne	r1, r0, r0
8780979c:	10001000 	andne	r1, r0, r0
878097a0:	10001000 	andne	r1, r0, r0
878097a4:	00000000 	andeq	r0, r0, r0
878097a8:	00000040 	andeq	r0, r0, r0, asr #32
878097ac:	00000000 	andeq	r0, r0, r0
878097b0:	80020000 	andhi	r0, r2, r0
878097b4:	40054005 	andmi	r4, r5, r5
878097b8:	4000c003 	andmi	ip, r0, r3
878097bc:	c03f0020 	eorsgt	r0, pc, r0, lsr #32
878097c0:	40044004 	andmi	r4, r4, r4
878097c4:	00008003 	andeq	r8, r0, r3
878097c8:	80030000 	andhi	r0, r3, r0
878097cc:	40044004 	andmi	r4, r4, r4
878097d0:	00004006 	andeq	r4, r0, r6
878097d4:	80030000 	andhi	r0, r3, r0
878097d8:	40244004 	eormi	r4, r4, r4
878097dc:	4000c03f 	andmi	ip, r0, pc, lsr r0
878097e0:	80030000 	andhi	r0, r3, r0
878097e4:	40054005 	andmi	r4, r5, r5
878097e8:	00004003 	andeq	r4, r0, r3
878097ec:	40040000 	andmi	r0, r4, r0
878097f0:	4024c01f 	eormi	ip, r4, pc, lsl r0
878097f4:	00204024 	eoreq	r4, r0, r4, lsr #32
878097f8:	e0020000 	and	r0, r2, r0
878097fc:	50055005 	andpl	r5, r5, r5
87809800:	20045006 	andcs	r5, r4, r6
87809804:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
87809808:	00044004 	andeq	r4, r4, r4
8780980c:	4000c003 	andmi	ip, r0, r3
87809810:	40040000 	andmi	r0, r4, r0
87809814:	4000c027 	andmi	ip, r0, r7, lsr #32
87809818:	00000000 	andeq	r0, r0, r0
8780981c:	10001000 	andne	r1, r0, r0
87809820:	e0271004 	eor	r1, r7, r4
87809824:	00000000 	andeq	r0, r0, r0
87809828:	c03f4020 	eorsgt	r4, pc, r0, lsr #32
8780982c:	00074001 	andeq	r4, r7, r1
87809830:	4004c004 	andmi	ip, r4, r4
87809834:	40204020 	eormi	r4, r0, r0, lsr #32
87809838:	4000c03f 	andmi	ip, r0, pc, lsr r0
8780983c:	00004000 	andeq	r4, r0, r0
87809840:	0004c007 	andeq	ip, r4, r7
87809844:	0004c007 	andeq	ip, r4, r7
87809848:	0000c003 	andeq	ip, r0, r3
8780984c:	c0074004 	andgt	r4, r7, r4
87809850:	00044004 	andeq	r4, r4, r4
87809854:	4000c003 	andmi	ip, r0, r3
87809858:	80030000 	andhi	r0, r3, r0
8780985c:	40044004 	andmi	r4, r4, r4
87809860:	00008003 	andeq	r8, r0, r3
87809864:	f0071004 			; <UNDEFINED> instruction: 0xf0071004
87809868:	40045004 	andmi	r5, r4, r4
8780986c:	00008003 	andeq	r8, r0, r3
87809870:	80030000 	andhi	r0, r3, r0
87809874:	50044004 	andpl	r4, r4, r4
87809878:	1000f007 	andne	pc, r0, r7
8780987c:	c0074004 	andgt	r4, r7, r4
87809880:	00044002 	andeq	r4, r4, r2
87809884:	00000004 	andeq	r0, r0, r4
87809888:	40060000 	andmi	r0, r6, r0
8780988c:	40054005 	andmi	r4, r5, r5
87809890:	0000c004 	andeq	ip, r0, r4
87809894:	00040000 	andeq	r0, r4, r0
87809898:	4004801f 	andmi	r8, r4, pc, lsl r0
8780989c:	00004000 	andeq	r4, r0, r0
878098a0:	80070004 	andhi	r0, r7, r4
878098a4:	40044000 	andmi	r4, r4, r0
878098a8:	4000c007 	andmi	ip, r0, r7
878098ac:	00070004 	andeq	r0, r7, r4
878098b0:	8001c004 	andhi	ip, r1, r4
878098b4:	00040006 	andeq	r0, r4, r6
878098b8:	c0010006 	andgt	r0, r1, r6
878098bc:	c0010007 	andgt	r0, r1, r7
878098c0:	00000006 	andeq	r0, r0, r6
878098c4:	c0064004 	andgt	r4, r6, r4
878098c8:	c0060001 	andgt	r0, r6, r1
878098cc:	00004004 	andeq	r4, r0, r4
878098d0:	10071004 	andne	r1, r7, r4
878098d4:	8001e004 	andhi	lr, r1, r4
878098d8:	00040006 	andeq	r0, r4, r6
878098dc:	40040000 	andmi	r0, r4, r0
878098e0:	4006c005 	andmi	ip, r6, r5
878098e4:	00004004 	andeq	r4, r0, r4
878098e8:	00000000 	andeq	r0, r0, r0
878098ec:	e07b0004 	rsbs	r0, fp, r4
878098f0:	00002040 	andeq	r2, r0, r0, asr #32
878098f4:	00000000 	andeq	r0, r0, r0
878098f8:	f0ff0000 			; <UNDEFINED> instruction: 0xf0ff0000
878098fc:	00000000 	andeq	r0, r0, r0
87809900:	20400000 	subcs	r0, r0, r0
87809904:	0004e07b 	andeq	lr, r4, fp, ror r0
87809908:	00000000 	andeq	r0, r0, r0
8780990c:	00800040 	addeq	r0, r0, r0, asr #32
87809910:	00200040 	eoreq	r0, r0, r0, asr #32
87809914:	00400020 	subeq	r0, r0, r0, lsr #32

87809918 <asc2_2412>:
	...
87809948:	0f000000 	svceq	0x00000000
8780994c:	fe0f3880 	cdp2	8, 0, cr3, cr15, cr0, {4}
87809950:	38800f38 	stmcc	r0, {r3, r4, r5, r8, r9, sl, fp}
	...
87809964:	00010000 	andeq	r0, r1, r0
87809968:	00000600 	andeq	r0, r0, r0, lsl #12
8780996c:	3800000c 	stmdacc	r0, {r2, r3}
87809970:	00310000 	eorseq	r0, r1, r0
87809974:	00000600 	andeq	r0, r0, r0, lsl #12
87809978:	3800000c 	stmdacc	r0, {r2, r3}
8780997c:	00300000 	eorseq	r0, r0, r0
	...
87809988:	67008061 	strvs	r8, [r0, -r1, rrx]
8780998c:	80f907f8 	ldrshthi	r0, [r9], #120	; 0x78
87809990:	00806100 	addeq	r6, r0, r0, lsl #2
87809994:	61008061 	tstvs	r0, r1, rrx
87809998:	80610080 	rsbhi	r0, r1, r0, lsl #1
8780999c:	07f86700 	ldrbeq	r6, [r8, r0, lsl #14]!
878099a0:	610080f9 	strdvs	r8, [r0, -r9]
878099a4:	00000080 	andeq	r0, r0, r0, lsl #1
878099a8:	00000000 	andeq	r0, r0, r0
878099ac:	c0010000 	andgt	r0, r1, r0
878099b0:	f0e003e0 			; <UNDEFINED> instruction: 0xf0e003e0
878099b4:	04083006 	streq	r3, [r8], #-6
878099b8:	ff1f0818 			; <UNDEFINED> instruction: 0xff1f0818
878099bc:	080e04fe 	stmdaeq	lr, {r1, r2, r3, r4, r5, r6, r7, sl}
878099c0:	03f08707 	mvnseq	r8, #1835008	; 0x1c0000
878099c4:	0000e081 	andeq	lr, r0, r1, lsl #1
878099c8:	00000000 	andeq	r0, r0, r0
878099cc:	0600f001 	streq	pc, [r0], -r1
878099d0:	0404000c 	streq	r0, [r4], #-12
878099d4:	700c0608 	andvc	r0, ip, r8, lsl #12
878099d8:	00c0f901 	sbceq	pc, r0, r1, lsl #18
878099dc:	3b00000e 	blcc	87809a1c <asc2_2412+0x104>
878099e0:	18ec00e0 	stmiane	ip!, {r5, r6, r7}^
878099e4:	04080807 	streq	r0, [r8], #-2055	; 0x807
878099e8:	0300180c 	movweq	r1, #2060	; 0x80c
878099ec:	000000e0 	andeq	r0, r0, r0, ror #1
878099f0:	00e00100 	rsceq	r0, r0, r0, lsl #2
878099f4:	f803f007 			; <UNDEFINED> instruction: 0xf803f007
878099f8:	081c0418 	ldmdaeq	ip, {r3, r4, sl}
878099fc:	07081704 	streq	r1, [r8, -r4, lsl #14]
87809a00:	c003d0e1 	andgt	sp, r3, r1, ror #1
87809a04:	b02300e0 	eorlt	r0, r3, r0, ror #1
87809a08:	00083c00 	andeq	r3, r8, r0, lsl #24
87809a0c:	00000820 	andeq	r0, r0, r0, lsr #16
87809a10:	00000010 	andeq	r0, r0, r0, lsl r0
87809a14:	01000000 	mrseq	r0, (UNDEF: 0)
87809a18:	00310000 	eorseq	r0, r1, r0
87809a1c:	00003200 	andeq	r3, r0, r0, lsl #4
87809a20:	0000001c 	andeq	r0, r0, ip, lsl r0
	...
87809a48:	ff01007f 			; <UNDEFINED> instruction: 0xff01007f
87809a4c:	f08007c0 			; <UNDEFINED> instruction: 0xf08007c0
87809a50:	1018000c 	andsne	r0, r8, ip
87809a54:	00200400 	eoreq	r0, r0, r0, lsl #8
87809a58:	00000002 	andeq	r0, r0, r2
87809a5c:	20000000 	andcs	r0, r0, r0
87809a60:	00100200 	andseq	r0, r0, r0, lsl #4
87809a64:	18000c04 	stmdane	r0, {r2, sl, fp}
87809a68:	01f08007 	mvnseq	r8, r7
87809a6c:	7f00c0ff 	svcvc	0x0000c0ff
	...
87809a84:	66000042 	strvs	r0, [r0], -r2, asr #32
87809a88:	00660000 	rsbeq	r0, r6, r0
87809a8c:	00003c00 	andeq	r3, r0, r0, lsl #24
87809a90:	ff030018 			; <UNDEFINED> instruction: 0xff030018
87809a94:	001800c0 	andseq	r0, r8, r0, asr #1
87809a98:	00003c00 	andeq	r3, r0, r0, lsl #24
87809a9c:	66000066 	strvs	r0, [r0], -r6, rrx
87809aa0:	00420000 	subeq	r0, r2, r0
87809aa4:	00000000 	andeq	r0, r0, r0
87809aa8:	08000008 	stmdaeq	r0, {r3}
87809aac:	00080000 	andeq	r0, r8, r0
87809ab0:	00000800 	andeq	r0, r0, r0, lsl #16
87809ab4:	ff010008 			; <UNDEFINED> instruction: 0xff010008
87809ab8:	000800c0 	andeq	r0, r8, r0, asr #1
87809abc:	00000800 	andeq	r0, r0, r0, lsl #16
87809ac0:	08000008 	stmdaeq	r0, {r3}
87809ac4:	00080000 	andeq	r0, r8, r0
87809ac8:	00000000 	andeq	r0, r0, r0
87809acc:	00000100 	andeq	r0, r0, r0, lsl #2
87809ad0:	32000031 	andcc	r0, r0, #49	; 0x31
87809ad4:	001c0000 	andseq	r0, ip, r0
	...
87809af0:	08000008 	stmdaeq	r0, {r3}
87809af4:	00080000 	andeq	r0, r8, r0
87809af8:	00000800 	andeq	r0, r0, r0, lsl #16
87809afc:	08000008 	stmdaeq	r0, {r3}
87809b00:	00080000 	andeq	r0, r8, r0
87809b04:	00000800 	andeq	r0, r0, r0, lsl #16
87809b08:	08000008 	stmdaeq	r0, {r3}
	...
87809b18:	38000038 	stmdacc	r0, {r3, r4, r5}
87809b1c:	00380000 	eorseq	r0, r8, r0
	...
87809b38:	00000600 	andeq	r0, r0, r0, lsl #12
87809b3c:	7000001c 	andvc	r0, r0, ip, lsl r0
87809b40:	00800100 	addeq	r0, r0, r0, lsl #2
87809b44:	3800000e 	stmdacc	r0, {r1, r2, r3}
87809b48:	00c00000 	sbceq	r0, r0, r0
87809b4c:	1c000007 	stcne	0, cr0, [r0], {7}
87809b50:	00300000 	eorseq	r0, r0, r0
	...
87809b5c:	ff01807f 			; <UNDEFINED> instruction: 0xff01807f
87809b60:	708003e0 	addvc	r0, r0, r0, ror #7
87809b64:	04180006 	ldreq	r0, [r8], #-6
87809b68:	00040800 	andeq	r0, r4, r0, lsl #16
87809b6c:	18000608 	stmdane	r0, {r3, r9, sl}
87809b70:	01708003 	cmneq	r0, r3
87809b74:	7f00e0ff 	svcvc	0x0000e0ff
87809b78:	00000080 	andeq	r0, r0, r0, lsl #1
87809b7c:	00000000 	andeq	r0, r0, r0
87809b80:	00010000 	andeq	r0, r1, r0
87809b84:	08000108 	stmdaeq	r0, {r3, r8}
87809b88:	03080001 	movweq	r0, #32769	; 0x8001
87809b8c:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809b90:	080000f8 	stmdaeq	r0, {r3, r4, r5, r6, r7}
87809b94:	00080000 	andeq	r0, r8, r0
87809b98:	00000800 	andeq	r0, r0, r0, lsl #16
87809b9c:	00000000 	andeq	r0, r0, r0
87809ba0:	01000000 	mrseq	r0, (UNDEF: 0)
87809ba4:	c00238c0 	andgt	r3, r2, r0, asr #17
87809ba8:	98000458 	stmdals	r0, {r3, r4, r6, sl}
87809bac:	04180104 	ldreq	r0, [r8], #-260	; 0x104
87809bb0:	04041802 	streq	r1, [r4], #-2050	; 0x802
87809bb4:	181c0618 	ldmdane	ip, {r3, r4, r9, sl}
87809bb8:	0118f803 	tsteq	r8, r3, lsl #16
87809bbc:	0000f8e0 	andeq	pc, r0, r0, ror #17
87809bc0:	00000000 	andeq	r0, r0, r0
87809bc4:	01000000 	mrseq	r0, (UNDEF: 0)
87809bc8:	c003e0c0 	andgt	lr, r3, r0, asr #1
87809bcc:	080004f0 	stmdaeq	r0, {r4, r5, r6, r7, sl}
87809bd0:	04080804 	streq	r0, [r8], #-2052	; 0x804
87809bd4:	18060808 	stmdane	r6, {r3, fp}
87809bd8:	18f40308 	ldmne	r4!, {r3, r8, r9}^
87809bdc:	00f0e701 	rscseq	lr, r0, r1, lsl #14
87809be0:	0000e001 	andeq	lr, r0, r1
	...
87809bec:	0d000003 	stceq	0, cr0, [r0, #-12]
87809bf0:	00110000 	andseq	r0, r1, r0
87809bf4:	00006100 	andeq	r6, r0, r0, lsl #2
87809bf8:	01030881 	smlabbeq	r3, r1, r8, r0
87809bfc:	f8ff0708 			; <UNDEFINED> instruction: 0xf8ff0708
87809c00:	00f8ff0f 	rscseq	pc, r8, pc, lsl #30
87809c04:	01000801 	tsteq	r0, r1, lsl #16
87809c08:	00000008 	andeq	r0, r0, r8
87809c0c:	00000000 	andeq	r0, r0, r0
87809c10:	fc07e000 	stc2	0, cr14, [r7], {-0}
87809c14:	080806d0 	stmdaeq	r8, {r4, r6, r7, r9, sl}
87809c18:	06081006 	streq	r1, [r8], -r6
87809c1c:	10060810 	andne	r0, r6, r0, lsl r8
87809c20:	38180608 	ldmdacc	r8, {r3, r9, sl}
87809c24:	06f00f06 	ldrbteq	r0, [r0], r6, lsl #30
87809c28:	0000c007 	andeq	ip, r0, r7
	...
87809c34:	ff01803f 			; <UNDEFINED> instruction: 0xff01803f
87809c38:	308403e0 	addcc	r0, r4, r0, ror #7
87809c3c:	04180802 	ldreq	r0, [r8], #-2050	; 0x802
87809c40:	10040810 	andne	r0, r4, r0, lsl r8
87809c44:	08100408 	ldmdaeq	r0, {r3, sl}
87809c48:	03101807 	tsteq	r0, #458752	; 0x70000
87809c4c:	0700f00f 	streq	pc, [r0, -pc]
87809c50:	000000c0 	andeq	r0, r0, r0, asr #1
87809c54:	00000000 	andeq	r0, r0, r0
87809c58:	c0030000 	andgt	r0, r3, r0
87809c5c:	00000700 	andeq	r0, r0, r0, lsl #14
87809c60:	06000006 	streq	r0, [r0], -r6
87809c64:	0706f800 	streq	pc, [r6, -r0, lsl #16]
87809c68:	001806f8 			; <UNDEFINED> instruction: 0x001806f8
87809c6c:	0700e006 	streq	lr, [r0, -r6]
87809c70:	00060000 	andeq	r0, r6, r0
87809c74:	00000000 	andeq	r0, r0, r0
87809c78:	01000000 	mrseq	r0, (UNDEF: 0)
87809c7c:	f703e0e1 			; <UNDEFINED> instruction: 0xf703e0e1
87809c80:	103406f0 	ldrshtne	r0, [r4], -r0
87809c84:	04081804 	streq	r1, [r8], #-2052	; 0x804
87809c88:	0c040818 	stceq	8, cr0, [r4], {24}
87809c8c:	080c0408 	stmdaeq	ip, {r3, sl}
87809c90:	03181606 	tsteq	r8, #6291456	; 0x600000
87809c94:	c101f0f3 	strdgt	pc, [r1, -r3]
87809c98:	000000e0 	andeq	r0, r0, r0, ror #1
87809c9c:	00000000 	andeq	r0, r0, r0
87809ca0:	fc0300f8 	stc2	0, cr0, [r3], {248}	; 0xf8
87809ca4:	38060330 	stmdacc	r6, {r4, r5, r8, r9}
87809ca8:	04080204 	streq	r0, [r8], #-516	; 0x204
87809cac:	02040802 	andeq	r0, r4, #131072	; 0x20000
87809cb0:	10040408 	andne	r0, r4, r8, lsl #8
87809cb4:	01f00803 	mvnseq	r0, r3, lsl #16
87809cb8:	7f00c0ff 	svcvc	0x0000c0ff
	...
87809cd0:	70003870 	andvc	r3, r0, r0, ror r8
87809cd4:	38700038 	ldmdacc	r0!, {r3, r4, r5}^
	...
87809cf4:	30001a30 	andcc	r1, r0, r0, lsr sl
87809cf8:	0000001c 	andeq	r0, r0, ip, lsl r0
	...
87809d0c:	08000000 	stmdaeq	r0, {}	; <UNPREDICTABLE>
87809d10:	00140000 	andseq	r0, r4, r0
87809d14:	00002200 	andeq	r2, r0, r0, lsl #4
87809d18:	80000041 	andhi	r0, r0, r1, asr #32
87809d1c:	40000180 	andmi	r0, r0, r0, lsl #3
87809d20:	04200002 	strteq	r0, [r0], #-2
87809d24:	00081000 	andeq	r1, r8, r0
87809d28:	00000008 	andeq	r0, r0, r8
87809d2c:	00000000 	andeq	r0, r0, r0
87809d30:	21000021 	tstcs	r0, r1, lsr #32
87809d34:	00210000 	eoreq	r0, r1, r0
87809d38:	00002100 	andeq	r2, r0, r0, lsl #2
87809d3c:	21000021 	tstcs	r0, r1, lsr #32
87809d40:	00210000 	eoreq	r0, r1, r0
87809d44:	00002100 	andeq	r2, r0, r0, lsl #2
87809d48:	21000021 	tstcs	r0, r1, lsr #32
	...
87809d54:	00080000 	andeq	r0, r8, r0
87809d58:	10000408 	andne	r0, r0, r8, lsl #8
87809d5c:	01200002 	teqeq	r0, r2
87809d60:	80004000 	andhi	r4, r0, r0
87809d64:	00410080 	subeq	r0, r1, r0, lsl #1
87809d68:	00002200 	andeq	r2, r0, r0, lsl #4
87809d6c:	08000014 	stmdaeq	r0, {r2, r4}
87809d70:	00000000 	andeq	r0, r0, r0
87809d74:	03000000 	movweq	r0, #0
87809d78:	c00400c0 	andgt	r0, r4, r0, asr #1
87809d7c:	00000400 	andeq	r0, r0, r0, lsl #8
87809d80:	08380008 	ldmdaeq	r8!, {r3}
87809d84:	0808380f 	stmdaeq	r8, {r0, r1, r2, r3, fp, ip, sp}
87809d88:	00100838 	andseq	r0, r0, r8, lsr r8
87809d8c:	0700300c 	streq	r3, [r0, -ip]
87809d90:	c00300e0 	andgt	r0, r3, r0, ror #1
	...
87809d9c:	ff00803f 			; <UNDEFINED> instruction: 0xff00803f
87809da0:	708003e0 	addvc	r0, r0, r0, ror #7
87809da4:	06100f02 	ldreq	r0, [r0], -r2, lsl #30
87809da8:	c0048870 	andgt	r8, r4, r0, ror r8
87809dac:	08830488 	stmeq	r3, {r3, r7, sl}
87809db0:	02887f04 	addeq	r7, r8, #4, 30
87809db4:	010390c0 	smlabteq	r3, r0, r0, r9
87809db8:	40fe0020 	rscsmi	r0, lr, r0, lsr #32
87809dbc:	00080000 	andeq	r0, r8, r0
87809dc0:	01001800 	tsteq	r0, r0, lsl #16
87809dc4:	083e00f8 	ldmdaeq	lr!, {r3, r4, r5, r6, r7}
87809dc8:	0700c201 	streq	ip, [r0, -r1, lsl #4]
87809dcc:	e2070002 	and	r0, r7, #2
87809dd0:	00fe0000 	rscseq	r0, lr, r0
87809dd4:	00c81f00 	sbceq	r1, r8, r0, lsl #30
87809dd8:	0000f801 	andeq	pc, r0, r1, lsl #16
87809ddc:	08000038 	stmdaeq	r0, {r3, r4, r5}
87809de0:	07080004 	streq	r0, [r8, -r4]
87809de4:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809de8:	080804f8 	stmdaeq	r8, {r3, r4, r5, r6, r7, sl}
87809dec:	04080804 	streq	r0, [r8], #-2052	; 0x804
87809df0:	08040808 	stmdaeq	r4, {r3, fp}
87809df4:	08180608 	ldmdaeq	r8, {r3, r9, sl}
87809df8:	0118f403 	tsteq	r8, r3, lsl #8
87809dfc:	0100f0e7 	smlatteq	r0, r7, r0, pc	; <UNPREDICTABLE>
87809e00:	000000e0 	andeq	r0, r0, r0, ror #1
87809e04:	00000000 	andeq	r0, r0, r0
87809e08:	ff01803f 			; <UNDEFINED> instruction: 0xff01803f
87809e0c:	708003e0 	addvc	r0, r0, r0, ror #7
87809e10:	04180002 	ldreq	r0, [r8], #-2
87809e14:	00040800 	andeq	r0, r4, r0, lsl #16
87809e18:	08000408 	stmdaeq	r0, {r3, sl}
87809e1c:	06100004 	ldreq	r0, [r0], -r4
87809e20:	80072000 	andhi	r2, r7, r0
87809e24:	000000c0 	andeq	r0, r0, r0, asr #1
87809e28:	07080004 	streq	r0, [r8, -r4]
87809e2c:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809e30:	080004f8 	stmdaeq	r0, {r3, r4, r5, r6, r7, sl}
87809e34:	04080004 	streq	r0, [r8], #-4
87809e38:	00040800 	andeq	r0, r4, r0, lsl #16
87809e3c:	10000218 	andne	r0, r0, r8, lsl r2
87809e40:	01708003 	cmneq	r0, r3
87809e44:	7f00e0ff 	svcvc	0x0000e0ff
87809e48:	00000080 	andeq	r0, r0, r0, lsl #1
87809e4c:	07080004 	streq	r0, [r8, -r4]
87809e50:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809e54:	080804f8 	stmdaeq	r8, {r3, r4, r5, r6, r7, sl}
87809e58:	04080804 	streq	r0, [r8], #-2052	; 0x804
87809e5c:	08040808 	stmdaeq	r4, {r3, fp}
87809e60:	083e0408 	ldmdaeq	lr!, {r3, sl}
87809e64:	06080004 	streq	r0, [r8], -r4
87809e68:	00011800 	andeq	r1, r1, r0, lsl #16
87809e6c:	00000060 	andeq	r0, r0, r0, rrx
87809e70:	07080004 	streq	r0, [r8, -r4]
87809e74:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809e78:	080804f8 	stmdaeq	r8, {r3, r4, r5, r6, r7, sl}
87809e7c:	04000804 	streq	r0, [r0], #-2052	; 0x804
87809e80:	08040008 	stmdaeq	r4, {r3}
87809e84:	003e0400 	eorseq	r0, lr, r0, lsl #8
87809e88:	06000006 	streq	r0, [r0], -r6
87809e8c:	80010000 	andhi	r0, r1, r0
	...
87809e98:	ff01803f 			; <UNDEFINED> instruction: 0xff01803f
87809e9c:	708003e0 	addvc	r0, r0, r0, ror #7
87809ea0:	04180006 	ldreq	r0, [r8], #-6
87809ea4:	02040800 	andeq	r0, r4, #0, 16
87809ea8:	08020408 	stmdaeq	r2, {r3, sl}
87809eac:	07f00302 	ldrbeq	r0, [r0, r2, lsl #6]!
87809eb0:	0200f083 	andeq	pc, r0, #131	; 0x83
87809eb4:	00020000 	andeq	r0, r2, r0
87809eb8:	07080004 	streq	r0, [r8, -r4]
87809ebc:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809ec0:	080804f8 	stmdaeq	r8, {r3, r4, r5, r6, r7, sl}
87809ec4:	00000800 	andeq	r0, r0, r0, lsl #16
87809ec8:	08000008 	stmdaeq	r0, {r3}
87809ecc:	00080000 	andeq	r0, r8, r0
87809ed0:	07080804 	streq	r0, [r8, -r4, lsl #16]
87809ed4:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809ed8:	080004f8 	stmdaeq	r0, {r3, r4, r5, r6, r7, sl}
87809edc:	00000000 	andeq	r0, r0, r0
87809ee0:	00040000 	andeq	r0, r4, r0
87809ee4:	08000408 	stmdaeq	r0, {r3, sl}
87809ee8:	07080004 	streq	r0, [r8, -r4]
87809eec:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809ef0:	080004f8 	stmdaeq	r0, {r3, r4, r5, r6, r7, sl}
87809ef4:	04080004 	streq	r0, [r8], #-4
87809ef8:	00000800 	andeq	r0, r0, r0, lsl #16
	...
87809f04:	00000600 	andeq	r0, r0, r0, lsl #12
87809f08:	01000007 	tsteq	r0, r7
87809f0c:	04010004 	streq	r0, [r1], #-4
87809f10:	00040100 	andeq	r0, r4, r0, lsl #2
87809f14:	feff0703 	cdp2	7, 15, cr0, cr15, cr3, {0}
87809f18:	04fcff07 	ldrbteq	pc, [ip], #3847	; 0xf07	; <UNPREDICTABLE>
87809f1c:	00040000 	andeq	r0, r4, r0
87809f20:	00000400 	andeq	r0, r0, r0, lsl #8
87809f24:	07080004 	streq	r0, [r8, -r4]
87809f28:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809f2c:	080c04f8 	stmdaeq	ip, {r3, r4, r5, r6, r7, sl}
87809f30:	00001800 	andeq	r1, r0, r0, lsl #16
87809f34:	c704003e 	smladxgt	r4, lr, r0, r0
87809f38:	c8030580 	stmdagt	r3, {r7, r8, sl}
87809f3c:	04f80006 	ldrbteq	r0, [r8], #6
87809f40:	00043800 	andeq	r3, r4, r0, lsl #16
87809f44:	08000018 	stmdaeq	r0, {r3, r4}
87809f48:	07080004 	streq	r0, [r8, -r4]
87809f4c:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809f50:	080004f8 	stmdaeq	r0, {r3, r4, r5, r6, r7, sl}
87809f54:	00080000 	andeq	r0, r8, r0
87809f58:	00000800 	andeq	r0, r0, r0, lsl #16
87809f5c:	08000008 	stmdaeq	r0, {r3}
87809f60:	00080000 	andeq	r0, r8, r0
87809f64:	00001800 	andeq	r1, r0, r0, lsl #16
87809f68:	00000060 	andeq	r0, r0, r0, rrx
87809f6c:	07080004 	streq	r0, [r8, -r4]
87809f70:	8007f8ff 	strdhi	pc, [r7], -pc	; <UNPREDICTABLE>
87809f74:	00fc0708 	rscseq	r0, ip, r8, lsl #14
87809f78:	00c07f00 	sbceq	r7, r0, r0, lsl #30
87809f7c:	0700f803 	streq	pc, [r0, -r3, lsl #16]
87809f80:	007800c0 	rsbseq	r0, r8, r0, asr #1
87809f84:	07088007 	streq	r8, [r8, -r7]
87809f88:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809f8c:	080004f8 	stmdaeq	r0, {r3, r4, r5, r6, r7, sl}
87809f90:	07080004 	streq	r0, [r8, -r4]
87809f94:	0007f8ff 	strdeq	pc, [r7], -pc	; <UNPREDICTABLE>
87809f98:	00c00308 	sbceq	r0, r0, r8, lsl #6
87809f9c:	0000e000 	andeq	lr, r0, r0
87809fa0:	1e000038 	mcrne	0, 0, r0, cr0, cr8, {1}
87809fa4:	00070000 	andeq	r0, r7, r0
87809fa8:	04c00100 	strbeq	r0, [r0], #256	; 0x100
87809fac:	ff07f000 			; <UNDEFINED> instruction: 0xff07f000
87809fb0:	000004f8 	strdeq	r0, [r0], -r8
87809fb4:	00000000 	andeq	r0, r0, r0
87809fb8:	ff01807f 			; <UNDEFINED> instruction: 0xff01807f
87809fbc:	708003e0 	addvc	r0, r0, r0, ror #7
87809fc0:	04180006 	ldreq	r0, [r8], #-6
87809fc4:	00040800 	andeq	r0, r4, r0, lsl #16
87809fc8:	18000608 	stmdane	r0, {r3, r9, sl}
87809fcc:	01300003 	teqeq	r0, r3
87809fd0:	7f00e0ff 	svcvc	0x0000e0ff
87809fd4:	00000080 	andeq	r0, r0, r0, lsl #1
87809fd8:	07080004 	streq	r0, [r8, -r4]
87809fdc:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
87809fe0:	080404f8 	stmdaeq	r4, {r3, r4, r5, r6, r7, sl}
87809fe4:	04000404 	streq	r0, [r0], #-1028	; 0x404
87809fe8:	04040004 	streq	r0, [r4], #-4
87809fec:	00040400 	andeq	r0, r4, r0, lsl #8
87809ff0:	03000c06 	movweq	r0, #3078	; 0xc06
87809ff4:	f00100f8 			; <UNDEFINED> instruction: 0xf00100f8
	...
8780a000:	ff01807f 			; <UNDEFINED> instruction: 0xff01807f
8780a004:	708003e0 	addvc	r0, r0, r0, ror #7
8780a008:	04880006 	streq	r0, [r8], #6
8780a00c:	00048800 	andeq	r8, r4, r0, lsl #16
8780a010:	3c0006c8 	stccc	6, cr0, [r0], {200}	; 0xc8
8780a014:	013e0003 	teqeq	lr, r3
8780a018:	7f00e6ff 	svcvc	0x0000e6ff
8780a01c:	00000084 	andeq	r0, r0, r4, lsl #1
8780a020:	07080004 	streq	r0, [r8, -r4]
8780a024:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
8780a028:	080804f8 	stmdaeq	r8, {r3, r4, r5, r6, r7, sl}
8780a02c:	04000804 	streq	r0, [r0], #-2052	; 0x804
8780a030:	0f04000c 	svceq	0x0004000c
8780a034:	c00b0400 	andgt	r0, fp, r0, lsl #8
8780a038:	03f01006 	mvnseq	r1, #6
8780a03c:	e00138f0 	strd	r3, [r1], -r0
8780a040:	08000008 	stmdaeq	r0, {r3}
8780a044:	01000000 	mrseq	r0, (UNDEF: 0)
8780a048:	f003f8e0 			; <UNDEFINED> instruction: 0xf003f8e0
8780a04c:	10300630 	eorsne	r0, r0, r0, lsr r6
8780a050:	04081804 	streq	r1, [r8], #-2052	; 0x804
8780a054:	0c040818 	stceq	8, cr0, [r4], {24}
8780a058:	080c0408 	stmdaeq	ip, {r3, sl}
8780a05c:	02180602 	andseq	r0, r8, #2097152	; 0x200000
8780a060:	8107f007 	tsthi	r7, r7
8780a064:	000000e0 	andeq	r0, r0, r0, ror #1
8780a068:	06008001 	streq	r8, [r0], -r1
8780a06c:	00040000 	andeq	r0, r4, r0
8780a070:	00000400 	andeq	r0, r0, r0, lsl #8
8780a074:	07080004 	streq	r0, [r8, -r4]
8780a078:	ff07f8ff 			; <UNDEFINED> instruction: 0xff07f8ff
8780a07c:	080004f8 	stmdaeq	r0, {r3, r4, r5, r6, r7, sl}
8780a080:	04000004 	streq	r0, [r0], #-4
8780a084:	00060000 	andeq	r0, r6, r0
8780a088:	00800100 	addeq	r0, r0, r0, lsl #2
8780a08c:	07000004 	streq	r0, [r0, -r4]
8780a090:	ff07e0ff 			; <UNDEFINED> instruction: 0xff07e0ff
8780a094:	180004f0 	stmdane	r0, {r4, r5, r6, r7, sl}
8780a098:	00080000 	andeq	r0, r8, r0
8780a09c:	00000800 	andeq	r0, r0, r0, lsl #16
8780a0a0:	08000008 	stmdaeq	r0, {r3}
8780a0a4:	04080000 	streq	r0, [r8], #-0
8780a0a8:	ff071000 			; <UNDEFINED> instruction: 0xff071000
8780a0ac:	000004e0 	andeq	r0, r0, r0, ror #9
8780a0b0:	06000004 	streq	r0, [r0], -r4
8780a0b4:	e0070000 	and	r0, r7, r0
8780a0b8:	00fe0700 	rscseq	r0, lr, r0, lsl #14
8780a0bc:	00e01f04 	rsceq	r1, r0, r4, lsl #30
8780a0c0:	0000f801 	andeq	pc, r0, r1, lsl #16
8780a0c4:	e0010038 	and	r0, r1, r8, lsr r0
8780a0c8:	07003e04 	streq	r3, [r0, -r4, lsl #28]
8780a0cc:	000600c0 	andeq	r0, r6, r0, asr #1
8780a0d0:	00000400 	andeq	r0, r0, r0, lsl #8
8780a0d4:	07000004 	streq	r0, [r0, -r4]
8780a0d8:	ff0700e0 			; <UNDEFINED> instruction: 0xff0700e0
8780a0dc:	f81f04c0 			; <UNDEFINED> instruction: 0xf81f04c0
8780a0e0:	07c00700 	strbeq	r0, [r0, r0, lsl #14]
8780a0e4:	ff0700f8 			; <UNDEFINED> instruction: 0xff0700f8
8780a0e8:	f83f0480 			; <UNDEFINED> instruction: 0xf83f0480
8780a0ec:	04c00700 	strbeq	r0, [r0], #1792	; 0x700
8780a0f0:	000700f8 	strdeq	r0, [r7], -r8
8780a0f4:	00000400 	andeq	r0, r0, r0, lsl #8
8780a0f8:	04000000 	streq	r0, [r0], #-0
8780a0fc:	00060800 	andeq	r0, r6, r0, lsl #16
8780a100:	78c00718 	stmiavc	r0, {r3, r4, r8, r9, sl}^
8780a104:	00c8f105 	sbceq	pc, r8, r5, lsl #2
8780a108:	1f00003e 	svcne	0x0000003e
8780a10c:	e8630480 	stmda	r3!, {r7, sl}^
8780a110:	06f88007 	ldrbteq	r8, [r8], r7
8780a114:	00041800 	andeq	r1, r4, r0, lsl #16
8780a118:	00000008 	andeq	r0, r0, r8
8780a11c:	06000004 	streq	r0, [r0], -r4
8780a120:	80070000 	andhi	r0, r7, r0
8780a124:	08e00700 	stmiaeq	r0!, {r8, r9, sl}^
8780a128:	00087c04 	andeq	r7, r8, r4, lsl #24
8780a12c:	0700f81f 	smuadeq	r0, pc, r8	; <UNPREDICTABLE>
8780a130:	081800f8 	ldmdaeq	r8, {r3, r4, r5, r6, r7}
8780a134:	0708e004 	streq	lr, [r8, -r4]
8780a138:	00060000 	andeq	r0, r6, r0
8780a13c:	00000400 	andeq	r0, r0, r0, lsl #8
8780a140:	01000000 	mrseq	r0, (UNDEF: 0)
8780a144:	00060800 	andeq	r0, r6, r0, lsl #16
8780a148:	f8000438 			; <UNDEFINED> instruction: 0xf8000438
8780a14c:	04e80304 	strbteq	r0, [r8], #772	; 0x304
8780a150:	7c04080f 	stcvc	8, cr0, [r4], {15}
8780a154:	08f00508 	ldmeq	r0!, {r3, r8, sl}^
8780a158:	0708c007 	streq	ip, [r8, -r7]
8780a15c:	00041800 	andeq	r1, r4, r0, lsl #16
8780a160:	00000060 	andeq	r0, r0, r0, rrx
	...
8780a170:	3f000000 	svccc	0x00000000
8780a174:	0020feff 	strdeq	pc, [r0], -pc	; <UNPREDICTABLE>
8780a178:	02002002 	andeq	r2, r0, #2
8780a17c:	20020020 	andcs	r0, r2, r0, lsr #32
8780a180:	00200200 	eoreq	r0, r0, r0, lsl #4
8780a184:	00000002 	andeq	r0, r0, r2
8780a188:	00000000 	andeq	r0, r0, r0
8780a18c:	00080000 	andeq	r0, r8, r0
8780a190:	00000700 	andeq	r0, r0, r0, lsl #14
8780a194:	0000c000 	andeq	ip, r0, r0
8780a198:	06000038 			; <UNDEFINED> instruction: 0x06000038
8780a19c:	c0010000 	andgt	r0, r1, r0
8780a1a0:	00300000 	eorseq	r0, r0, r0
8780a1a4:	00000e00 	andeq	r0, r0, r0, lsl #28
8780a1a8:	00000001 	andeq	r0, r0, r1
8780a1ac:	00000000 	andeq	r0, r0, r0
8780a1b0:	00200000 	eoreq	r0, r0, r0
8780a1b4:	02002002 	andeq	r2, r0, #2
8780a1b8:	20020020 	andcs	r0, r2, r0, lsr #32
8780a1bc:	00200200 	eoreq	r0, r0, r0, lsl #4
8780a1c0:	feff3f02 	cdp2	15, 15, cr3, cr15, cr2, {0}
	...
8780a1d8:	00000800 	andeq	r0, r0, r0, lsl #16
8780a1dc:	30000010 	andcc	r0, r0, r0, lsl r0
8780a1e0:	00200000 	eoreq	r0, r0, r0
8780a1e4:	00003000 	andeq	r3, r0, r0
8780a1e8:	08000010 	stmdaeq	r0, {r4}
	...
8780a1f4:	00010000 	andeq	r0, r1, r0
8780a1f8:	00000100 	andeq	r0, r0, r0, lsl #2
8780a1fc:	01000001 	tsteq	r0, r1
8780a200:	00010000 	andeq	r0, r1, r0
8780a204:	00000100 	andeq	r0, r0, r0, lsl #2
8780a208:	01000001 	tsteq	r0, r1
8780a20c:	00010000 	andeq	r0, r1, r0
8780a210:	00000100 	andeq	r0, r0, r0, lsl #2
8780a214:	01000001 	tsteq	r0, r1
	...
8780a220:	00002000 	andeq	r2, r0, r0
8780a224:	10000020 	andne	r0, r0, r0, lsr #32
8780a228:	00100000 	andseq	r0, r0, r0
	...
8780a240:	1900f000 	stmdbne	r0, {ip, sp, lr, pc}
8780a244:	181b00f8 	ldmdane	fp, {r3, r4, r5, r6, r7}
8780a248:	00082200 	andeq	r2, r8, r0, lsl #4
8780a24c:	24000826 	strcs	r0, [r0], #-2086	; 0x826
8780a250:	10240008 	eorne	r0, r4, r8
8780a254:	00f83f00 	rscseq	r3, r8, r0, lsl #30
8780a258:	0000f81f 	andeq	pc, r0, pc, lsl r8	; <UNPREDICTABLE>
8780a25c:	18000008 	stmdane	r0, {r3}
8780a260:	04000000 	streq	r0, [r0], #-0
8780a264:	ff070000 			; <UNDEFINED> instruction: 0xff070000
8780a268:	f0ff0ff8 			; <UNDEFINED> instruction: 0xf0ff0ff8
8780a26c:	00181800 	andseq	r1, r8, r0, lsl #16
8780a270:	20000810 	andcs	r0, r0, r0, lsl r8
8780a274:	08200008 	stmdaeq	r0!, {r3}
8780a278:	00183000 	andseq	r3, r8, r0
8780a27c:	0f00f01f 	svceq	0x0000f01f
8780a280:	000000c0 	andeq	r0, r0, r0, asr #1
8780a284:	00000000 	andeq	r0, r0, r0
8780a288:	1f00c007 	svcne	0x0000c007
8780a28c:	301800f0 	ldrshcc	r0, [r8], -r0	; <UNPREDICTABLE>
8780a290:	00082000 	andeq	r2, r8, r0
8780a294:	20000820 	andcs	r0, r0, r0, lsr #16
8780a298:	083c0008 	ldmdaeq	ip!, {r3}
8780a29c:	00101c00 	andseq	r1, r0, r0, lsl #24
8780a2a0:	00006000 	andeq	r6, r0, r0
	...
8780a2ac:	1f00c007 	svcne	0x0000c007
8780a2b0:	183800f0 	ldmdane	r8!, {r4, r5, r6, r7}
8780a2b4:	00082000 	andeq	r2, r8, r0
8780a2b8:	20000820 	andcs	r0, r0, r0, lsr #16
8780a2bc:	10100408 	andsne	r0, r0, r8, lsl #8
8780a2c0:	0ff8ff07 	svceq	0x00f8ff07
8780a2c4:	0000f0ff 	strdeq	pc, [r0], -pc	; <UNPREDICTABLE>
8780a2c8:	00000010 	andeq	r0, r0, r0, lsl r0
8780a2cc:	00000000 	andeq	r0, r0, r0
8780a2d0:	07000000 	streq	r0, [r0, -r0]
8780a2d4:	f01f00c0 			; <UNDEFINED> instruction: 0xf01f00c0
8780a2d8:	00301200 	eorseq	r1, r0, r0, lsl #4
8780a2dc:	22001822 	andcs	r1, r0, #2228224	; 0x220000
8780a2e0:	08220008 	stmdaeq	r2!, {r3}
8780a2e4:	00083200 	andeq	r3, r8, r0, lsl #4
8780a2e8:	0e00101e 	mcreq	0, 0, r1, cr0, cr14, {0}
8780a2ec:	00000020 	andeq	r0, r0, r0, lsr #32
8780a2f0:	00000000 	andeq	r0, r0, r0
8780a2f4:	20000020 	andcs	r0, r0, r0, lsr #32
8780a2f8:	08200008 	stmdaeq	r0!, {r3}
8780a2fc:	03f8ff01 	mvnseq	pc, #1, 30
8780a300:	2006f8ff 	strdcs	pc, [r6], -pc	; <UNPREDICTABLE>
8780a304:	08200408 	stmdaeq	r0!, {r3, sl}
8780a308:	07082004 	streq	r2, [r8, -r4]
8780a30c:	00030020 	andeq	r0, r3, r0, lsr #32
	...
8780a318:	0e000e00 	cdpeq	14, 0, cr0, cr0, cr0, {0}
8780a31c:	f31f006e 	vhadd.u16	q0, <illegal reg q7.5>, q15
8780a320:	00b13100 	adcseq	r3, r1, r0, lsl #2
8780a324:	2000b120 	andcs	fp, r0, r0, lsr #2
8780a328:	913100b1 	ldrhls	r0, [r1, -r1]!
8780a32c:	00131f00 	andseq	r1, r3, r0, lsl #30
8780a330:	20001e2e 	andcs	r1, r0, lr, lsr #28
8780a334:	0030000e 	eorseq	r0, r0, lr
8780a338:	04000000 	streq	r0, [r0], #-0
8780a33c:	ff070800 			; <UNDEFINED> instruction: 0xff070800
8780a340:	f8ff0ff8 			; <UNDEFINED> instruction: 0xf8ff0ff8
8780a344:	00081000 	andeq	r1, r8, r0
8780a348:	20000020 	andcs	r0, r0, r0, lsr #32
8780a34c:	08200000 	stmdaeq	r0!, {}	; <UNPREDICTABLE>
8780a350:	00f83f00 	rscseq	r3, r8, r0, lsl #30
8780a354:	0000f81f 	andeq	pc, r0, pc, lsl r8	; <UNPREDICTABLE>
8780a358:	00000008 	andeq	r0, r0, r8
8780a35c:	00000000 	andeq	r0, r0, r0
8780a360:	20000000 	andcs	r0, r0, r0
8780a364:	08200008 	stmdaeq	r0!, {r3}
8780a368:	06082000 	streq	r2, [r8], -r0
8780a36c:	3f06f83f 	svccc	0x0006f83f
8780a370:	080000f8 	stmdaeq	r0, {r3, r4, r5, r6, r7}
8780a374:	00080000 	andeq	r0, r8, r0
8780a378:	00000800 	andeq	r0, r0, r0, lsl #16
	...
8780a388:	03000003 	movweq	r0, #3
8780a38c:	00012000 	andeq	r2, r1, r0
8780a390:	20000120 	andcs	r0, r0, r0, lsr #2
8780a394:	fe3f0603 	cdp2	6, 3, cr0, cr15, cr3, {0}
8780a398:	00fc3f06 	rscseq	r3, ip, r6, lsl #30
	...
8780a3a4:	04000000 	streq	r0, [r0], #-0
8780a3a8:	ff070800 			; <UNDEFINED> instruction: 0xff070800
8780a3ac:	f8ff0ff8 			; <UNDEFINED> instruction: 0xf8ff0ff8
8780a3b0:	00880100 	addeq	r0, r8, r0, lsl #2
8780a3b4:	2f000003 	svccs	0x00000003
8780a3b8:	f83800c0 			; <UNDEFINED> instruction: 0xf83800c0
8780a3bc:	00382000 	eorseq	r2, r8, r0
8780a3c0:	00000820 	andeq	r0, r0, r0, lsr #16
8780a3c4:	00000008 	andeq	r0, r0, r8
8780a3c8:	00000000 	andeq	r0, r0, r0
8780a3cc:	00040000 	andeq	r0, r4, r0
8780a3d0:	08000408 	stmdaeq	r0, {r3, sl}
8780a3d4:	07080004 	streq	r0, [r8, -r4]
8780a3d8:	ff0ff8ff 			; <UNDEFINED> instruction: 0xff0ff8ff
8780a3dc:	080000f8 	stmdaeq	r0, {r3, r4, r5, r6, r7}
8780a3e0:	00080000 	andeq	r0, r8, r0
8780a3e4:	00000800 	andeq	r0, r0, r0, lsl #16
8780a3e8:	00000000 	andeq	r0, r0, r0
8780a3ec:	00082000 	andeq	r2, r8, r0
8780a3f0:	3f00f83f 	svccc	0x0000f83f
8780a3f4:	081000f8 	ldmdaeq	r0, {r3, r4, r5, r6, r7}
8780a3f8:	00002000 	andeq	r2, r0, r0
8780a3fc:	3f00f83f 	svccc	0x0000f83f
8780a400:	081000f8 	ldmdaeq	r0, {r3, r4, r5, r6, r7}
8780a404:	00002000 	andeq	r2, r0, r0
8780a408:	3f00f83f 	svccc	0x0000f83f
8780a40c:	080000f8 	stmdaeq	r0, {r3, r4, r5, r6, r7}
8780a410:	00000000 	andeq	r0, r0, r0
8780a414:	3f000820 	svccc	0x00000820
8780a418:	f83f00f8 			; <UNDEFINED> instruction: 0xf83f00f8
8780a41c:	00081000 	andeq	r1, r8, r0
8780a420:	20000010 	andcs	r0, r0, r0, lsl r0
8780a424:	08200000 	stmdaeq	r0!, {}	; <UNPREDICTABLE>
8780a428:	00f83f00 	rscseq	r3, r8, r0, lsl #30
8780a42c:	0000f81f 	andeq	pc, r0, pc, lsl r8	; <UNPREDICTABLE>
8780a430:	00000008 	andeq	r0, r0, r8
8780a434:	00000000 	andeq	r0, r0, r0
8780a438:	0f00c007 	svceq	0x0000c007
8780a43c:	301800f0 	ldrshcc	r0, [r8], -r0	; <UNPREDICTABLE>
8780a440:	00083000 	andeq	r3, r8, r0
8780a444:	20000820 	andcs	r0, r0, r0, lsr #16
8780a448:	08300008 	ldmdaeq	r0!, {r3}
8780a44c:	00301800 	eorseq	r1, r0, r0, lsl #16
8780a450:	0700f00f 	streq	pc, [r0, -pc]
8780a454:	000000c0 	andeq	r0, r0, r0, asr #1
8780a458:	00000000 	andeq	r0, r0, r0
8780a45c:	3f000120 	svccc	0x00000120
8780a460:	ff3f00ff 			; <UNDEFINED> instruction: 0xff3f00ff
8780a464:	00111000 	andseq	r1, r1, r0
8780a468:	20000920 	andcs	r0, r0, r0, lsr #18
8780a46c:	08200008 	stmdaeq	r0!, {r3}
8780a470:	00383000 	eorseq	r3, r8, r0
8780a474:	0f00f01f 	svceq	0x0000f01f
8780a478:	000000c0 	andeq	r0, r0, r0, asr #1
8780a47c:	00000000 	andeq	r0, r0, r0
8780a480:	1f00c007 	svcne	0x0000c007
8780a484:	183800f0 	ldmdane	r8!, {r4, r5, r6, r7}
8780a488:	00082000 	andeq	r2, r8, r0
8780a48c:	20000820 	andcs	r0, r0, r0, lsr #16
8780a490:	11100009 	tstne	r0, r9
8780a494:	00ff1f00 	rscseq	r1, pc, r0, lsl #30
8780a498:	0000ff3f 	andeq	pc, r0, pc, lsr pc	; <UNPREDICTABLE>
8780a49c:	00000001 	andeq	r0, r0, r1
8780a4a0:	00082000 	andeq	r2, r8, r0
8780a4a4:	20000820 	andcs	r0, r0, r0, lsr #16
8780a4a8:	f83f0008 			; <UNDEFINED> instruction: 0xf83f0008
8780a4ac:	00f83f00 	rscseq	r3, r8, r0, lsl #30
8780a4b0:	10000808 	andne	r0, r0, r8, lsl #16
8780a4b4:	08200008 	stmdaeq	r0!, {r3}
8780a4b8:	00002000 	andeq	r2, r0, r0
8780a4bc:	30000030 	andcc	r0, r0, r0, lsr r0
	...
8780a4c8:	0c000000 	stceq	0, cr0, [r0], {-0}
8780a4cc:	181e0078 	ldmdane	lr, {r3, r4, r5, r6}
8780a4d0:	00083300 	andeq	r3, r8, r0, lsl #6
8780a4d4:	21000823 	tstcs	r0, r3, lsr #16
8780a4d8:	88210008 	stmdahi	r1!, {r3}
8780a4dc:	00982100 	addseq	r2, r8, r0, lsl #2
8780a4e0:	3800f030 	stmdacc	r0, {r4, r5, ip, sp, lr, pc}
8780a4e4:	00000060 	andeq	r0, r0, r0, rrx
8780a4e8:	00000000 	andeq	r0, r0, r0
8780a4ec:	20000020 	andcs	r0, r0, r0, lsr #32
8780a4f0:	00200000 	eoreq	r0, r0, r0
8780a4f4:	03f0ff00 	mvnseq	pc, #0, 30
8780a4f8:	2000f8ff 	strdcs	pc, [r0], -pc	; <UNPREDICTABLE>
8780a4fc:	08200008 	stmdaeq	r0!, {r3}
8780a500:	00082000 	andeq	r2, r8, r0
8780a504:	00003000 	andeq	r3, r0, r0
	...
8780a510:	3f000020 	svccc	0x00000020
8780a514:	f87f00f0 			; <UNDEFINED> instruction: 0xf87f00f0
8780a518:	00180000 	andseq	r0, r8, r0
8780a51c:	00000800 	andeq	r0, r0, r0, lsl #16
8780a520:	10200008 	eorne	r0, r0, r8
8780a524:	00f83f00 	rscseq	r3, r8, r0, lsl #30
8780a528:	0000f07f 	andeq	pc, r0, pc, ror r0	; <UNPREDICTABLE>
8780a52c:	00000010 	andeq	r0, r0, r0, lsl r0
8780a530:	00000000 	andeq	r0, r0, r0
8780a534:	30000020 	andcc	r0, r0, r0, lsr #32
8780a538:	003c0000 	eorseq	r0, ip, r0
8780a53c:	00803f00 	addeq	r3, r0, r0, lsl #30
8780a540:	0000f023 	andeq	pc, r0, r3, lsr #32
8780a544:	70000078 	andvc	r0, r0, r8, ror r0
8780a548:	00802300 	addeq	r2, r0, r0, lsl #6
8780a54c:	3000003c 	andcc	r0, r0, ip, lsr r0
8780a550:	00200000 	eoreq	r0, r0, r0
8780a554:	00002000 	andeq	r2, r0, r0
8780a558:	3f00003c 	svccc	0x0000003c
8780a55c:	f82300e0 			; <UNDEFINED> instruction: 0xf82300e0
8780a560:	00e00000 	rsceq	r0, r0, r0
8780a564:	3e000027 	cdpcc	0, 0, cr0, cr0, cr7, {1}
8780a568:	e03f0000 	eors	r0, pc, r0
8780a56c:	00f82100 	rscseq	r2, r8, r0, lsl #2
8780a570:	3e00e001 	cdpcc	0, 0, cr14, cr0, cr1, {0}
8780a574:	00200000 	eoreq	r0, r0, r0
8780a578:	00000000 	andeq	r0, r0, r0
8780a57c:	20000820 	andcs	r0, r0, r0, lsr #16
8780a580:	38380008 	ldmdacc	r8!, {r3}
8780a584:	00683e00 	rsbeq	r3, r8, r0, lsl #28
8780a588:	03008027 	movweq	r8, #39	; 0x27
8780a58c:	f82c00c8 			; <UNDEFINED> instruction: 0xf82c00c8
8780a590:	00383800 	eorseq	r3, r8, r0, lsl #16
8780a594:	20001820 	andcs	r1, r0, r0, lsr #16
8780a598:	00000008 	andeq	r0, r0, r8
8780a59c:	00000000 	andeq	r0, r0, r0
8780a5a0:	30000020 	andcc	r0, r0, r0, lsr #32
8780a5a4:	013c0003 	teqeq	ip, r3
8780a5a8:	00833f00 	addeq	r3, r3, r0, lsl #30
8780a5ac:	0000ec23 	andeq	lr, r0, r3, lsr #24
8780a5b0:	80230070 	eorhi	r0, r3, r0, ror r0
8780a5b4:	00003c00 	andeq	r3, r0, r0, lsl #24
8780a5b8:	20000020 	andcs	r0, r0, r0, lsr #32
	...
8780a5c4:	38000000 	stmdacc	r0, {}	; <UNPREDICTABLE>
8780a5c8:	38200008 	stmdacc	r0!, {r3}
8780a5cc:	00f82000 	rscseq	r2, r8, r0
8780a5d0:	2f00e823 	svccs	0x0000e823
8780a5d4:	083e0088 	ldmdaeq	lr!, {r3, r7}
8780a5d8:	00083800 	andeq	r3, r8, r0, lsl #16
8780a5dc:	00001820 	andeq	r1, r0, r0, lsr #16
8780a5e0:	00000070 	andeq	r0, r0, r0, ror r0
	...
8780a5f4:	14000008 	strne	r0, [r0], #-8
8780a5f8:	fcf71f00 	ldc2l	15, cr1, [r7]
8780a5fc:	20060030 	andcs	r0, r6, r0, lsr r0
8780a600:	00000200 	andeq	r0, r0, r0, lsl #4
	...
8780a618:	ffff0000 			; <UNDEFINED> instruction: 0xffff0000
8780a61c:	000000ff 	strdeq	r0, [r0], -pc	; <UNPREDICTABLE>
	...
8780a630:	00200000 	eoreq	r0, r0, r0
8780a634:	06003002 	streq	r3, [r0], -r2
8780a638:	00fcf71f 	rscseq	pc, ip, pc, lsl r7	; <UNPREDICTABLE>
8780a63c:	08000014 	stmdaeq	r0, {r2, r4}
	...
8780a650:	18000000 	stmdane	r0, {}	; <UNPREDICTABLE>
8780a654:	00600000 	rsbeq	r0, r0, r0
8780a658:	00004000 	andeq	r4, r0, r0
8780a65c:	20000040 	andcs	r0, r0, r0, asr #32
8780a660:	00100000 	andseq	r0, r0, r0
8780a664:	00000800 	andeq	r0, r0, r0, lsl #16
8780a668:	04000004 	streq	r0, [r0], #-4
8780a66c:	000c0000 	andeq	r0, ip, r0
8780a670:	00001000 	andeq	r1, r0, r0

8780a674 <asc2_1608>:
	...
8780a688:	cc1f0000 	ldcgt	0, cr0, [pc], {-0}
8780a68c:	00000c00 	andeq	r0, r0, r0, lsl #24
8780a690:	00000000 	andeq	r0, r0, r0
8780a694:	00080000 	andeq	r0, r8, r0
8780a698:	00600030 	rsbeq	r0, r0, r0, lsr r0
8780a69c:	00300008 	eorseq	r0, r0, r8
8780a6a0:	00000060 	andeq	r0, r0, r0, rrx
8780a6a4:	fc032002 	stc2	0, cr2, [r3], {2}
8780a6a8:	2002201e 	andcs	r2, r2, lr, lsl r0
8780a6ac:	201efc03 	andscs	pc, lr, r3, lsl #24
8780a6b0:	00002002 	andeq	r2, r0, r2
8780a6b4:	180e0000 	stmdane	lr, {}	; <UNPREDICTABLE>
8780a6b8:	ff3f0411 			; <UNDEFINED> instruction: 0xff3f0411
8780a6bc:	780c8410 	stmdavc	ip, {r4, sl, pc}
8780a6c0:	00000000 	andeq	r0, r0, r0
8780a6c4:	8410000f 	ldrhi	r0, [r0], #-15
8780a6c8:	c000380f 	andgt	r3, r0, pc, lsl #16
8780a6cc:	84187807 	ldrhi	r7, [r8], #-2055	; 0x807
8780a6d0:	00007800 	andeq	r7, r0, r0, lsl #16
8780a6d4:	840f7800 	strhi	r7, [pc], #-2048	; 8780a6dc <asc2_1608+0x68>
8780a6d8:	2411c410 	ldrcs	ip, [r1], #-1040	; 0x410
8780a6dc:	e400980e 	str	r9, [r0], #-2062	; 0x80e
8780a6e0:	08008400 	stmdaeq	r0, {sl, pc}
8780a6e4:	00680008 	rsbeq	r0, r8, r8
8780a6e8:	00000070 	andeq	r0, r0, r0, ror r0
	...
8780a6f8:	e0070000 	and	r0, r7, r0
8780a6fc:	04201818 	strteq	r1, [r0], #-2072	; 0x818
8780a700:	00000240 	andeq	r0, r0, r0, asr #4
8780a704:	02400000 	subeq	r0, r0, #0
8780a708:	18180420 	ldmdane	r8, {r5, sl}
8780a70c:	0000e007 	andeq	lr, r0, r7
8780a710:	00000000 	andeq	r0, r0, r0
8780a714:	40024002 	andmi	r4, r2, r2
8780a718:	f00f8001 			; <UNDEFINED> instruction: 0xf00f8001
8780a71c:	40028001 	andmi	r8, r2, r1
8780a720:	00004002 	andeq	r4, r0, r2
8780a724:	80008000 	andhi	r8, r0, r0
8780a728:	f80f8000 			; <UNDEFINED> instruction: 0xf80f8000
8780a72c:	80008000 	andhi	r8, r0, r0
8780a730:	00008000 	andeq	r8, r0, r0
8780a734:	0d000100 	stfeqs	f0, [r0, #-0]
8780a738:	00000e00 	andeq	r0, r0, r0, lsl #28
	...
8780a744:	80000000 	andhi	r0, r0, r0
8780a748:	80008000 	andhi	r8, r0, r0
8780a74c:	80008000 	andhi	r8, r0, r0
8780a750:	80008000 	andhi	r8, r0, r0
8780a754:	0c000000 	stceq	0, cr0, [r0], {-0}
8780a758:	00000c00 	andeq	r0, r0, r0, lsl #24
	...
8780a764:	06000000 	streq	r0, [r0], -r0
8780a768:	60001800 	andvs	r1, r0, r0, lsl #16
8780a76c:	00068001 	andeq	r8, r6, r1
8780a770:	00200018 	eoreq	r0, r0, r8, lsl r0
8780a774:	f0070000 			; <UNDEFINED> instruction: 0xf0070000
8780a778:	04100808 	ldreq	r0, [r0], #-2056	; 0x808
8780a77c:	08080410 	stmdaeq	r8, {r4, sl}
8780a780:	0000f007 	andeq	pc, r0, r7
8780a784:	04080000 	streq	r0, [r8], #-0
8780a788:	fc1f0408 	ldc2	4, cr0, [pc], {8}
8780a78c:	04000400 	streq	r0, [r0], #-1024	; 0x400
8780a790:	00000000 	andeq	r0, r0, r0
8780a794:	0c0e0000 	stceq	0, cr0, [lr], {-0}
8780a798:	24101410 	ldrcs	r1, [r0], #-1040	; 0x410
8780a79c:	84114410 	ldrhi	r4, [r1], #-1040	; 0x410
8780a7a0:	00000c0e 	andeq	r0, r0, lr, lsl #24
8780a7a4:	180c0000 	stmdane	ip, {}	; <UNPREDICTABLE>
8780a7a8:	04110410 	ldreq	r0, [r1], #-1040	; 0x410
8780a7ac:	88120411 	ldmdahi	r2, {r0, r4, sl}
8780a7b0:	0000700c 	andeq	r7, r0, ip
8780a7b4:	e0000000 	and	r0, r0, r0
8780a7b8:	24042003 	strcs	r2, [r4], #-3
8780a7bc:	fc1f2408 	ldc2	4, cr2, [pc], {8}
8780a7c0:	00002400 	andeq	r2, r0, r0, lsl #8
8780a7c4:	981f0000 	ldmdals	pc, {}	; <UNPREDICTABLE>
8780a7c8:	04118410 	ldreq	r8, [r1], #-1040	; 0x410
8780a7cc:	88100411 	ldmdahi	r0, {r0, r4, sl}
8780a7d0:	00007010 	andeq	r7, r0, r0, lsl r0
8780a7d4:	f0070000 			; <UNDEFINED> instruction: 0xf0070000
8780a7d8:	04118808 	ldreq	r8, [r1], #-2056	; 0x808
8780a7dc:	88180411 	ldmdahi	r8, {r0, r4, sl}
8780a7e0:	00007000 	andeq	r7, r0, r0
8780a7e4:	001c0000 	andseq	r0, ip, r0
8780a7e8:	fc100010 	ldc2	0, cr0, [r0], {16}
8780a7ec:	001c0013 	andseq	r0, ip, r3, lsl r0
8780a7f0:	00000010 	andeq	r0, r0, r0, lsl r0
8780a7f4:	380e0000 	stmdacc	lr, {}	; <UNPREDICTABLE>
8780a7f8:	84104411 	ldrhi	r4, [r0], #-1041	; 0x411
8780a7fc:	44118410 	ldrmi	r8, [r1], #-1040	; 0x410
8780a800:	0000380e 	andeq	r3, r0, lr, lsl #16
8780a804:	00070000 	andeq	r0, r7, r0
8780a808:	44108c08 	ldrmi	r8, [r0], #-3080	; 0xc08
8780a80c:	88084410 	stmdahi	r8, {r4, sl, lr}
8780a810:	0000f007 	andeq	pc, r0, r7
8780a814:	00000000 	andeq	r0, r0, r0
8780a818:	0c030000 	stceq	0, cr0, [r3], {-0}
8780a81c:	00000c03 	andeq	r0, r0, r3, lsl #24
	...
8780a828:	06010100 	streq	r0, [r1], -r0, lsl #2
	...
8780a834:	80000000 	andhi	r0, r0, r0
8780a838:	20024001 	andcs	r4, r2, r1
8780a83c:	08081004 	stmdaeq	r8, {r2, ip}
8780a840:	00000410 	andeq	r0, r0, r0, lsl r4
8780a844:	20022002 	andcs	r2, r2, r2
8780a848:	20022002 	andcs	r2, r2, r2
8780a84c:	20022002 	andcs	r2, r2, r2
8780a850:	00002002 	andeq	r2, r0, r2
8780a854:	04100000 	ldreq	r0, [r0], #-0
8780a858:	10040808 	andne	r0, r4, r8, lsl #16
8780a85c:	40012002 	andmi	r2, r1, r2
8780a860:	00008000 	andeq	r8, r0, r0
8780a864:	000e0000 	andeq	r0, lr, r0
8780a868:	0c100012 	ldceq	0, cr0, [r0], {18}
8780a86c:	80106c10 	andshi	r6, r0, r0, lsl ip
8780a870:	0000000f 	andeq	r0, r0, pc
8780a874:	180ce003 	stmdane	ip, {r0, r1, sp, lr, pc}
8780a878:	2414e413 	ldrcs	lr, [r4], #-1043	; 0x413
8780a87c:	2808c417 	stmdacs	r8, {r0, r1, r2, r4, sl, lr, pc}
8780a880:	0000d007 	andeq	sp, r0, r7
8780a884:	3c000400 	cfstrscc	mvf0, [r0], {-0}
8780a888:	401cc403 	andsmi	ip, ip, r3, lsl #8
8780a88c:	e4004007 	str	r4, [r0], #-7
8780a890:	04001c00 	streq	r1, [r0], #-3072	; 0xc00
8780a894:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a898:	04110411 	ldreq	r0, [r1], #-1041	; 0x411
8780a89c:	880e0411 	stmdahi	lr, {r0, r4, sl}
8780a8a0:	00007000 	andeq	r7, r0, r0
8780a8a4:	180ce003 	stmdane	ip, {r0, r1, sp, lr, pc}
8780a8a8:	04100410 	ldreq	r0, [r0], #-1040	; 0x410
8780a8ac:	08100410 	ldmdaeq	r0, {r4, sl}
8780a8b0:	0000101c 	andeq	r1, r0, ip, lsl r0
8780a8b4:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a8b8:	04100410 	ldreq	r0, [r0], #-1040	; 0x410
8780a8bc:	08080410 	stmdaeq	r8, {r4, sl}
8780a8c0:	0000f007 	andeq	pc, r0, r7
8780a8c4:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a8c8:	04110411 	ldreq	r0, [r1], #-1041	; 0x411
8780a8cc:	0410c417 	ldreq	ip, [r0], #-1047	; 0x417
8780a8d0:	00001808 	andeq	r1, r0, r8, lsl #16
8780a8d4:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a8d8:	00110411 	andseq	r0, r1, r1, lsl r4
8780a8dc:	0010c017 	andseq	ip, r0, r7, lsl r0
8780a8e0:	00000008 	andeq	r0, r0, r8
8780a8e4:	180ce003 	stmdane	ip, {r0, r1, sp, lr, pc}
8780a8e8:	04100410 	ldreq	r0, [r0], #-1040	; 0x410
8780a8ec:	781c4410 	ldmdavc	ip, {r4, sl, lr}
8780a8f0:	00004000 	andeq	r4, r0, r0
8780a8f4:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a8f8:	80008410 	andhi	r8, r0, r0, lsl r4
8780a8fc:	84108000 	ldrhi	r8, [r0], #-0
8780a900:	0410fc1f 	ldreq	pc, [r0], #-3103	; 0xc1f
8780a904:	04100000 	ldreq	r0, [r0], #-0
8780a908:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a90c:	04100410 	ldreq	r0, [r0], #-1040	; 0x410
8780a910:	00000000 	andeq	r0, r0, r0
8780a914:	01000300 	mrseq	r0, LR_irq
8780a918:	01100110 	tsteq	r0, r0, lsl r1
8780a91c:	0010fe1f 	andseq	pc, r0, pc, lsl lr	; <UNPREDICTABLE>
8780a920:	00000010 	andeq	r0, r0, r0, lsl r0
8780a924:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a928:	80030411 	andhi	r0, r3, r1, lsl r4
8780a92c:	1c186414 	cfldrsne	mvf6, [r8], {20}
8780a930:	00000410 	andeq	r0, r0, r0, lsl r4
8780a934:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a938:	04000410 	streq	r0, [r0], #-1040	; 0x410
8780a93c:	04000400 	streq	r0, [r0], #-1024	; 0x400
8780a940:	00000c00 	andeq	r0, r0, r0, lsl #24
8780a944:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a948:	fc00001f 	stc2	0, cr0, [r0], {31}
8780a94c:	fc1f001f 	ldc2	0, cr0, [pc], {31}
8780a950:	00000410 	andeq	r0, r0, r0, lsl r4
8780a954:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a958:	0003040c 	andeq	r0, r3, ip, lsl #8
8780a95c:	1810e000 	ldmdane	r0, {sp, lr, pc}
8780a960:	0010fc1f 	andseq	pc, r0, pc, lsl ip	; <UNPREDICTABLE>
8780a964:	0808f007 	stmdaeq	r8, {r0, r1, r2, ip, sp, lr, pc}
8780a968:	04100410 	ldreq	r0, [r0], #-1040	; 0x410
8780a96c:	08080410 	stmdaeq	r8, {r4, sl}
8780a970:	0000f007 	andeq	pc, r0, r7
8780a974:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a978:	80108410 	andshi	r8, r0, r0, lsl r4
8780a97c:	80108010 	andshi	r8, r0, r0, lsl r0
8780a980:	0000000f 	andeq	r0, r0, pc
8780a984:	1808f007 	stmdane	r8, {r0, r1, r2, ip, sp, lr, pc}
8780a988:	24102410 	ldrcs	r2, [r0], #-1040	; 0x410
8780a98c:	0a081c10 	beq	87a119d4 <__bss_end__+0x1f682c>
8780a990:	0000f207 	andeq	pc, r0, r7, lsl #4
8780a994:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a998:	00110411 	andseq	r0, r1, r1, lsl r4
8780a99c:	3011c011 	andscc	ip, r1, r1, lsl r0
8780a9a0:	04000c0e 	streq	r0, [r0], #-3086	; 0xc0e
8780a9a4:	1c0e0000 	stcne	0, cr0, [lr], {-0}
8780a9a8:	84100411 	ldrhi	r0, [r0], #-1041	; 0x411
8780a9ac:	44108410 	ldrmi	r8, [r0], #-1040	; 0x410
8780a9b0:	0000381c 	andeq	r3, r0, ip, lsl r8
8780a9b4:	00100018 	andseq	r0, r0, r8, lsl r0
8780a9b8:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780a9bc:	00100410 	andseq	r0, r0, r0, lsl r4
8780a9c0:	00000018 	andeq	r0, r0, r8, lsl r0
8780a9c4:	f81f0010 			; <UNDEFINED> instruction: 0xf81f0010
8780a9c8:	04000410 	streq	r0, [r0], #-1040	; 0x410
8780a9cc:	04100400 	ldreq	r0, [r0], #-1024	; 0x400
8780a9d0:	0010f81f 	andseq	pc, r0, pc, lsl r8	; <UNPREDICTABLE>
8780a9d4:	001e0010 	andseq	r0, lr, r0, lsl r0
8780a9d8:	1c00e011 	stcne	0, cr14, [r0], {17}
8780a9dc:	80137000 	andshi	r7, r3, r0
8780a9e0:	0010001c 	andseq	r0, r0, ip, lsl r0
8780a9e4:	3c10c01f 	ldccc	0, cr12, [r0], {31}
8780a9e8:	001fe000 	andseq	lr, pc, r0
8780a9ec:	3c10e000 	ldccc	0, cr14, [r0], {-0}
8780a9f0:	0000c01f 	andeq	ip, r0, pc, lsl r0
8780a9f4:	0c180410 	cfldrseq	mvf0, [r8], {16}
8780a9f8:	c0013416 	andgt	r3, r1, r6, lsl r4
8780a9fc:	3416c001 	ldrcc	ip, [r6], #-1
8780aa00:	04100c18 	ldreq	r0, [r0], #-3096	; 0xc18
8780aa04:	001c0010 	andseq	r0, ip, r0, lsl r0
8780aa08:	fc000413 	stc2	4, cr0, [r0], {19}
8780aa0c:	001c0413 	andseq	r0, ip, r3, lsl r4
8780aa10:	00000010 	andeq	r0, r0, r0, lsl r0
8780aa14:	1c100408 	cfldrsne	mvf0, [r0], {8}
8780aa18:	84106410 	ldrhi	r6, [r0], #-1040	; 0x410
8780aa1c:	041c0413 	ldreq	r0, [ip], #-1043	; 0x413
8780aa20:	00001810 	andeq	r1, r0, r0, lsl r8
8780aa24:	00000000 	andeq	r0, r0, r0
8780aa28:	fe7f0000 	cdp2	0, 7, cr0, cr15, cr0, {0}
8780aa2c:	02400240 	subeq	r0, r0, #64, 4
8780aa30:	00000240 	andeq	r0, r0, r0, asr #4
8780aa34:	00300000 	eorseq	r0, r0, r0
8780aa38:	8003000c 	andhi	r0, r3, ip
8780aa3c:	1c006000 	stcne	0, cr6, [r0], {-0}
8780aa40:	00000300 	andeq	r0, r0, r0, lsl #6
8780aa44:	02400000 	subeq	r0, r0, #0
8780aa48:	02400240 	subeq	r0, r0, #64, 4
8780aa4c:	0000fe7f 	andeq	pc, r0, pc, ror lr	; <UNPREDICTABLE>
	...
8780aa58:	00400020 	subeq	r0, r0, r0, lsr #32
8780aa5c:	00400040 	subeq	r0, r0, r0, asr #32
8780aa60:	00000020 	andeq	r0, r0, r0, lsr #32
8780aa64:	01000100 	mrseq	r0, (UNDEF: 16)
8780aa68:	01000100 	mrseq	r0, (UNDEF: 16)
8780aa6c:	01000100 	mrseq	r0, (UNDEF: 16)
8780aa70:	01000100 	mrseq	r0, (UNDEF: 16)
8780aa74:	00400000 	subeq	r0, r0, r0
8780aa78:	00200040 	eoreq	r0, r0, r0, asr #32
	...
8780aa84:	98000000 	stmdals	r0, {}	; <UNPREDICTABLE>
8780aa88:	44012401 	strmi	r2, [r1], #-1025	; 0x401
8780aa8c:	44014401 	strmi	r4, [r1], #-1025	; 0x401
8780aa90:	0400fc00 	streq	pc, [r0], #-3072	; 0xc00
8780aa94:	fc1f0010 	ldc2	0, cr0, [pc], {16}
8780aa98:	04018800 	streq	r8, [r1], #-2048	; 0x800
8780aa9c:	88000401 	stmdahi	r0, {r0, sl}
8780aaa0:	00007000 	andeq	r7, r0, r0
8780aaa4:	70000000 	andvc	r0, r0, r0
8780aaa8:	04018800 	streq	r8, [r1], #-2048	; 0x800
8780aaac:	04010401 	streq	r0, [r1], #-1025	; 0x401
8780aab0:	00008800 	andeq	r8, r0, r0, lsl #16
8780aab4:	70000000 	andvc	r0, r0, r0
8780aab8:	04018800 	streq	r8, [r1], #-2048	; 0x800
8780aabc:	08110401 	ldmdaeq	r1, {r0, sl}
8780aac0:	0400fc1f 	streq	pc, [r0], #-3103	; 0xc1f
8780aac4:	f8000000 			; <UNDEFINED> instruction: 0xf8000000
8780aac8:	44014401 	strmi	r4, [r1], #-1025	; 0x401
8780aacc:	44014401 	strmi	r4, [r1], #-1025	; 0x401
8780aad0:	0000c800 	andeq	ip, r0, r0, lsl #16
8780aad4:	04010000 	streq	r0, [r1], #-0
8780aad8:	fc0f0401 	stc2	4, cr0, [pc], {1}
8780aadc:	04110411 	ldreq	r0, [r1], #-1041	; 0x411
8780aae0:	00180011 	andseq	r0, r8, r1, lsl r0
8780aae4:	d6000000 	strle	r0, [r0], -r0
8780aae8:	29012901 	stmdbcs	r1, {r0, r8, fp, sp}
8780aaec:	c9012901 	stmdbgt	r1, {r0, r8, fp, sp}
8780aaf0:	00000601 	andeq	r0, r0, r1, lsl #12
8780aaf4:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780aaf8:	00018400 	andeq	r8, r1, r0, lsl #8
8780aafc:	04010001 	streq	r0, [r1], #-1
8780ab00:	0400fc00 	streq	pc, [r0], #-3072	; 0xc00
8780ab04:	04010000 	streq	r0, [r1], #-0
8780ab08:	fc190419 	ldc2	4, cr0, [r9], {25}
8780ab0c:	04000400 	streq	r0, [r0], #-1024	; 0x400
8780ab10:	00000000 	andeq	r0, r0, r0
8780ab14:	03000000 	movweq	r0, #0
8780ab18:	01010100 	mrseq	r0, (UNDEF: 17)
8780ab1c:	fe190119 	mrc2	1, 0, r0, cr9, cr9, {0}
8780ab20:	00000000 	andeq	r0, r0, r0
8780ab24:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780ab28:	40002400 	andmi	r2, r0, r0, lsl #8
8780ab2c:	0c01b401 	cfstrseq	mvf11, [r1], {1}
8780ab30:	00000401 	andeq	r0, r0, r1, lsl #8
8780ab34:	04100000 	ldreq	r0, [r0], #-0
8780ab38:	fc1f0410 	ldc2	4, cr0, [pc], {16}
8780ab3c:	04000400 	streq	r0, [r0], #-1024	; 0x400
8780ab40:	00000000 	andeq	r0, r0, r0
8780ab44:	fc010401 	stc2	4, cr0, [r1], {1}
8780ab48:	00010401 	andeq	r0, r1, r1, lsl #8
8780ab4c:	0401fc01 	streq	pc, [r1], #-3073	; 0xc01
8780ab50:	fc000001 	stc2	0, cr0, [r0], {1}
8780ab54:	fc010401 	stc2	4, cr0, [r1], {1}
8780ab58:	00018400 	andeq	r8, r1, r0, lsl #8
8780ab5c:	04010001 	streq	r0, [r1], #-1
8780ab60:	0400fc00 	streq	pc, [r0], #-3072	; 0xc00
8780ab64:	f8000000 			; <UNDEFINED> instruction: 0xf8000000
8780ab68:	04010401 	streq	r0, [r1], #-1025	; 0x401
8780ab6c:	04010401 	streq	r0, [r1], #-1025	; 0x401
8780ab70:	0000f800 	andeq	pc, r0, r0, lsl #16
8780ab74:	ff010101 			; <UNDEFINED> instruction: 0xff010101
8780ab78:	04018500 	streq	r8, [r1], #-1280	; 0x500
8780ab7c:	88000401 	stmdahi	r0, {r0, sl}
8780ab80:	00007000 	andeq	r7, r0, r0
8780ab84:	70000000 	andvc	r0, r0, r0
8780ab88:	04018800 	streq	r8, [r1], #-2048	; 0x800
8780ab8c:	05010401 	streq	r0, [r1, #-1025]	; 0x401
8780ab90:	0100ff01 	tsteq	r0, r1, lsl #30
8780ab94:	04010401 	streq	r0, [r1], #-1025	; 0x401
8780ab98:	8400fc01 	strhi	pc, [r0], #-3073	; 0xc01
8780ab9c:	00010401 	andeq	r0, r1, r1, lsl #8
8780aba0:	00008001 	andeq	r8, r0, r1
8780aba4:	cc000000 	stcgt	0, cr0, [r0], {-0}
8780aba8:	24012401 	strcs	r2, [r1], #-1025	; 0x401
8780abac:	24012401 	strcs	r2, [r1], #-1025	; 0x401
8780abb0:	00009801 	andeq	r9, r0, r1, lsl #16
8780abb4:	00010000 	andeq	r0, r1, r0
8780abb8:	f8070001 			; <UNDEFINED> instruction: 0xf8070001
8780abbc:	04010401 	streq	r0, [r1], #-1025	; 0x401
8780abc0:	00000000 	andeq	r0, r0, r0
8780abc4:	f8010001 			; <UNDEFINED> instruction: 0xf8010001
8780abc8:	04000400 	streq	r0, [r0], #-1024	; 0x400
8780abcc:	08010400 	stmdaeq	r1, {sl}
8780abd0:	0400fc01 	streq	pc, [r0], #-3073	; 0xc01
8780abd4:	80010001 	andhi	r0, r1, r1
8780abd8:	0c007001 	stceq	0, cr7, [r0], {1}
8780abdc:	60011000 	andvs	r1, r1, r0
8780abe0:	00018001 	andeq	r8, r1, r1
8780abe4:	0c01f001 	stceq	0, cr15, [r1], {1}
8780abe8:	c0013000 	andgt	r3, r1, r0
8780abec:	0c013000 	stceq	0, cr3, [r1], {-0}
8780abf0:	0001f001 	andeq	pc, r1, r1
8780abf4:	04010000 	streq	r0, [r1], #-0
8780abf8:	74008c01 	strvc	r8, [r0], #-3073	; 0xc01
8780abfc:	8c017001 	stchi	0, cr7, [r1], {1}
8780ac00:	00000401 	andeq	r0, r0, r1, lsl #8
8780ac04:	81010101 	tsthi	r1, r1, lsl #2
8780ac08:	0e007101 	adfeqs	f7, f0, f1
8780ac0c:	60011800 	andvs	r1, r1, r0, lsl #16
8780ac10:	00018001 	andeq	r8, r1, r1
8780ac14:	84010000 	strhi	r0, [r1], #-0
8780ac18:	34010c01 	strcc	r0, [r1], #-3073	; 0xc01
8780ac1c:	84014401 	strhi	r4, [r1], #-1025	; 0x401
8780ac20:	00000c01 	andeq	r0, r0, r1, lsl #24
	...
8780ac2c:	fc3e0001 	ldc2	0, cr0, [lr], #-4
8780ac30:	02400240 	subeq	r0, r0, #64, 4
	...
8780ac3c:	0000ffff 	strdeq	pc, [r0], -pc	; <UNPREDICTABLE>
8780ac40:	00000000 	andeq	r0, r0, r0
8780ac44:	02400000 	subeq	r0, r0, #0
8780ac48:	fc3e0240 	ldc2	2, cr0, [lr], #-256	; 0xffffff00
8780ac4c:	00000001 	andeq	r0, r0, r1
8780ac50:	00000000 	andeq	r0, r0, r0
8780ac54:	00600000 	rsbeq	r0, r0, r0
8780ac58:	00800080 	addeq	r0, r0, r0, lsl #1
8780ac5c:	00400040 	subeq	r0, r0, r0, asr #32
8780ac60:	00200020 	eoreq	r0, r0, r0, lsr #32

8780ac64 <asc2_3216>:
	...
8780acfc:	c000f007 	andgt	pc, r0, r7
8780ad00:	e0e1ff07 	rsc	pc, r1, r7, lsl #30
8780ad04:	e001f007 	and	pc, r1, r7
8780ad08:	c0000000 	andgt	r0, r0, r0
	...
8780ad6c:	00002000 	andeq	r2, r0, r0
8780ad70:	0000c001 	andeq	ip, r0, r1
8780ad74:	00008007 	andeq	r8, r0, r7
8780ad78:	0000001f 	andeq	r0, r0, pc, lsl r0
8780ad7c:	0000001e 	andeq	r0, r0, lr, lsl r0
8780ad80:	0000201c 	andeq	r2, r0, ip, lsl r0
8780ad84:	0000c001 	andeq	ip, r0, r1
8780ad88:	00008007 	andeq	r8, r0, r7
8780ad8c:	0000001f 	andeq	r0, r0, pc, lsl r0
8780ad90:	0000001e 	andeq	r0, r0, lr, lsl r0
8780ad94:	0000001c 	andeq	r0, r0, ip, lsl r0
	...
8780ade8:	000c1800 	andeq	r1, ip, r0, lsl #16
8780adec:	000c1800 	andeq	r1, ip, r0, lsl #16
8780adf0:	e00f1800 	and	r1, pc, r0, lsl #16
8780adf4:	00fc1f00 	rscseq	r1, ip, r0, lsl #30
8780adf8:	000cf803 	andeq	pc, ip, r3, lsl #16
8780adfc:	000c1800 	andeq	r1, ip, r0, lsl #16
8780ae00:	000c1800 	andeq	r1, ip, r0, lsl #16
8780ae04:	000c1800 	andeq	r1, ip, r0, lsl #16
8780ae08:	000c1800 	andeq	r1, ip, r0, lsl #16
8780ae0c:	e00f1800 	and	r1, pc, r0, lsl #16
8780ae10:	00fc1f00 	rscseq	r1, ip, r0, lsl #30
8780ae14:	000cf803 	andeq	pc, ip, r3, lsl #16
8780ae18:	000c1800 	andeq	r1, ip, r0, lsl #16
8780ae1c:	000c1800 	andeq	r1, ip, r0, lsl #16
	...
8780ae6c:	80070000 	andhi	r0, r7, r0
8780ae70:	c0077800 	andgt	r7, r7, r0, lsl #16
8780ae74:	4006fc00 	andmi	pc, r6, r0, lsl #24
8780ae78:	20000e01 	andcs	r0, r0, r1, lsl #28
8780ae7c:	20000703 	andcs	r0, r0, r3, lsl #14
8780ae80:	20800302 	addcs	r0, r0, r2, lsl #6
8780ae84:	fcffff0f 	ldc2l	15, cr15, [pc], #60	; 8780aec8 <asc2_3216+0x264>
8780ae88:	20c00102 	sbccs	r0, r0, r2, lsl #2
8780ae8c:	60e00002 	rscvs	r0, r0, r2
8780ae90:	40703001 	rsbsmi	r3, r0, r1
8780ae94:	803ff001 	eorshi	pc, pc, r1
8780ae98:	001ff000 	andseq	pc, pc, r0
	...
8780aee4:	0000fe00 	andeq	pc, r0, r0, lsl #28
8780aee8:	0000ff01 	andeq	pc, r0, r1, lsl #30
8780aeec:	00800103 	addeq	r0, r0, r3, lsl #2
8780aef0:	60800002 	addvs	r0, r0, r2
8780aef4:	c0810103 	addgt	r0, r1, r3, lsl #2
8780aef8:	0007ff01 	andeq	pc, r7, r1, lsl #30
8780aefc:	0018fe00 	andseq	pc, r8, r0, lsl #28
8780af00:	00e00000 	rsceq	r0, r0, r0
8780af04:	00bf0300 	adcseq	r0, pc, r0, lsl #6
8780af08:	c0ff0c00 	rscsgt	r0, pc, r0, lsl #24
8780af0c:	60807100 	addvs	r7, r0, r0, lsl #2
8780af10:	2000c101 	andcs	ip, r0, r1, lsl #2
8780af14:	60800103 	addvs	r0, r0, r3, lsl #2
8780af18:	c0ff0000 	rscsgt	r0, pc, r0
8780af1c:	003f0000 	eorseq	r0, pc, r0
	...
8780af64:	001f0000 	andseq	r0, pc, r0
8780af68:	c07f0000 	rsbsgt	r0, pc, r0
8780af6c:	c0c0fc00 	sbcgt	pc, r0, r0, lsl #24
8780af70:	6080ff01 	addvs	pc, r0, r1, lsl #30
8780af74:	20e00303 	rsccs	r0, r0, r3, lsl #6
8780af78:	20780202 	rsbscs	r0, r8, r2, lsl #4
8780af7c:	201e0602 	andscs	r0, lr, r2, lsl #12
8780af80:	4007fc03 	andmi	pc, r7, r3, lsl #24
8780af84:	8003f001 	andhi	pc, r3, r1
8780af88:	c0030100 	andgt	r0, r3, r0, lsl #2
8780af8c:	601c0100 	andsvs	r0, ip, r0, lsl #2
8780af90:	20e00100 	rsccs	r0, r0, r0, lsl #2
8780af94:	20000100 	andcs	r0, r0, r0, lsl #2
8780af98:	40000100 	andmi	r0, r0, r0, lsl #2
8780af9c:	80010000 	andhi	r0, r1, r0
	...
8780afe8:	00002000 	andeq	r2, r0, r0
8780afec:	0000601c 	andeq	r6, r0, ip, lsl r0
8780aff0:	0000401c 	andeq	r4, r0, ip, lsl r0
8780aff4:	0000801f 	andeq	r8, r0, pc, lsl r0
8780aff8:	0000000f 	andeq	r0, r0, pc
	...
8780b080:	00f80700 	rscseq	r0, r8, r0, lsl #14
8780b084:	00ff3f00 	rscseq	r3, pc, r0, lsl #30
8780b088:	c0077800 	andgt	r7, r7, r0, lsl #16
8780b08c:	e000c001 	and	ip, r0, r1
8780b090:	30000003 	andcc	r0, r0, r3
8780b094:	08000004 	stmdaeq	r0, {r2}
8780b098:	04000008 	streq	r0, [r0], #-8
8780b09c:	02000010 	andeq	r0, r0, #16
	...
8780b0e8:	02000010 	andeq	r0, r0, #16
8780b0ec:	04000008 	streq	r0, [r0], #-8
8780b0f0:	08000004 	stmdaeq	r0, {r2}
8780b0f4:	30000003 	andcc	r0, r0, r3
8780b0f8:	e000c001 	and	ip, r0, r1
8780b0fc:	c0077800 	andgt	r7, r7, r0, lsl #16
8780b100:	00ff3f00 	rscseq	r3, pc, r0, lsl #30
8780b104:	00f80700 	rscseq	r0, r8, r0, lsl #14
	...
8780b16c:	00180c00 	andseq	r0, r8, r0, lsl #24
8780b170:	00380e00 	eorseq	r0, r8, r0, lsl #28
8780b174:	00380e00 	eorseq	r0, r8, r0, lsl #28
8780b178:	00300600 	eorseq	r0, r0, r0, lsl #12
8780b17c:	00600300 	rsbeq	r0, r0, r0, lsl #6
8780b180:	80436100 	subhi	r6, r3, r0, lsl #2
8780b184:	80ffff00 	rscshi	pc, pc, r0, lsl #30
8780b188:	00436100 	subeq	r6, r3, r0, lsl #2
8780b18c:	00600300 	rsbeq	r0, r0, r0, lsl #6
8780b190:	00300600 	eorseq	r0, r0, r0, lsl #12
8780b194:	00380e00 	eorseq	r0, r8, r0, lsl #28
8780b198:	00380e00 	eorseq	r0, r8, r0, lsl #28
8780b19c:	00180c00 	andseq	r0, r8, r0, lsl #24
	...
8780b1ec:	00800000 	addeq	r0, r0, r0
8780b1f0:	00800000 	addeq	r0, r0, r0
8780b1f4:	00800000 	addeq	r0, r0, r0
8780b1f8:	00800000 	addeq	r0, r0, r0
8780b1fc:	00800000 	addeq	r0, r0, r0
8780b200:	00800000 	addeq	r0, r0, r0
8780b204:	00ff7f00 	rscseq	r7, pc, r0, lsl #30
8780b208:	00800000 	addeq	r0, r0, r0
8780b20c:	00800000 	addeq	r0, r0, r0
8780b210:	00800000 	addeq	r0, r0, r0
8780b214:	00800000 	addeq	r0, r0, r0
8780b218:	00800000 	addeq	r0, r0, r0
8780b21c:	00800000 	addeq	r0, r0, r0
	...
8780b268:	01000000 	mrseq	r0, (UNDEF: 0)
8780b26c:	e3000000 	movw	r0, #0
8780b270:	e2000000 	and	r0, r0, #0
8780b274:	fc000000 	stc2	0, cr0, [r0], {-0}
8780b278:	78000000 	stmdavc	r0, {}	; <UNPREDICTABLE>
	...
8780b2e8:	00800000 	addeq	r0, r0, r0
8780b2ec:	00800000 	addeq	r0, r0, r0
8780b2f0:	00800000 	addeq	r0, r0, r0
8780b2f4:	00800000 	addeq	r0, r0, r0
8780b2f8:	00800000 	addeq	r0, r0, r0
8780b2fc:	00800000 	addeq	r0, r0, r0
8780b300:	00800000 	addeq	r0, r0, r0
8780b304:	00800000 	addeq	r0, r0, r0
8780b308:	00800000 	addeq	r0, r0, r0
8780b30c:	00800000 	addeq	r0, r0, r0
8780b310:	00800000 	addeq	r0, r0, r0
8780b314:	00800000 	addeq	r0, r0, r0
8780b318:	00800000 	addeq	r0, r0, r0
8780b31c:	00800000 	addeq	r0, r0, r0
	...
8780b36c:	c0000000 	andgt	r0, r0, r0
8780b370:	e0010000 	and	r0, r1, r0
8780b374:	e0010000 	and	r0, r1, r0
8780b378:	c0000000 	andgt	r0, r0, r0
	...
8780b3e8:	0c000000 	stceq	0, cr0, [r0], {-0}
8780b3ec:	38000000 	stmdacc	r0, {}	; <UNPREDICTABLE>
8780b3f0:	e0000000 	and	r0, r0, r0
8780b3f4:	80030000 	andhi	r0, r3, r0
8780b3f8:	000e0000 	andeq	r0, lr, r0
8780b3fc:	00380000 	eorseq	r0, r8, r0
8780b400:	00e00000 	rsceq	r0, r0, r0
8780b404:	00800300 	addeq	r0, r0, r0, lsl #6
8780b408:	00000e00 	andeq	r0, r0, r0, lsl #28
8780b40c:	00003800 	andeq	r3, r0, r0, lsl #16
8780b410:	0000e000 	andeq	lr, r0, r0
8780b414:	00008003 	andeq	r8, r0, r3
8780b418:	0000000e 	andeq	r0, r0, lr
8780b41c:	00000018 	andeq	r0, r0, r8, lsl r0
	...
8780b46c:	00f80f00 	rscseq	r0, r8, r0, lsl #30
8780b470:	00ff7f00 	rscseq	r7, pc, r0, lsl #30
8780b474:	8007f000 	andhi	pc, r7, r0
8780b478:	c0008001 	andgt	r8, r0, r1
8780b47c:	60000003 	andvs	r0, r0, r3
8780b480:	20000002 	andcs	r0, r0, r2
8780b484:	20000002 	andcs	r0, r0, r2
8780b488:	20000002 	andcs	r0, r0, r2
8780b48c:	60000003 	andvs	r0, r0, r3
8780b490:	c0008001 	andgt	r8, r0, r1
8780b494:	8003e000 	andhi	lr, r3, r0
8780b498:	00ff7f00 	rscseq	r7, pc, r0, lsl #30
8780b49c:	00f80f00 	rscseq	r0, r8, r0, lsl #30
	...
8780b4f0:	20008000 	andcs	r8, r0, r0
8780b4f4:	20008000 	andcs	r8, r0, r0
8780b4f8:	20008000 	andcs	r8, r0, r0
8780b4fc:	60008000 	andvs	r8, r0, r0
8780b500:	e0ffff01 	rscs	pc, pc, r1, lsl #30
8780b504:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780b508:	60000000 	andvs	r0, r0, r0
8780b50c:	20000000 	andcs	r0, r0, r0
8780b510:	20000000 	andcs	r0, r0, r0
8780b514:	20000000 	andcs	r0, r0, r0
	...
8780b56c:	e0007800 	and	r7, r0, r0, lsl #16
8780b570:	60019800 	andvs	r9, r1, r0, lsl #16
8780b574:	60020001 	andvs	r0, r2, r1
8780b578:	60040002 	andvs	r0, r4, r2
8780b57c:	60080002 	andvs	r0, r8, r2
8780b580:	60100002 	andsvs	r0, r0, r2
8780b584:	60200002 	eorvs	r0, r0, r2
8780b588:	60400002 	subvs	r0, r0, r2
8780b58c:	60800003 	addvs	r0, r0, r3
8780b590:	60008301 	andvs	r8, r0, r1, lsl #6
8780b594:	e000fe01 	and	pc, r0, r1, lsl #28
8780b598:	80077c00 	andhi	r7, r7, r0, lsl #24
	...
8780b5ec:	8007f000 	andhi	pc, r7, r0
8780b5f0:	c007f001 	andgt	pc, r7, r1
8780b5f4:	40000001 	andmi	r0, r0, r1
8780b5f8:	20000002 	andcs	r0, r0, r2
8780b5fc:	20000102 	andcs	r0, r0, r2, lsl #2
8780b600:	20000102 	andcs	r0, r0, r2, lsl #2
8780b604:	20000102 	andcs	r0, r0, r2, lsl #2
8780b608:	20800303 	addcs	r0, r0, r3, lsl #6
8780b60c:	40808601 	addmi	r8, r0, r1, lsl #12
8780b610:	c0c0fc01 	sbcgt	pc, r0, r1, lsl #24
8780b614:	807f7800 	rsbshi	r7, pc, r0, lsl #16
8780b618:	001e0000 	andseq	r0, lr, r0
	...
8780b668:	00080000 	andeq	r0, r8, r0
8780b66c:	00380000 	eorseq	r0, r8, r0
8780b670:	00480000 	subeq	r0, r8, r0
8780b674:	00880100 	addeq	r0, r8, r0, lsl #2
8780b678:	00080600 	andeq	r0, r8, r0, lsl #12
8780b67c:	10080c00 	andne	r0, r8, r0, lsl #24
8780b680:	10083000 	andne	r3, r8, r0
8780b684:	10084000 	andne	r4, r8, r0
8780b688:	f0ffff01 			; <UNDEFINED> instruction: 0xf0ffff01
8780b68c:	f0ffff03 			; <UNDEFINED> instruction: 0xf0ffff03
8780b690:	f0ffff03 			; <UNDEFINED> instruction: 0xf0ffff03
8780b694:	10080000 	andne	r0, r8, r0
8780b698:	10080000 	andne	r0, r8, r0
8780b69c:	10080000 	andne	r0, r8, r0
	...
8780b6ec:	80070000 	andhi	r0, r7, r0
8780b6f0:	40861f00 	addmi	r1, r6, r0, lsl #30
8780b6f4:	2000e103 	andcs	lr, r0, r3, lsl #2
8780b6f8:	20000203 	andcs	r0, r0, r3, lsl #4
8780b6fc:	20000403 	andcs	r0, r0, r3, lsl #8
8780b700:	20000403 	andcs	r0, r0, r3, lsl #8
8780b704:	20000403 	andcs	r0, r0, r3, lsl #8
8780b708:	20000403 	andcs	r0, r0, r3, lsl #8
8780b70c:	40000603 	andmi	r0, r0, r3, lsl #12
8780b710:	c0010303 	andgt	r0, r1, r3, lsl #6
8780b714:	80ff0103 	rscshi	r0, pc, r3, lsl #2
8780b718:	007e0003 	rsbseq	r0, lr, r3
	...
8780b76c:	00fc0700 	rscseq	r0, ip, r0, lsl #14
8780b770:	00ff3f00 	rscseq	r3, pc, r0, lsl #30
8780b774:	80c37000 	sbchi	r7, r3, r0
8780b778:	c0808100 	addgt	r8, r0, r0, lsl #2
8780b77c:	60000101 	andvs	r0, r0, r1, lsl #2
8780b780:	20000203 	andcs	r0, r0, r3, lsl #4
8780b784:	20000202 	andcs	r0, r0, r2, lsl #4
8780b788:	20000202 	andcs	r0, r0, r2, lsl #4
8780b78c:	20000202 	andcs	r0, r0, r2, lsl #4
8780b790:	40000302 	andmi	r0, r0, r2, lsl #6
8780b794:	c080c101 	addgt	ip, r0, r1, lsl #2
8780b798:	80ffc000 	rscshi	ip, pc, r0
8780b79c:	007e0000 	rsbseq	r0, lr, r0
	...
8780b7ec:	0000f000 	andeq	pc, r0, r0
8780b7f0:	0000c003 	andeq	ip, r0, r3
8780b7f4:	00008003 	andeq	r8, r0, r3
8780b7f8:	00000003 	andeq	r0, r0, r3
8780b7fc:	e0070003 	and	r0, r7, r3
8780b800:	e03f0003 	eors	r0, pc, r3
8780b804:	00c00103 	sbceq	r0, r0, r3, lsl #2
8780b808:	00000603 	andeq	r0, r0, r3, lsl #12
8780b80c:	00001803 	andeq	r1, r0, r3, lsl #16
8780b810:	00006003 	andeq	r6, r0, r3
8780b814:	00008003 	andeq	r8, r0, r3
8780b818:	00000003 	andeq	r0, r0, r3
	...
8780b868:	001f0000 	andseq	r0, pc, r0
8780b86c:	803f7800 	eorshi	r7, pc, r0, lsl #16
8780b870:	c060fc00 	rsbgt	pc, r0, r0, lsl #24
8780b874:	40c08e01 	sbcmi	r8, r0, r1, lsl #28
8780b878:	20800703 	addcs	r0, r0, r3, lsl #14
8780b87c:	20000302 	andcs	r0, r0, r2, lsl #6
8780b880:	20800102 	addcs	r0, r0, r2, lsl #2
8780b884:	20800102 	addcs	r0, r0, r2, lsl #2
8780b888:	20c00102 	sbccs	r0, r0, r2, lsl #2
8780b88c:	40e00103 	rscmi	r0, r0, r3, lsl #2
8780b890:	c0708601 	rsbsgt	r8, r0, r1, lsl #12
8780b894:	803ffc00 	eorshi	pc, pc, r0, lsl #24
8780b898:	001f7800 	andseq	r7, pc, r0, lsl #16
	...
8780b8e8:	00003f00 	andeq	r3, r0, r0, lsl #30
8780b8ec:	c081ff00 	addgt	pc, r1, r0, lsl #30
8780b8f0:	c0c1c001 	sbcgt	ip, r1, r1
8780b8f4:	20600001 	rsbcs	r0, r0, r1
8780b8f8:	20200002 	eorcs	r0, r0, r2
8780b8fc:	20200002 	eorcs	r0, r0, r2
8780b900:	20200002 	eorcs	r0, r0, r2
8780b904:	60200002 	eorvs	r0, r0, r2
8780b908:	c0400002 	subgt	r0, r0, r2
8780b90c:	80c10001 	sbchi	r0, r1, r1
8780b910:	008fc100 	addeq	ip, pc, r0, lsl #2
8780b914:	00fe7f00 	rscseq	r7, lr, r0, lsl #30
8780b918:	00f01f00 	rscseq	r1, r0, r0, lsl #30
	...
8780b97c:	c0000300 	andgt	r0, r0, r0, lsl #6
8780b980:	e0810700 	add	r0, r1, r0, lsl #14
8780b984:	e0810700 	add	r0, r1, r0, lsl #14
8780b988:	c0000300 	andgt	r0, r0, r0, lsl #6
	...
8780b9fc:	66000600 	strvs	r0, [r0], -r0, lsl #12
8780ba00:	78000600 	stmdavc	r0, {r9, sl}
	...
8780ba6c:	00800000 	addeq	r0, r0, r0
8780ba70:	00c00100 	sbceq	r0, r0, r0, lsl #2
8780ba74:	00600300 	rsbeq	r0, r0, r0, lsl #6
8780ba78:	00300600 	eorseq	r0, r0, r0, lsl #12
8780ba7c:	00180c00 	andseq	r0, r8, r0, lsl #24
8780ba80:	000c1800 	andeq	r1, ip, r0, lsl #16
8780ba84:	00063000 	andeq	r3, r6, r0
8780ba88:	00036000 	andeq	r6, r3, r0
8780ba8c:	8001c000 	andhi	ip, r1, r0
8780ba90:	40000001 	andmi	r0, r0, r1
8780ba94:	20000002 	andcs	r0, r0, r2
8780ba98:	10000004 	andne	r0, r0, r4
	...
8780bae8:	00100400 	andseq	r0, r0, r0, lsl #8
8780baec:	00100400 	andseq	r0, r0, r0, lsl #8
8780baf0:	00100400 	andseq	r0, r0, r0, lsl #8
8780baf4:	00100400 	andseq	r0, r0, r0, lsl #8
8780baf8:	00100400 	andseq	r0, r0, r0, lsl #8
8780bafc:	00100400 	andseq	r0, r0, r0, lsl #8
8780bb00:	00100400 	andseq	r0, r0, r0, lsl #8
8780bb04:	00100400 	andseq	r0, r0, r0, lsl #8
8780bb08:	00100400 	andseq	r0, r0, r0, lsl #8
8780bb0c:	00100400 	andseq	r0, r0, r0, lsl #8
8780bb10:	00100400 	andseq	r0, r0, r0, lsl #8
8780bb14:	00100400 	andseq	r0, r0, r0, lsl #8
8780bb18:	00100400 	andseq	r0, r0, r0, lsl #8
8780bb1c:	00100400 	andseq	r0, r0, r0, lsl #8
	...
8780bb6c:	10000004 	andne	r0, r0, r4
8780bb70:	20000002 	andcs	r0, r0, r2
8780bb74:	40000001 	andmi	r0, r0, r1
8780bb78:	8001c000 	andhi	ip, r1, r0
8780bb7c:	00036000 	andeq	r6, r3, r0
8780bb80:	00063000 	andeq	r3, r6, r0
8780bb84:	000c1800 	andeq	r1, ip, r0, lsl #16
8780bb88:	00180c00 	andseq	r0, r8, r0, lsl #24
8780bb8c:	00300600 	eorseq	r0, r0, r0, lsl #12
8780bb90:	00600300 	rsbeq	r0, r0, r0, lsl #6
8780bb94:	00c00100 	sbceq	r0, r0, r0, lsl #2
8780bb98:	00800000 	addeq	r0, r0, r0
	...
8780bbec:	00007800 	andeq	r7, r0, r0, lsl #16
8780bbf0:	0000f801 	andeq	pc, r0, r1, lsl #16
8780bbf4:	00003802 	andeq	r3, r0, r2, lsl #16
8780bbf8:	00000002 	andeq	r0, r0, r2
8780bbfc:	c0000004 	andgt	r0, r0, r4
8780bc00:	e0790004 	rsbs	r0, r9, r4
8780bc04:	e0810004 	add	r0, r1, r4
8780bc08:	c0000104 	andgt	r0, r0, r4, lsl #2
8780bc0c:	00000304 	andeq	r0, r0, r4, lsl #6
8780bc10:	00000202 	andeq	r0, r0, r2, lsl #4
8780bc14:	00000603 	andeq	r0, r0, r3, lsl #12
8780bc18:	0000fc01 	andeq	pc, r0, r1, lsl #24
8780bc1c:	0000f800 	andeq	pc, r0, r0, lsl #16
	...
8780bc68:	00f80f00 	rscseq	r0, r8, r0, lsl #30
8780bc6c:	00fe3f00 	rscseq	r3, lr, r0, lsl #30
8780bc70:	80077000 	andhi	r7, r7, r0
8780bc74:	c000c000 	andgt	ip, r0, r0
8780bc78:	40f80101 	rscsmi	r0, r8, r1, lsl #2
8780bc7c:	20fc0703 	rscscs	r0, ip, r3, lsl #14
8780bc80:	20041e02 	andcs	r1, r4, r2, lsl #28
8780bc84:	20083002 	andcs	r3, r8, r2
8780bc88:	20302002 	eorscs	r2, r0, r2
8780bc8c:	20fc3f02 	rscscs	r3, ip, r2, lsl #30
8780bc90:	40043f01 	andmi	r3, r4, r1, lsl #30
8780bc94:	c00c8001 	andgt	r8, ip, r1
8780bc98:	8031e000 	eorshi	lr, r1, r0
8780bc9c:	00c21f00 	sbceq	r1, r2, r0, lsl #30
	...
8780bce4:	20000000 	andcs	r0, r0, r0
8780bce8:	60000000 	andvs	r0, r0, r0
8780bcec:	e0030000 	and	r0, r3, r0
8780bcf0:	203e0000 	eorscs	r0, lr, r0
8780bcf4:	20e00300 	rsccs	r0, r0, r0, lsl #6
8780bcf8:	00203e00 	eoreq	r3, r0, r0, lsl #28
8780bcfc:	0020e003 	eoreq	lr, r0, r3
8780bd00:	00208003 	eoreq	r8, r0, r3
8780bd04:	0020fc07 	eoreq	pc, r0, r7, lsl #24
8780bd08:	00e03f00 	rsceq	r3, r0, r0, lsl #30
8780bd0c:	20fe0300 	rscscs	r0, lr, r0, lsl #6
8780bd10:	e03f0000 	eors	r0, pc, r0
8780bd14:	e0010000 	and	r0, r1, r0
8780bd18:	20000000 	andcs	r0, r0, r0
8780bd1c:	20000000 	andcs	r0, r0, r0
	...
8780bd68:	20000002 	andcs	r0, r0, r2
8780bd6c:	20000002 	andcs	r0, r0, r2
8780bd70:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780bd74:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780bd78:	20000102 	andcs	r0, r0, r2, lsl #2
8780bd7c:	20000102 	andcs	r0, r0, r2, lsl #2
8780bd80:	20000102 	andcs	r0, r0, r2, lsl #2
8780bd84:	20000102 	andcs	r0, r0, r2, lsl #2
8780bd88:	20000102 	andcs	r0, r0, r2, lsl #2
8780bd8c:	20000303 	andcs	r0, r0, r3, lsl #6
8780bd90:	60808601 	addvs	r8, r0, r1, lsl #12
8780bd94:	c0c0fc01 	sbcgt	pc, r0, r1, lsl #24
8780bd98:	807ff800 	rsbshi	pc, pc, r0, lsl #16
8780bd9c:	001f0000 	andseq	r0, pc, r0
	...
8780bde8:	00f80700 	rscseq	r0, r8, r0, lsl #14
8780bdec:	00ff3f00 	rscseq	r3, pc, r0, lsl #30
8780bdf0:	80077000 	andhi	r7, r7, r0
8780bdf4:	c000c000 	andgt	ip, r0, r0
8780bdf8:	40000001 	andmi	r0, r0, r1
8780bdfc:	20000003 	andcs	r0, r0, r3
8780be00:	20000002 	andcs	r0, r0, r2
8780be04:	20000002 	andcs	r0, r0, r2
8780be08:	20000002 	andcs	r0, r0, r2
8780be0c:	20000002 	andcs	r0, r0, r2
8780be10:	40000001 	andmi	r0, r0, r1
8780be14:	c0008001 	andgt	r8, r0, r1
8780be18:	8001c003 	andhi	ip, r1, r3
8780be1c:	00063000 	andeq	r3, r6, r0
	...
8780be68:	20000002 	andcs	r0, r0, r2
8780be6c:	20000002 	andcs	r0, r0, r2
8780be70:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780be74:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780be78:	20000002 	andcs	r0, r0, r2
8780be7c:	20000002 	andcs	r0, r0, r2
8780be80:	20000002 	andcs	r0, r0, r2
8780be84:	20000002 	andcs	r0, r0, r2
8780be88:	60000003 	andvs	r0, r0, r3
8780be8c:	40000001 	andmi	r0, r0, r1
8780be90:	c0008001 	andgt	r8, r0, r1
8780be94:	8007f000 	andhi	pc, r7, r0
8780be98:	00fe7f00 	rscseq	r7, lr, r0, lsl #30
8780be9c:	00f80f00 	rscseq	r0, r8, r0, lsl #30
	...
8780bee8:	20000002 	andcs	r0, r0, r2
8780beec:	20000002 	andcs	r0, r0, r2
8780bef0:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780bef4:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780bef8:	20000102 	andcs	r0, r0, r2, lsl #2
8780befc:	20000102 	andcs	r0, r0, r2, lsl #2
8780bf00:	20000102 	andcs	r0, r0, r2, lsl #2
8780bf04:	20000102 	andcs	r0, r0, r2, lsl #2
8780bf08:	20000102 	andcs	r0, r0, r2, lsl #2
8780bf0c:	20800302 	addcs	r0, r0, r2, lsl #6
8780bf10:	20e00f02 	rsccs	r0, r0, r2, lsl #30
8780bf14:	60000003 	andvs	r0, r0, r3
8780bf18:	e000c003 	and	ip, r0, r3
8780bf1c:	00036000 	andeq	r6, r3, r0
	...
8780bf68:	20000002 	andcs	r0, r0, r2
8780bf6c:	20000002 	andcs	r0, r0, r2
8780bf70:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780bf74:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780bf78:	20000102 	andcs	r0, r0, r2, lsl #2
8780bf7c:	20000102 	andcs	r0, r0, r2, lsl #2
8780bf80:	00000102 	andeq	r0, r0, r2, lsl #2
8780bf84:	00000102 	andeq	r0, r0, r2, lsl #2
8780bf88:	00000102 	andeq	r0, r0, r2, lsl #2
8780bf8c:	00000102 	andeq	r0, r0, r2, lsl #2
8780bf90:	00800302 	addeq	r0, r0, r2, lsl #6
8780bf94:	00e00f03 	rsceq	r0, r0, r3, lsl #30
8780bf98:	00000003 	andeq	r0, r0, r3
8780bf9c:	0000c003 	andeq	ip, r0, r3
8780bfa0:	00006000 	andeq	r6, r0, r0
	...
8780bfe8:	00f80700 	rscseq	r0, r8, r0, lsl #14
8780bfec:	00fe3f00 	rscseq	r3, lr, r0, lsl #30
8780bff0:	80077000 	andhi	r7, r7, r0
8780bff4:	c001c001 	andgt	ip, r1, r1
8780bff8:	40000001 	andmi	r0, r0, r1
8780bffc:	20000003 	andcs	r0, r0, r3
8780c000:	20000002 	andcs	r0, r0, r2
8780c004:	20000002 	andcs	r0, r0, r2
8780c008:	20000002 	andcs	r0, r0, r2
8780c00c:	20200001 	eorcs	r0, r0, r1
8780c010:	40200001 	eormi	r0, r0, r1
8780c014:	803fc003 	eorshi	ip, pc, r3
8780c018:	803f3000 	eorshi	r3, pc, r0
8780c01c:	00200000 	eoreq	r0, r0, r0
8780c020:	00200000 	eoreq	r0, r0, r0
	...
8780c064:	20000002 	andcs	r0, r0, r2
8780c068:	20000002 	andcs	r0, r0, r2
8780c06c:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c070:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c074:	20800002 	addcs	r0, r0, r2
8780c078:	20800002 	addcs	r0, r0, r2
8780c07c:	00800000 	addeq	r0, r0, r0
8780c080:	00800000 	addeq	r0, r0, r0
8780c084:	00800000 	addeq	r0, r0, r0
8780c088:	20800002 	addcs	r0, r0, r2
8780c08c:	20800002 	addcs	r0, r0, r2
8780c090:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c094:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c098:	20000002 	andcs	r0, r0, r2
8780c09c:	20000002 	andcs	r0, r0, r2
	...
8780c0f0:	20000002 	andcs	r0, r0, r2
8780c0f4:	20000002 	andcs	r0, r0, r2
8780c0f8:	20000002 	andcs	r0, r0, r2
8780c0fc:	20000002 	andcs	r0, r0, r2
8780c100:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c104:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c108:	20000002 	andcs	r0, r0, r2
8780c10c:	20000002 	andcs	r0, r0, r2
8780c110:	20000002 	andcs	r0, r0, r2
8780c114:	20000002 	andcs	r0, r0, r2
	...
8780c168:	0e000000 	cdpeq	0, 0, cr0, cr0, cr0, {0}
8780c16c:	0f000000 	svceq	0x00000000
8780c170:	0f000000 	svceq	0x00000000
8780c174:	01000000 	mrseq	r0, (UNDEF: 0)
8780c178:	01000002 	tsteq	r0, r2
8780c17c:	01000002 	tsteq	r0, r2
8780c180:	03000002 	movweq	r0, #2
8780c184:	06000002 	streq	r0, [r0], -r2
8780c188:	fcffff03 	ldc2l	15, cr15, [pc], #12	; 8780c19c <asc2_3216+0x1538>
8780c18c:	f8ffff03 			; <UNDEFINED> instruction: 0xf8ffff03
8780c190:	00000002 	andeq	r0, r0, r2
8780c194:	00000002 	andeq	r0, r0, r2
8780c198:	00000002 	andeq	r0, r0, r2
8780c19c:	00000002 	andeq	r0, r0, r2
	...
8780c1e8:	20000002 	andcs	r0, r0, r2
8780c1ec:	20000002 	andcs	r0, r0, r2
8780c1f0:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c1f4:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c1f8:	20c00002 	sbccs	r0, r0, r2
8780c1fc:	20000102 	andcs	r0, r0, r2, lsl #2
8780c200:	00800700 	addeq	r0, r0, r0, lsl #14
8780c204:	00e00f00 	rsceq	r0, r0, r0, lsl #30
8780c208:	00f83000 	rscseq	r3, r8, r0
8780c20c:	203e6002 	eorscs	r6, lr, r2
8780c210:	200f8003 	andcs	r8, pc, r3
8780c214:	e0030003 	and	r0, r3, r3
8780c218:	e0000002 	and	r0, r0, r2
8780c21c:	20000002 	andcs	r0, r0, r2
8780c220:	20000000 	andcs	r0, r0, r0
	...
8780c268:	20000002 	andcs	r0, r0, r2
8780c26c:	20000002 	andcs	r0, r0, r2
8780c270:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c274:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c278:	20000002 	andcs	r0, r0, r2
8780c27c:	20000002 	andcs	r0, r0, r2
8780c280:	20000000 	andcs	r0, r0, r0
8780c284:	20000000 	andcs	r0, r0, r0
8780c288:	20000000 	andcs	r0, r0, r0
8780c28c:	20000000 	andcs	r0, r0, r0
8780c290:	20000000 	andcs	r0, r0, r0
8780c294:	60000000 	andvs	r0, r0, r0
8780c298:	e0000000 	and	r0, r0, r0
8780c29c:	00030000 	andeq	r0, r3, r0
	...
8780c2e4:	20000002 	andcs	r0, r0, r2
8780c2e8:	20000002 	andcs	r0, r0, r2
8780c2ec:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c2f0:	2000e003 	andcs	lr, r0, r3
8780c2f4:	2000ff03 	andcs	pc, r0, r3, lsl #30
8780c2f8:	00f01f00 	rscseq	r1, r0, r0, lsl #30
8780c2fc:	80ff0100 	rscshi	r0, pc, r0, lsl #2
8780c300:	e00f0000 	and	r0, pc, r0
8780c304:	001e0000 	andseq	r0, lr, r0
8780c308:	00e00300 	rsceq	r0, r0, r0, lsl #6
8780c30c:	20003e00 	andcs	r3, r0, r0, lsl #28
8780c310:	2000e003 	andcs	lr, r0, r3
8780c314:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c318:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c31c:	20000002 	andcs	r0, r0, r2
8780c320:	20000002 	andcs	r0, r0, r2
	...
8780c364:	20000002 	andcs	r0, r0, r2
8780c368:	20000002 	andcs	r0, r0, r2
8780c36c:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c370:	20008003 	andcs	r8, r0, r3
8780c374:	2000f003 	andcs	pc, r0, r3
8780c378:	0000fc00 	andeq	pc, r0, r0, lsl #24
8780c37c:	00001f00 	andeq	r1, r0, r0, lsl #30
8780c380:	00c00700 	sbceq	r0, r0, r0, lsl #14
8780c384:	00f00100 	rscseq	r0, r0, r0, lsl #2
8780c388:	007c0000 	rsbseq	r0, ip, r0
8780c38c:	801f0002 	andshi	r0, pc, r2
8780c390:	e0070002 	and	r0, r7, r2
8780c394:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c398:	00000002 	andeq	r0, r0, r2
8780c39c:	00000002 	andeq	r0, r0, r2
	...
8780c3e8:	00f80f00 	rscseq	r0, r8, r0, lsl #30
8780c3ec:	00fe3f00 	rscseq	r3, lr, r0, lsl #30
8780c3f0:	8007f000 	andhi	pc, r7, r0
8780c3f4:	c0008001 	andgt	r8, r0, r1
8780c3f8:	40000001 	andmi	r0, r0, r1
8780c3fc:	20000002 	andcs	r0, r0, r2
8780c400:	20000002 	andcs	r0, r0, r2
8780c404:	20000002 	andcs	r0, r0, r2
8780c408:	20000002 	andcs	r0, r0, r2
8780c40c:	40000001 	andmi	r0, r0, r1
8780c410:	c0008001 	andgt	r8, r0, r1
8780c414:	8003f000 	andhi	pc, r3, r0
8780c418:	00fe3f00 	rscseq	r3, lr, r0, lsl #30
8780c41c:	00f80f00 	rscseq	r0, r8, r0, lsl #30
	...
8780c468:	20000002 	andcs	r0, r0, r2
8780c46c:	20000002 	andcs	r0, r0, r2
8780c470:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c474:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c478:	20800002 	addcs	r0, r0, r2
8780c47c:	20800002 	addcs	r0, r0, r2
8780c480:	00800002 	addeq	r0, r0, r2
8780c484:	00800002 	addeq	r0, r0, r2
8780c488:	00800002 	addeq	r0, r0, r2
8780c48c:	00800002 	addeq	r0, r0, r2
8780c490:	00800103 	addeq	r0, r0, r3, lsl #2
8780c494:	00008301 	andeq	r8, r0, r1, lsl #6
8780c498:	0000fe00 	andeq	pc, r0, r0, lsl #28
8780c49c:	00007c00 	andeq	r7, r0, r0, lsl #24
	...
8780c4e8:	00f80f00 	rscseq	r0, r8, r0, lsl #30
8780c4ec:	00ff7f00 	rscseq	r7, pc, r0, lsl #30
8780c4f0:	8003f000 	andhi	pc, r3, r0
8780c4f4:	c0018001 	andgt	r8, r1, r1
8780c4f8:	40060001 	andmi	r0, r6, r1
8780c4fc:	20040002 	andcs	r0, r4, r2
8780c500:	20040002 	andcs	r0, r4, r2
8780c504:	20060002 	andcs	r0, r6, r2
8780c508:	e0030002 	and	r0, r3, r2
8780c50c:	f8000001 			; <UNDEFINED> instruction: 0xf8000001
8780c510:	5c008001 	stcpl	0, cr8, [r0], {1}
8780c514:	8c03e000 	stchi	0, cr14, [r3], {-0}
8780c518:	0cff3f00 	ldcleq	15, cr3, [pc]	; 8780c520 <asc2_3216+0x18bc>
8780c51c:	18fc0f00 	ldmne	ip!, {r8, r9, sl, fp}^
	...
8780c568:	20000002 	andcs	r0, r0, r2
8780c56c:	20000002 	andcs	r0, r0, r2
8780c570:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c574:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c578:	20000102 	andcs	r0, r0, r2, lsl #2
8780c57c:	20000102 	andcs	r0, r0, r2, lsl #2
8780c580:	00800102 	addeq	r0, r0, r2, lsl #2
8780c584:	00e00102 	rsceq	r0, r0, r2, lsl #2
8780c588:	00fc0102 	rscseq	r0, ip, r2, lsl #2
8780c58c:	803f0303 	eorshi	r0, pc, r3, lsl #6
8780c590:	e0078601 	and	r8, r7, r1, lsl #12
8780c594:	e000fc01 	and	pc, r0, r1, lsl #24
8780c598:	2000f800 	andcs	pc, r0, r0, lsl #16
8780c59c:	20000000 	andcs	r0, r0, r0
	...
8780c5ec:	00067800 	andeq	r7, r6, r0, lsl #16
8780c5f0:	e001fe00 	and	pc, r1, r0, lsl #28
8780c5f4:	c0008601 	andgt	r8, r0, r1, lsl #12
8780c5f8:	40000303 	andmi	r0, r0, r3, lsl #6
8780c5fc:	20000302 	andcs	r0, r0, r2, lsl #6
8780c600:	20800102 	addcs	r0, r0, r2, lsl #2
8780c604:	20800102 	addcs	r0, r0, r2, lsl #2
8780c608:	20c00102 	sbccs	r0, r0, r2, lsl #2
8780c60c:	20c00002 	sbccs	r0, r0, r2
8780c610:	60e00001 	rscvs	r0, r0, r1
8780c614:	c0708001 	rsbsgt	r8, r0, r1
8780c618:	803fe003 	eorshi	lr, pc, r3
8780c61c:	001f0000 	andseq	r0, pc, r0
	...
8780c668:	00006000 	andeq	r6, r0, r0
8780c66c:	00008003 	andeq	r8, r0, r3
8780c670:	00000003 	andeq	r0, r0, r3
8780c674:	00000002 	andeq	r0, r0, r2
8780c678:	20000002 	andcs	r0, r0, r2
8780c67c:	20000002 	andcs	r0, r0, r2
8780c680:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c684:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780c688:	20000002 	andcs	r0, r0, r2
8780c68c:	20000002 	andcs	r0, r0, r2
8780c690:	00000002 	andeq	r0, r0, r2
8780c694:	00000002 	andeq	r0, r0, r2
8780c698:	00008003 	andeq	r8, r0, r3
8780c69c:	0000e000 	andeq	lr, r0, r0
	...
8780c6e4:	00000002 	andeq	r0, r0, r2
8780c6e8:	00000002 	andeq	r0, r0, r2
8780c6ec:	00ffff03 	rscseq	pc, pc, r3, lsl #30
8780c6f0:	c0ffff03 	rscsgt	pc, pc, r3, lsl #30
8780c6f4:	40000002 	andmi	r0, r0, r2
8780c6f8:	60000002 	andvs	r0, r0, r2
8780c6fc:	20000000 	andcs	r0, r0, r0
8780c700:	20000000 	andcs	r0, r0, r0
8780c704:	20000000 	andcs	r0, r0, r0
8780c708:	20000000 	andcs	r0, r0, r0
8780c70c:	40000002 	andmi	r0, r0, r2
8780c710:	80000002 	andhi	r0, r0, r2
8780c714:	00ffff03 	rscseq	pc, pc, r3, lsl #30
8780c718:	00000002 	andeq	r0, r0, r2
8780c71c:	00000002 	andeq	r0, r0, r2
	...
8780c768:	00000002 	andeq	r0, r0, r2
8780c76c:	00000002 	andeq	r0, r0, r2
8780c770:	0000c003 	andeq	ip, r0, r3
8780c774:	0000fc03 	andeq	pc, r0, r3, lsl #24
8780c778:	00c03f02 	sbceq	r3, r0, r2, lsl #30
8780c77c:	00f80300 	rscseq	r0, r8, r0, lsl #6
8780c780:	807f0000 	rsbshi	r0, pc, r0
8780c784:	e0070000 	and	r0, r7, r0
8780c788:	80070000 	andhi	r0, r7, r0
8780c78c:	00780000 	rsbseq	r0, r8, r0
8780c790:	00c00302 	sbceq	r0, r0, r2, lsl #6
8780c794:	00003c02 	andeq	r3, r0, r2, lsl #24
8780c798:	0000c003 	andeq	ip, r0, r3
8780c79c:	00000002 	andeq	r0, r0, r2
8780c7a0:	00000002 	andeq	r0, r0, r2
	...
8780c7e4:	00000002 	andeq	r0, r0, r2
8780c7e8:	0000c003 	andeq	ip, r0, r3
8780c7ec:	0080ff03 	addeq	pc, r0, r3, lsl #30
8780c7f0:	00fe3f02 	rscseq	r3, lr, r2, lsl #30
8780c7f4:	e07f0002 	rsbs	r0, pc, r2
8780c7f8:	000f0000 	andeq	r0, pc, r0
8780c7fc:	00f00002 	rscseq	r0, r0, r2
8780c800:	0000ef03 	andeq	lr, r0, r3, lsl #30
8780c804:	0080ff03 	addeq	pc, r0, r3, lsl #30
8780c808:	00fe0f02 	rscseq	r0, lr, r2, lsl #30
8780c80c:	e03f0000 	eors	r0, pc, r0
8780c810:	001f0000 	andseq	r0, pc, r0
8780c814:	00e00702 	rsceq	r0, r0, r2, lsl #14
8780c818:	0000f803 	andeq	pc, r0, r3, lsl #16
8780c81c:	00000003 	andeq	r0, r0, r3
8780c820:	00000002 	andeq	r0, r0, r2
	...
8780c868:	20000002 	andcs	r0, r0, r2
8780c86c:	20000002 	andcs	r0, r0, r2
8780c870:	e0008003 	and	r8, r0, r3
8780c874:	2003f003 	andcs	pc, r3, r3
8780c878:	200cfc02 	andcs	pc, ip, r2, lsl #24
8780c87c:	00301f02 	eorseq	r1, r0, r2, lsl #30
8780c880:	00c00700 	sbceq	r0, r0, r0, lsl #14
8780c884:	00f00700 	rscseq	r0, r0, r0, lsl #14
8780c888:	007c1802 	rsbseq	r1, ip, r2, lsl #16
8780c88c:	201f6002 	andscs	r6, pc, r2
8780c890:	e0038003 	and	r8, r3, r3
8780c894:	e0000002 	and	r0, r0, r2
8780c898:	20000002 	andcs	r0, r0, r2
8780c89c:	20000000 	andcs	r0, r0, r0
	...
8780c8e8:	00000002 	andeq	r0, r0, r2
8780c8ec:	00000003 	andeq	r0, r0, r3
8780c8f0:	0000c003 	andeq	ip, r0, r3
8780c8f4:	0000f803 	andeq	pc, r0, r3, lsl #16
8780c8f8:	20003e02 	andcs	r3, r0, r2, lsl #28
8780c8fc:	20c00f02 	sbccs	r0, r0, r2, lsl #30
8780c900:	e0ff0100 	rscs	r0, pc, r0, lsl #2
8780c904:	e07f0000 	rsbs	r0, pc, r0
8780c908:	20800300 	addcs	r0, r0, r0, lsl #6
8780c90c:	20001c02 	andcs	r1, r0, r2, lsl #24
8780c910:	00007002 	andeq	r7, r0, r2
8780c914:	00008003 	andeq	r8, r0, r3
8780c918:	00000002 	andeq	r0, r0, r2
8780c91c:	00000002 	andeq	r0, r0, r2
	...
8780c968:	20000000 	andcs	r0, r0, r0
8780c96c:	e0006000 	and	r6, r0, r0
8780c970:	e0038003 	and	r8, r3, r3
8780c974:	a00f0003 	andge	r0, pc, r3
8780c978:	203e0002 	eorscs	r0, lr, r2
8780c97c:	20f80002 	rscscs	r0, r8, r2
8780c980:	20e00302 	rsccs	r0, r0, r2, lsl #6
8780c984:	20800f02 	addcs	r0, r0, r2, lsl #30
8780c988:	20003e02 	andcs	r3, r0, r2, lsl #28
8780c98c:	20007802 	andcs	r7, r0, r2, lsl #16
8780c990:	6000e003 	andvs	lr, r0, r3
8780c994:	e0008003 	and	r8, r0, r3
8780c998:	00030002 	andeq	r0, r3, r2
	...
8780c9fc:	fcffff1f 	ldc2l	15, cr15, [pc], #124	; 8780ca80 <asc2_3216+0x1e1c>
8780ca00:	04000010 	streq	r0, [r0], #-16
8780ca04:	04000010 	streq	r0, [r0], #-16
8780ca08:	04000010 	streq	r0, [r0], #-16
8780ca0c:	04000010 	streq	r0, [r0], #-16
8780ca10:	04000010 	streq	r0, [r0], #-16
8780ca14:	04000010 	streq	r0, [r0], #-16
8780ca18:	04000010 	streq	r0, [r0], #-16
	...
8780ca70:	00000007 	andeq	r0, r0, r7
8780ca74:	0000c003 	andeq	ip, r0, r3
8780ca78:	00007800 	andeq	r7, r0, r0, lsl #16
8780ca7c:	00001e00 	andeq	r1, r0, r0, lsl #28
8780ca80:	00c00300 	sbceq	r0, r0, r0, lsl #6
8780ca84:	00f00000 	rscseq	r0, r0, r0
8780ca88:	001e0000 	andseq	r0, lr, r0
8780ca8c:	80070000 	andhi	r0, r7, r0
8780ca90:	f0000000 			; <UNDEFINED> instruction: 0xf0000000
8780ca94:	3c000000 	stccc	0, cr0, [r0], {-0}
8780ca98:	06000000 	streq	r0, [r0], -r0
	...
8780caec:	04000010 	streq	r0, [r0], #-16
8780caf0:	04000010 	streq	r0, [r0], #-16
8780caf4:	04000010 	streq	r0, [r0], #-16
8780caf8:	04000010 	streq	r0, [r0], #-16
8780cafc:	04000010 	streq	r0, [r0], #-16
8780cb00:	04000010 	streq	r0, [r0], #-16
8780cb04:	04000010 	streq	r0, [r0], #-16
8780cb08:	fcffff1f 	ldc2l	15, cr15, [pc], #124	; 8780cb8c <asc2_3216+0x1f28>
	...
8780cb74:	00000002 	andeq	r0, r0, r2
8780cb78:	00000004 	andeq	r0, r0, r4
8780cb7c:	00000008 	andeq	r0, r0, r8
8780cb80:	00000018 	andeq	r0, r0, r8, lsl r0
8780cb84:	00000010 	andeq	r0, r0, r0, lsl r0
8780cb88:	00000018 	andeq	r0, r0, r8, lsl r0
8780cb8c:	00000018 	andeq	r0, r0, r8, lsl r0
8780cb90:	00000004 	andeq	r0, r0, r4
8780cb94:	00000002 	andeq	r0, r0, r2
	...
8780cbe4:	01000000 	mrseq	r0, (UNDEF: 0)
8780cbe8:	01000000 	mrseq	r0, (UNDEF: 0)
8780cbec:	01000000 	mrseq	r0, (UNDEF: 0)
8780cbf0:	01000000 	mrseq	r0, (UNDEF: 0)
8780cbf4:	01000000 	mrseq	r0, (UNDEF: 0)
8780cbf8:	01000000 	mrseq	r0, (UNDEF: 0)
8780cbfc:	01000000 	mrseq	r0, (UNDEF: 0)
8780cc00:	01000000 	mrseq	r0, (UNDEF: 0)
8780cc04:	01000000 	mrseq	r0, (UNDEF: 0)
8780cc08:	01000000 	mrseq	r0, (UNDEF: 0)
8780cc0c:	01000000 	mrseq	r0, (UNDEF: 0)
8780cc10:	01000000 	mrseq	r0, (UNDEF: 0)
8780cc14:	01000000 	mrseq	r0, (UNDEF: 0)
8780cc18:	01000000 	mrseq	r0, (UNDEF: 0)
8780cc1c:	01000000 	mrseq	r0, (UNDEF: 0)
8780cc20:	01000000 	mrseq	r0, (UNDEF: 0)
	...
8780cc70:	00000010 	andeq	r0, r0, r0, lsl r0
8780cc74:	00000010 	andeq	r0, r0, r0, lsl r0
8780cc78:	00000010 	andeq	r0, r0, r0, lsl r0
8780cc7c:	00000018 	andeq	r0, r0, r8, lsl r0
8780cc80:	00000008 	andeq	r0, r0, r8
8780cc84:	00000004 	andeq	r0, r0, r4
	...
8780cce8:	80030000 	andhi	r0, r3, r0
8780ccec:	c08f0100 	addgt	r0, pc, r0, lsl #2
8780ccf0:	608c0300 	addvs	r0, ip, r0, lsl #6
8780ccf4:	20180600 	andscs	r0, r8, r0, lsl #12
8780ccf8:	20100400 	andscs	r0, r0, r0, lsl #8
8780ccfc:	20100400 	andscs	r0, r0, r0, lsl #8
8780cd00:	20200400 	eorcs	r0, r0, r0, lsl #8
8780cd04:	40200400 	eormi	r0, r0, r0, lsl #8
8780cd08:	40200600 	eormi	r0, r0, r0, lsl #12
8780cd0c:	c0ff0300 	rscsgt	r0, pc, r0, lsl #6
8780cd10:	e0ff0100 	rscs	r0, pc, r0, lsl #2
8780cd14:	20000000 	andcs	r0, r0, r0
8780cd18:	20000000 	andcs	r0, r0, r0
8780cd1c:	c0000000 	andgt	r0, r0, r0
	...
8780cd68:	00000002 	andeq	r0, r0, r2
8780cd6c:	00000002 	andeq	r0, r0, r2
8780cd70:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780cd74:	c0ffff07 	rscsgt	pc, pc, r7, lsl #30
8780cd78:	c0800100 	addgt	r0, r0, r0, lsl #2
8780cd7c:	60000200 	andvs	r0, r0, r0, lsl #4
8780cd80:	20000200 	andcs	r0, r0, r0, lsl #4
8780cd84:	20000400 	andcs	r0, r0, r0, lsl #8
8780cd88:	20000400 	andcs	r0, r0, r0, lsl #8
8780cd8c:	20000400 	andcs	r0, r0, r0, lsl #8
8780cd90:	40000600 	andmi	r0, r0, r0, lsl #12
8780cd94:	c0000300 	andgt	r0, r0, r0, lsl #6
8780cd98:	80ff0100 	rscshi	r0, pc, r0, lsl #2
8780cd9c:	00fe0000 	rscseq	r0, lr, r0
	...
8780cdec:	007e0000 	rsbseq	r0, lr, r0
8780cdf0:	80ff0100 	rscshi	r0, pc, r0, lsl #2
8780cdf4:	c0810300 	addgt	r0, r1, r0, lsl #6
8780cdf8:	40000200 	andmi	r0, r0, r0, lsl #4
8780cdfc:	20000600 	andcs	r0, r0, r0, lsl #12
8780ce00:	20000400 	andcs	r0, r0, r0, lsl #8
8780ce04:	20000400 	andcs	r0, r0, r0, lsl #8
8780ce08:	20000400 	andcs	r0, r0, r0, lsl #8
8780ce0c:	20000600 	andcs	r0, r0, r0, lsl #12
8780ce10:	40c00300 	sbcmi	r0, r0, r0, lsl #6
8780ce14:	80c00100 	sbchi	r0, r0, r0, lsl #2
8780ce18:	00030000 	andeq	r0, r3, r0
	...
8780ce6c:	007e0000 	rsbseq	r0, lr, r0
8780ce70:	80ff0100 	rscshi	r0, pc, r0, lsl #2
8780ce74:	c0800300 	addgt	r0, r0, r0, lsl #6
8780ce78:	60000600 	andvs	r0, r0, r0, lsl #12
8780ce7c:	20000400 	andcs	r0, r0, r0, lsl #8
8780ce80:	20000400 	andcs	r0, r0, r0, lsl #8
8780ce84:	20000400 	andcs	r0, r0, r0, lsl #8
8780ce88:	40000402 	andmi	r0, r0, r2, lsl #8
8780ce8c:	80000202 	andhi	r0, r0, r2, lsl #4
8780ce90:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780ce94:	c0ffff07 	rscsgt	pc, pc, r7, lsl #30
8780ce98:	40000000 	andmi	r0, r0, r0
8780ce9c:	40000000 	andmi	r0, r0, r0
	...
8780ceec:	007e0000 	rsbseq	r0, lr, r0
8780cef0:	80ff0100 	rscshi	r0, pc, r0, lsl #2
8780cef4:	c0110300 	andsgt	r0, r1, r0, lsl #6
8780cef8:	40100200 	andsmi	r0, r0, r0, lsl #4
8780cefc:	60100400 	andsvs	r0, r0, r0, lsl #8
8780cf00:	20100400 	andscs	r0, r0, r0, lsl #8
8780cf04:	20100400 	andscs	r0, r0, r0, lsl #8
8780cf08:	20100400 	andscs	r0, r0, r0, lsl #8
8780cf0c:	20100600 	andscs	r0, r0, r0, lsl #12
8780cf10:	40100300 	andsmi	r0, r0, r0, lsl #6
8780cf14:	c0f00100 	rscsgt	r0, r0, r0, lsl #2
8780cf18:	00710000 	rsbseq	r0, r1, r0
	...
8780cf6c:	00000400 	andeq	r0, r0, r0, lsl #8
8780cf70:	20000400 	andcs	r0, r0, r0, lsl #8
8780cf74:	20000400 	andcs	r0, r0, r0, lsl #8
8780cf78:	20000400 	andcs	r0, r0, r0, lsl #8
8780cf7c:	e0ff7f00 	rscs	r7, pc, r0, lsl #30
8780cf80:	e0ffff01 	rscs	pc, pc, r1, lsl #30
8780cf84:	20000401 	andcs	r0, r0, r1, lsl #8
8780cf88:	20000403 	andcs	r0, r0, r3, lsl #8
8780cf8c:	20000402 	andcs	r0, r0, r2, lsl #8
8780cf90:	20000402 	andcs	r0, r0, r2, lsl #8
8780cf94:	00000402 	andeq	r0, r0, r2, lsl #8
8780cf98:	00000002 	andeq	r0, r0, r2
8780cf9c:	0000c001 	andeq	ip, r0, r1
8780cfa0:	0000c001 	andeq	ip, r0, r1
	...
8780cfec:	1c000000 	stcne	0, cr0, [r0], {-0}
8780cff0:	3ee30000 	cdpcc	0, 14, cr0, cr3, cr0, {0}
8780cff4:	c2ff0300 	rscsgt	r0, pc, #0, 6
8780cff8:	c30c0200 	movwgt	r0, #49664	; 0xc200
8780cffc:	c1040400 	tstgt	r4, r0, lsl #8
8780d000:	c1040400 	tstgt	r4, r0, lsl #8
8780d004:	c1040400 	tstgt	r4, r0, lsl #8
8780d008:	c1040400 	tstgt	r4, r0, lsl #8
8780d00c:	c10c0600 	tstgt	ip, r0, lsl #12
8780d010:	c3f80300 	mvnsgt	r0, #0, 6
8780d014:	62f00500 	rscsvs	r0, r0, #0, 10
8780d018:	7e000600 	cfmadd32vc	mvax0, mvfx0, mvfx0, mvfx0
8780d01c:	3c000600 	stccc	6, cr0, [r0], {-0}
	...
8780d068:	20000002 	andcs	r0, r0, r2
8780d06c:	20000002 	andcs	r0, r0, r2
8780d070:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780d074:	e0ffff07 	rscs	pc, pc, r7, lsl #30
8780d078:	20000100 	andcs	r0, r0, r0, lsl #2
8780d07c:	20000200 	andcs	r0, r0, r0, lsl #4
8780d080:	00000600 	andeq	r0, r0, r0, lsl #12
8780d084:	00000400 	andeq	r0, r0, r0, lsl #8
8780d088:	00000400 	andeq	r0, r0, r0, lsl #8
8780d08c:	20000400 	andcs	r0, r0, r0, lsl #8
8780d090:	20000600 	andcs	r0, r0, r0, lsl #12
8780d094:	e0ff0300 	rscs	r0, pc, r0, lsl #6
8780d098:	e0ff0100 	rscs	r0, pc, r0, lsl #2
8780d09c:	20000000 	andcs	r0, r0, r0
8780d0a0:	20000000 	andcs	r0, r0, r0
	...
8780d0f0:	20000400 	andcs	r0, r0, r0, lsl #8
8780d0f4:	20000400 	andcs	r0, r0, r0, lsl #8
8780d0f8:	20000400 	andcs	r0, r0, r0, lsl #8
8780d0fc:	20000400 	andcs	r0, r0, r0, lsl #8
8780d100:	e0ff8703 	rscs	r8, pc, r3, lsl #14
8780d104:	e0ff8f03 	rscs	r8, pc, r3, lsl #30
8780d108:	20008003 	andcs	r8, r0, r3
8780d10c:	20000000 	andcs	r0, r0, r0
8780d110:	20000000 	andcs	r0, r0, r0
8780d114:	20000000 	andcs	r0, r0, r0
	...
8780d170:	06000000 	streq	r0, [r0], -r0
8780d174:	07000000 	streq	r0, [r0, -r0]
8780d178:	01000000 	mrseq	r0, (UNDEF: 0)
8780d17c:	01000000 	mrseq	r0, (UNDEF: 0)
8780d180:	01000400 	tsteq	r0, r0, lsl #8
8780d184:	01000400 	tsteq	r0, r0, lsl #8
8780d188:	03000400 	movweq	r0, #1024	; 0x400
8780d18c:	06000400 	streq	r0, [r0], -r0, lsl #8
8780d190:	fcff8703 	ldc2l	7, cr8, [pc], #12	; 8780d1a4 <asc2_3216+0x2540>
8780d194:	f8ff8f03 			; <UNDEFINED> instruction: 0xf8ff8f03
8780d198:	00008003 	andeq	r8, r0, r3
	...
8780d1e8:	20000002 	andcs	r0, r0, r2
8780d1ec:	20000002 	andcs	r0, r0, r2
8780d1f0:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780d1f4:	e0ffff07 	rscs	pc, pc, r7, lsl #30
8780d1f8:	20080000 	andcs	r0, r8, r0
8780d1fc:	20100000 	andscs	r0, r0, r0
8780d200:	00300000 	eorseq	r0, r0, r0
8780d204:	00fc0000 	rscseq	r0, ip, r0
8780d208:	008e0500 	addeq	r0, lr, r0, lsl #10
8780d20c:	a0070700 	andge	r0, r7, r0, lsl #14
8780d210:	e0010600 	and	r0, r1, r0, lsl #12
8780d214:	e0000400 	and	r0, r0, r0, lsl #8
8780d218:	20000400 	andcs	r0, r0, r0, lsl #8
8780d21c:	20000000 	andcs	r0, r0, r0
	...
8780d270:	20000002 	andcs	r0, r0, r2
8780d274:	20000002 	andcs	r0, r0, r2
8780d278:	20000002 	andcs	r0, r0, r2
8780d27c:	20000002 	andcs	r0, r0, r2
8780d280:	e0ffff03 	rscs	pc, pc, r3, lsl #30
8780d284:	e0ffff07 	rscs	pc, pc, r7, lsl #30
8780d288:	20000000 	andcs	r0, r0, r0
8780d28c:	20000000 	andcs	r0, r0, r0
8780d290:	20000000 	andcs	r0, r0, r0
8780d294:	20000000 	andcs	r0, r0, r0
	...
8780d2e8:	20000400 	andcs	r0, r0, r0, lsl #8
8780d2ec:	e0ff0700 	rscs	r0, pc, r0, lsl #14
8780d2f0:	e0ff0f00 	rscs	r0, pc, r0, lsl #30
8780d2f4:	20000200 	andcs	r0, r0, r0, lsl #4
8780d2f8:	00000400 	andeq	r0, r0, r0, lsl #8
8780d2fc:	20000400 	andcs	r0, r0, r0, lsl #8
8780d300:	e0ff0700 	rscs	r0, pc, r0, lsl #14
8780d304:	e0ff0300 	rscs	r0, pc, r0, lsl #6
8780d308:	20000200 	andcs	r0, r0, r0, lsl #4
8780d30c:	00000400 	andeq	r0, r0, r0, lsl #8
8780d310:	20000400 	andcs	r0, r0, r0, lsl #8
8780d314:	e0ff0700 	rscs	r0, pc, r0, lsl #14
8780d318:	e0ff0300 	rscs	r0, pc, r0, lsl #6
8780d31c:	20000000 	andcs	r0, r0, r0
	...
8780d368:	20000400 	andcs	r0, r0, r0, lsl #8
8780d36c:	20000400 	andcs	r0, r0, r0, lsl #8
8780d370:	e0ff0700 	rscs	r0, pc, r0, lsl #14
8780d374:	e0ff0f00 	rscs	r0, pc, r0, lsl #30
8780d378:	20000100 	andcs	r0, r0, r0, lsl #2
8780d37c:	20000200 	andcs	r0, r0, r0, lsl #4
8780d380:	00000200 	andeq	r0, r0, r0, lsl #4
8780d384:	00000400 	andeq	r0, r0, r0, lsl #8
8780d388:	00000400 	andeq	r0, r0, r0, lsl #8
8780d38c:	20000400 	andcs	r0, r0, r0, lsl #8
8780d390:	20000600 	andcs	r0, r0, r0, lsl #12
8780d394:	e0ff0300 	rscs	r0, pc, r0, lsl #6
8780d398:	e0ff0100 	rscs	r0, pc, r0, lsl #2
8780d39c:	20000000 	andcs	r0, r0, r0
8780d3a0:	20000000 	andcs	r0, r0, r0
	...
8780d3ec:	007e0000 	rsbseq	r0, lr, r0
8780d3f0:	80ff0000 	rscshi	r0, pc, r0
8780d3f4:	c0810300 	addgt	r0, r1, r0, lsl #6
8780d3f8:	40000200 	andmi	r0, r0, r0, lsl #4
8780d3fc:	20000600 	andcs	r0, r0, r0, lsl #12
8780d400:	20000400 	andcs	r0, r0, r0, lsl #8
8780d404:	20000400 	andcs	r0, r0, r0, lsl #8
8780d408:	20000400 	andcs	r0, r0, r0, lsl #8
8780d40c:	20000600 	andcs	r0, r0, r0, lsl #12
8780d410:	40000200 	andmi	r0, r0, r0, lsl #4
8780d414:	c0810300 	addgt	r0, r1, r0, lsl #6
8780d418:	80ff0100 	rscshi	r0, pc, r0, lsl #2
8780d41c:	007e0000 	rsbseq	r0, lr, r0
	...
8780d468:	01000400 	tsteq	r0, r0, lsl #8
8780d46c:	01000400 	tsteq	r0, r0, lsl #8
8780d470:	ffff0700 			; <UNDEFINED> instruction: 0xffff0700
8780d474:	ffff0f00 			; <UNDEFINED> instruction: 0xffff0f00
8780d478:	c1000100 	mrsgt	r0, (UNDEF: 16)
8780d47c:	41000200 	mrsmi	r0, R8_usr
8780d480:	20000400 	andcs	r0, r0, r0, lsl #8
8780d484:	20000400 	andcs	r0, r0, r0, lsl #8
8780d488:	20000400 	andcs	r0, r0, r0, lsl #8
8780d48c:	20000400 	andcs	r0, r0, r0, lsl #8
8780d490:	40000600 	andmi	r0, r0, r0, lsl #12
8780d494:	c0010300 	andgt	r0, r1, r0, lsl #6
8780d498:	80ff0100 	rscshi	r0, pc, r0, lsl #2
8780d49c:	007e0000 	rsbseq	r0, lr, r0
	...
8780d4ec:	007e0000 	rsbseq	r0, lr, r0
8780d4f0:	80ff0100 	rscshi	r0, pc, r0, lsl #2
8780d4f4:	c0800300 	addgt	r0, r0, r0, lsl #6
8780d4f8:	60000200 	andvs	r0, r0, r0, lsl #4
8780d4fc:	20000400 	andcs	r0, r0, r0, lsl #8
8780d500:	20000400 	andcs	r0, r0, r0, lsl #8
8780d504:	20000400 	andcs	r0, r0, r0, lsl #8
8780d508:	20000400 	andcs	r0, r0, r0, lsl #8
8780d50c:	41000200 	mrsmi	r0, R8_usr
8780d510:	c1000300 	mrsgt	r0, LR_irq
8780d514:	ffff0300 			; <UNDEFINED> instruction: 0xffff0300
8780d518:	ffff0700 			; <UNDEFINED> instruction: 0xffff0700
8780d51c:	01000000 	mrseq	r0, (UNDEF: 0)
8780d520:	01000000 	mrseq	r0, (UNDEF: 0)
	...
8780d568:	20000400 	andcs	r0, r0, r0, lsl #8
8780d56c:	20000400 	andcs	r0, r0, r0, lsl #8
8780d570:	20000400 	andcs	r0, r0, r0, lsl #8
8780d574:	20000400 	andcs	r0, r0, r0, lsl #8
8780d578:	e0ff0f00 	rscs	r0, pc, r0, lsl #30
8780d57c:	e0ff0f00 	rscs	r0, pc, r0, lsl #30
8780d580:	20c00000 	sbccs	r0, r0, r0
8780d584:	20000100 	andcs	r0, r0, r0, lsl #2
8780d588:	20000200 	andcs	r0, r0, r0, lsl #4
8780d58c:	20000600 	andcs	r0, r0, r0, lsl #12
8780d590:	00000400 	andeq	r0, r0, r0, lsl #8
8780d594:	00000400 	andeq	r0, r0, r0, lsl #8
8780d598:	00000700 	andeq	r0, r0, r0, lsl #14
8780d59c:	00000300 	andeq	r0, r0, r0, lsl #6
	...
8780d5ec:	e0030000 	and	r0, r3, r0
8780d5f0:	e0c00100 	sbc	r0, r0, r0, lsl #2
8780d5f4:	40e00300 	rscmi	r0, r0, r0, lsl #6
8780d5f8:	20300600 	eorscs	r0, r0, r0, lsl #12
8780d5fc:	20300400 	eorscs	r0, r0, r0, lsl #8
8780d600:	20180400 	andscs	r0, r8, r0, lsl #8
8780d604:	20180400 	andscs	r0, r8, r0, lsl #8
8780d608:	20180400 	andscs	r0, r8, r0, lsl #8
8780d60c:	200c0400 	andcs	r0, ip, r0, lsl #8
8780d610:	600c0200 	andvs	r0, ip, r0, lsl #4
8780d614:	c0070300 	andgt	r0, r7, r0, lsl #6
8780d618:	80830700 	addhi	r0, r3, r0, lsl #14
	...
8780d66c:	00000400 	andeq	r0, r0, r0, lsl #8
8780d670:	00000400 	andeq	r0, r0, r0, lsl #8
8780d674:	00000400 	andeq	r0, r0, r0, lsl #8
8780d678:	00000c00 	andeq	r0, r0, r0, lsl #24
8780d67c:	80ff1f00 	rscshi	r1, pc, r0, lsl #30
8780d680:	c0ffff00 	rscsgt	pc, pc, r0, lsl #30
8780d684:	60000400 	andvs	r0, r0, r0, lsl #8
8780d688:	20000400 	andcs	r0, r0, r0, lsl #8
8780d68c:	20000400 	andcs	r0, r0, r0, lsl #8
8780d690:	20000400 	andcs	r0, r0, r0, lsl #8
8780d694:	40000400 	andmi	r0, r0, r0, lsl #8
8780d698:	80010000 	andhi	r0, r1, r0
	...
8780d6e8:	00000400 	andeq	r0, r0, r0, lsl #8
8780d6ec:	00000400 	andeq	r0, r0, r0, lsl #8
8780d6f0:	80ff0700 	rscshi	r0, pc, r0, lsl #14
8780d6f4:	c0ff0f00 	rscsgt	r0, pc, r0, lsl #30
8780d6f8:	60000000 	andvs	r0, r0, r0
8780d6fc:	20000000 	andcs	r0, r0, r0
8780d700:	20000000 	andcs	r0, r0, r0
8780d704:	20000000 	andcs	r0, r0, r0
8780d708:	20000000 	andcs	r0, r0, r0
8780d70c:	40000400 	andmi	r0, r0, r0, lsl #8
8780d710:	80000400 	andhi	r0, r0, r0, lsl #8
8780d714:	e0ff0700 	rscs	r0, pc, r0, lsl #14
8780d718:	c0ff0f00 	rscsgt	r0, pc, r0, lsl #30
8780d71c:	40000000 	andmi	r0, r0, r0
8780d720:	40000000 	andmi	r0, r0, r0
	...
8780d768:	00000400 	andeq	r0, r0, r0, lsl #8
8780d76c:	00000400 	andeq	r0, r0, r0, lsl #8
8780d770:	00800700 	addeq	r0, r0, r0, lsl #14
8780d774:	00f00700 	rscseq	r0, r0, r0, lsl #14
8780d778:	00fe0400 	rscseq	r0, lr, r0, lsl #8
8780d77c:	c01f0400 	andsgt	r0, pc, r0, lsl #8
8780d780:	e0030000 	and	r0, r3, r0
8780d784:	80030000 	andhi	r0, r3, r0
8780d788:	001c0000 	andseq	r0, ip, r0
8780d78c:	00600400 	rsbeq	r0, r0, r0, lsl #8
8780d790:	00800700 	addeq	r0, r0, r0, lsl #14
8780d794:	00000600 	andeq	r0, r0, r0, lsl #12
8780d798:	00000400 	andeq	r0, r0, r0, lsl #8
	...
8780d7e4:	00000400 	andeq	r0, r0, r0, lsl #8
8780d7e8:	00000600 	andeq	r0, r0, r0, lsl #12
8780d7ec:	00c00700 	sbceq	r0, r0, r0, lsl #14
8780d7f0:	00fc0700 	rscseq	r0, ip, r0, lsl #14
8780d7f4:	803f0400 	eorshi	r0, pc, r0, lsl #8
8780d7f8:	e0030000 	and	r0, r3, r0
8780d7fc:	800f0400 	andhi	r0, pc, r0, lsl #8
8780d800:	00f00600 	rscseq	r0, r0, r0, lsl #12
8780d804:	00f00700 	rscseq	r0, r0, r0, lsl #14
8780d808:	80ff0700 	rscshi	r0, pc, r0, lsl #14
8780d80c:	e00f0400 	and	r0, pc, r0, lsl #8
8780d810:	80030000 	andhi	r0, r3, r0
8780d814:	003c0400 	eorseq	r0, ip, r0, lsl #8
8780d818:	00c00700 	sbceq	r0, r0, r0, lsl #14
8780d81c:	00000600 	andeq	r0, r0, r0, lsl #12
8780d820:	00000400 	andeq	r0, r0, r0, lsl #8
	...
8780d868:	20000000 	andcs	r0, r0, r0
8780d86c:	20000400 	andcs	r0, r0, r0, lsl #8
8780d870:	60000400 	andvs	r0, r0, r0, lsl #8
8780d874:	e0000700 	and	r0, r0, r0, lsl #14
8780d878:	20830700 	addcs	r0, r3, r0, lsl #14
8780d87c:	00e60700 	rsceq	r0, r6, r0, lsl #14
8780d880:	00f80400 	rscseq	r0, r8, r0, lsl #8
8780d884:	003c0000 	eorseq	r0, ip, r0
8780d888:	205e0400 	subscs	r0, lr, r0, lsl #8
8780d88c:	a0870500 	addge	r0, r7, r0, lsl #10
8780d890:	e0010600 	and	r0, r1, r0, lsl #12
8780d894:	60000400 	andvs	r0, r0, r0, lsl #8
8780d898:	20000400 	andcs	r0, r0, r0, lsl #8
8780d89c:	20000000 	andcs	r0, r0, r0
	...
8780d8e8:	00000400 	andeq	r0, r0, r0, lsl #8
8780d8ec:	03000400 	movweq	r0, #1024	; 0x400
8780d8f0:	03000700 	movweq	r0, #1792	; 0x700
8780d8f4:	01e00700 	mvneq	r0, r0, lsl #14
8780d8f8:	01f80400 	mvnseq	r0, r0, lsl #8
8780d8fc:	021f0400 	andseq	r0, pc, #0, 8
8780d900:	fc070000 	stc2	0, cr0, [r7], {-0}
8780d904:	e0000000 	and	r0, r0, r0
8780d908:	00070000 	andeq	r0, r7, r0
8780d90c:	00380400 	eorseq	r0, r8, r0, lsl #8
8780d910:	00c00700 	sbceq	r0, r0, r0, lsl #14
8780d914:	00000600 	andeq	r0, r0, r0, lsl #12
8780d918:	00000400 	andeq	r0, r0, r0, lsl #8
8780d91c:	00000400 	andeq	r0, r0, r0, lsl #8
	...
8780d96c:	60800700 	addvs	r0, r0, r0, lsl #14
8780d970:	e0000600 	and	r0, r0, r0, lsl #12
8780d974:	e0030400 	and	r0, r3, r0, lsl #8
8780d978:	a0070400 	andge	r0, r7, r0, lsl #8
8780d97c:	200e0400 	andcs	r0, lr, r0, lsl #8
8780d980:	203c0400 	eorscs	r0, ip, r0, lsl #8
8780d984:	20700400 	rsbscs	r0, r0, r0, lsl #8
8780d988:	20e00500 	rsccs	r0, r0, r0, lsl #10
8780d98c:	20800700 	addcs	r0, r0, r0, lsl #14
8780d990:	60000700 	andvs	r0, r0, r0, lsl #14
8780d994:	e0000400 	and	r0, r0, r0, lsl #8
8780d998:	80030000 	andhi	r0, r3, r0
	...
8780da00:	00800000 	addeq	r0, r0, r0
8780da04:	00800000 	addeq	r0, r0, r0
8780da08:	00400100 	subeq	r0, r0, r0, lsl #2
8780da0c:	f83ffe07 			; <UNDEFINED> instruction: 0xf83ffe07
8780da10:	04000008 	streq	r0, [r0], #-8
8780da14:	02000010 	andeq	r0, r0, #16
8780da18:	02000010 	andeq	r0, r0, #16
	...
8780da84:	ffffffff 			; <UNDEFINED> instruction: 0xffffffff
	...
8780daec:	02000010 	andeq	r0, r0, #16
8780daf0:	02000010 	andeq	r0, r0, #16
8780daf4:	04000008 	streq	r0, [r0], #-8
8780daf8:	f83ffe07 			; <UNDEFINED> instruction: 0xf83ffe07
8780dafc:	00400100 	subeq	r0, r0, r0, lsl #2
8780db00:	00800000 	addeq	r0, r0, r0
8780db04:	00800000 	addeq	r0, r0, r0
	...
8780db68:	00000018 	andeq	r0, r0, r8, lsl r0
8780db6c:	00000030 	andeq	r0, r0, r0, lsr r0
8780db70:	00000040 	andeq	r0, r0, r0, asr #32
8780db74:	00000040 	andeq	r0, r0, r0, asr #32
8780db78:	00000040 	andeq	r0, r0, r0, asr #32
8780db7c:	00000020 	andeq	r0, r0, r0, lsr #32
8780db80:	00000030 	andeq	r0, r0, r0, lsr r0
8780db84:	00000008 	andeq	r0, r0, r8
8780db88:	00000004 	andeq	r0, r0, r4
8780db8c:	00000002 	andeq	r0, r0, r2
8780db90:	00000002 	andeq	r0, r0, r2
8780db94:	00000002 	andeq	r0, r0, r2
8780db98:	0000000c 	andeq	r0, r0, ip
8780db9c:	00000018 	andeq	r0, r0, r8, lsl r0
	...

8780dbe4 <.LANCHOR0>:
8780dbe4:	00000000 	andeq	r0, r0, r0
8780dbe8:	003b001f 	eorseq	r0, fp, pc, lsl r0
8780dbec:	0078005a 	rsbseq	r0, r8, sl, asr r0
8780dbf0:	00b50097 	umlalseq	r0, r5, r7, r0
8780dbf4:	00f300d4 	ldrsbteq	r0, [r3], #4
8780dbf8:	01300111 	teqeq	r0, r1, lsl r1
8780dbfc:	0000014e 	andeq	r0, r0, lr, asr #2
8780dc00:	1f1c1f00 	svcne	0x001c1f00
8780dc04:	1f1e1f1e 	svcne	0x001e1f1e
8780dc08:	1e1f1e1f 	mrcne	14, 0, r1, cr15, cr15, {0}
8780dc0c:	0000001f 	andeq	r0, r0, pc, lsl r0
8780dc10:	6b736174 	blvs	894e61e8 <__bss_end__+0x1ccb040>
8780dc14:	000a0d31 	andeq	r0, sl, r1, lsr sp
8780dc18:	6b736174 	blvs	894e61f0 <__bss_end__+0x1ccb048>
8780dc1c:	000a0d32 	andeq	r0, sl, r2, lsr sp
8780dc20:	6b736174 	blvs	894e61f8 <__bss_end__+0x1ccb050>
8780dc24:	00000031 	andeq	r0, r0, r1, lsr r0
8780dc28:	6b736174 	blvs	894e6200 <__bss_end__+0x1ccb058>
8780dc2c:	00000032 	andeq	r0, r0, r2, lsr r0
8780dc30:	65737361 	ldrbvs	r7, [r3, #-865]!	; 0x361
8780dc34:	66207472 			; <UNDEFINED> instruction: 0x66207472
8780dc38:	656c6961 	strbvs	r6, [ip, #-2401]!	; 0x961
8780dc3c:	25203a64 	strcs	r3, [r0, #-2660]!	; 0xa64
8780dc40:	64252073 	strtvs	r2, [r5], #-115	; 0x73
8780dc44:	0000000a 	andeq	r0, r0, sl
8780dc48:	4e4e5552 	mcrmi	5, 2, r5, cr14, cr2, {2}
8780dc4c:	00474e49 	subeq	r4, r7, r9, asr #28
8780dc50:	444e4550 	strbmi	r4, [lr], #-1360	; 0x550
8780dc54:	454d4954 	strbmi	r4, [sp, #-2388]	; 0x954
8780dc58:	5f54554f 	svcpl	0x0054554f
8780dc5c:	50535553 	subspl	r5, r3, r3, asr r5
8780dc60:	45444e45 	strbmi	r4, [r4, #-3653]	; 0xe45
8780dc64:	00000044 	andeq	r0, r0, r4, asr #32
8780dc68:	444e4550 	strbmi	r4, [lr], #-1360	; 0x550
8780dc6c:	5355535f 	cmppl	r5, #2080374785	; 0x7c000001
8780dc70:	444e4550 	strbmi	r4, [lr], #-1360	; 0x550
8780dc74:	00004445 	andeq	r4, r0, r5, asr #8
8780dc78:	45454c53 	strbmi	r4, [r5, #-3155]	; 0xc53
8780dc7c:	55535f50 	ldrbpl	r5, [r3, #-3920]	; 0xf50
8780dc80:	4e455053 	mcrmi	0, 2, r5, cr5, cr3, {2}
8780dc84:	00444544 	subeq	r4, r4, r4, asr #10
8780dc88:	444e4550 	strbmi	r4, [lr], #-1360	; 0x550
8780dc8c:	454d4954 	strbmi	r4, [sp, #-2388]	; 0x954
8780dc90:	0054554f 	subseq	r5, r4, pc, asr #10
8780dc94:	444e4550 	strbmi	r4, [lr], #-1360	; 0x550
8780dc98:	00000000 	andeq	r0, r0, r0
8780dc9c:	45454c53 	strbmi	r4, [r5, #-3155]	; 0xc53
8780dca0:	00000050 	andeq	r0, r0, r0, asr r0
8780dca4:	4f4e4241 	svcmi	0x004e4241
8780dca8:	4c414d52 	mcrrmi	13, 5, r4, r1, cr2
8780dcac:	00000000 	andeq	r0, r0, r0
8780dcb0:	44414552 	strbmi	r4, [r1], #-1362	; 0x552
8780dcb4:	00000059 	andeq	r0, r0, r9, asr r0
8780dcb8:	206b7374 	rsbcs	r7, fp, r4, ror r3
8780dcbc:	656d616e 	strbvs	r6, [sp, #-366]!	; 0x16e
8780dcc0:	7325203a 	teqvc	r5, #58	; 0x3a
8780dcc4:	00000000 	andeq	r0, r0, r0
8780dcc8:	206b7374 	rsbcs	r7, fp, r4, ror r3
8780dccc:	74617473 	strbtvc	r7, [r1], #-1139	; 0x473
8780dcd0:	7325203a 	teqvc	r5, #58	; 0x3a
8780dcd4:	00000000 	andeq	r0, r0, r0
8780dcd8:	206b7473 	rsbcs	r7, fp, r3, ror r4
8780dcdc:	657a6973 	ldrbvs	r6, [sl, #-2419]!	; 0x973
8780dce0:	6425203a 	strtvs	r2, [r5], #-58	; 0x3a
8780dce4:	00000000 	andeq	r0, r0, r0
8780dce8:	206b7473 	rsbcs	r7, fp, r3, ror r4
8780dcec:	65736162 	ldrbvs	r6, [r3, #-354]!	; 0x162
8780dcf0:	7830203a 	ldmdavc	r0!, {r1, r3, r4, r5, sp}
8780dcf4:	00007025 	andeq	r7, r0, r5, lsr #32
8780dcf8:	206b7473 	rsbcs	r7, fp, r3, ror r4
8780dcfc:	20706f74 	rsbscs	r6, r0, r4, ror pc
8780dd00:	7830203a 	ldmdavc	r0!, {r1, r3, r4, r5, sp}
8780dd04:	00007025 	andeq	r7, r0, r5, lsr #32
8780dd08:	656c6469 	strbvs	r6, [ip, #-1129]!	; 0x469
8780dd0c:	00000000 	andeq	r0, r0, r0
8780dd10:	20203072 	eorcs	r3, r0, r2, ror r0
8780dd14:	30257830 	eorcc	r7, r5, r0, lsr r8
8780dd18:	72207838 	eorvc	r7, r0, #56, 16	; 0x380000
8780dd1c:	30202031 	eorcc	r2, r0, r1, lsr r0
8780dd20:	38302578 	ldmdacc	r0!, {r3, r4, r5, r6, r8, sl, sp}
8780dd24:	32722078 	rsbscc	r2, r2, #120	; 0x78
8780dd28:	78302020 	ldmdavc	r0!, {r5, sp}
8780dd2c:	78383025 	ldmdavc	r8!, {r0, r2, r5, ip, sp}
8780dd30:	20337220 	eorscs	r7, r3, r0, lsr #4
8780dd34:	25783020 	ldrbcs	r3, [r8, #-32]!
8780dd38:	0a783830 	beq	8961be00 <__bss_end__+0x1e00c58>
8780dd3c:	00000000 	andeq	r0, r0, r0
8780dd40:	20203472 	eorcs	r3, r0, r2, ror r4
8780dd44:	30257830 	eorcc	r7, r5, r0, lsr r8
8780dd48:	72207838 	eorvc	r7, r0, #56, 16	; 0x380000
8780dd4c:	30202035 	eorcc	r2, r0, r5, lsr r0
8780dd50:	38302578 	ldmdacc	r0!, {r3, r4, r5, r6, r8, sl, sp}
8780dd54:	36722078 			; <UNDEFINED> instruction: 0x36722078
8780dd58:	78302020 	ldmdavc	r0!, {r5, sp}
8780dd5c:	78383025 	ldmdavc	r8!, {r0, r2, r5, ip, sp}
8780dd60:	20377220 	eorscs	r7, r7, r0, lsr #4
8780dd64:	25783020 	ldrbcs	r3, [r8, #-32]!
8780dd68:	0a783830 	beq	8961be30 <__bss_end__+0x1e00c88>
8780dd6c:	00000000 	andeq	r0, r0, r0
8780dd70:	20203872 	eorcs	r3, r0, r2, ror r8
8780dd74:	30257830 	eorcc	r7, r5, r0, lsr r8
8780dd78:	72207838 	eorvc	r7, r0, #56, 16	; 0x380000
8780dd7c:	30202039 	eorcc	r2, r0, r9, lsr r0
8780dd80:	38302578 	ldmdacc	r0!, {r3, r4, r5, r6, r8, sl, sp}
8780dd84:	31722078 	cmncc	r2, r8, ror r0
8780dd88:	78302030 	ldmdavc	r0!, {r4, r5, sp}
8780dd8c:	78383025 	ldmdavc	r8!, {r0, r2, r5, ip, sp}
8780dd90:	31317220 	teqcc	r1, r0, lsr #4
8780dd94:	25783020 	ldrbcs	r3, [r8, #-32]!
8780dd98:	0a783830 	beq	8961be60 <__bss_end__+0x1e00cb8>
8780dd9c:	00000000 	andeq	r0, r0, r0
8780dda0:	20323172 	eorscs	r3, r2, r2, ror r1
8780dda4:	30257830 	eorcc	r7, r5, r0, lsr r8
8780dda8:	70207838 	eorvc	r7, r0, r8, lsr r8
8780ddac:	30202063 	eorcc	r2, r0, r3, rrx
8780ddb0:	38302578 	ldmdacc	r0!, {r3, r4, r5, r6, r8, sl, sp}
8780ddb4:	00000a78 	andeq	r0, r0, r8, ror sl
8780ddb8:	72737073 	rsbsvc	r7, r3, #115	; 0x73
8780ddbc:	25783020 	ldrbcs	r3, [r8, #-32]!
8780ddc0:	0a783830 	beq	8961be88 <__bss_end__+0x1e00ce0>
8780ddc4:	00000000 	andeq	r0, r0, r0
8780ddc8:	65646e75 	strbvs	r6, [r4, #-3701]!	; 0xe75
8780ddcc:	656e6966 	strbvs	r6, [lr, #-2406]!	; 0x966
8780ddd0:	000a0d64 	andeq	r0, sl, r4, ror #26
8780ddd4:	65646e75 	strbvs	r6, [r4, #-3701]!	; 0xe75
8780ddd8:	656e6966 	strbvs	r6, [lr, #-2406]!	; 0x966
8780dddc:	706f2064 	rsbvc	r2, pc, r4, rrx
8780dde0:	65646f63 	strbvs	r6, [r4, #-3939]!	; 0xf63
8780dde4:	25783020 	ldrbcs	r3, [r8, #-32]!
8780dde8:	00000a78 	andeq	r0, r0, r8, ror sl
8780ddec:	65646e75 	strbvs	r6, [r4, #-3701]!	; 0xe75
8780ddf0:	656e6966 	strbvs	r6, [lr, #-2406]!	; 0x966
8780ddf4:	62612064 	rsbvs	r2, r1, #100	; 0x64
8780ddf8:	2c74726f 	lfmcs	f7, 2, [r4], #-444	; 0xfffffe44
8780ddfc:	6c616820 	stclvs	8, cr6, [r1], #-128	; 0xffffff80
8780de00:	676e6974 			; <UNDEFINED> instruction: 0x676e6974
8780de04:	0000000a 	andeq	r0, r0, sl
8780de08:	00007325 	andeq	r7, r0, r5, lsr #6
8780de0c:	61746164 	cmnvs	r4, r4, ror #2
8780de10:	6f626120 	svcvs	0x00626120
8780de14:	21217472 	teqcs	r1, r2, ror r4
8780de18:	00000a0d 	andeq	r0, r0, sp, lsl #20
8780de1c:	66657270 			; <UNDEFINED> instruction: 0x66657270
8780de20:	68637465 	stmdavs	r3!, {r0, r2, r5, r6, sl, ip, sp, lr}^
8780de24:	6f626120 	svcvs	0x00626120
8780de28:	21217472 	teqcs	r1, r2, ror r4
8780de2c:	00000a0d 	andeq	r0, r0, sp, lsl #20
8780de30:	4c554e3c 	mrrcmi	14, 3, r4, r5, cr12
8780de34:	00003e4c 	andeq	r3, r0, ip, asr #28
8780de38:	23232323 	teqcs	r3, #-1946157056	; 0x8c000000
8780de3c:	0a0d2323 	beq	87b56ad0 <__bss_end__+0x33b928>
8780de40:	00000000 	andeq	r0, r0, r0
8780de44:	326d6369 	rsbcc	r6, sp, #-1543503871	; 0xa4000001
8780de48:	38303630 	ldmdacc	r0!, {r4, r5, r9, sl, ip, sp}
8780de4c:	20646920 	rsbcs	r6, r4, r0, lsr #18
8780de50:	2325203d 	teqcs	r5, #61	; 0x3d
8780de54:	000a0d58 	andeq	r0, sl, r8, asr sp
8780de58:	63756f54 	cmnvs	r5, #84, 30	; 0x150
8780de5c:	72462068 	subvc	r2, r6, #104	; 0x68
8780de60:	61776d69 	cmnvs	r7, r9, ror #26
8780de64:	56206572 			; <UNDEFINED> instruction: 0x56206572
8780de68:	69737265 	ldmdbvs	r3!, {r0, r2, r5, r6, r9, ip, sp, lr}^
8780de6c:	253a6e6f 	ldrcs	r6, [sl, #-3695]!	; 0xe6f
8780de70:	0a0d5823 	beq	87b63f04 <__bss_end__+0x348d5c>
8780de74:	00000000 	andeq	r0, r0, r0

Disassembly of section .data:

8780de78 <__data_end>:
8780de78:	8780de78 			; <UNDEFINED> instruction: 0x8780de78
8780de7c:	8780de78 			; <UNDEFINED> instruction: 0x8780de78

8780de80 <k_stat_list>:
8780de80:	8780de80 	strhi	sp, [r0, r0, lsl #29]
8780de84:	8780de80 	strhi	sp, [r0, r0, lsl #29]

8780de88 <k_cpu_tick_per_second>:
8780de88:	000003e8 	andeq	r0, r0, r8, ror #7
8780de8c:	00000000 	andeq	r0, r0, r0

8780de90 <_ctype>:
8780de90:	08080808 	stmdaeq	r8, {r3, fp}
8780de94:	08080808 	stmdaeq	r8, {r3, fp}
8780de98:	28282808 	stmdacs	r8!, {r3, fp, sp}
8780de9c:	08082828 	stmdaeq	r8, {r3, r5, fp, sp}
8780dea0:	08080808 	stmdaeq	r8, {r3, fp}
8780dea4:	08080808 	stmdaeq	r8, {r3, fp}
8780dea8:	08080808 	stmdaeq	r8, {r3, fp}
8780deac:	08080808 	stmdaeq	r8, {r3, fp}
8780deb0:	101010a0 	andsne	r1, r0, r0, lsr #1
8780deb4:	10101010 	andsne	r1, r0, r0, lsl r0
8780deb8:	10101010 	andsne	r1, r0, r0, lsl r0
8780debc:	10101010 	andsne	r1, r0, r0, lsl r0
8780dec0:	04040404 	streq	r0, [r4], #-1028	; 0x404
8780dec4:	04040404 	streq	r0, [r4], #-1028	; 0x404
8780dec8:	10100404 	andsne	r0, r0, r4, lsl #8
8780decc:	10101010 	andsne	r1, r0, r0, lsl r0
8780ded0:	41414110 	cmpmi	r1, r0, lsl r1
8780ded4:	01414141 	cmpeq	r1, r1, asr #2
8780ded8:	01010101 	tsteq	r1, r1, lsl #2
8780dedc:	01010101 	tsteq	r1, r1, lsl #2
8780dee0:	01010101 	tsteq	r1, r1, lsl #2
8780dee4:	01010101 	tsteq	r1, r1, lsl #2
8780dee8:	10010101 	andne	r0, r1, r1, lsl #2
8780deec:	10101010 	andsne	r1, r0, r0, lsl r0
8780def0:	42424210 	submi	r4, r2, #16, 4
8780def4:	02424242 	subeq	r4, r2, #536870916	; 0x20000004
8780def8:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780defc:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780df00:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780df04:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780df08:	10020202 	andne	r0, r2, r2, lsl #4
8780df0c:	08101010 	ldmdaeq	r0, {r4, ip}
	...
8780df30:	101010a0 	andsne	r1, r0, r0, lsr #1
8780df34:	10101010 	andsne	r1, r0, r0, lsl r0
8780df38:	10101010 	andsne	r1, r0, r0, lsl r0
8780df3c:	10101010 	andsne	r1, r0, r0, lsl r0
8780df40:	10101010 	andsne	r1, r0, r0, lsl r0
8780df44:	10101010 	andsne	r1, r0, r0, lsl r0
8780df48:	10101010 	andsne	r1, r0, r0, lsl r0
8780df4c:	10101010 	andsne	r1, r0, r0, lsl r0
8780df50:	01010101 	tsteq	r1, r1, lsl #2
8780df54:	01010101 	tsteq	r1, r1, lsl #2
8780df58:	01010101 	tsteq	r1, r1, lsl #2
8780df5c:	01010101 	tsteq	r1, r1, lsl #2
8780df60:	01010101 	tsteq	r1, r1, lsl #2
8780df64:	10010101 	andne	r0, r1, r1, lsl #2
8780df68:	01010101 	tsteq	r1, r1, lsl #2
8780df6c:	02010101 	andeq	r0, r1, #1073741824	; 0x40000000
8780df70:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780df74:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780df78:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780df7c:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780df80:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780df84:	10020202 	andne	r0, r2, r2, lsl #4
8780df88:	02020202 	andeq	r0, r2, #536870912	; 0x20000000
8780df8c:	02020202 	andeq	r0, r2, #536870912	; 0x20000000

8780df90 <release.7465>:
8780df90:	地址 0x000000008780df90 越界。


Disassembly of section .bss:

8780df98 <__bss_start__>:
	...

8780e098 <__fiq_stack_base>:
	...

8780e198 <__fiq_stack_limit>:
	...

8780f198 <__abt_stack_base>:
	...

8780f298 <__abt_stack_limit>:
	...

8780f398 <__sys_stack_base>:
	...

8780fb98 <__svc_stack_base>:
	...

87810b98 <k_cpu_cycle_per_tick>:
87810b98:	00000000 	andeq	r0, r0, r0

87810b9c <k_next_task>:
87810b9c:	00000000 	andeq	r0, r0, r0

87810ba0 <k_curr_task>:
	...

87810ba8 <k_tick_count>:
	...

87810bb0 <k_knl_state>:
87810bb0:	00000000 	andeq	r0, r0, r0

87810bb4 <k_sched_lock_nest_cnt>:
	...

87810bb5 <k_irq_nest_cnt>:
87810bb5:	00000000 	andeq	r0, r0, r0

87810bb8 <int_handle_table>:
	...

87810fb8 <gic_data>:
	...

87810fc0 <g_pcOutBuf>:
	...

878113c0 <g_pcInBuf>:
	...

878117c0 <state.9873>:
878117c0:	00000000 	andeq	r0, r0, r0

878117c4 <irqNesting>:
878117c4:	00000000 	andeq	r0, r0, r0

878117c8 <irqTable>:
	...

87811cc8 <state.9824>:
	...

87811cc9 <state.7499>:
87811cc9:	00000000 	andeq	r0, r0, r0
87811ccd:	00000000 	andeq	r0, r0, r0

87811cd0 <task2_stack>:
	...

87811ed0 <task1>:
	...

87811f50 <task2>:
	...

87811fd0 <task1_stack>:
	...

878121d0 <k_rdyq>:
	...

87812228 <k_idle_task_stk>:
	...

87812428 <k_mmheap_ctl>:
	...

878130b0 <k_idle_task>:
	...

87813130 <k_mmheap_default_pool>:
	...

8781b130 <___strtok>:
8781b130:	00000000 	andeq	r0, r0, r0

8781b134 <tftlcd_dev>:
	...

8781b154 <icm20608_dev>:
	...

8781b18c <ft5426_dev>:
	...

8781b1a4 <backlight_dev>:
8781b1a4:	00000000 	andeq	r0, r0, r0

Disassembly of section .ARM.attributes:

00000000 <.ARM.attributes>:
   0:	00003041 	andeq	r3, r0, r1, asr #32
   4:	61656100 	cmnvs	r5, r0, lsl #2
   8:	01006962 	tsteq	r0, r2, ror #18
   c:	00000026 	andeq	r0, r0, r6, lsr #32
  10:	412d3705 	teqmi	sp, r5, lsl #14
  14:	070a0600 	streq	r0, [sl, -r0, lsl #12]
  18:	09010841 	stmdbeq	r1, {r0, r6, fp}
  1c:	12040a02 	andne	r0, r4, #8192	; 0x2000
  20:	15011404 	strne	r1, [r1, #-1028]	; 0x404
  24:	18031701 	stmdane	r3, {r0, r8, r9, sl, ip}
  28:	1b021a01 	blne	86834 <STACK_SIZE_IRQ+0x85834>
  2c:	22011c03 	andcs	r1, r1, #768	; 0x300
  30:	地址 0x0000000000000030 越界。


Disassembly of section .debug_frame:

00000000 <.debug_frame>:
   0:	0000000c 	andeq	r0, r0, ip
   4:	ffffffff 			; <UNDEFINED> instruction: 0xffffffff
   8:	7c010001 	stcvc	0, cr0, [r1], {1}
   c:	000d0c0e 	andeq	r0, sp, lr, lsl #24
  10:	0000000c 	andeq	r0, r0, ip
  14:	00000000 	andeq	r0, r0, r0
  18:	87808f91 			; <UNDEFINED> instruction: 0x87808f91
  1c:	0000025c 	andeq	r0, r0, ip, asr r2
  20:	0000000c 	andeq	r0, r0, ip
  24:	ffffffff 			; <UNDEFINED> instruction: 0xffffffff
  28:	7c020001 	stcvc	0, cr0, [r2], {1}
  2c:	000d0c0e 	andeq	r0, sp, lr, lsl #24
  30:	00000018 	andeq	r0, r0, r8, lsl r0
  34:	00000020 	andeq	r0, r0, r0, lsr #32
  38:	87809208 	strhi	r9, [r0, r8, lsl #4]
  3c:	00000030 	andeq	r0, r0, r0, lsr r0
  40:	8e100e50 	mrchi	14, 0, r0, cr0, cr0, {2}
  44:	0ece4703 	cdpeq	7, 12, cr4, cr14, cr3, {0}
  48:	00000010 	andeq	r0, r0, r0, lsl r0
  4c:	0000000c 	andeq	r0, r0, ip
  50:	ffffffff 			; <UNDEFINED> instruction: 0xffffffff
  54:	7c020001 	stcvc	0, cr0, [r2], {1}
  58:	000d0c0e 	andeq	r0, sp, lr, lsl #24
  5c:	00000020 	andeq	r0, r0, r0, lsr #32
  60:	0000004c 	andeq	r0, r0, ip, asr #32
  64:	87809244 	strhi	r9, [r0, r4, asr #4]
  68:	00000116 	andeq	r0, r0, r6, lsl r1
  6c:	841c0e42 	ldrhi	r0, [ip], #-3650	; 0xe42
  70:	86068507 	strhi	r8, [r6], -r7, lsl #10
  74:	88048705 	stmdahi	r4, {r0, r2, r8, r9, sl, pc}
  78:	8e028903 	cdphi	9, 0, cr8, cr2, cr3, {0}
  7c:	00000001 	andeq	r0, r0, r1

Disassembly of section .debug_line:

00000000 <.debug_line>:
   0:	00000100 	andeq	r0, r0, r0, lsl #2
   4:	00c70002 	sbceq	r0, r7, r2
   8:	01020000 	mrseq	r0, (UNDEF: 2)
   c:	000d0efb 	strdeq	r0, [sp], -fp
  10:	01010101 	tsteq	r1, r1, lsl #2
  14:	01000000 	mrseq	r0, (UNDEF: 0)
  18:	2f010000 	svccs	0x00010000
  1c:	656d6f68 	strbvs	r6, [sp, #-3944]!	; 0xf68
  20:	7763742f 	strbvc	r7, [r3, -pc, lsr #8]!
  24:	75622d67 	strbvc	r2, [r2, #-3431]!	; 0xd67
  28:	73646c69 	cmnvc	r4, #26880	; 0x6900
  2c:	6576616c 	ldrbvs	r6, [r6, #-364]!	; 0x16c
  30:	726f772f 	rsbvc	r7, pc, #12320768	; 0xbc0000
  34:	6170736b 	cmnvs	r0, fp, ror #6
  38:	742f6563 	strtvc	r6, [pc], #-1379	; 40 <BIT_F>
  3c:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
  40:	656b616d 	strbvs	r6, [fp, #-365]!	; 0x16d
  44:	6c65722d 	sfmvs	f7, 2, [r5], #-180	; 0xffffff4c
  48:	65736165 	ldrbvs	r6, [r3, #-357]!	; 0x165
  4c:	62616c2f 	rsbvs	r6, r1, #12032	; 0x2f00
  50:	642f6c65 	strtvs	r6, [pc], #-3173	; 58 <BIT_F+0x18>
  54:	656b636f 	strbvs	r6, [fp, #-879]!	; 0x36f
  58:	72742d72 	rsbsvc	r2, r4, #7296	; 0x1c80
  5c:	79747375 	ldmdbvc	r4!, {r0, r2, r4, r5, r6, r8, r9, ip, sp, lr}^
  60:	646d612d 	strbtvs	r6, [sp], #-301	; 0x12d
  64:	742d3436 	strtvc	r3, [sp], #-1078	; 0x436
  68:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
  6c:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
  70:	61742f64 	cmnvs	r4, r4, ror #30
  74:	74656772 	strbtvc	r6, [r5], #-1906	; 0x772
  78:	6d72612f 	ldfvse	f6, [r2, #-188]!	; 0xffffff44
  7c:	6e696c2d 	cdpvs	12, 6, cr6, cr9, cr13, {1}
  80:	672d7875 			; <UNDEFINED> instruction: 0x672d7875
  84:	6165756e 	cmnvs	r5, lr, ror #10
  88:	66686962 	strbtvs	r6, [r8], -r2, ror #18
  8c:	616e732f 	cmnvs	lr, pc, lsr #6
  90:	6f687370 	svcvs	0x00687370
  94:	672f7374 			; <UNDEFINED> instruction: 0x672f7374
  98:	6c2d6363 	stcvs	3, cr6, [sp], #-396	; 0xfffffe74
  9c:	72616e69 	rsbvc	r6, r1, #1680	; 0x690
  a0:	2e342d6f 	cdpcs	13, 3, cr2, cr4, cr15, {3}
  a4:	30322d39 	eorscc	r2, r2, r9, lsr sp
  a8:	302e3731 	eorcc	r3, lr, r1, lsr r7
  ac:	696c2f31 	stmdbvs	ip!, {r0, r4, r5, r8, r9, sl, fp, sp}^
  b0:	63636762 	cmnvs	r3, #25690112	; 0x1880000
  b4:	6e6f632f 	cdpvs	3, 6, cr6, cr15, cr15, {1}
  b8:	2f676966 	svccs	0x00676966
  bc:	006d7261 	rsbeq	r7, sp, r1, ror #4
  c0:	62696c00 	rsbvs	r6, r9, #0, 24
  c4:	6e756631 	mrcvs	6, 3, r6, cr5, cr1, {1}
  c8:	532e7363 	teqpl	lr, #-1946157055	; 0x8c000001
  cc:	00000100 	andeq	r0, r0, r0, lsl #2
  d0:	02050000 	andeq	r0, r5, #0
  d4:	87808f90 			; <UNDEFINED> instruction: 0x87808f90
  d8:	0107cf03 	tsteq	r7, r3, lsl #30
  dc:	2f212121 	svccs	0x00212121
  e0:	30212f21 	eorcc	r2, r1, r1, lsr #30
  e4:	14029102 	strne	r9, [r2], #-258	; 0x102
  e8:	21212221 	teqcs	r1, r1, lsr #4
  ec:	2f4c2221 	svccs	0x004c2221
  f0:	207aeb03 	rsbscs	lr, sl, r3, lsl #22
  f4:	5805af03 	stmdapl	r5, {r0, r1, r8, r9, sl, fp, sp, pc}
  f8:	2f2f2121 	svccs	0x002f2121
  fc:	022f2f2f 	eoreq	r2, pc, #47, 30	; 0xbc
 100:	01010001 	tsteq	r1, r1
 104:	000000dd 	ldrdeq	r0, [r0], -sp
 108:	00c30002 	sbceq	r0, r3, r2
 10c:	01020000 	mrseq	r0, (UNDEF: 2)
 110:	000d0efb 	strdeq	r0, [sp], -fp
 114:	01010101 	tsteq	r1, r1, lsl #2
 118:	01000000 	mrseq	r0, (UNDEF: 0)
 11c:	2f010000 	svccs	0x00010000
 120:	656d6f68 	strbvs	r6, [sp, #-3944]!	; 0xf68
 124:	7763742f 	strbvc	r7, [r3, -pc, lsr #8]!
 128:	75622d67 	strbvc	r2, [r2, #-3431]!	; 0xd67
 12c:	73646c69 	cmnvc	r4, #26880	; 0x6900
 130:	6576616c 	ldrbvs	r6, [r6, #-364]!	; 0x16c
 134:	726f772f 	rsbvc	r7, pc, #12320768	; 0xbc0000
 138:	6170736b 	cmnvs	r0, fp, ror #6
 13c:	742f6563 	strtvc	r6, [pc], #-1379	; 144 <STACK_SIZE_ABT+0x44>
 140:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 144:	656b616d 	strbvs	r6, [fp, #-365]!	; 0x16d
 148:	6c65722d 	sfmvs	f7, 2, [r5], #-180	; 0xffffff4c
 14c:	65736165 	ldrbvs	r6, [r3, #-357]!	; 0x165
 150:	62616c2f 	rsbvs	r6, r1, #12032	; 0x2f00
 154:	642f6c65 	strtvs	r6, [pc], #-3173	; 15c <STACK_SIZE_ABT+0x5c>
 158:	656b636f 	strbvs	r6, [fp, #-879]!	; 0x36f
 15c:	72742d72 	rsbsvc	r2, r4, #7296	; 0x1c80
 160:	79747375 	ldmdbvc	r4!, {r0, r2, r4, r5, r6, r8, r9, ip, sp, lr}^
 164:	646d612d 	strbtvs	r6, [sp], #-301	; 0x12d
 168:	742d3436 	strtvc	r3, [sp], #-1078	; 0x436
 16c:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 170:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
 174:	61742f64 	cmnvs	r4, r4, ror #30
 178:	74656772 	strbtvc	r6, [r5], #-1906	; 0x772
 17c:	6d72612f 	ldfvse	f6, [r2, #-188]!	; 0xffffff44
 180:	6e696c2d 	cdpvs	12, 6, cr6, cr9, cr13, {1}
 184:	672d7875 			; <UNDEFINED> instruction: 0x672d7875
 188:	6165756e 	cmnvs	r5, lr, ror #10
 18c:	66686962 	strbtvs	r6, [r8], -r2, ror #18
 190:	616e732f 	cmnvs	lr, pc, lsr #6
 194:	6f687370 	svcvs	0x00687370
 198:	672f7374 			; <UNDEFINED> instruction: 0x672f7374
 19c:	6c2d6363 	stcvs	3, cr6, [sp], #-396	; 0xfffffe74
 1a0:	72616e69 	rsbvc	r6, r1, #1680	; 0x690
 1a4:	2e342d6f 	cdpcs	13, 3, cr2, cr4, cr15, {3}
 1a8:	30322d39 	eorscc	r2, r2, r9, lsr sp
 1ac:	302e3731 	eorcc	r3, lr, r1, lsr r7
 1b0:	696c2f31 	stmdbvs	ip!, {r0, r4, r5, r8, r9, sl, fp, sp}^
 1b4:	63636762 	cmnvs	r3, #25690112	; 0x1880000
 1b8:	6e6f632f 	cdpvs	3, 6, cr6, cr15, cr15, {1}
 1bc:	2f676966 	svccs	0x00676966
 1c0:	006d7261 	rsbeq	r7, sp, r1, ror #4
 1c4:	61706200 	cmnvs	r0, r0, lsl #4
 1c8:	532e6962 	teqpl	lr, #1605632	; 0x188000
 1cc:	00000100 	andeq	r0, r0, r0, lsl #2
 1d0:	02050000 	andeq	r0, r5, #0
 1d4:	87809208 	strhi	r9, [r0, r8, lsl #4]
 1d8:	0101fd03 	tsteq	r1, r3, lsl #26
 1dc:	592f4cbc 	stmdbpl	pc!, {r2, r3, r4, r5, r7, sl, fp, lr}	; <UNPREDICTABLE>
 1e0:	01000102 	tsteq	r0, r2, lsl #2
 1e4:	0000e301 	andeq	lr, r0, r1, lsl #6
 1e8:	c7000200 	strgt	r0, [r0, -r0, lsl #4]
 1ec:	02000000 	andeq	r0, r0, #0
 1f0:	0d0efb01 	vstreq	d15, [lr, #-4]
 1f4:	01010100 	mrseq	r0, (UNDEF: 17)
 1f8:	00000001 	andeq	r0, r0, r1
 1fc:	01000001 	tsteq	r0, r1
 200:	6d6f682f 	stclvs	8, cr6, [pc, #-188]!	; 14c <STACK_SIZE_ABT+0x4c>
 204:	63742f65 	cmnvs	r4, #404	; 0x194
 208:	622d6777 	eorvs	r6, sp, #31195136	; 0x1dc0000
 20c:	646c6975 	strbtvs	r6, [ip], #-2421	; 0x975
 210:	76616c73 			; <UNDEFINED> instruction: 0x76616c73
 214:	6f772f65 	svcvs	0x00772f65
 218:	70736b72 	rsbsvc	r6, r3, r2, ror fp
 21c:	2f656361 	svccs	0x00656361
 220:	67776374 			; <UNDEFINED> instruction: 0x67776374
 224:	6b616d2d 	blvs	185b6e0 <STACK_SIZE_IRQ+0x185a6e0>
 228:	65722d65 	ldrbvs	r2, [r2, #-3429]!	; 0xd65
 22c:	7361656c 	cmnvc	r1, #108, 10	; 0x1b000000
 230:	616c2f65 	cmnvs	ip, r5, ror #30
 234:	2f6c6562 	svccs	0x006c6562
 238:	6b636f64 	blvs	18dbfd0 <STACK_SIZE_IRQ+0x18dafd0>
 23c:	742d7265 	strtvc	r7, [sp], #-613	; 0x265
 240:	74737572 	ldrbtvc	r7, [r3], #-1394	; 0x572
 244:	6d612d79 	stclvs	13, cr2, [r1, #-484]!	; 0xfffffe1c
 248:	2d343664 	ldccs	6, cr3, [r4, #-400]!	; 0xfffffe70
 24c:	67776374 			; <UNDEFINED> instruction: 0x67776374
 250:	6975622d 	ldmdbvs	r5!, {r0, r2, r3, r5, r9, sp, lr}^
 254:	742f646c 	strtvc	r6, [pc], #-1132	; 25c <STACK_SIZE_ABT+0x15c>
 258:	65677261 	strbvs	r7, [r7, #-609]!	; 0x261
 25c:	72612f74 	rsbvc	r2, r1, #116, 30	; 0x1d0
 260:	696c2d6d 	stmdbvs	ip!, {r0, r2, r3, r5, r6, r8, sl, fp, sp}^
 264:	2d78756e 	cfldr64cs	mvdx7, [r8, #-440]!	; 0xfffffe48
 268:	65756e67 	ldrbvs	r6, [r5, #-3687]!	; 0xe67
 26c:	68696261 	stmdavs	r9!, {r0, r5, r6, r9, sp, lr}^
 270:	6e732f66 	cdpvs	15, 7, cr2, cr3, cr6, {3}
 274:	68737061 	ldmdavs	r3!, {r0, r5, r6, ip, sp, lr}^
 278:	2f73746f 	svccs	0x0073746f
 27c:	2d636367 	stclcs	3, cr6, [r3, #-412]!	; 0xfffffe64
 280:	616e696c 	cmnvs	lr, ip, ror #18
 284:	342d6f72 	strtcc	r6, [sp], #-3954	; 0xf72
 288:	322d392e 	eorcc	r3, sp, #753664	; 0xb8000
 28c:	2e373130 	mrccs	1, 1, r3, cr7, cr0, {1}
 290:	6c2f3130 	stfvss	f3, [pc], #-192	; 1d8 <STACK_SIZE_ABT+0xd8>
 294:	63676269 	cmnvs	r7, #-1879048186	; 0x90000006
 298:	6f632f63 	svcvs	0x00632f63
 29c:	6769666e 	strbvs	r6, [r9, -lr, ror #12]!
 2a0:	6d72612f 	ldfvse	f6, [r2, #-188]!	; 0xffffff44
 2a4:	696c0000 	stmdbvs	ip!, {}^	; <UNPREDICTABLE>
 2a8:	75663162 	strbvc	r3, [r6, #-354]!	; 0x162
 2ac:	2e73636e 	cdpcs	3, 7, cr6, cr3, cr14, {3}
 2b0:	00010053 	andeq	r0, r1, r3, asr r0
 2b4:	05000000 	streq	r0, [r0, #-0]
 2b8:	80923802 	addshi	r3, r2, r2, lsl #16
 2bc:	0ab00387 	beq	fec010e0 <__bss_end__+0x773e5f38>
 2c0:	032f2101 	teqeq	pc, #1073741824	; 0x40000000
 2c4:	022e77bb 	eoreq	r7, lr, #49020928	; 0x2ec0000
 2c8:	01010001 	tsteq	r1, r1
 2cc:	00000113 	andeq	r0, r0, r3, lsl r1
 2d0:	00c70002 	sbceq	r0, r7, r2
 2d4:	01020000 	mrseq	r0, (UNDEF: 2)
 2d8:	000d0efb 	strdeq	r0, [sp], -fp
 2dc:	01010101 	tsteq	r1, r1, lsl #2
 2e0:	01000000 	mrseq	r0, (UNDEF: 0)
 2e4:	2f010000 	svccs	0x00010000
 2e8:	656d6f68 	strbvs	r6, [sp, #-3944]!	; 0xf68
 2ec:	7763742f 	strbvc	r7, [r3, -pc, lsr #8]!
 2f0:	75622d67 	strbvc	r2, [r2, #-3431]!	; 0xd67
 2f4:	73646c69 	cmnvc	r4, #26880	; 0x6900
 2f8:	6576616c 	ldrbvs	r6, [r6, #-364]!	; 0x16c
 2fc:	726f772f 	rsbvc	r7, pc, #12320768	; 0xbc0000
 300:	6170736b 	cmnvs	r0, fp, ror #6
 304:	742f6563 	strtvc	r6, [pc], #-1379	; 30c <STACK_SIZE_ABT+0x20c>
 308:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 30c:	656b616d 	strbvs	r6, [fp, #-365]!	; 0x16d
 310:	6c65722d 	sfmvs	f7, 2, [r5], #-180	; 0xffffff4c
 314:	65736165 	ldrbvs	r6, [r3, #-357]!	; 0x165
 318:	62616c2f 	rsbvs	r6, r1, #12032	; 0x2f00
 31c:	642f6c65 	strtvs	r6, [pc], #-3173	; 324 <STACK_SIZE_ABT+0x224>
 320:	656b636f 	strbvs	r6, [fp, #-879]!	; 0x36f
 324:	72742d72 	rsbsvc	r2, r4, #7296	; 0x1c80
 328:	79747375 	ldmdbvc	r4!, {r0, r2, r4, r5, r6, r8, r9, ip, sp, lr}^
 32c:	646d612d 	strbtvs	r6, [sp], #-301	; 0x12d
 330:	742d3436 	strtvc	r3, [sp], #-1078	; 0x436
 334:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 338:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
 33c:	61742f64 	cmnvs	r4, r4, ror #30
 340:	74656772 	strbtvc	r6, [r5], #-1906	; 0x772
 344:	6d72612f 	ldfvse	f6, [r2, #-188]!	; 0xffffff44
 348:	6e696c2d 	cdpvs	12, 6, cr6, cr9, cr13, {1}
 34c:	672d7875 			; <UNDEFINED> instruction: 0x672d7875
 350:	6165756e 	cmnvs	r5, lr, ror #10
 354:	66686962 	strbtvs	r6, [r8], -r2, ror #18
 358:	616e732f 	cmnvs	lr, pc, lsr #6
 35c:	6f687370 	svcvs	0x00687370
 360:	672f7374 			; <UNDEFINED> instruction: 0x672f7374
 364:	6c2d6363 	stcvs	3, cr6, [sp], #-396	; 0xfffffe74
 368:	72616e69 	rsbvc	r6, r1, #1680	; 0x690
 36c:	2e342d6f 	cdpcs	13, 3, cr2, cr4, cr15, {3}
 370:	30322d39 	eorscc	r2, r2, r9, lsr sp
 374:	302e3731 	eorcc	r3, lr, r1, lsr r7
 378:	696c2f31 	stmdbvs	ip!, {r0, r4, r5, r8, r9, sl, fp, sp}^
 37c:	63636762 	cmnvs	r3, #25690112	; 0x1880000
 380:	696c0000 	stmdbvs	ip!, {}^	; <UNPREDICTABLE>
 384:	63636762 	cmnvs	r3, #25690112	; 0x1880000
 388:	00632e32 	rsbeq	r2, r3, r2, lsr lr
 38c:	6c000001 	stcvs	0, cr0, [r0], {1}
 390:	63676269 	cmnvs	r7, #-1879048186	; 0x90000006
 394:	682e3263 	stmdavs	lr!, {r0, r1, r5, r6, r9, ip, sp}
 398:	00000100 	andeq	r0, r0, r0, lsl #2
 39c:	02050000 	andeq	r0, r5, #0
 3a0:	87809244 	strhi	r9, [r0, r4, asr #4]
 3a4:	0107b003 	tsteq	r7, r3
 3a8:	200c032e 	andcs	r0, ip, lr, lsr #6
 3ac:	03207403 	teqeq	r0, #50331648	; 0x3000000
 3b0:	74033c0c 	strvc	r3, [r3], #-3084	; 0xc0c
 3b4:	0b03212e 	bleq	c8874 <STACK_SIZE_IRQ+0xc7874>
 3b8:	4c4b223c 	sfmmi	f2, 2, [fp], {60}	; 0x3c
 3bc:	6803f921 	stmdavs	r3, {r0, r5, r8, fp, ip, sp, lr, pc}
 3c0:	3c18033c 	ldccc	3, cr0, [r8], {60}	; 0x3c
 3c4:	81212d23 	teqhi	r1, r3, lsr #26
 3c8:	86523032 			; <UNDEFINED> instruction: 0x86523032
 3cc:	33304d29 	teqcc	r0, #2624	; 0xa40
 3d0:	214921b9 	strhcs	r2, [r9, #-25]	; 0xffffffe7
 3d4:	21320821 	teqcs	r2, r1, lsr #16
 3d8:	2e550330 	mrccs	3, 2, r0, cr5, cr0, {1}
 3dc:	05024b49 	streq	r4, [r2, #-2889]	; 0xb49
 3e0:	地址 0x00000000000003e0 越界。


Disassembly of section .debug_info:

00000000 <.debug_info>:
   0:	000001bb 			; <UNDEFINED> instruction: 0x000001bb
   4:	00000002 	andeq	r0, r0, r2
   8:	01040000 	mrseq	r0, (UNDEF: 4)
   c:	00000000 	andeq	r0, r0, r0
  10:	87808f90 			; <UNDEFINED> instruction: 0x87808f90
  14:	87809206 	strhi	r9, [r0, r6, lsl #4]
  18:	6d6f682f 	stclvs	8, cr6, [pc, #-188]!	; ffffff64 <__bss_end__+0x787e4dbc>
  1c:	63742f65 	cmnvs	r4, #404	; 0x194
  20:	622d6777 	eorvs	r6, sp, #31195136	; 0x1dc0000
  24:	646c6975 	strbtvs	r6, [ip], #-2421	; 0x975
  28:	76616c73 			; <UNDEFINED> instruction: 0x76616c73
  2c:	6f772f65 	svcvs	0x00772f65
  30:	70736b72 	rsbsvc	r6, r3, r2, ror fp
  34:	2f656361 	svccs	0x00656361
  38:	67776374 			; <UNDEFINED> instruction: 0x67776374
  3c:	6b616d2d 	blvs	185b4f8 <STACK_SIZE_IRQ+0x185a4f8>
  40:	65722d65 	ldrbvs	r2, [r2, #-3429]!	; 0xd65
  44:	7361656c 	cmnvc	r1, #108, 10	; 0x1b000000
  48:	616c2f65 	cmnvs	ip, r5, ror #30
  4c:	2f6c6562 	svccs	0x006c6562
  50:	6b636f64 	blvs	18dbde8 <STACK_SIZE_IRQ+0x18dade8>
  54:	742d7265 	strtvc	r7, [sp], #-613	; 0x265
  58:	74737572 	ldrbtvc	r7, [r3], #-1394	; 0x572
  5c:	6d612d79 	stclvs	13, cr2, [r1, #-484]!	; 0xfffffe1c
  60:	2d343664 	ldccs	6, cr3, [r4, #-400]!	; 0xfffffe70
  64:	67776374 			; <UNDEFINED> instruction: 0x67776374
  68:	6975622d 	ldmdbvs	r5!, {r0, r2, r3, r5, r9, sp, lr}^
  6c:	742f646c 	strtvc	r6, [pc], #-1132	; 74 <BIT_F+0x34>
  70:	65677261 	strbvs	r7, [r7, #-609]!	; 0x261
  74:	72612f74 	rsbvc	r2, r1, #116, 30	; 0x1d0
  78:	696c2d6d 	stmdbvs	ip!, {r0, r2, r3, r5, r6, r8, sl, fp, sp}^
  7c:	2d78756e 	cfldr64cs	mvdx7, [r8, #-440]!	; 0xfffffe48
  80:	65756e67 	ldrbvs	r6, [r5, #-3687]!	; 0xe67
  84:	68696261 	stmdavs	r9!, {r0, r5, r6, r9, sp, lr}^
  88:	6e732f66 	cdpvs	15, 7, cr2, cr3, cr6, {3}
  8c:	68737061 	ldmdavs	r3!, {r0, r5, r6, ip, sp, lr}^
  90:	2f73746f 	svccs	0x0073746f
  94:	2d636367 	stclcs	3, cr6, [r3, #-412]!	; 0xfffffe64
  98:	616e696c 	cmnvs	lr, ip, ror #18
  9c:	342d6f72 	strtcc	r6, [sp], #-3954	; 0xf72
  a0:	322d392e 	eorcc	r3, sp, #753664	; 0xb8000
  a4:	2e373130 	mrccs	1, 1, r3, cr7, cr0, {1}
  a8:	6c2f3130 	stfvss	f3, [pc], #-192	; fffffff0 <__bss_end__+0x787e4e48>
  ac:	63676269 	cmnvs	r7, #-1879048186	; 0x90000006
  b0:	6f632f63 	svcvs	0x00632f63
  b4:	6769666e 	strbvs	r6, [r9, -lr, ror #12]!
  b8:	6d72612f 	ldfvse	f6, [r2, #-188]!	; 0xffffff44
  bc:	62696c2f 	rsbvs	r6, r9, #12032	; 0x2f00
  c0:	6e756631 	mrcvs	6, 3, r6, cr5, cr1, {1}
  c4:	532e7363 	teqpl	lr, #-1946157055	; 0x8c000001
  c8:	6f682f00 	svcvs	0x00682f00
  cc:	742f656d 	strtvc	r6, [pc], #-1389	; d4 <BIT_I+0x54>
  d0:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
  d4:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
  d8:	616c7364 	cmnvs	ip, r4, ror #6
  dc:	772f6576 			; <UNDEFINED> instruction: 0x772f6576
  e0:	736b726f 	cmnvc	fp, #-268435450	; 0xf0000006
  e4:	65636170 	strbvs	r6, [r3, #-368]!	; 0x170
  e8:	7763742f 	strbvc	r7, [r3, -pc, lsr #8]!
  ec:	616d2d67 	cmnvs	sp, r7, ror #26
  f0:	722d656b 	eorvc	r6, sp, #448790528	; 0x1ac00000
  f4:	61656c65 	cmnvs	r5, r5, ror #24
  f8:	6c2f6573 	cfstr32vs	mvfx6, [pc], #-460	; ffffff34 <__bss_end__+0x787e4d8c>
  fc:	6c656261 	sfmvs	f6, 2, [r5], #-388	; 0xfffffe7c
 100:	636f642f 	cmnvs	pc, #788529152	; 0x2f000000
 104:	2d72656b 	cfldr64cs	mvdx6, [r2, #-428]!	; 0xfffffe54
 108:	73757274 	cmnvc	r5, #116, 4	; 0x40000007
 10c:	612d7974 	teqvs	sp, r4, ror r9
 110:	3436646d 	ldrtcc	r6, [r6], #-1133	; 0x46d
 114:	7763742d 	strbvc	r7, [r3, -sp, lsr #8]!
 118:	75622d67 	strbvc	r2, [r2, #-3431]!	; 0xd67
 11c:	2f646c69 	svccs	0x00646c69
 120:	67726174 			; <UNDEFINED> instruction: 0x67726174
 124:	612f7465 	teqvs	pc, r5, ror #8
 128:	6c2d6d72 	stcvs	13, cr6, [sp], #-456	; 0xfffffe38
 12c:	78756e69 	ldmdavc	r5!, {r0, r3, r5, r6, r9, sl, fp, sp, lr}^
 130:	756e672d 	strbvc	r6, [lr, #-1837]!	; 0x72d
 134:	69626165 	stmdbvs	r2!, {r0, r2, r5, r6, r8, sp, lr}^
 138:	5f2f6668 	svcpl	0x002f6668
 13c:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
 140:	75622f64 	strbvc	r2, [r2, #-3940]!	; 0xf64
 144:	73646c69 	cmnvc	r4, #26880	; 0x6900
 148:	3638782f 	ldrtcc	r7, [r8], -pc, lsr #16
 14c:	2d34365f 	ldccs	6, cr3, [r4, #-380]!	; 0xfffffe84
 150:	6e6b6e75 	mcrvs	14, 3, r6, cr11, cr5, {3}
 154:	2d6e776f 	stclcs	7, cr7, [lr, #-444]!	; 0xfffffe44
 158:	756e696c 	strbvc	r6, [lr, #-2412]!	; 0x96c
 15c:	6e672d78 	mcrvs	13, 3, r2, cr7, cr8, {3}
 160:	72612f75 	rsbvc	r2, r1, #468	; 0x1d4
 164:	696c2d6d 	stmdbvs	ip!, {r0, r2, r3, r5, r6, r8, sl, fp, sp}^
 168:	2d78756e 	cfldr64cs	mvdx7, [r8, #-440]!	; 0xfffffe48
 16c:	65756e67 	ldrbvs	r6, [r5, #-3687]!	; 0xe67
 170:	68696261 	stmdavs	r9!, {r0, r5, r6, r9, sp, lr}^
 174:	63672f66 	cmnvs	r7, #408	; 0x198
 178:	696c2d63 	stmdbvs	ip!, {r0, r1, r5, r6, r8, sl, fp, sp}^
 17c:	6f72616e 	svcvs	0x0072616e
 180:	392e342d 	stmdbcc	lr!, {r0, r2, r3, r5, sl, ip, sp}
 184:	3130322d 	teqcc	r0, sp, lsr #4
 188:	31302e37 	teqcc	r0, r7, lsr lr
 18c:	6174732d 	cmnvs	r4, sp, lsr #6
 190:	2f326567 	svccs	0x00326567
 194:	2d6d7261 	sfmcs	f7, 2, [sp, #-388]!	; 0xfffffe7c
 198:	756e696c 	strbvc	r6, [lr, #-2412]!	; 0x96c
 19c:	6e672d78 	mcrvs	13, 3, r2, cr7, cr8, {3}
 1a0:	62616575 	rsbvs	r6, r1, #490733568	; 0x1d400000
 1a4:	2f666869 	svccs	0x00666869
 1a8:	6762696c 	strbvs	r6, [r2, -ip, ror #18]!
 1ac:	47006363 	strmi	r6, [r0, -r3, ror #6]
 1b0:	4120554e 	teqmi	r0, lr, asr #10
 1b4:	2e322053 	mrccs	0, 1, r2, cr2, cr3, {2}
 1b8:	302e3432 	eorcc	r3, lr, r2, lsr r4
 1bc:	b7800100 	strlt	r0, [r0, r0, lsl #2]
 1c0:	02000001 	andeq	r0, r0, #1
 1c4:	00001400 	andeq	r1, r0, r0, lsl #8
 1c8:	04010400 	streq	r0, [r1], #-1024	; 0x400
 1cc:	08000001 	stmdaeq	r0, {r0}
 1d0:	38878092 	stmcc	r7, {r1, r4, r7, pc}
 1d4:	2f878092 	svccs	0x00878092
 1d8:	656d6f68 	strbvs	r6, [sp, #-3944]!	; 0xf68
 1dc:	7763742f 	strbvc	r7, [r3, -pc, lsr #8]!
 1e0:	75622d67 	strbvc	r2, [r2, #-3431]!	; 0xd67
 1e4:	73646c69 	cmnvc	r4, #26880	; 0x6900
 1e8:	6576616c 	ldrbvs	r6, [r6, #-364]!	; 0x16c
 1ec:	726f772f 	rsbvc	r7, pc, #12320768	; 0xbc0000
 1f0:	6170736b 	cmnvs	r0, fp, ror #6
 1f4:	742f6563 	strtvc	r6, [pc], #-1379	; 1fc <STACK_SIZE_ABT+0xfc>
 1f8:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 1fc:	656b616d 	strbvs	r6, [fp, #-365]!	; 0x16d
 200:	6c65722d 	sfmvs	f7, 2, [r5], #-180	; 0xffffff4c
 204:	65736165 	ldrbvs	r6, [r3, #-357]!	; 0x165
 208:	62616c2f 	rsbvs	r6, r1, #12032	; 0x2f00
 20c:	642f6c65 	strtvs	r6, [pc], #-3173	; 214 <STACK_SIZE_ABT+0x114>
 210:	656b636f 	strbvs	r6, [fp, #-879]!	; 0x36f
 214:	72742d72 	rsbsvc	r2, r4, #7296	; 0x1c80
 218:	79747375 	ldmdbvc	r4!, {r0, r2, r4, r5, r6, r8, r9, ip, sp, lr}^
 21c:	646d612d 	strbtvs	r6, [sp], #-301	; 0x12d
 220:	742d3436 	strtvc	r3, [sp], #-1078	; 0x436
 224:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 228:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
 22c:	61742f64 	cmnvs	r4, r4, ror #30
 230:	74656772 	strbtvc	r6, [r5], #-1906	; 0x772
 234:	6d72612f 	ldfvse	f6, [r2, #-188]!	; 0xffffff44
 238:	6e696c2d 	cdpvs	12, 6, cr6, cr9, cr13, {1}
 23c:	672d7875 			; <UNDEFINED> instruction: 0x672d7875
 240:	6165756e 	cmnvs	r5, lr, ror #10
 244:	66686962 	strbtvs	r6, [r8], -r2, ror #18
 248:	616e732f 	cmnvs	lr, pc, lsr #6
 24c:	6f687370 	svcvs	0x00687370
 250:	672f7374 			; <UNDEFINED> instruction: 0x672f7374
 254:	6c2d6363 	stcvs	3, cr6, [sp], #-396	; 0xfffffe74
 258:	72616e69 	rsbvc	r6, r1, #1680	; 0x690
 25c:	2e342d6f 	cdpcs	13, 3, cr2, cr4, cr15, {3}
 260:	30322d39 	eorscc	r2, r2, r9, lsr sp
 264:	302e3731 	eorcc	r3, lr, r1, lsr r7
 268:	696c2f31 	stmdbvs	ip!, {r0, r4, r5, r8, r9, sl, fp, sp}^
 26c:	63636762 	cmnvs	r3, #25690112	; 0x1880000
 270:	6e6f632f 	cdpvs	3, 6, cr6, cr15, cr15, {1}
 274:	2f676966 	svccs	0x00676966
 278:	2f6d7261 	svccs	0x006d7261
 27c:	62617062 	rsbvs	r7, r1, #98	; 0x62
 280:	00532e69 	subseq	r2, r3, r9, ror #28
 284:	6d6f682f 	stclvs	8, cr6, [pc, #-188]!	; 1d0 <STACK_SIZE_ABT+0xd0>
 288:	63742f65 	cmnvs	r4, #404	; 0x194
 28c:	622d6777 	eorvs	r6, sp, #31195136	; 0x1dc0000
 290:	646c6975 	strbtvs	r6, [ip], #-2421	; 0x975
 294:	76616c73 			; <UNDEFINED> instruction: 0x76616c73
 298:	6f772f65 	svcvs	0x00772f65
 29c:	70736b72 	rsbsvc	r6, r3, r2, ror fp
 2a0:	2f656361 	svccs	0x00656361
 2a4:	67776374 			; <UNDEFINED> instruction: 0x67776374
 2a8:	6b616d2d 	blvs	185b764 <STACK_SIZE_IRQ+0x185a764>
 2ac:	65722d65 	ldrbvs	r2, [r2, #-3429]!	; 0xd65
 2b0:	7361656c 	cmnvc	r1, #108, 10	; 0x1b000000
 2b4:	616c2f65 	cmnvs	ip, r5, ror #30
 2b8:	2f6c6562 	svccs	0x006c6562
 2bc:	6b636f64 	blvs	18dc054 <STACK_SIZE_IRQ+0x18db054>
 2c0:	742d7265 	strtvc	r7, [sp], #-613	; 0x265
 2c4:	74737572 	ldrbtvc	r7, [r3], #-1394	; 0x572
 2c8:	6d612d79 	stclvs	13, cr2, [r1, #-484]!	; 0xfffffe1c
 2cc:	2d343664 	ldccs	6, cr3, [r4, #-400]!	; 0xfffffe70
 2d0:	67776374 			; <UNDEFINED> instruction: 0x67776374
 2d4:	6975622d 	ldmdbvs	r5!, {r0, r2, r3, r5, r9, sp, lr}^
 2d8:	742f646c 	strtvc	r6, [pc], #-1132	; 2e0 <STACK_SIZE_ABT+0x1e0>
 2dc:	65677261 	strbvs	r7, [r7, #-609]!	; 0x261
 2e0:	72612f74 	rsbvc	r2, r1, #116, 30	; 0x1d0
 2e4:	696c2d6d 	stmdbvs	ip!, {r0, r2, r3, r5, r6, r8, sl, fp, sp}^
 2e8:	2d78756e 	cfldr64cs	mvdx7, [r8, #-440]!	; 0xfffffe48
 2ec:	65756e67 	ldrbvs	r6, [r5, #-3687]!	; 0xe67
 2f0:	68696261 	stmdavs	r9!, {r0, r5, r6, r9, sp, lr}^
 2f4:	625f2f66 	subsvs	r2, pc, #408	; 0x198
 2f8:	646c6975 	strbtvs	r6, [ip], #-2421	; 0x975
 2fc:	6975622f 	ldmdbvs	r5!, {r0, r1, r2, r3, r5, r9, sp, lr}^
 300:	2f73646c 	svccs	0x0073646c
 304:	5f363878 	svcpl	0x00363878
 308:	752d3436 	strvc	r3, [sp, #-1078]!	; 0x436
 30c:	6f6e6b6e 	svcvs	0x006e6b6e
 310:	6c2d6e77 	stcvs	14, cr6, [sp], #-476	; 0xfffffe24
 314:	78756e69 	ldmdavc	r5!, {r0, r3, r5, r6, r9, sl, fp, sp, lr}^
 318:	756e672d 	strbvc	r6, [lr, #-1837]!	; 0x72d
 31c:	6d72612f 	ldfvse	f6, [r2, #-188]!	; 0xffffff44
 320:	6e696c2d 	cdpvs	12, 6, cr6, cr9, cr13, {1}
 324:	672d7875 			; <UNDEFINED> instruction: 0x672d7875
 328:	6165756e 	cmnvs	r5, lr, ror #10
 32c:	66686962 	strbtvs	r6, [r8], -r2, ror #18
 330:	6363672f 	cmnvs	r3, #12320768	; 0xbc0000
 334:	6e696c2d 	cdpvs	12, 6, cr6, cr9, cr13, {1}
 338:	2d6f7261 	sfmcs	f7, 2, [pc, #-388]!	; 1bc <STACK_SIZE_ABT+0xbc>
 33c:	2d392e34 	ldccs	14, cr2, [r9, #-208]!	; 0xffffff30
 340:	37313032 			; <UNDEFINED> instruction: 0x37313032
 344:	2d31302e 	ldccs	0, cr3, [r1, #-184]!	; 0xffffff48
 348:	67617473 			; <UNDEFINED> instruction: 0x67617473
 34c:	612f3265 	teqvs	pc, r5, ror #4
 350:	6c2d6d72 	stcvs	13, cr6, [sp], #-456	; 0xfffffe38
 354:	78756e69 	ldmdavc	r5!, {r0, r3, r5, r6, r9, sl, fp, sp, lr}^
 358:	756e672d 	strbvc	r6, [lr, #-1837]!	; 0x72d
 35c:	69626165 	stmdbvs	r2!, {r0, r2, r5, r6, r8, sp, lr}^
 360:	6c2f6668 	stcvs	6, cr6, [pc], #-416	; 1c8 <STACK_SIZE_ABT+0xc8>
 364:	63676269 	cmnvs	r7, #-1879048186	; 0x90000006
 368:	4e470063 	cdpmi	0, 4, cr0, cr7, cr3, {3}
 36c:	53412055 	movtpl	r2, #4181	; 0x1055
 370:	322e3220 	eorcc	r3, lr, #32, 4
 374:	00302e34 	eorseq	r2, r0, r4, lsr lr
 378:	01bb8001 			; <UNDEFINED> instruction: 0x01bb8001
 37c:	00020000 	andeq	r0, r2, r0
 380:	00000028 	andeq	r0, r0, r8, lsr #32
 384:	01e50104 	mvneq	r0, r4, lsl #2
 388:	92380000 	eorsls	r0, r8, #0
 38c:	92448780 	subls	r8, r4, #128, 14	; 0x2000000
 390:	682f8780 	stmdavs	pc!, {r7, r8, r9, sl, pc}	; <UNPREDICTABLE>
 394:	2f656d6f 	svccs	0x00656d6f
 398:	67776374 			; <UNDEFINED> instruction: 0x67776374
 39c:	6975622d 	ldmdbvs	r5!, {r0, r2, r3, r5, r9, sp, lr}^
 3a0:	6c73646c 	cfldrdvs	mvd6, [r3], #-432	; 0xfffffe50
 3a4:	2f657661 	svccs	0x00657661
 3a8:	6b726f77 	blvs	1c9c18c <STACK_SIZE_IRQ+0x1c9b18c>
 3ac:	63617073 	cmnvs	r1, #115	; 0x73
 3b0:	63742f65 	cmnvs	r4, #404	; 0x194
 3b4:	6d2d6777 	stcvs	7, cr6, [sp, #-476]!	; 0xfffffe24
 3b8:	2d656b61 	fstmdbxcs	r5!, {d22-d69}	;@ Deprecated
 3bc:	656c6572 	strbvs	r6, [ip, #-1394]!	; 0x572
 3c0:	2f657361 	svccs	0x00657361
 3c4:	6562616c 	strbvs	r6, [r2, #-364]!	; 0x16c
 3c8:	6f642f6c 	svcvs	0x00642f6c
 3cc:	72656b63 	rsbvc	r6, r5, #101376	; 0x18c00
 3d0:	7572742d 	ldrbvc	r7, [r2, #-1069]!	; 0x42d
 3d4:	2d797473 	cfldrdcs	mvd7, [r9, #-460]!	; 0xfffffe34
 3d8:	36646d61 	strbtcc	r6, [r4], -r1, ror #26
 3dc:	63742d34 	cmnvs	r4, #52, 26	; 0xd00
 3e0:	622d6777 	eorvs	r6, sp, #31195136	; 0x1dc0000
 3e4:	646c6975 	strbtvs	r6, [ip], #-2421	; 0x975
 3e8:	7261742f 	rsbvc	r7, r1, #788529152	; 0x2f000000
 3ec:	2f746567 	svccs	0x00746567
 3f0:	2d6d7261 	sfmcs	f7, 2, [sp, #-388]!	; 0xfffffe7c
 3f4:	756e696c 	strbvc	r6, [lr, #-2412]!	; 0x96c
 3f8:	6e672d78 	mcrvs	13, 3, r2, cr7, cr8, {3}
 3fc:	62616575 	rsbvs	r6, r1, #490733568	; 0x1d400000
 400:	2f666869 	svccs	0x00666869
 404:	70616e73 	rsbvc	r6, r1, r3, ror lr
 408:	746f6873 	strbtvc	r6, [pc], #-2163	; 410 <STACK_SIZE_ABT+0x310>
 40c:	63672f73 	cmnvs	r7, #460	; 0x1cc
 410:	696c2d63 	stmdbvs	ip!, {r0, r1, r5, r6, r8, sl, fp, sp}^
 414:	6f72616e 	svcvs	0x0072616e
 418:	392e342d 	stmdbcc	lr!, {r0, r2, r3, r5, sl, ip, sp}
 41c:	3130322d 	teqcc	r0, sp, lsr #4
 420:	31302e37 	teqcc	r0, r7, lsr lr
 424:	62696c2f 	rsbvs	r6, r9, #12032	; 0x2f00
 428:	2f636367 	svccs	0x00636367
 42c:	666e6f63 	strbtvs	r6, [lr], -r3, ror #30
 430:	612f6769 	teqvs	pc, r9, ror #14
 434:	6c2f6d72 	stcvs	13, cr6, [pc], #-456	; 274 <STACK_SIZE_ABT+0x174>
 438:	66316269 	ldrtvs	r6, [r1], -r9, ror #4
 43c:	73636e75 	cmnvc	r3, #1872	; 0x750
 440:	2f00532e 	svccs	0x0000532e
 444:	656d6f68 	strbvs	r6, [sp, #-3944]!	; 0xf68
 448:	7763742f 	strbvc	r7, [r3, -pc, lsr #8]!
 44c:	75622d67 	strbvc	r2, [r2, #-3431]!	; 0xd67
 450:	73646c69 	cmnvc	r4, #26880	; 0x6900
 454:	6576616c 	ldrbvs	r6, [r6, #-364]!	; 0x16c
 458:	726f772f 	rsbvc	r7, pc, #12320768	; 0xbc0000
 45c:	6170736b 	cmnvs	r0, fp, ror #6
 460:	742f6563 	strtvc	r6, [pc], #-1379	; 468 <STACK_SIZE_ABT+0x368>
 464:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 468:	656b616d 	strbvs	r6, [fp, #-365]!	; 0x16d
 46c:	6c65722d 	sfmvs	f7, 2, [r5], #-180	; 0xffffff4c
 470:	65736165 	ldrbvs	r6, [r3, #-357]!	; 0x165
 474:	62616c2f 	rsbvs	r6, r1, #12032	; 0x2f00
 478:	642f6c65 	strtvs	r6, [pc], #-3173	; 480 <STACK_SIZE_ABT+0x380>
 47c:	656b636f 	strbvs	r6, [fp, #-879]!	; 0x36f
 480:	72742d72 	rsbsvc	r2, r4, #7296	; 0x1c80
 484:	79747375 	ldmdbvc	r4!, {r0, r2, r4, r5, r6, r8, r9, ip, sp, lr}^
 488:	646d612d 	strbtvs	r6, [sp], #-301	; 0x12d
 48c:	742d3436 	strtvc	r3, [sp], #-1078	; 0x436
 490:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 494:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
 498:	61742f64 	cmnvs	r4, r4, ror #30
 49c:	74656772 	strbtvc	r6, [r5], #-1906	; 0x772
 4a0:	6d72612f 	ldfvse	f6, [r2, #-188]!	; 0xffffff44
 4a4:	6e696c2d 	cdpvs	12, 6, cr6, cr9, cr13, {1}
 4a8:	672d7875 			; <UNDEFINED> instruction: 0x672d7875
 4ac:	6165756e 	cmnvs	r5, lr, ror #10
 4b0:	66686962 	strbtvs	r6, [r8], -r2, ror #18
 4b4:	75625f2f 	strbvc	r5, [r2, #-3887]!	; 0xf2f
 4b8:	2f646c69 	svccs	0x00646c69
 4bc:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
 4c0:	782f7364 	stmdavc	pc!, {r2, r5, r6, r8, r9, ip, sp, lr}	; <UNPREDICTABLE>
 4c4:	365f3638 			; <UNDEFINED> instruction: 0x365f3638
 4c8:	6e752d34 	mrcvs	13, 3, r2, cr5, cr4, {1}
 4cc:	776f6e6b 	strbvc	r6, [pc, -fp, ror #28]!
 4d0:	696c2d6e 	stmdbvs	ip!, {r1, r2, r3, r5, r6, r8, sl, fp, sp}^
 4d4:	2d78756e 	cfldr64cs	mvdx7, [r8, #-440]!	; 0xfffffe48
 4d8:	2f756e67 	svccs	0x00756e67
 4dc:	2d6d7261 	sfmcs	f7, 2, [sp, #-388]!	; 0xfffffe7c
 4e0:	756e696c 	strbvc	r6, [lr, #-2412]!	; 0x96c
 4e4:	6e672d78 	mcrvs	13, 3, r2, cr7, cr8, {3}
 4e8:	62616575 	rsbvs	r6, r1, #490733568	; 0x1d400000
 4ec:	2f666869 	svccs	0x00666869
 4f0:	2d636367 	stclcs	3, cr6, [r3, #-412]!	; 0xfffffe64
 4f4:	616e696c 	cmnvs	lr, ip, ror #18
 4f8:	342d6f72 	strtcc	r6, [sp], #-3954	; 0xf72
 4fc:	322d392e 	eorcc	r3, sp, #753664	; 0xb8000
 500:	2e373130 	mrccs	1, 1, r3, cr7, cr0, {1}
 504:	732d3130 	teqvc	sp, #48, 2
 508:	65676174 	strbvs	r6, [r7, #-372]!	; 0x174
 50c:	72612f32 	rsbvc	r2, r1, #50, 30	; 0xc8
 510:	696c2d6d 	stmdbvs	ip!, {r0, r2, r3, r5, r6, r8, sl, fp, sp}^
 514:	2d78756e 	cfldr64cs	mvdx7, [r8, #-440]!	; 0xfffffe48
 518:	65756e67 	ldrbvs	r6, [r5, #-3687]!	; 0xe67
 51c:	68696261 	stmdavs	r9!, {r0, r5, r6, r9, sp, lr}^
 520:	696c2f66 	stmdbvs	ip!, {r1, r2, r5, r6, r8, r9, sl, fp, sp}^
 524:	63636762 	cmnvs	r3, #25690112	; 0x1880000
 528:	554e4700 	strbpl	r4, [lr, #-1792]	; 0x700
 52c:	20534120 	subscs	r4, r3, r0, lsr #2
 530:	34322e32 	ldrtcc	r2, [r2], #-3634	; 0xe32
 534:	0100302e 	tsteq	r0, lr, lsr #32
 538:	00015a80 	andeq	r5, r1, r0, lsl #21
 53c:	3c000400 	cfstrscc	mvf0, [r0], {-0}
 540:	04000000 	streq	r0, [r0], #-0
 544:	00012b01 	andeq	r2, r1, r1, lsl #22
 548:	024f0100 	subeq	r0, pc, #0, 2
 54c:	000d0000 	andeq	r0, sp, r0
 550:	92440000 	subls	r0, r4, #0
 554:	01168780 	tsteq	r6, r0, lsl #15
 558:	02cc0000 	sbceq	r0, ip, #0
 55c:	08020000 	stmdaeq	r2, {}	; <UNPREDICTABLE>
 560:	0000f307 	andeq	pc, r0, r7, lsl #6
 564:	05040300 	streq	r0, [r4, #-768]	; 0x300
 568:	00746e69 	rsbseq	r6, r4, r9, ror #28
 56c:	fd070402 	stc2	4, cr0, [r7, #-8]
 570:	02000000 	andeq	r0, r0, #0
 574:	010a0801 	tsteq	sl, r1, lsl #16
 578:	02020000 	andeq	r0, r2, #0
 57c:	00011807 	andeq	r1, r1, r7, lsl #16
 580:	07040200 	streq	r0, [r4, -r0, lsl #4]
 584:	000000f8 	strdeq	r0, [r0], -r8
 588:	0c060102 	stfeqs	f0, [r6], {2}
 58c:	02000001 	andeq	r0, r0, #1
 590:	02360502 	eorseq	r0, r6, #8388608	; 0x800000
 594:	08020000 	stmdaeq	r2, {}	; <UNPREDICTABLE>
 598:	00022005 	andeq	r2, r2, r5
 59c:	05040200 	streq	r0, [r4, #-512]	; 0x200
 5a0:	00000225 	andeq	r0, r0, r5, lsr #4
 5a4:	17070402 	strne	r0, [r7, -r2, lsl #8]
 5a8:	02000002 	andeq	r0, r0, #2
 5ac:	01130801 	tsteq	r3, r1, lsl #16
 5b0:	2e040000 	cdpcs	0, 0, cr0, cr4, cr0, {0}
 5b4:	02000002 	andeq	r0, r0, #2
 5b8:	00003382 	andeq	r3, r0, r2, lsl #7
 5bc:	02010400 	andeq	r0, r1, #0, 8
 5c0:	86020000 	strhi	r0, [r2], -r0
 5c4:	00000025 	andeq	r0, r0, r5, lsr #32
 5c8:	11040402 	tstne	r4, r2, lsl #8
 5cc:	02000002 	andeq	r0, r0, #2
 5d0:	02090308 	andeq	r0, r9, #8, 6	; 0x20000000
 5d4:	08020000 	stmdaeq	r2, {}	; <UNPREDICTABLE>
 5d8:	00024804 	andeq	r4, r2, r4, lsl #16
 5dc:	03100200 	tsteq	r0, #0, 4
 5e0:	00000240 	andeq	r0, r0, r0, asr #4
 5e4:	00000005 	andeq	r0, r0, r5
 5e8:	03b00100 	movseq	r0, #0, 2
 5ec:	00000084 	andeq	r0, r0, r4, lsl #1
 5f0:	87809244 	strhi	r9, [r0, r4, asr #4]
 5f4:	00000116 	andeq	r0, r0, r6, lsl r1
 5f8:	01579c01 	cmpeq	r7, r1, lsl #24
 5fc:	6e060000 	cdpvs	0, 0, cr0, cr6, cr0, {0}
 600:	03b00100 	movseq	r0, #0, 2
 604:	00000084 	andeq	r0, r0, r4, lsl #1
 608:	00000000 	andeq	r0, r0, r0
 60c:	01006406 	tsteq	r0, r6, lsl #8
 610:	008403b0 			; <UNDEFINED> instruction: 0x008403b0
 614:	00280000 	eoreq	r0, r8, r0
 618:	72060000 	andvc	r0, r6, #0
 61c:	b0010070 	andlt	r0, r1, r0, ror r0
 620:	00015703 	andeq	r5, r1, r3, lsl #14
 624:	00005000 	andeq	r5, r0, r0
 628:	00710700 	rsbseq	r0, r1, r0, lsl #14
 62c:	8403b201 	strhi	fp, [r3], #-513	; 0x201
 630:	70000000 	andvc	r0, r0, r0
 634:	07000000 	streq	r0, [r0, -r0]
 638:	b2010072 	andlt	r0, r1, #114	; 0x72
 63c:	00008403 	andeq	r8, r0, r3, lsl #8
 640:	0000d000 	andeq	sp, r0, r0
 644:	00790700 	rsbseq	r0, r9, r0, lsl #14
 648:	8403b201 	strhi	fp, [r3], #-513	; 0x201
 64c:	28000000 	stmdacs	r0, {}	; <UNPREDICTABLE>
 650:	07000001 	streq	r0, [r0, -r1]
 654:	00317a6c 	eorseq	r7, r1, ip, ror #20
 658:	7903b301 	stmdbvc	r3, {r0, r8, r9, ip, sp, pc}
 65c:	50000000 	andpl	r0, r0, r0
 660:	07000001 	streq	r0, [r0, -r1]
 664:	00327a6c 	eorseq	r7, r2, ip, ror #20
 668:	7903b301 	stmdbvc	r3, {r0, r8, r9, ip, sp, pc}
 66c:	63000000 	movwvs	r0, #0
 670:	07000001 	streq	r0, [r0, -r1]
 674:	b3010069 	movwlt	r0, #4201	; 0x1069
 678:	00007903 	andeq	r7, r0, r3, lsl #18
 67c:	00017600 	andeq	r7, r1, r0, lsl #12
 680:	006b0700 	rsbeq	r0, fp, r0, lsl #14
 684:	7903b301 	stmdbvc	r3, {r0, r8, r9, ip, sp, pc}
 688:	b9000000 	stmdblt	r0, {}	; <UNPREDICTABLE>
 68c:	00000001 	andeq	r0, r0, r1
 690:	00840408 	addeq	r0, r4, r8, lsl #8
 694:	地址 0x0000000000000694 越界。


Disassembly of section .debug_abbrev:

00000000 <.debug_abbrev>:
   0:	10001101 	andne	r1, r0, r1, lsl #2
   4:	12011106 	andne	r1, r1, #-2147483647	; 0x80000001
   8:	1b080301 	blne	200c14 <STACK_SIZE_IRQ+0x1ffc14>
   c:	13082508 	movwne	r2, #34056	; 0x8508
  10:	00000005 	andeq	r0, r0, r5
  14:	10001101 	andne	r1, r0, r1, lsl #2
  18:	12011106 	andne	r1, r1, #-2147483647	; 0x80000001
  1c:	1b080301 	blne	200c28 <STACK_SIZE_IRQ+0x1ffc28>
  20:	13082508 	movwne	r2, #34056	; 0x8508
  24:	00000005 	andeq	r0, r0, r5
  28:	10001101 	andne	r1, r0, r1, lsl #2
  2c:	12011106 	andne	r1, r1, #-2147483647	; 0x80000001
  30:	1b080301 	blne	200c3c <STACK_SIZE_IRQ+0x1ffc3c>
  34:	13082508 	movwne	r2, #34056	; 0x8508
  38:	00000005 	andeq	r0, r0, r5
  3c:	25011101 	strcs	r1, [r1, #-257]	; 0x101
  40:	030b130e 	movweq	r1, #45838	; 0xb30e
  44:	110e1b0e 	tstne	lr, lr, lsl #22
  48:	10061201 	andne	r1, r6, r1, lsl #4
  4c:	02000017 	andeq	r0, r0, #23
  50:	0b0b0024 	bleq	2c00e8 <STACK_SIZE_IRQ+0x2bf0e8>
  54:	0e030b3e 	vmoveq.16	d3[0], r0
  58:	24030000 	strcs	r0, [r3], #-0
  5c:	3e0b0b00 	vmlacc.f64	d0, d11, d0
  60:	0008030b 	andeq	r0, r8, fp, lsl #6
  64:	00160400 	andseq	r0, r6, r0, lsl #8
  68:	0b3a0e03 	bleq	e8387c <STACK_SIZE_IRQ+0xe8287c>
  6c:	13490b3b 	movtne	r0, #39739	; 0x9b3b
  70:	2e050000 	cdpcs	0, 0, cr0, cr5, cr0, {0}
  74:	03193f01 	tsteq	r9, #1, 30
  78:	3b0b3a0e 	blcc	2ce8b8 <STACK_SIZE_IRQ+0x2cd8b8>
  7c:	49192705 	ldmdbmi	r9, {r0, r2, r8, r9, sl, sp}
  80:	12011113 	andne	r1, r1, #-1073741820	; 0xc0000004
  84:	97184006 	ldrls	r4, [r8, -r6]
  88:	13011942 	movwne	r1, #6466	; 0x1942
  8c:	05060000 	streq	r0, [r6, #-0]
  90:	3a080300 	bcc	200c98 <STACK_SIZE_IRQ+0x1ffc98>
  94:	49053b0b 	stmdbmi	r5, {r0, r1, r3, r8, r9, fp, ip, sp}
  98:	00170213 	andseq	r0, r7, r3, lsl r2
  9c:	00340700 	eorseq	r0, r4, r0, lsl #14
  a0:	0b3a0803 	bleq	e820b4 <STACK_SIZE_IRQ+0xe810b4>
  a4:	1349053b 	movtne	r0, #38203	; 0x953b
  a8:	00001702 	andeq	r1, r0, r2, lsl #14
  ac:	0b000f08 	bleq	3cd4 <STACK_SIZE_IRQ+0x2cd4>
  b0:	0013490b 	andseq	r4, r3, fp, lsl #18
	...

Disassembly of section .debug_aranges:

00000000 <.debug_aranges>:
   0:	0000001c 	andeq	r0, r0, ip, lsl r0
   4:	00000002 	andeq	r0, r0, r2
   8:	00040000 	andeq	r0, r4, r0
   c:	00000000 	andeq	r0, r0, r0
  10:	87808f90 			; <UNDEFINED> instruction: 0x87808f90
  14:	00000276 	andeq	r0, r0, r6, ror r2
	...
  20:	0000001c 	andeq	r0, r0, ip, lsl r0
  24:	01bf0002 			; <UNDEFINED> instruction: 0x01bf0002
  28:	00040000 	andeq	r0, r4, r0
  2c:	00000000 	andeq	r0, r0, r0
  30:	87809208 	strhi	r9, [r0, r8, lsl #4]
  34:	00000030 	andeq	r0, r0, r0, lsr r0
	...
  40:	0000001c 	andeq	r0, r0, ip, lsl r0
  44:	037a0002 	cmneq	sl, #2
  48:	00040000 	andeq	r0, r4, r0
  4c:	00000000 	andeq	r0, r0, r0
  50:	87809238 			; <UNDEFINED> instruction: 0x87809238
  54:	0000000c 	andeq	r0, r0, ip
	...
  60:	0000001c 	andeq	r0, r0, ip, lsl r0
  64:	05390002 	ldreq	r0, [r9, #-2]!
  68:	00040000 	andeq	r0, r4, r0
  6c:	00000000 	andeq	r0, r0, r0
  70:	87809244 	strhi	r9, [r0, r4, asr #4]
  74:	00000116 	andeq	r0, r0, r6, lsl r1
	...

Disassembly of section .debug_loc:

00000000 <.debug_loc>:
   0:	00000000 	andeq	r0, r0, r0
   4:	0000000c 	andeq	r0, r0, ip
   8:	93500006 	cmpls	r0, #6
   c:	04935104 	ldreq	r5, [r3], #260	; 0x104
  10:	0000000c 	andeq	r0, r0, ip
  14:	00000116 	andeq	r0, r0, r6, lsl r1
  18:	03f30006 	mvnseq	r0, #6
  1c:	9f2500f5 	svcls	0x002500f5
	...
  2c:	00000014 	andeq	r0, r0, r4, lsl r0
  30:	93520006 	cmpls	r2, #6
  34:	04935304 	ldreq	r5, [r3], #772	; 0x304
  38:	00000014 	andeq	r0, r0, r4, lsl r0
  3c:	00000116 	andeq	r0, r0, r6, lsl r1
  40:	03f30006 	mvnseq	r0, #6
  44:	9f2502f5 	svcls	0x002502f5
	...
  54:	000000fc 	strdeq	r0, [r0], -ip
  58:	00910002 	addseq	r0, r1, r2
  5c:	000000fc 	strdeq	r0, [r0], -ip
  60:	00000116 	andeq	r0, r0, r6, lsl r1
  64:	00910002 	addseq	r0, r1, r2
	...
  74:	00000076 	andeq	r0, r0, r6, ror r0
  78:	089e000a 	ldmeq	lr, {r1, r3}
	...
  84:	00000076 	andeq	r0, r0, r6, ror r0
  88:	000000c0 	andeq	r0, r0, r0, asr #1
  8c:	93500006 	cmpls	r0, #6
  90:	04935104 	ldreq	r5, [r3], #260	; 0x104
  94:	000000ca 	andeq	r0, r0, sl, asr #1
  98:	000000ea 	andeq	r0, r0, sl, ror #1
  9c:	93500006 	cmpls	r0, #6
  a0:	04935104 	ldreq	r5, [r3], #260	; 0x104
  a4:	000000f2 	strdeq	r0, [r0], -r2
  a8:	000000fc 	strdeq	r0, [r0], -ip
  ac:	93500006 	cmpls	r0, #6
  b0:	04935104 	ldreq	r5, [r3], #260	; 0x104
  b4:	000000fc 	strdeq	r0, [r0], -ip
  b8:	00000116 	andeq	r0, r0, r6, lsl r1
  bc:	089e000a 	ldmeq	lr, {r1, r3}
	...
  d0:	0000000a 	andeq	r0, r0, sl
  d4:	00000062 	andeq	r0, r0, r2, rrx
  d8:	93540006 	cmpls	r4, #6
  dc:	04935504 	ldreq	r5, [r3], #1284	; 0x504
  e0:	00000076 	andeq	r0, r0, r6, ror r0
  e4:	00000084 	andeq	r0, r0, r4, lsl #1
  e8:	93540006 	cmpls	r4, #6
  ec:	04935504 	ldreq	r5, [r3], #1284	; 0x504
  f0:	00000092 	muleq	r0, r2, r0
  f4:	000000a0 	andeq	r0, r0, r0, lsr #1
  f8:	93540006 	cmpls	r4, #6
  fc:	04935504 	ldreq	r5, [r3], #1284	; 0x504
 100:	000000a2 	andeq	r0, r0, r2, lsr #1
 104:	000000c8 	andeq	r0, r0, r8, asr #1
 108:	93540006 	cmpls	r4, #6
 10c:	04935504 	ldreq	r5, [r3], #1284	; 0x504
 110:	000000cc 	andeq	r0, r0, ip, asr #1
 114:	00000116 	andeq	r0, r0, r6, lsl r1
 118:	93540006 	cmpls	r4, #6
 11c:	04935504 	ldreq	r5, [r3], #1284	; 0x504
	...
 128:	0000000e 	andeq	r0, r0, lr
 12c:	00000018 	andeq	r0, r0, r8, lsl r0
 130:	93500006 	cmpls	r0, #6
 134:	04935104 	ldreq	r5, [r3], #260	; 0x104
 138:	0000004e 	andeq	r0, r0, lr, asr #32
 13c:	000000ae 	andeq	r0, r0, lr, lsr #1
 140:	93560006 	cmpls	r6, #6
 144:	04935704 	ldreq	r5, [r3], #1796	; 0x704
	...
 150:	0000010c 	andeq	r0, r0, ip, lsl #2
 154:	00000116 	andeq	r0, r0, r6, lsl r1
 158:	00560001 	subseq	r0, r6, r1
 15c:	00000000 	andeq	r0, r0, r0
 160:	2c000000 	stccs	0, cr0, [r0], {-0}
 164:	2e000000 	cdpcs	0, 0, cr0, cr0, cr0, {0}
 168:	01000000 	mrseq	r0, (UNDEF: 0)
 16c:	00005300 	andeq	r5, r0, r0, lsl #6
 170:	00000000 	andeq	r0, r0, r0
 174:	00800000 	addeq	r0, r0, r0
 178:	00920000 	addseq	r0, r2, r0
 17c:	00010000 	andeq	r0, r1, r0
 180:	00009253 	andeq	r9, r0, r3, asr r2
 184:	00009400 	andeq	r9, r0, r0, lsl #8
 188:	73000300 	movwvc	r0, #768	; 0x300
 18c:	00949f7f 	addseq	r9, r4, pc, ror pc
 190:	00a20000 	adceq	r0, r2, r0
 194:	00010000 	andeq	r0, r1, r0
 198:	0000a253 	andeq	sl, r0, r3, asr r2
 19c:	0000a400 	andeq	sl, r0, r0, lsl #8
 1a0:	73000300 	movwvc	r0, #768	; 0x300
 1a4:	00a49f7f 	adceq	r9, r4, pc, ror pc
 1a8:	00b60000 	adcseq	r0, r6, r0
 1ac:	00010000 	andeq	r0, r1, r0
 1b0:	00000053 	andeq	r0, r0, r3, asr r0
 1b4:	00000000 	andeq	r0, r0, r0
 1b8:	00002e00 	andeq	r2, r0, r0, lsl #28
 1bc:	00008200 	andeq	r8, r0, r0, lsl #4
 1c0:	53000100 	movwpl	r0, #256	; 0x100
 1c4:	00000082 	andeq	r0, r0, r2, lsl #1
 1c8:	000000f2 	strdeq	r0, [r0], -r2
 1cc:	005e0001 	subseq	r0, lr, r1
 1d0:	00000000 	andeq	r0, r0, r0
 1d4:	地址 0x00000000000001d4 越界。


Disassembly of section .debug_str:

00000000 <.debug_str>:
   0:	64755f5f 	ldrbtvs	r5, [r5], #-3935	; 0xf5f
   4:	6f6d7669 	svcvs	0x006d7669
   8:	34696464 	strbtcc	r6, [r9], #-1124	; 0x464
   c:	6f682f00 	svcvs	0x00682f00
  10:	742f656d 	strtvc	r6, [pc], #-1389	; 18 <MODE_ABT+0x1>
  14:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
  18:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
  1c:	616c7364 	cmnvs	ip, r4, ror #6
  20:	772f6576 			; <UNDEFINED> instruction: 0x772f6576
  24:	736b726f 	cmnvc	fp, #-268435450	; 0xf0000006
  28:	65636170 	strbvs	r6, [r3, #-368]!	; 0x170
  2c:	7763742f 	strbvc	r7, [r3, -pc, lsr #8]!
  30:	616d2d67 	cmnvs	sp, r7, ror #26
  34:	722d656b 	eorvc	r6, sp, #448790528	; 0x1ac00000
  38:	61656c65 	cmnvs	r5, r5, ror #24
  3c:	6c2f6573 	cfstr32vs	mvfx6, [pc], #-460	; fffffe78 <__bss_end__+0x787e4cd0>
  40:	6c656261 	sfmvs	f6, 2, [r5], #-388	; 0xfffffe7c
  44:	636f642f 	cmnvs	pc, #788529152	; 0x2f000000
  48:	2d72656b 	cfldr64cs	mvdx6, [r2, #-428]!	; 0xfffffe54
  4c:	73757274 	cmnvc	r5, #116, 4	; 0x40000007
  50:	612d7974 	teqvs	sp, r4, ror r9
  54:	3436646d 	ldrtcc	r6, [r6], #-1133	; 0x46d
  58:	7763742d 	strbvc	r7, [r3, -sp, lsr #8]!
  5c:	75622d67 	strbvc	r2, [r2, #-3431]!	; 0xd67
  60:	2f646c69 	svccs	0x00646c69
  64:	67726174 			; <UNDEFINED> instruction: 0x67726174
  68:	612f7465 	teqvs	pc, r5, ror #8
  6c:	6c2d6d72 	stcvs	13, cr6, [sp], #-456	; 0xfffffe38
  70:	78756e69 	ldmdavc	r5!, {r0, r3, r5, r6, r9, sl, fp, sp, lr}^
  74:	756e672d 	strbvc	r6, [lr, #-1837]!	; 0x72d
  78:	69626165 	stmdbvs	r2!, {r0, r2, r5, r6, r8, sp, lr}^
  7c:	5f2f6668 	svcpl	0x002f6668
  80:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
  84:	75622f64 	strbvc	r2, [r2, #-3940]!	; 0xf64
  88:	73646c69 	cmnvc	r4, #26880	; 0x6900
  8c:	3638782f 	ldrtcc	r7, [r8], -pc, lsr #16
  90:	2d34365f 	ldccs	6, cr3, [r4, #-380]!	; 0xfffffe84
  94:	6e6b6e75 	mcrvs	14, 3, r6, cr11, cr5, {3}
  98:	2d6e776f 	stclcs	7, cr7, [lr, #-444]!	; 0xfffffe44
  9c:	756e696c 	strbvc	r6, [lr, #-2412]!	; 0x96c
  a0:	6e672d78 	mcrvs	13, 3, r2, cr7, cr8, {3}
  a4:	72612f75 	rsbvc	r2, r1, #468	; 0x1d4
  a8:	696c2d6d 	stmdbvs	ip!, {r0, r2, r3, r5, r6, r8, sl, fp, sp}^
  ac:	2d78756e 	cfldr64cs	mvdx7, [r8, #-440]!	; 0xfffffe48
  b0:	65756e67 	ldrbvs	r6, [r5, #-3687]!	; 0xe67
  b4:	68696261 	stmdavs	r9!, {r0, r5, r6, r9, sp, lr}^
  b8:	63672f66 	cmnvs	r7, #408	; 0x198
  bc:	696c2d63 	stmdbvs	ip!, {r0, r1, r5, r6, r8, sl, fp, sp}^
  c0:	6f72616e 	svcvs	0x0072616e
  c4:	392e342d 	stmdbcc	lr!, {r0, r2, r3, r5, sl, ip, sp}
  c8:	3130322d 	teqcc	r0, sp, lsr #4
  cc:	31302e37 	teqcc	r0, r7, lsr lr
  d0:	6174732d 	cmnvs	r4, sp, lsr #6
  d4:	2f326567 	svccs	0x00326567
  d8:	2d6d7261 	sfmcs	f7, 2, [sp, #-388]!	; 0xfffffe7c
  dc:	756e696c 	strbvc	r6, [lr, #-2412]!	; 0x96c
  e0:	6e672d78 	mcrvs	13, 3, r2, cr7, cr8, {3}
  e4:	62616575 	rsbvs	r6, r1, #490733568	; 0x1d400000
  e8:	2f666869 	svccs	0x00666869
  ec:	6762696c 	strbvs	r6, [r2, -ip, ror #18]!
  f0:	6c006363 	stcvs	3, cr6, [r0], {99}	; 0x63
  f4:	20676e6f 	rsbcs	r6, r7, pc, ror #28
  f8:	676e6f6c 	strbvs	r6, [lr, -ip, ror #30]!
  fc:	736e7520 	cmnvc	lr, #32, 10	; 0x8000000
 100:	656e6769 	strbvs	r6, [lr, #-1897]!	; 0x769
 104:	6e692064 	cdpvs	0, 6, cr2, cr9, cr4, {3}
 108:	6e750074 	mrcvs	0, 3, r0, cr5, cr4, {3}
 10c:	6e676973 	mcrvs	9, 3, r6, cr7, cr3, {3}
 110:	63206465 	teqvs	r0, #1694498816	; 0x65000000
 114:	00726168 	rsbseq	r6, r2, r8, ror #2
 118:	726f6873 	rsbvc	r6, pc, #7536640	; 0x730000
 11c:	6e752074 	mrcvs	0, 3, r2, cr5, cr4, {3}
 120:	6e676973 	mcrvs	9, 3, r6, cr7, cr3, {3}
 124:	69206465 	stmdbvs	r0!, {r0, r2, r5, r6, sl, sp, lr}
 128:	4700746e 	strmi	r7, [r0, -lr, ror #8]
 12c:	4320554e 	teqmi	r0, #327155712	; 0x13800000
 130:	392e3420 	stmdbcc	lr!, {r5, sl, ip, sp}
 134:	2d20342e 	cfstrscs	mvf3, [r0, #-184]!	; 0xffffff48
 138:	6372616d 	cmnvs	r2, #1073741851	; 0x4000001b
 13c:	72613d68 	rsbvc	r3, r1, #104, 26	; 0x1a00
 140:	2d37766d 	ldccs	6, cr7, [r7, #-436]!	; 0xfffffe4c
 144:	6d2d2061 	stcvs	0, cr2, [sp, #-388]!	; 0xfffffe7c
 148:	656e7574 	strbvs	r7, [lr, #-1396]!	; 0x574
 14c:	726f633d 	rsbvc	r6, pc, #-201326592	; 0xf4000000
 150:	2d786574 	cfldr64cs	mvdx6, [r8, #-464]!	; 0xfffffe30
 154:	2d203961 	stccs	9, cr3, [r0, #-388]!	; 0xfffffe7c
 158:	6f6c666d 	svcvs	0x006c666d
 15c:	612d7461 	teqvs	sp, r1, ror #8
 160:	683d6962 	ldmdavs	sp!, {r1, r5, r6, r8, fp, sp, lr}
 164:	20647261 	rsbcs	r7, r4, r1, ror #4
 168:	70666d2d 	rsbvc	r6, r6, sp, lsr #26
 16c:	66763d75 			; <UNDEFINED> instruction: 0x66763d75
 170:	2d337670 	ldccs	6, cr7, [r3, #-448]!	; 0xfffffe40
 174:	20363164 	eorscs	r3, r6, r4, ror #2
 178:	68746d2d 	ldmdavs	r4!, {r0, r2, r3, r5, r8, sl, fp, sp, lr}^
 17c:	20626d75 	rsbcs	r6, r2, r5, ror sp
 180:	6c746d2d 	ldclvs	13, cr6, [r4], #-180	; 0xffffff4c
 184:	69642d73 	stmdbvs	r4!, {r0, r1, r4, r5, r6, r8, sl, fp, sp}^
 188:	63656c61 	cmnvs	r5, #24832	; 0x6100
 18c:	6e673d74 	mcrvs	13, 3, r3, cr7, cr4, {3}
 190:	672d2075 			; <UNDEFINED> instruction: 0x672d2075
 194:	20672d20 	rsbcs	r2, r7, r0, lsr #26
 198:	2d20672d 	stccs	7, cr6, [r0, #-180]!	; 0xffffff4c
 19c:	2d20324f 	sfmcs	f3, 4, [r0, #-316]!	; 0xfffffec4
 1a0:	2d20324f 	sfmcs	f3, 4, [r0, #-316]!	; 0xfffffec4
 1a4:	2d20324f 	sfmcs	f3, 4, [r0, #-316]!	; 0xfffffec4
 1a8:	69756266 	ldmdbvs	r5!, {r1, r2, r5, r6, r9, sp, lr}^
 1ac:	6e69646c 	cdpvs	4, 6, cr6, cr9, cr12, {3}
 1b0:	696c2d67 	stmdbvs	ip!, {r0, r1, r2, r5, r6, r8, sl, fp, sp}^
 1b4:	63636762 	cmnvs	r3, #25690112	; 0x1880000
 1b8:	6e662d20 	cdpvs	13, 6, cr2, cr6, cr0, {1}
 1bc:	74732d6f 	ldrbtvc	r2, [r3], #-3439	; 0xd6f
 1c0:	2d6b6361 	stclcs	3, cr6, [fp, #-388]!	; 0xfffffe7c
 1c4:	746f7270 	strbtvc	r7, [pc], #-624	; 1cc <STACK_SIZE_ABT+0xcc>
 1c8:	6f746365 	svcvs	0x00746365
 1cc:	662d2072 			; <UNDEFINED> instruction: 0x662d2072
 1d0:	20434950 	subcs	r4, r3, r0, asr r9
 1d4:	6f6e662d 	svcvs	0x006e662d
 1d8:	6c6e692d 	stclvs	9, cr6, [lr], #-180	; 0xffffff4c
 1dc:	20656e69 	rsbcs	r6, r5, r9, ror #28
 1e0:	7865662d 	stmdavc	r5!, {r0, r2, r3, r5, r9, sl, sp, lr}^
 1e4:	74706563 	ldrbtvc	r6, [r0], #-1379	; 0x563
 1e8:	736e6f69 	cmnvc	lr, #420	; 0x1a4
 1ec:	76662d20 	strbtvc	r2, [r6], -r0, lsr #26
 1f0:	62697369 	rsbvs	r7, r9, #-1543503871	; 0xa4000001
 1f4:	74696c69 	strbtvc	r6, [r9], #-3177	; 0xc69
 1f8:	69683d79 	stmdbvs	r8!, {r0, r3, r4, r5, r6, r8, sl, fp, ip, sp}^
 1fc:	6e656464 	cdpvs	4, 6, cr6, cr5, cr4, {3}
 200:	49445500 	stmdbmi	r4, {r8, sl, ip, lr}^
 204:	65707974 	ldrbvs	r7, [r0, #-2420]!	; 0x974
 208:	6d6f6300 	stclvs	3, cr6, [pc, #-0]	; 210 <STACK_SIZE_ABT+0x110>
 20c:	78656c70 	stmdavc	r5!, {r4, r5, r6, sl, fp, sp, lr}^
 210:	6f6c6620 	svcvs	0x006c6620
 214:	73007461 	movwvc	r7, #1121	; 0x461
 218:	74657a69 	strbtvc	r7, [r5], #-2665	; 0xa69
 21c:	00657079 	rsbeq	r7, r5, r9, ror r0
 220:	676e6f6c 	strbvs	r6, [lr, -ip, ror #30]!
 224:	6e6f6c20 	cdpvs	12, 6, cr6, cr15, cr0, {1}
 228:	6e692067 	cdpvs	0, 6, cr2, cr9, cr7, {3}
 22c:	53550074 	cmppl	r5, #116	; 0x74
 230:	70797449 	rsbsvc	r7, r9, r9, asr #8
 234:	68730065 	ldmdavs	r3!, {r0, r2, r5, r6}^
 238:	2074726f 	rsbscs	r7, r4, pc, ror #4
 23c:	00746e69 	rsbseq	r6, r4, r9, ror #28
 240:	706d6f63 	rsbvc	r6, sp, r3, ror #30
 244:	2078656c 	rsbscs	r6, r8, ip, ror #10
 248:	62756f64 	rsbsvs	r6, r5, #100, 30	; 0x190
 24c:	2f00656c 	svccs	0x0000656c
 250:	656d6f68 	strbvs	r6, [sp, #-3944]!	; 0xf68
 254:	7763742f 	strbvc	r7, [r3, -pc, lsr #8]!
 258:	75622d67 	strbvc	r2, [r2, #-3431]!	; 0xd67
 25c:	73646c69 	cmnvc	r4, #26880	; 0x6900
 260:	6576616c 	ldrbvs	r6, [r6, #-364]!	; 0x16c
 264:	726f772f 	rsbvc	r7, pc, #12320768	; 0xbc0000
 268:	6170736b 	cmnvs	r0, fp, ror #6
 26c:	742f6563 	strtvc	r6, [pc], #-1379	; 274 <STACK_SIZE_ABT+0x174>
 270:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 274:	656b616d 	strbvs	r6, [fp, #-365]!	; 0x16d
 278:	6c65722d 	sfmvs	f7, 2, [r5], #-180	; 0xffffff4c
 27c:	65736165 	ldrbvs	r6, [r3, #-357]!	; 0x165
 280:	62616c2f 	rsbvs	r6, r1, #12032	; 0x2f00
 284:	642f6c65 	strtvs	r6, [pc], #-3173	; 28c <STACK_SIZE_ABT+0x18c>
 288:	656b636f 	strbvs	r6, [fp, #-879]!	; 0x36f
 28c:	72742d72 	rsbsvc	r2, r4, #7296	; 0x1c80
 290:	79747375 	ldmdbvc	r4!, {r0, r2, r4, r5, r6, r8, r9, ip, sp, lr}^
 294:	646d612d 	strbtvs	r6, [sp], #-301	; 0x12d
 298:	742d3436 	strtvc	r3, [sp], #-1078	; 0x436
 29c:	2d677763 	stclcs	7, cr7, [r7, #-396]!	; 0xfffffe74
 2a0:	6c697562 	cfstr64vs	mvdx7, [r9], #-392	; 0xfffffe78
 2a4:	61742f64 	cmnvs	r4, r4, ror #30
 2a8:	74656772 	strbtvc	r6, [r5], #-1906	; 0x772
 2ac:	6d72612f 	ldfvse	f6, [r2, #-188]!	; 0xffffff44
 2b0:	6e696c2d 	cdpvs	12, 6, cr6, cr9, cr13, {1}
 2b4:	672d7875 			; <UNDEFINED> instruction: 0x672d7875
 2b8:	6165756e 	cmnvs	r5, lr, ror #10
 2bc:	66686962 	strbtvs	r6, [r8], -r2, ror #18
 2c0:	616e732f 	cmnvs	lr, pc, lsr #6
 2c4:	6f687370 	svcvs	0x00687370
 2c8:	672f7374 			; <UNDEFINED> instruction: 0x672f7374
 2cc:	6c2d6363 	stcvs	3, cr6, [sp], #-396	; 0xfffffe74
 2d0:	72616e69 	rsbvc	r6, r1, #1680	; 0x690
 2d4:	2e342d6f 	cdpcs	13, 3, cr2, cr4, cr15, {3}
 2d8:	30322d39 	eorscc	r2, r2, r9, lsr sp
 2dc:	302e3731 	eorcc	r3, lr, r1, lsr r7
 2e0:	696c2f31 	stmdbvs	ip!, {r0, r4, r5, r8, r9, sl, fp, sp}^
 2e4:	63636762 	cmnvs	r3, #25690112	; 0x1880000
 2e8:	62696c2f 	rsbvs	r6, r9, #12032	; 0x2f00
 2ec:	32636367 	rsbcc	r6, r3, #-1677721599	; 0x9c000001
 2f0:	地址 0x00000000000002f0 越界。

