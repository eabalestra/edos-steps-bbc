
init:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <start>:
   0:	ff010113          	addi	sp,sp,-16
   4:	00112623          	sw	ra,12(sp)
   8:	00812423          	sw	s0,8(sp)
   c:	01010413          	addi	s0,sp,16
  10:	420000ef          	jal	ra,430 <main>
  14:	3d4000ef          	jal	ra,3e8 <exit>
  18:	00c12083          	lw	ra,12(sp)
  1c:	00812403          	lw	s0,8(sp)
  20:	01010113          	addi	sp,sp,16
  24:	00008067          	ret

00000028 <console_read_line>:
  28:	fe010113          	addi	sp,sp,-32
  2c:	00112e23          	sw	ra,28(sp)
  30:	00812c23          	sw	s0,24(sp)
  34:	00912a23          	sw	s1,20(sp)
  38:	01212823          	sw	s2,16(sp)
  3c:	01312623          	sw	s3,12(sp)
  40:	01412423          	sw	s4,8(sp)
  44:	01512223          	sw	s5,4(sp)
  48:	02010413          	addi	s0,sp,32
  4c:	4d400a93          	li	s5,1236
  50:	00000493          	li	s1,0
  54:	05b00913          	li	s2,91
  58:	00d00993          	li	s3,13
  5c:	06000a13          	li	s4,96
  60:	03248c63          	beq	s1,s2,98 <console_read_line+0x70>
  64:	3b4000ef          	jal	ra,418 <console_getc>
  68:	03350263          	beq	a0,s3,8c <console_read_line+0x64>
  6c:	fe150793          	addi	a5,a0,-31
  70:	fefa68e3          	bltu	s4,a5,60 <console_read_line+0x38>
  74:	00aa8023          	sb	a0,0(s5)
  78:	000a8513          	mv	a0,s5
  7c:	384000ef          	jal	ra,400 <console_puts>
  80:	00148493          	addi	s1,s1,1
  84:	001a8a93          	addi	s5,s5,1
  88:	fd9ff06f          	j	60 <console_read_line+0x38>
  8c:	4d400793          	li	a5,1236
  90:	009787b3          	add	a5,a5,s1
  94:	00078023          	sb	zero,0(a5)
  98:	4d400513          	li	a0,1236
  9c:	01c12083          	lw	ra,28(sp)
  a0:	01812403          	lw	s0,24(sp)
  a4:	01412483          	lw	s1,20(sp)
  a8:	01012903          	lw	s2,16(sp)
  ac:	00c12983          	lw	s3,12(sp)
  b0:	00812a03          	lw	s4,8(sp)
  b4:	00412a83          	lw	s5,4(sp)
  b8:	02010113          	addi	sp,sp,32
  bc:	00008067          	ret

000000c0 <memset>:
  c0:	ff010113          	addi	sp,sp,-16
  c4:	00812623          	sw	s0,12(sp)
  c8:	01010413          	addi	s0,sp,16
  cc:	00060c63          	beqz	a2,e4 <memset+0x24>
  d0:	00c50633          	add	a2,a0,a2
  d4:	00050793          	mv	a5,a0
  d8:	00178793          	addi	a5,a5,1
  dc:	feb78fa3          	sb	a1,-1(a5)
  e0:	fef61ce3          	bne	a2,a5,d8 <memset+0x18>
  e4:	00c12403          	lw	s0,12(sp)
  e8:	01010113          	addi	sp,sp,16
  ec:	00008067          	ret

000000f0 <memcpy>:
  f0:	ff010113          	addi	sp,sp,-16
  f4:	00812623          	sw	s0,12(sp)
  f8:	01010413          	addi	s0,sp,16
  fc:	02060063          	beqz	a2,11c <memcpy+0x2c>
 100:	00c50633          	add	a2,a0,a2
 104:	00050793          	mv	a5,a0
 108:	00158593          	addi	a1,a1,1
 10c:	00178793          	addi	a5,a5,1
 110:	fff5c703          	lbu	a4,-1(a1)
 114:	fee78fa3          	sb	a4,-1(a5)
 118:	fef618e3          	bne	a2,a5,108 <memcpy+0x18>
 11c:	00c12403          	lw	s0,12(sp)
 120:	01010113          	addi	sp,sp,16
 124:	00008067          	ret

