
./level0:     file format elf32-i386

Disassembly of section .plt:

080481d0 <.plt>:
 80481d0:	ff 25 04 e1 0e 08    	jmp    DWORD PTR ds:0x80ee104
 80481d6:	68 00 00 00 00       	push   0x0
 80481db:	e9 00 00 00 00       	jmp    80481e0 <.plt+0x10>
 80481e0:	ff 25 08 e1 0e 08    	jmp    DWORD PTR ds:0x80ee108
 80481e6:	68 00 00 00 00       	push   0x0
 80481eb:	e9 00 00 00 00       	jmp    80481f0 <.plt+0x20>
 80481f0:	ff 25 0c e1 0e 08    	jmp    DWORD PTR ds:0x80ee10c
 80481f6:	68 00 00 00 00       	push   0x0
 80481fb:	e9 00 00 00 00       	jmp    8048200 <.plt+0x30>
 8048200:	ff 25 10 e1 0e 08    	jmp    DWORD PTR ds:0x80ee110
 8048206:	68 00 00 00 00       	push   0x0
 804820b:	e9 00 00 00 00       	jmp    8048210 <.plt+0x40>
 8048210:	ff 25 14 e1 0e 08    	jmp    DWORD PTR ds:0x80ee114
 8048216:	68 00 00 00 00       	push   0x0
 804821b:	e9 00 00 00 00       	jmp    8048220 <.plt+0x50>
 8048220:	ff 25 18 e1 0e 08    	jmp    DWORD PTR ds:0x80ee118
 8048226:	68 00 00 00 00       	push   0x0
 804822b:	e9 00 00 00 00       	jmp    8048230 <.plt+0x60>
 8048230:	ff 25 1c e1 0e 08    	jmp    DWORD PTR ds:0x80ee11c
 8048236:	68 00 00 00 00       	push   0x0
 804823b:	e9 00 00 00 00       	jmp    8048240 <.plt+0x70>
 8048240:	ff 25 20 e1 0e 08    	jmp    DWORD PTR ds:0x80ee120
 8048246:	68 00 00 00 00       	push   0x0
 804824b:	e9 00 00 00 00       	jmp    8048250 <.plt+0x80>
 8048250:	ff 25 24 e1 0e 08    	jmp    DWORD PTR ds:0x80ee124
 8048256:	68 00 00 00 00       	push   0x0
 804825b:	e9 00 00 00 00       	jmp    8048260 <.plt+0x90>
 8048260:	ff 25 28 e1 0e 08    	jmp    DWORD PTR ds:0x80ee128
 8048266:	68 00 00 00 00       	push   0x0
 804826b:	e9 00 00 00 00       	jmp    8048270 <.plt+0xa0>
 8048270:	ff 25 2c e1 0e 08    	jmp    DWORD PTR ds:0x80ee12c
 8048276:	68 00 00 00 00       	push   0x0
 804827b:	e9 00 00 00 00       	jmp    8048280 <.plt+0xb0>
 8048280:	ff 25 30 e1 0e 08    	jmp    DWORD PTR ds:0x80ee130
 8048286:	68 00 00 00 00       	push   0x0
 804828b:	e9 00 00 00 00       	jmp    8048290 <.plt+0xc0>
 8048290:	ff 25 34 e1 0e 08    	jmp    DWORD PTR ds:0x80ee134
 8048296:	68 00 00 00 00       	push   0x0
 804829b:	e9 00 00 00 00       	jmp    80482a0 <.plt+0xd0>
 80482a0:	ff 25 38 e1 0e 08    	jmp    DWORD PTR ds:0x80ee138
 80482a6:	68 00 00 00 00       	push   0x0
 80482ab:	e9 00 00 00 00       	jmp    80482b0 <.plt+0xe0>
 80482b0:	ff 25 3c e1 0e 08    	jmp    DWORD PTR ds:0x80ee13c
 80482b6:	68 00 00 00 00       	push   0x0
 80482bb:	e9 00 00 00 00       	jmp    80482c0 <.plt+0xf0>
 80482c0:	ff 25 40 e1 0e 08    	jmp    DWORD PTR ds:0x80ee140
 80482c6:	68 00 00 00 00       	push   0x0
 80482cb:	e9 00 00 00 00       	jmp    80482d0 <.plt+0x100>
 80482d0:	ff 25 44 e1 0e 08    	jmp    DWORD PTR ds:0x80ee144
 80482d6:	68 00 00 00 00       	push   0x0
 80482db:	e9 00 00 00 00       	jmp    80482e0

