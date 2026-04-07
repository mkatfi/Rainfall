
./level9:     file format elf32-i386

Disassembly of section .plt:

080484a0 <.plt>:
 80484a0:	ff 35 48 9b 04 08    	push   DWORD PTR ds:0x8049b48
 80484a6:	ff 25 4c 9b 04 08    	jmp    DWORD PTR ds:0x8049b4c
 80484ac:	00 00                	add    BYTE PTR [eax],al
	...

080484b0 <__cxa_atexit@plt>:
 80484b0:	ff 25 50 9b 04 08    	jmp    DWORD PTR ds:0x8049b50
 80484b6:	68 00 00 00 00       	push   0x0
 80484bb:	e9 e0 ff ff ff       	jmp    80484a0 <.plt>

080484c0 <__gmon_start__@plt>:
 80484c0:	ff 25 54 9b 04 08    	jmp    DWORD PTR ds:0x8049b54
 80484c6:	68 08 00 00 00       	push   0x8
 80484cb:	e9 d0 ff ff ff       	jmp    80484a0 <.plt>

080484d0 <_ZNSt8ios_base4InitC1Ev@plt>:
 80484d0:	ff 25 58 9b 04 08    	jmp    DWORD PTR ds:0x8049b58
 80484d6:	68 10 00 00 00       	push   0x10
 80484db:	e9 c0 ff ff ff       	jmp    80484a0 <.plt>

080484e0 <__libc_start_main@plt>:
 80484e0:	ff 25 5c 9b 04 08    	jmp    DWORD PTR ds:0x8049b5c
 80484e6:	68 18 00 00 00       	push   0x18
 80484eb:	e9 b0 ff ff ff       	jmp    80484a0 <.plt>

080484f0 <_exit@plt>:
 80484f0:	ff 25 60 9b 04 08    	jmp    DWORD PTR ds:0x8049b60
 80484f6:	68 20 00 00 00       	push   0x20
 80484fb:	e9 a0 ff ff ff       	jmp    80484a0 <.plt>

08048500 <_ZNSt8ios_base4InitD1Ev@plt>:
 8048500:	ff 25 64 9b 04 08    	jmp    DWORD PTR ds:0x8049b64
 8048506:	68 28 00 00 00       	push   0x28
 804850b:	e9 90 ff ff ff       	jmp    80484a0 <.plt>

08048510 <memcpy@plt>:
 8048510:	ff 25 68 9b 04 08    	jmp    DWORD PTR ds:0x8049b68
 8048516:	68 30 00 00 00       	push   0x30
 804851b:	e9 80 ff ff ff       	jmp    80484a0 <.plt>

08048520 <strlen@plt>:
 8048520:	ff 25 6c 9b 04 08    	jmp    DWORD PTR ds:0x8049b6c
 8048526:	68 38 00 00 00       	push   0x38
 804852b:	e9 70 ff ff ff       	jmp    80484a0 <.plt>

08048530 <_Znwj@plt>:
 8048530:	ff 25 70 9b 04 08    	jmp    DWORD PTR ds:0x8049b70
 8048536:	68 40 00 00 00       	push   0x40
 804853b:	e9 60 ff ff ff       	jmp    80484a0 <.plt>

Disassembly of section .text:

080485f4 <main>:
 80485f4:	55                   	push   ebp
 80485f5:	89 e5                	mov    ebp,esp
 80485f7:	53                   	push   ebx
 80485f8:	83 e4 f0             	and    esp,0xfffffff0
 80485fb:	83 ec 20             	sub    esp,0x20
 80485fe:	83 7d 08 01          	cmp    DWORD PTR [ebp+0x8],0x1
 8048602:	7f 0c                	jg     8048610 <main+0x1c>
 8048604:	c7 04 24 01 00 00 00 	mov    DWORD PTR [esp],0x1
 804860b:	e8 e0 fe ff ff       	call   80484f0 <_exit@plt>
 8048610:	c7 04 24 6c 00 00 00 	mov    DWORD PTR [esp],0x6c
 8048617:	e8 14 ff ff ff       	call   8048530 <_Znwj@plt>
 804861c:	89 c3                	mov    ebx,eax
 804861e:	c7 44 24 04 05 00 00 	mov    DWORD PTR [esp+0x4],0x5
 8048625:	00 
 8048626:	89 1c 24             	mov    DWORD PTR [esp],ebx
 8048629:	e8 c8 00 00 00       	call   80486f6 <_ZN1NC1Ei>
 804862e:	89 5c 24 1c          	mov    DWORD PTR [esp+0x1c],ebx
 8048632:	c7 04 24 6c 00 00 00 	mov    DWORD PTR [esp],0x6c
 8048639:	e8 f2 fe ff ff       	call   8048530 <_Znwj@plt>
 804863e:	89 c3                	mov    ebx,eax
 8048640:	c7 44 24 04 06 00 00 	mov    DWORD PTR [esp+0x4],0x6
 8048647:	00 
 8048648:	89 1c 24             	mov    DWORD PTR [esp],ebx
 804864b:	e8 a6 00 00 00       	call   80486f6 <_ZN1NC1Ei>
 8048650:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
 8048654:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
 8048658:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
 804865c:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
 8048660:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
 8048664:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 8048667:	83 c0 04             	add    eax,0x4
 804866a:	8b 00                	mov    eax,DWORD PTR [eax]
 804866c:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048670:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
 8048674:	89 04 24             	mov    DWORD PTR [esp],eax
 8048677:	e8 92 00 00 00       	call   804870e <_ZN1N13setAnnotationEPc>
 804867c:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
 8048680:	8b 00                	mov    eax,DWORD PTR [eax]
 8048682:	8b 10                	mov    edx,DWORD PTR [eax]
 8048684:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
 8048688:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 804868c:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
 8048690:	89 04 24             	mov    DWORD PTR [esp],eax
 8048693:	ff d2                	call   edx
 8048695:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 8048698:	c9                   	leave  
 8048699:	c3                   	ret    

080486f6 <_ZN1NC1Ei>:
 80486f6:	55                   	push   ebp
 80486f7:	89 e5                	mov    ebp,esp
 80486f9:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 80486fc:	c7 00 48 88 04 08    	mov    DWORD PTR [eax],0x8048848
 8048702:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048705:	8b 55 0c             	mov    edx,DWORD PTR [ebp+0xc]
 8048708:	89 50 68             	mov    DWORD PTR [eax+0x68],edx
 804870b:	5d                   	pop    ebp
 804870c:	c3                   	ret    
 804870d:	90                   	nop

0804870e <_ZN1N13setAnnotationEPc>:
 804870e:	55                   	push   ebp
 804870f:	89 e5                	mov    ebp,esp
 8048711:	83 ec 18             	sub    esp,0x18
 8048714:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 8048717:	89 04 24             	mov    DWORD PTR [esp],eax
 804871a:	e8 01 fe ff ff       	call   8048520 <strlen@plt>
 804871f:	8b 55 08             	mov    edx,DWORD PTR [ebp+0x8]
 8048722:	83 c2 04             	add    edx,0x4
 8048725:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
 8048729:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 804872c:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
 8048730:	89 14 24             	mov    DWORD PTR [esp],edx
 8048733:	e8 d8 fd ff ff       	call   8048510 <memcpy@plt>
 8048738:	c9                   	leave  
 8048739:	c3                   	ret    

0804873a <_ZN1NplERS_>:
 804873a:	55                   	push   ebp
 804873b:	89 e5                	mov    ebp,esp
 804873d:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048740:	8b 50 68             	mov    edx,DWORD PTR [eax+0x68]
 8048743:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 8048746:	8b 40 68             	mov    eax,DWORD PTR [eax+0x68]
 8048749:	01 d0                	add    eax,edx
 804874b:	5d                   	pop    ebp
 804874c:	c3                   	ret    
 804874d:	90                   	nop

0804874e <_ZN1NmiERS_>:
 804874e:	55                   	push   ebp
 804874f:	89 e5                	mov    ebp,esp
 8048751:	8b 45 08             	mov    eax,DWORD PTR [ebp+0x8]
 8048754:	8b 50 68             	mov    edx,DWORD PTR [eax+0x68]
 8048757:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
 804875a:	8b 40 68             	mov    eax,DWORD PTR [eax+0x68]
 804875d:	89 d1                	mov    ecx,edx
 804875f:	29 c1                	sub    ecx,eax
 8048761:	89 c8                	mov    eax,ecx
 8048763:	5d                   	pop    ebp
 8048764:	c3                   	ret    
 8048765:	90                   	nop
 8048766:	90                   	nop
 8048767:	90                   	nop
 8048768:	90                   	nop
 8048769:	90                   	nop
 804876a:	90                   	nop
 804876b:	90                   	nop
 804876c:	90                   	nop
 804876d:	90                   	nop
 804876e:	90                   	nop
 804876f:	90                   	nop