00000128 <strlen>:
 128:	ff010113          	addi	sp,sp,-16
 12c:	00812623          	sw	s0,12(sp)
 130:	01010413          	addi	s0,sp,16
 134:	00054783          	lbu	a5,0(a0)
 138:	02078463          	beqz	a5,160 <strlen+0x38>
 13c:	00050713          	mv	a4,a0
 140:	00000513          	li	a0,0
 144:	00150513          	addi	a0,a0,1
 148:	00a707b3          	add	a5,a4,a0
 14c:	0007c783          	lbu	a5,0(a5)
 150:	fe079ae3          	bnez	a5,144 <strlen+0x1c>
 154:	00c12403          	lw	s0,12(sp)
 158:	01010113          	addi	sp,sp,16
 15c:	00008067          	ret
 160:	00000513          	li	a0,0
 164:	ff1ff06f          	j	154 <strlen+0x2c>

00000168 <strcpy>:
 168:	ff010113          	addi	sp,sp,-16
 16c:	00812623          	sw	s0,12(sp)
 170:	01010413          	addi	s0,sp,16
 174:	0005c783          	lbu	a5,0(a1)
 178:	02078663          	beqz	a5,1a4 <strcpy+0x3c>
 17c:	00050713          	mv	a4,a0
 180:	00158593          	addi	a1,a1,1
 184:	00170713          	addi	a4,a4,1
 188:	fef70fa3          	sb	a5,-1(a4)
 18c:	0005c783          	lbu	a5,0(a1)
 190:	fe0798e3          	bnez	a5,180 <strcpy+0x18>
 194:	00070023          	sb	zero,0(a4)
 198:	00c12403          	lw	s0,12(sp)
 19c:	01010113          	addi	sp,sp,16
 1a0:	00008067          	ret
 1a4:	00050713          	mv	a4,a0
 1a8:	fedff06f          	j	194 <strcpy+0x2c>

000001ac <strcmp>:
 1ac:	ff010113          	addi	sp,sp,-16
 1b0:	00812623          	sw	s0,12(sp)
 1b4:	01010413          	addi	s0,sp,16
 1b8:	00054783          	lbu	a5,0(a0)
 1bc:	02078063          	beqz	a5,1dc <strcmp+0x30>
 1c0:	0005c703          	lbu	a4,0(a1)
 1c4:	00070c63          	beqz	a4,1dc <strcmp+0x30>
 1c8:	00f71a63          	bne	a4,a5,1dc <strcmp+0x30>
 1cc:	00150513          	addi	a0,a0,1
 1d0:	00158593          	addi	a1,a1,1
 1d4:	00054783          	lbu	a5,0(a0)
 1d8:	fe0794e3          	bnez	a5,1c0 <strcmp+0x14>
 1dc:	0005c503          	lbu	a0,0(a1)
 1e0:	40a78533          	sub	a0,a5,a0
 1e4:	00c12403          	lw	s0,12(sp)
 1e8:	01010113          	addi	sp,sp,16
 1ec:	00008067          	ret