Disassembly of section .text:

08048ec0 <main>:
 8048ec0:	55                   	push   ebp
 8048ec1:	89 e5                	mov    ebp,esp
 8048ec3:	83 e4 f0             	and    esp,0xfffffff0
 8048ec6:	83 ec 20             	sub    esp,0x20
 8048ec9:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 8048ecc:	83 c0 04             	add    eax,0x4
 8048ecf:	8b 00                	mov    eax,DWORD PTR [eax]
 8048ed1:	89 04 24             	mov    DWORD PTR [esp],eax
 8048ed4:	e8 37 08 00 00       	call   8049710 <atoi>
 8048ed9:	3d a7 01 00 00       	cmp    eax,0x1a7
 8048ede:	75 78                	jne    8048f58 <main+0x98>
 8048ee0:	c7 04 24 48 53 0c 08 	mov    DWORD PTR [esp],0x80c5348
 8048ee7:	e8 04 7d 00 00       	call   8050bf0 <__strdup>
 8048eec:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
 8048ef0:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
 8048ef7:	00 
 8048ef8:	e8 83 b7 00 00       	call   8054680 <__getegid>
 8048efd:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
 8048f01:	e8 6a b7 00 00       	call   8054670 <__geteuid>
 8048f06:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
 8048f0a:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 8048f0e:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 8048f12:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 8048f16:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048f1a:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 8048f1e:	89 04 24             	mov    DWORD PTR [esp],eax
 8048f21:	e8 da b7 00 00       	call   8054700 <__setresgid>
 8048f26:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 8048f2a:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 8048f2e:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 8048f32:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048f36:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 8048f3a:	89 04 24             	mov    DWORD PTR [esp],eax
 8048f3d:	e8 4e b7 00 00       	call   8054690 <__setresuid>
 8048f42:	8d 44 24 10          	lea    eax,[esp+0x10]
 8048f46:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048f4a:	c7 04 24 48 53 0c 08 	mov    DWORD PTR [esp],0x80c5348
 8048f51:	e8 ea b6 00 00       	call   8054640 <execv>
 8048f56:	eb 28                	jmp    8048f80 <main+0xc0>
 8048f58:	a1 70 e1 0e 08       	mov    eax,ds:0x80ee170
 8048f5d:	89 c2                	mov    edx,eax
 8048f5f:	b8 50 53 0c 08       	mov    eax,0x80c5350
 8048f64:	89 54 24 0c          	mov    DWORD PTR [esp+0xc],edx
 8048f68:	c7 44 24 08 05 00 00 	mov    DWORD PTR [esp+0x8],0x5
 8048f6f:	00 
 8048f70:	c7 44 24 04 01 00 00 	mov    DWORD PTR [esp+0x4],0x1
 8048f77:	00 
 8048f78:	89 04 24             	mov    DWORD PTR [esp],eax
 8048f7b:	e8 b0 12 00 00       	call   804a230 <_IO_fwrite>
 8048f80:	b8 00 00 00 00       	mov    eax,0x0
 8048f85:	c9                   	leave  
 8048f86:	c3                   	ret    
 8048f87:	90                   	nop
 8048f88:	90                   	nop
 8048f89:	90                   	nop
 8048f8a:	90                   	nop
 8048f8b:	90                   	nop
 8048f8c:	90                   	nop
 8048f8d:	90                   	nop
 8048f8e:	90                   	nop
 8048f8f:	90                   	nop
