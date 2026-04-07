
./level3:     file format elf32-i386

Disassembly of section .plt:

08048380 <.plt>:
 8048380:	ff 35 1c 98 04 08    	push   DWORD PTR ds:0x804981c
 8048386:	ff 25 20 98 04 08    	jmp    DWORD PTR ds:0x8049820
 804838c:	00 00                	add    BYTE PTR [eax],al
	...

08048390 <printf@plt>:
 8048390:	ff 25 24 98 04 08    	jmp    DWORD PTR ds:0x8049824
 8048396:	68 00 00 00 00       	push   0x0
 804839b:	e9 e0 ff ff ff       	jmp    8048380 <.plt>

080483a0 <fgets@plt>:
 80483a0:	ff 25 28 98 04 08    	jmp    DWORD PTR ds:0x8049828
 80483a6:	68 08 00 00 00       	push   0x8
 80483ab:	e9 d0 ff ff ff       	jmp    8048380 <.plt>

080483b0 <fwrite@plt>:
 80483b0:	ff 25 2c 98 04 08    	jmp    DWORD PTR ds:0x804982c
 80483b6:	68 10 00 00 00       	push   0x10
 80483bb:	e9 c0 ff ff ff       	jmp    8048380 <.plt>

080483c0 <system@plt>:
 80483c0:	ff 25 30 98 04 08    	jmp    DWORD PTR ds:0x8049830
 80483c6:	68 18 00 00 00       	push   0x18
 80483cb:	e9 b0 ff ff ff       	jmp    8048380 <.plt>

080483d0 <__gmon_start__@plt>:
 80483d0:	ff 25 34 98 04 08    	jmp    DWORD PTR ds:0x8049834
 80483d6:	68 20 00 00 00       	push   0x20
 80483db:	e9 a0 ff ff ff       	jmp    8048380 <.plt>

080483e0 <__libc_start_main@plt>:
 80483e0:	ff 25 38 98 04 08    	jmp    DWORD PTR ds:0x8049838
 80483e6:	68 28 00 00 00       	push   0x28
 80483eb:	e9 90 ff ff ff       	jmp    8048380 <.plt>

Disassembly of section .text:

080484a4 <v>:
 80484a4:	55                   	push   ebp
 80484a5:	89 e5                	mov    ebp,esp
 80484a7:	81 ec 18 02 00 00    	sub    esp,0x218
 80484ad:	a1 60 98 04 08       	mov    eax,ds:0x8049860
 80484b2:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 80484b6:	c7 44 24 04 00 02 00 	mov    DWORD PTR [esp+0x4],0x200
 80484bd:	00 
 80484be:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484c4:	89 04 24             	mov    DWORD PTR [esp],eax
 80484c7:	e8 d4 fe ff ff       	call   80483a0 <fgets@plt>
 80484cc:	8d 85 f8 fd ff ff    	lea    eax,[ebp-0x208]
 80484d2:	89 04 24             	mov    DWORD PTR [esp],eax
 80484d5:	e8 b6 fe ff ff       	call   8048390 <printf@plt>
 80484da:	a1 8c 98 04 08       	mov    eax,ds:0x804988c
 80484df:	83 f8 40             	cmp    eax,0x40
 80484e2:	75 34                	jne    8048518 <v+0x74>
 80484e4:	a1 80 98 04 08       	mov    eax,ds:0x8049880
 80484e9:	89 c2                	mov    edx,eax
 80484eb:	b8 00 86 04 08       	mov    eax,0x8048600
 80484f0:	89 54 24 0c          	mov    DWORD PTR [esp+0xc],edx
 80484f4:	c7 44 24 08 0c 00 00 	mov    DWORD PTR [esp+0x8],0xc
 80484fb:	00 
 80484fc:	c7 44 24 04 01 00 00 	mov    DWORD PTR [esp+0x4],0x1
 8048503:	00 
 8048504:	89 04 24             	mov    DWORD PTR [esp],eax
 8048507:	e8 a4 fe ff ff       	call   80483b0 <fwrite@plt>
 804850c:	c7 04 24 0d 86 04 08 	mov    DWORD PTR [esp],0x804860d
 8048513:	e8 a8 fe ff ff       	call   80483c0 <system@plt>
 8048518:	c9                   	leave  
 8048519:	c3                   	ret    

0804851a <main>:
 804851a:	55                   	push   ebp
 804851b:	89 e5                	mov    ebp,esp
 804851d:	83 e4 f0             	and    esp,0xfffffff0
 8048520:	e8 7f ff ff ff       	call   80484a4 <v>
 8048525:	c9                   	leave  
 8048526:	c3                   	ret    
 8048527:	90                   	nop
 8048528:	90                   	nop
 8048529:	90                   	nop
 804852a:	90                   	nop
 804852b:	90                   	nop
 804852c:	90                   	nop
 804852d:	90                   	nop
 804852e:	90                   	nop
 804852f:	90                   	nop