000001f0 <printf>:
 1f0:	f9010113          	addi	sp,sp,-112
 1f4:	04112623          	sw	ra,76(sp)
 1f8:	04812423          	sw	s0,72(sp)
 1fc:	04912223          	sw	s1,68(sp)
 200:	05212023          	sw	s2,64(sp)
 204:	03312e23          	sw	s3,60(sp)
 208:	03412c23          	sw	s4,56(sp)
 20c:	03512a23          	sw	s5,52(sp)
 210:	03612823          	sw	s6,48(sp)
 214:	03712623          	sw	s7,44(sp)
 218:	03812423          	sw	s8,40(sp)
 21c:	03912223          	sw	s9,36(sp)
 220:	03a12023          	sw	s10,32(sp)
 224:	01b12e23          	sw	s11,28(sp)
 228:	05010413          	addi	s0,sp,80
 22c:	00050493          	mv	s1,a0
 230:	00b42223          	sw	a1,4(s0)
 234:	00c42423          	sw	a2,8(s0)
 238:	00d42623          	sw	a3,12(s0)
 23c:	00e42823          	sw	a4,16(s0)
 240:	00f42a23          	sw	a5,20(s0)
 244:	01042c23          	sw	a6,24(s0)
 248:	01142e23          	sw	a7,28(s0)
 24c:	00440793          	addi	a5,s0,4
 250:	faf42e23          	sw	a5,-68(s0)
 254:	00054503          	lbu	a0,0(a0)
 258:	04050463          	beqz	a0,2a0 <printf+0xb0>
 25c:	02500a13          	li	s4,37
 260:	06400b13          	li	s6,100
 264:	00900a93          	li	s5,9
 268:	00a00c93          	li	s9,10
 26c:	07300d93          	li	s11,115
 270:	07800d13          	li	s10,120
 274:	00000c17          	auipc	s8,0x0
 278:	208c0c13          	addi	s8,s8,520 # 47c <main+0x4c>
 27c:	ffc00b93          	li	s7,-4
 280:	1080006f          	j	388 <printf+0x198>
 284:	00078a63          	beqz	a5,298 <printf+0xa8>
 288:	0f479a63          	bne	a5,s4,37c <printf+0x18c>
 28c:	000a0513          	mv	a0,s4
 290:	17c000ef          	jal	ra,40c <console_putc>
 294:	0e80006f          	j	37c <printf+0x18c>
 298:	02500513          	li	a0,37
 29c:	170000ef          	jal	ra,40c <console_putc>
 2a0:	04c12083          	lw	ra,76(sp)
 2a4:	04812403          	lw	s0,72(sp)
 2a8:	04412483          	lw	s1,68(sp)
 2ac:	04012903          	lw	s2,64(sp)
 2b0:	03c12983          	lw	s3,60(sp)
 2b4:	03812a03          	lw	s4,56(sp)
 2b8:	03412a83          	lw	s5,52(sp)
 2bc:	03012b03          	lw	s6,48(sp)
 2c0:	02c12b83          	lw	s7,44(sp)
 2c4:	02812c03          	lw	s8,40(sp)
 2c8:	02412c83          	lw	s9,36(sp)
 2cc:	02012d03          	lw	s10,32(sp)
 2d0:	01c12d83          	lw	s11,28(sp)
 2d4:	07010113          	addi	sp,sp,112
 2d8:	00008067          	ret
 2dc:	fbc42783          	lw	a5,-68(s0)
 2e0:	00478713          	addi	a4,a5,4
 2e4:	fae42e23          	sw	a4,-68(s0)
 2e8:	0007a483          	lw	s1,0(a5)
 2ec:	0004c503          	lbu	a0,0(s1)
 2f0:	08050663          	beqz	a0,37c <printf+0x18c>
 2f4:	118000ef          	jal	ra,40c <console_putc>
 2f8:	00148493          	addi	s1,s1,1
 2fc:	0004c503          	lbu	a0,0(s1)
 300:	fe051ae3          	bnez	a0,2f4 <printf+0x104>
 304:	0780006f          	j	37c <printf+0x18c>
 308:	fbc42783          	lw	a5,-68(s0)
 30c:	00478713          	addi	a4,a5,4
 310:	fae42e23          	sw	a4,-68(s0)
 314:	0007a903          	lw	s2,0(a5)
 318:	04094663          	bltz	s2,364 <printf+0x174>
 31c:	0b2ade63          	bge	s5,s2,3d8 <printf+0x1e8>
 320:	00100493          	li	s1,1
 324:	00249793          	slli	a5,s1,0x2
 328:	009787b3          	add	a5,a5,s1
 32c:	00179793          	slli	a5,a5,0x1
 330:	00078493          	mv	s1,a5
 334:	02f947b3          	div	a5,s2,a5
 338:	fefac6e3          	blt	s5,a5,324 <printf+0x134>
 33c:	04905063          	blez	s1,37c <printf+0x18c>
 340:	02994533          	div	a0,s2,s1
 344:	03050513          	addi	a0,a0,48
 348:	0ff57513          	zext.b	a0,a0
 34c:	0c0000ef          	jal	ra,40c <console_putc>
 350:	02996933          	rem	s2,s2,s1
 354:	00048793          	mv	a5,s1
 358:	0394c4b3          	div	s1,s1,s9
 35c:	fefac2e3          	blt	s5,a5,340 <printf+0x150>
 360:	01c0006f          	j	37c <printf+0x18c>
 364:	02d00513          	li	a0,45
 368:	0a4000ef          	jal	ra,40c <console_putc>
 36c:	41200933          	neg	s2,s2
 370:	fadff06f          	j	31c <printf+0x12c>
 374:	098000ef          	jal	ra,40c <console_putc>
 378:	00048993          	mv	s3,s1
 37c:	00198493          	addi	s1,s3,1
 380:	0019c503          	lbu	a0,1(s3)
 384:	f0050ee3          	beqz	a0,2a0 <printf+0xb0>
 388:	ff4516e3          	bne	a0,s4,374 <printf+0x184>
 38c:	00148993          	addi	s3,s1,1
 390:	0014c783          	lbu	a5,1(s1)
 394:	f7678ae3          	beq	a5,s6,308 <printf+0x118>
 398:	eefb76e3          	bgeu	s6,a5,284 <printf+0x94>
 39c:	f5b780e3          	beq	a5,s11,2dc <printf+0xec>
 3a0:	fda79ee3          	bne	a5,s10,37c <printf+0x18c>
 3a4:	fbc42783          	lw	a5,-68(s0)
 3a8:	00478713          	addi	a4,a5,4
 3ac:	fae42e23          	sw	a4,-68(s0)
 3b0:	0007a903          	lw	s2,0(a5)
 3b4:	01c00493          	li	s1,28
 3b8:	409957b3          	sra	a5,s2,s1
 3bc:	00f7f793          	andi	a5,a5,15
 3c0:	018787b3          	add	a5,a5,s8
 3c4:	0007c503          	lbu	a0,0(a5)
 3c8:	044000ef          	jal	ra,40c <console_putc>
 3cc:	ffc48493          	addi	s1,s1,-4
 3d0:	ff7494e3          	bne	s1,s7,3b8 <printf+0x1c8>
 3d4:	fa9ff06f          	j	37c <printf+0x18c>
 3d8:	03090513          	addi	a0,s2,48
 3dc:	0ff57513          	zext.b	a0,a0
 3e0:	02c000ef          	jal	ra,40c <console_putc>
 3e4:	f99ff06f          	j	37c <printf+0x18c>

