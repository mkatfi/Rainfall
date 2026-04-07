
./level5:     file format elf32-i386

Disassembly of section .plt:

08048370 <.plt>:
 8048370:	ff 35 1c 98 04 08    	push   DWORD PTR ds:0x804981c
 8048376:	ff 25 20 98 04 08    	jmp    DWORD PTR ds:0x8049820
 804837c:	00 00                	add    BYTE PTR [eax],al
	...

08048380 <printf@plt>:
 8048380:	ff 25 24 98 04 08    	jmp    DWORD PTR ds:0x8049824
 8048386:	68 00 00 00 00       	push   0x0
 804838b:	e9 e0 ff ff ff       	jmp    8048370 <.plt>

08048390 <_exit@plt>:
 8048390:	ff 25 28 98 04 08    	jmp    DWORD PTR ds:0x8049828
 8048396:	68 08 00 00 00       	push   0x8
 804839b:	e9 d0 ff ff ff       	jmp    8048370 <.plt>

080483a0 <fgets@plt>:
 80483a0:	ff 25 2c 98 04 08    	jmp    DWORD PTR ds:0x804982c
 80483a6:	68 10 00 00 00       	push   0x10
 80483ab:	e9 c0 ff ff ff       	jmp    8048370 <.plt>

080483b0 <system@plt>:
 80483b0:	ff 25 30 98 04 08    	jmp    DWORD PTR ds:0x8049830
 80483b6:	68 18 00 00 00       	push   0x18
 80483bb:	e9 b0 ff ff ff       	jmp    8048370 <.plt>

080483c0 <__gmon_start__@plt>:
 80483c0:	ff 25 34 98 04 08    	jmp    DWORD PTR ds:0x8049834
 80483c6:	68 20 00 00 00       	push   0x20
 80483cb:	e9 a0 ff ff ff       	jmp    8048370 <.plt>

080483d0 <exit@plt>:
 80483d0:	ff 25 38 98 04 08    	jmp    DWORD PTR ds:0x8049838
 80483d6:	68 28 00 00 00       	push   0x28
 80483db:	e9 90 ff ff ff       	jmp    8048370 <.plt>

080483e0 <__libc_start_main@plt>:
 80483e0:	ff 25 3c 98 04 08    	jmp    DWORD PTR ds:0x804983c
 80483e6:	68 30 00 00 00       	push   0x30
 80483eb:	e9 80 ff ff ff       	jmp    8048370 <.plt>

Disassembly of section .text:

080484a4 <o>:
 80484a4:	55                   	push   ebp
 80484a5:	89 e5                	mov    ebp,esp
 80484a7:	83 ec 18             	sub    esp,0x18
 80484aa:	c7 04 24 f0 85 04 08 	mov    DWORD PTR [esp],0x80485f0
 80484b1:	e8 fa fe ff ff       	call   80483b0 <system@plt>
 80484b6:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 80484bd:	e8 ce fe ff ff       	call   8048390 <_exit@plt>

080484c2 <n>:
 80484c2:	55                   	push   ebp
 80484c3:	89 e5                	mov    ebp,esp
 80484c5:	81 ec 18 02 00 00    	sub    esp,0x218
 80484cb:	a1 48 98 04 08       	mov    eax,ds:0x8049848
 80484d0:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 80484d4:	c7 44 24 04 00 02 00 	mov    DWORD PTR [esp+0x4],0x200
 80484db:	00 
 80484dc:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484e2:	89 04 24             	mov    DWORD PTR [esp],eax
 80484e5:	e8 b6 fe ff ff       	call   80483a0 <fgets@plt>
 80484ea:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484f0:	89 04 24             	mov    DWORD PTR [esp],eax
 80484f3:	e8 88 fe ff ff       	call   8048380 <printf@plt>
 80484f8:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 80484ff:	e8 cc fe ff ff       	call   80483d0 <exit@plt>

08048504 <main>:
 8048504:	55                   	push   ebp
 8048505:	89 e5                	mov    ebp,esp
 8048507:	83 e4 f0             	and    esp,0xfffffff0
 804850a:	e8 b3 ff ff ff       	call   80484c2 <n>
 804850f:	c9                   	leave  
 8048510:	c3                   	ret    
 8048511:	90                   	nop
 8048512:	90                   	nop
 8048513:	90                   	nop
 8048514:	90                   	nop
 8048515:	90                   	nop
 8048516:	90                   	nop
 8048517:	90                   	nop
 8048518:	90                   	nop
 8048519:	90                   	nop
 804851a:	90                   	nop
 804851b:	90                   	nop
 804851c:	90                   	nop
 804851d:	90                   	nop
 804851e:	90                   	nop
 804851f:	90                   	nop
