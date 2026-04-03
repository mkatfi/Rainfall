
./level2:     file format elf32-i386

Disassembly of section .plt:

08048390 <.plt>:
 8048390:	ff 35 2c 98 04 08    	push   DWORD PTR ds:0x804982c
 8048396:	ff 25 30 98 04 08    	jmp    DWORD PTR ds:0x8049830
 804839c:	00 00                	add    BYTE PTR [eax],al
	...

080483a0 <printf@plt>:
 80483a0:	ff 25 34 98 04 08    	jmp    DWORD PTR ds:0x8049834
 80483a6:	68 00 00 00 00       	push   0x0
 80483ab:	e9 e0 ff ff ff       	jmp    8048390 <.plt>

080483b0 <fflush@plt>:
 80483b0:	ff 25 38 98 04 08    	jmp    DWORD PTR ds:0x8049838
 80483b6:	68 08 00 00 00       	push   0x8
 80483bb:	e9 d0 ff ff ff       	jmp    8048390 <.plt>

080483c0 <gets@plt>:
 80483c0:	ff 25 3c 98 04 08    	jmp    DWORD PTR ds:0x804983c
 80483c6:	68 10 00 00 00       	push   0x10
 80483cb:	e9 c0 ff ff ff       	jmp    8048390 <.plt>

080483d0 <_exit@plt>:
 80483d0:	ff 25 40 98 04 08    	jmp    DWORD PTR ds:0x8049840
 80483d6:	68 18 00 00 00       	push   0x18
 80483db:	e9 b0 ff ff ff       	jmp    8048390 <.plt>

080483e0 <strdup@plt>:
 80483e0:	ff 25 44 98 04 08    	jmp    DWORD PTR ds:0x8049844
 80483e6:	68 20 00 00 00       	push   0x20
 80483eb:	e9 a0 ff ff ff       	jmp    8048390 <.plt>

080483f0 <puts@plt>:
 80483f0:	ff 25 48 98 04 08    	jmp    DWORD PTR ds:0x8049848
 80483f6:	68 28 00 00 00       	push   0x28
 80483fb:	e9 90 ff ff ff       	jmp    8048390 <.plt>

08048400 <__gmon_start__@plt>:
 8048400:	ff 25 4c 98 04 08    	jmp    DWORD PTR ds:0x804984c
 8048406:	68 30 00 00 00       	push   0x30
 804840b:	e9 80 ff ff ff       	jmp    8048390 <.plt>

08048410 <__libc_start_main@plt>:
 8048410:	ff 25 50 98 04 08    	jmp    DWORD PTR ds:0x8049850
 8048416:	68 38 00 00 00       	push   0x38
 804841b:	e9 70 ff ff ff       	jmp    8048390 <.plt>

Disassembly of section .text:

080484d4 <p>:
 80484d4:	55                   	push   ebp
 80484d5:	89 e5                	mov    ebp,esp
 80484d7:	83 ec 68             	sub    esp,0x68
 80484da:	a1 60 98 04 08       	mov    eax,ds:0x8049860
 80484df:	89 04 24             	mov    DWORD PTR [esp],eax
 80484e2:	e8 c9 fe ff ff       	call   80483b0 <fflush@plt>
 80484e7:	8d 45 b4             	lea    eax,[ebp-0x4c]
 80484ea:	89 04 24             	mov    DWORD PTR [esp],eax
 80484ed:	e8 ce fe ff ff       	call   80483c0 <gets@plt>
 80484f2:	8b 45 04             	mov    eax,DWORD PTR [ebp+0x4]
 80484f5:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
 80484f8:	8b 45 f4             	mov    eax,DWORD PTR [ebp-0xc]
 80484fb:	25 00 00 00 b0       	and    eax,0xb0000000
 8048500:	3d 00 00 00 b0       	cmp    eax,0xb0000000
 8048505:	75 20                	jne    8048527 <p+0x53>
 8048507:	b8 20 86 04 08       	mov    eax,0x8048620
 804850c:	8b 55 f4             	mov    edx,DWORD PTR [ebp-0xc]
 804850f:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
 8048513:	89 04 24             	mov    DWORD PTR [esp],eax
 8048516:	e8 85 fe ff ff       	call   80483a0 <printf@plt>
 804851b:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 8048522:	e8 a9 fe ff ff       	call   80483d0 <_exit@plt>
 8048527:	8d 45 b4             	lea    eax,[ebp-0x4c]
 804852a:	89 04 24             	mov    DWORD PTR [esp],eax
 804852d:	e8 be fe ff ff       	call   80483f0 <puts@plt>
 8048532:	8d 45 b4             	lea    eax,[ebp-0x4c]
 8048535:	89 04 24             	mov    DWORD PTR [esp],eax
 8048538:	e8 a3 fe ff ff       	call   80483e0 <strdup@plt>
 804853d:	c9                   	leave  
 804853e:	c3                   	ret    

0804853f <main>:
 804853f:	55                   	push   ebp
 8048540:	89 e5                	mov    ebp,esp
 8048542:	83 e4 f0             	and    esp,0xfffffff0
 8048545:	e8 8a ff ff ff       	call   80484d4 <p>
 804854a:	c9                   	leave  
 804854b:	c3                   	ret    
 804854c:	90                   	nop
 804854d:	90                   	nop
 804854e:	90                   	nop
 804854f:	90                   	nop