000003e8 <exit>:
 3e8:	00000893          	li	a7,0
 3ec:	00000073          	ecall
 3f0:	00008067          	ret

000003f4 <getpid>:
 3f4:	00100893          	li	a7,1
 3f8:	00000073          	ecall
 3fc:	00008067          	ret

00000400 <console_puts>:
 400:	00200893          	li	a7,2
 404:	00000073          	ecall
 408:	00008067          	ret

0000040c <console_putc>:
 40c:	00300893          	li	a7,3
 410:	00000073          	ecall
 414:	00008067          	ret

00000418 <console_getc>:
 418:	00400893          	li	a7,4
 41c:	00000073          	ecall
 420:	00008067          	ret

00000424 <sleep>:
 424:	00500893          	li	a7,5
 428:	00000073          	ecall
 42c:	00008067          	ret

00000430 <main>:
 430:	ff010113          	addi	sp,sp,-16
 434:	00112623          	sw	ra,12(sp)
 438:	00812423          	sw	s0,8(sp)
 43c:	01010413          	addi	s0,sp,16
 440:	fb5ff0ef          	jal	ra,3f4 <getpid>
 444:	00050593          	mv	a1,a0
 448:	00000517          	auipc	a0,0x0
 44c:	04850513          	addi	a0,a0,72 # 490 <main+0x60>
 450:	da1ff0ef          	jal	ra,1f0 <printf>
 454:	00400513          	li	a0,4
 458:	fcdff0ef          	jal	ra,424 <sleep>
 45c:	00000517          	auipc	a0,0x0
 460:	06850513          	addi	a0,a0,104 # 4c4 <main+0x94>
 464:	d8dff0ef          	jal	ra,1f0 <printf>
 468:	00000513          	li	a0,0
 46c:	00c12083          	lw	ra,12(sp)
 470:	00812403          	lw	s0,8(sp)
 474:	01010113          	addi	sp,sp,16
 478:	00008067          	ret
