
edr_module.ko:     file format elf64-x86-64
edr_module.ko
architecture: i386:x86-64, flags 0x00000011:
HAS_RELOC, HAS_SYMS
start address 0x0000000000000000

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .note.gnu.build-id 00000024  0000000000000000  0000000000000000  00000040  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 .note.Linux   00000030  0000000000000000  0000000000000000  00000064  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  2 .text         00000257  0000000000000000  0000000000000000  000000a0  2**4
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  3 .text.unlikely 000001a6  0000000000000000  0000000000000000  000002f7  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  4 .init.text    00000220  0000000000000000  0000000000000000  0000049d  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  5 .exit.text    00000075  0000000000000000  0000000000000000  000006bd  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  6 __mcount_loc  00000048  0000000000000000  0000000000000000  00000732  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
  7 .rodata.str1.1 0000008f  0000000000000000  0000000000000000  0000077a  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  8 .rodata.str1.8 000003f3  0000000000000000  0000000000000000  00000810  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  9 .modinfo      000000f1  0000000000000000  0000000000000000  00000c03  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 10 .retpoline_sites 00000018  0000000000000000  0000000000000000  00000cf4  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
 11 .return_sites 00000038  0000000000000000  0000000000000000  00000d0c  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
 12 __versions    000003c0  0000000000000000  0000000000000000  00000d60  2**5
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 13 .data         00000518  0000000000000000  0000000000000000  00001120  2**5
                  CONTENTS, ALLOC, LOAD, RELOC, DATA
 14 _kprobe_blacklist 00000008  0000000000000000  0000000000000000  00001638  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, DATA
 15 .gnu.linkonce.this_module 00000380  0000000000000000  0000000000000000  00001640  2**6
                  CONTENTS, ALLOC, LOAD, RELOC, DATA, LINK_ONCE_DISCARD
 16 .bss          00000034  0000000000000000  0000000000000000  000019c0  2**3
                  ALLOC
 17 .debug_info   00019c16  0000000000000000  0000000000000000  000019c0  2**0
                  CONTENTS, RELOC, READONLY, DEBUGGING, OCTETS
 18 .debug_abbrev 00000f73  0000000000000000  0000000000000000  0001b5d6  2**0
                  CONTENTS, READONLY, DEBUGGING, OCTETS
 19 .debug_loc    000011ab  0000000000000000  0000000000000000  0001c549  2**0
                  CONTENTS, RELOC, READONLY, DEBUGGING, OCTETS
 20 .debug_aranges 00000080  0000000000000000  0000000000000000  0001d6f4  2**0
                  CONTENTS, RELOC, READONLY, DEBUGGING, OCTETS
 21 .debug_ranges 00000460  0000000000000000  0000000000000000  0001d774  2**0
                  CONTENTS, RELOC, READONLY, DEBUGGING, OCTETS
 22 .debug_line   00001b40  0000000000000000  0000000000000000  0001dbd4  2**0
                  CONTENTS, RELOC, READONLY, DEBUGGING, OCTETS
 23 .debug_str    00011d1d  0000000000000000  0000000000000000  0001f714  2**0
                  CONTENTS, READONLY, DEBUGGING, OCTETS
 24 .comment      00000058  0000000000000000  0000000000000000  00031431  2**0
                  CONTENTS, READONLY
 25 .note.GNU-stack 00000000  0000000000000000  0000000000000000  00031489  2**0
                  CONTENTS, READONLY
 26 .debug_frame  000002c0  0000000000000000  0000000000000000  00031490  2**3
                  CONTENTS, RELOC, READONLY, DEBUGGING, OCTETS
SYMBOL TABLE:
0000000000000000 l    d  .note.gnu.build-id	0000000000000000 .note.gnu.build-id
0000000000000000 l    d  .note.Linux	0000000000000000 .note.Linux
0000000000000000 l    d  .text	0000000000000000 .text
0000000000000000 l    d  .text.unlikely	0000000000000000 .text.unlikely
0000000000000000 l    d  .init.text	0000000000000000 .init.text
0000000000000000 l    d  .exit.text	0000000000000000 .exit.text
0000000000000000 l    d  __mcount_loc	0000000000000000 __mcount_loc
0000000000000000 l    d  .rodata.str1.1	0000000000000000 .rodata.str1.1
0000000000000000 l    d  .rodata.str1.8	0000000000000000 .rodata.str1.8
0000000000000000 l    d  .modinfo	0000000000000000 .modinfo
0000000000000000 l    d  .retpoline_sites	0000000000000000 .retpoline_sites
0000000000000000 l    d  .return_sites	0000000000000000 .return_sites
0000000000000000 l    d  __versions	0000000000000000 __versions
0000000000000000 l    d  .data	0000000000000000 .data
0000000000000000 l    d  _kprobe_blacklist	0000000000000000 _kprobe_blacklist
0000000000000000 l    d  .gnu.linkonce.this_module	0000000000000000 .gnu.linkonce.this_module
0000000000000000 l    d  .bss	0000000000000000 .bss
0000000000000000 l    d  .debug_info	0000000000000000 .debug_info
0000000000000000 l    d  .debug_abbrev	0000000000000000 .debug_abbrev
0000000000000000 l    d  .debug_loc	0000000000000000 .debug_loc
0000000000000000 l    d  .debug_aranges	0000000000000000 .debug_aranges
0000000000000000 l    d  .debug_ranges	0000000000000000 .debug_ranges
0000000000000000 l    d  .debug_line	0000000000000000 .debug_line
0000000000000000 l    d  .debug_str	0000000000000000 .debug_str
0000000000000000 l    d  .comment	0000000000000000 .comment
0000000000000000 l    d  .note.GNU-stack	0000000000000000 .note.GNU-stack
0000000000000000 l    d  .debug_frame	0000000000000000 .debug_frame
0000000000000000 l    df *ABS*	0000000000000000 edr_module.mod.c
0000000000000071 l     O .modinfo	0000000000000023 __UNIQUE_ID_srcversion124
0000000000000094 l     O .modinfo	0000000000000009 __UNIQUE_ID_depends123
0000000000000000 l     O __versions	00000000000003c0 ____versions
000000000000009d l     O .modinfo	000000000000000c __UNIQUE_ID_retpoline122
00000000000000a9 l     O .modinfo	0000000000000010 __UNIQUE_ID_name121
00000000000000b9 l     O .modinfo	0000000000000038 __UNIQUE_ID_vermagic120
0000000000000000 l     O .note.Linux	0000000000000018 _note_9
0000000000000018 l     O .note.Linux	0000000000000018 _note_8
0000000000000000 l    df *ABS*	0000000000000000 edr_module.c
0000000000000030 l     F .text	000000000000001b file_hooked_unlink
0000000000000008 l     O .bss	0000000000000008 file_original_unlink
0000000000000050 l     F .text	000000000000001b file_hooked_execvet
0000000000000000 l     O .bss	0000000000000008 file_original_execvet
0000000000000070 l     F .text	000000000000006a file_hooked_write
0000000000000010 l     O .bss	0000000000000008 file_original_write
0000000000000000 l     F .text.unlikely	0000000000000034 file_hooked_write.cold
00000000000000e0 l     F .text	000000000000006a file_hooked_read
0000000000000018 l     O .bss	0000000000000008 file_original_read
0000000000000034 l     F .text.unlikely	0000000000000034 file_hooked_read.cold
0000000000000150 l     F .text	0000000000000058 common_dispatcher
0000000000000030 l     O .bss	0000000000000004 module_unloading
00000000000001b0 l     F .text	00000000000000a7 file_hooked_open
0000000000000020 l     O .bss	0000000000000008 file_original_open
0000000000000068 l     F .text.unlikely	0000000000000049 file_hooked_open.cold
00000000000000b1 l     F .text.unlikely	00000000000000c3 get_func_kallsyms_lookup_name
0000000000000028 l     O .bss	0000000000000008 _kallsyms_lookup_name
0000000000000174 l     F .text.unlikely	0000000000000032 remove_hook
0000000000000000 l     F .init.text	0000000000000220 ftrace_hook_init
00000000000000e0 l     O .data	0000000000000438 hooks
0000000000000000 l     F .exit.text	0000000000000075 ftrace_hook_exit
0000000000000000 l     O .modinfo	000000000000000c __UNIQUE_ID_version266
000000000000000c l     O .modinfo	0000000000000038 __UNIQUE_ID_description265
0000000000000044 l     O .modinfo	0000000000000021 __UNIQUE_ID_author264
0000000000000065 l     O .modinfo	000000000000000c __UNIQUE_ID_license263
0000000000000000 l     O _kprobe_blacklist	0000000000000008 _kbl_addr_override_function_with_return
0000000000000000         *UND*	0000000000000000 _copy_from_user
0000000000000000 g     O .gnu.linkonce.this_module	0000000000000380 __this_module
0000000000000000 g     F .exit.text	0000000000000075 cleanup_module
0000000000000000         *UND*	0000000000000000 register_ftrace_function
0000000000000000         *UND*	0000000000000000 __fentry__
0000000000000000 g     F .init.text	0000000000000220 init_module
0000000000000000         *UND*	0000000000000000 __x86_indirect_thunk_rax
0000000000000000         *UND*	0000000000000000 _printk
0000000000000000         *UND*	0000000000000000 __stack_chk_fail
0000000000000000         *UND*	0000000000000000 __ubsan_handle_out_of_bounds
0000000000000000         *UND*	0000000000000000 ftrace_set_filter_ip
0000000000000000         *UND*	0000000000000000 unregister_ftrace_function
0000000000000000         *UND*	0000000000000000 strstr
0000000000000000         *UND*	0000000000000000 current_task
0000000000000000         *UND*	0000000000000000 __x86_return_thunk
0000000000000000 g     F .text	0000000000000005 just_return_func
0000000000000000         *UND*	0000000000000000 register_kprobe
0000000000000000         *UND*	0000000000000000 unregister_kprobe
0000000000000010 g     F .text	000000000000001a override_function_with_return



Disassembly of section .text:

0000000000000000 <just_return_func>:
   0:	e9 00 00 00 00       	jmpq   5 <just_return_func+0x5>
			1: R_X86_64_PLT32	__x86_return_thunk-0x4
   5:	66 66 2e 0f 1f 84 00 	data16 nopw %cs:0x0(%rax,%rax,1)
   c:	00 00 00 00 

0000000000000010 <override_function_with_return>:
  10:	e8 00 00 00 00       	callq  15 <override_function_with_return+0x5>
			11: R_X86_64_PLT32	__fentry__-0x4
  15:	55                   	push   %rbp
  16:	48 c7 87 80 00 00 00 	movq   $0x0,0x80(%rdi)
  1d:	00 00 00 00 
			1d: R_X86_64_32S	just_return_func
  21:	48 89 e5             	mov    %rsp,%rbp
  24:	5d                   	pop    %rbp
  25:	e9 00 00 00 00       	jmpq   2a <override_function_with_return+0x1a>
			26: R_X86_64_PLT32	__x86_return_thunk-0x4
  2a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000000030 <file_hooked_unlink>:
  30:	e8 00 00 00 00       	callq  35 <file_hooked_unlink+0x5>
			31: R_X86_64_PLT32	__fentry__-0x4
  35:	55                   	push   %rbp
  36:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 3d <file_hooked_unlink+0xd>
			39: R_X86_64_PC32	.bss+0x4
  3d:	48 89 e5             	mov    %rsp,%rbp
  40:	e8 00 00 00 00       	callq  45 <file_hooked_unlink+0x15>
			41: R_X86_64_PLT32	__x86_indirect_thunk_rax-0x4
  45:	5d                   	pop    %rbp
  46:	e9 00 00 00 00       	jmpq   4b <file_hooked_unlink+0x1b>
			47: R_X86_64_PLT32	__x86_return_thunk-0x4
  4b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

0000000000000050 <file_hooked_execvet>:
  50:	e8 00 00 00 00       	callq  55 <file_hooked_execvet+0x5>
			51: R_X86_64_PLT32	__fentry__-0x4
  55:	55                   	push   %rbp
  56:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 5d <file_hooked_execvet+0xd>
			59: R_X86_64_PC32	.bss-0x4
  5d:	48 89 e5             	mov    %rsp,%rbp
  60:	e8 00 00 00 00       	callq  65 <file_hooked_execvet+0x15>
			61: R_X86_64_PLT32	__x86_indirect_thunk_rax-0x4
  65:	5d                   	pop    %rbp
  66:	e9 00 00 00 00       	jmpq   6b <file_hooked_execvet+0x1b>
			67: R_X86_64_PLT32	__x86_return_thunk-0x4
  6b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

0000000000000070 <file_hooked_write>:
  70:	e8 00 00 00 00       	callq  75 <file_hooked_write+0x5>
			71: R_X86_64_PLT32	__fentry__-0x4
  75:	55                   	push   %rbp
  76:	48 89 e5             	mov    %rsp,%rbp
  79:	41 57                	push   %r15
  7b:	49 89 cf             	mov    %rcx,%r15
  7e:	41 56                	push   %r14
  80:	49 89 d6             	mov    %rdx,%r14
  83:	41 55                	push   %r13
  85:	49 89 f5             	mov    %rsi,%r13
  88:	41 54                	push   %r12
  8a:	49 89 fc             	mov    %rdi,%r12
  8d:	53                   	push   %rbx
  8e:	48 8b 47 18          	mov    0x18(%rdi),%rax
  92:	48 8b 58 28          	mov    0x28(%rax),%rbx
  96:	48 85 db             	test   %rbx,%rbx
  99:	74 18                	je     b3 <file_hooked_write+0x43>
  9b:	48 c7 c6 00 00 00 00 	mov    $0x0,%rsi
			9e: R_X86_64_32S	.rodata.str1.1
  a2:	48 89 df             	mov    %rbx,%rdi
  a5:	e8 00 00 00 00       	callq  aa <file_hooked_write+0x3a>
			a6: R_X86_64_PLT32	strstr-0x4
  aa:	48 85 c0             	test   %rax,%rax
  ad:	0f 85 00 00 00 00    	jne    b3 <file_hooked_write+0x43>
			af: R_X86_64_PC32	.text.unlikely-0x4
  b3:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # ba <file_hooked_write+0x4a>
			b6: R_X86_64_PC32	.bss+0xc
  ba:	4c 89 f9             	mov    %r15,%rcx
  bd:	4c 89 f2             	mov    %r14,%rdx
  c0:	4c 89 ee             	mov    %r13,%rsi
  c3:	4c 89 e7             	mov    %r12,%rdi
  c6:	e8 00 00 00 00       	callq  cb <file_hooked_write+0x5b>
			c7: R_X86_64_PLT32	__x86_indirect_thunk_rax-0x4
  cb:	5b                   	pop    %rbx
  cc:	41 5c                	pop    %r12
  ce:	41 5d                	pop    %r13
  d0:	41 5e                	pop    %r14
  d2:	41 5f                	pop    %r15
  d4:	5d                   	pop    %rbp
  d5:	e9 00 00 00 00       	jmpq   da <file_hooked_write+0x6a>
			d6: R_X86_64_PLT32	__x86_return_thunk-0x4
  da:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

00000000000000e0 <file_hooked_read>:
  e0:	e8 00 00 00 00       	callq  e5 <file_hooked_read+0x5>
			e1: R_X86_64_PLT32	__fentry__-0x4
  e5:	55                   	push   %rbp
  e6:	48 89 e5             	mov    %rsp,%rbp
  e9:	41 57                	push   %r15
  eb:	49 89 cf             	mov    %rcx,%r15
  ee:	41 56                	push   %r14
  f0:	49 89 d6             	mov    %rdx,%r14
  f3:	41 55                	push   %r13
  f5:	49 89 f5             	mov    %rsi,%r13
  f8:	41 54                	push   %r12
  fa:	49 89 fc             	mov    %rdi,%r12
  fd:	53                   	push   %rbx
  fe:	48 8b 47 18          	mov    0x18(%rdi),%rax
 102:	48 8b 58 28          	mov    0x28(%rax),%rbx
 106:	48 85 db             	test   %rbx,%rbx
 109:	74 18                	je     123 <file_hooked_read+0x43>
 10b:	48 c7 c6 00 00 00 00 	mov    $0x0,%rsi
			10e: R_X86_64_32S	.rodata.str1.1+0xc
 112:	48 89 df             	mov    %rbx,%rdi
 115:	e8 00 00 00 00       	callq  11a <file_hooked_read+0x3a>
			116: R_X86_64_PLT32	strstr-0x4
 11a:	48 85 c0             	test   %rax,%rax
 11d:	0f 85 00 00 00 00    	jne    123 <file_hooked_read+0x43>
			11f: R_X86_64_PC32	.text.unlikely+0x30
 123:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 12a <file_hooked_read+0x4a>
			126: R_X86_64_PC32	.bss+0x14
 12a:	4c 89 f9             	mov    %r15,%rcx
 12d:	4c 89 f2             	mov    %r14,%rdx
 130:	4c 89 ee             	mov    %r13,%rsi
 133:	4c 89 e7             	mov    %r12,%rdi
 136:	e8 00 00 00 00       	callq  13b <file_hooked_read+0x5b>
			137: R_X86_64_PLT32	__x86_indirect_thunk_rax-0x4
 13b:	5b                   	pop    %rbx
 13c:	41 5c                	pop    %r12
 13e:	41 5d                	pop    %r13
 140:	41 5e                	pop    %r14
 142:	41 5f                	pop    %r15
 144:	5d                   	pop    %rbp
 145:	e9 00 00 00 00       	jmpq   14a <file_hooked_read+0x6a>
			146: R_X86_64_PLT32	__x86_return_thunk-0x4
 14a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000000150 <common_dispatcher>:
 150:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 156 <common_dispatcher+0x6>
			152: R_X86_64_PC32	.bss+0x2c
 156:	85 c0                	test   %eax,%eax
 158:	75 26                	jne    180 <common_dispatcher+0x30>
 15a:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 161 <common_dispatcher+0x11>
			15d: R_X86_64_PC32	__this_module+0x18c
 161:	48 39 c6             	cmp    %rax,%rsi
 164:	73 1f                	jae    185 <common_dispatcher+0x35>
 166:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 16d <common_dispatcher+0x1d>
			169: R_X86_64_PC32	__this_module+0x13c
 16d:	48 39 c6             	cmp    %rax,%rsi
 170:	72 26                	jb     198 <common_dispatcher+0x48>
 172:	8b 3d 00 00 00 00    	mov    0x0(%rip),%edi        # 178 <common_dispatcher+0x28>
			174: R_X86_64_PC32	__this_module+0x144
 178:	48 01 f8             	add    %rdi,%rax
 17b:	48 39 c6             	cmp    %rax,%rsi
 17e:	73 18                	jae    198 <common_dispatcher+0x48>
 180:	e9 00 00 00 00       	jmpq   185 <common_dispatcher+0x35>
			181: R_X86_64_PLT32	__x86_return_thunk-0x4
 185:	8b 3d 00 00 00 00    	mov    0x0(%rip),%edi        # 18b <common_dispatcher+0x3b>
			187: R_X86_64_PC32	__this_module+0x194
 18b:	48 01 f8             	add    %rdi,%rax
 18e:	48 39 c6             	cmp    %rax,%rsi
 191:	73 d3                	jae    166 <common_dispatcher+0x16>
 193:	e9 00 00 00 00       	jmpq   198 <common_dispatcher+0x48>
			194: R_X86_64_PLT32	__x86_return_thunk-0x4
 198:	48 8b 42 e8          	mov    -0x18(%rdx),%rax
 19c:	48 89 81 80 00 00 00 	mov    %rax,0x80(%rcx)
 1a3:	e9 00 00 00 00       	jmpq   1a8 <common_dispatcher+0x58>
			1a4: R_X86_64_PLT32	__x86_return_thunk-0x4
 1a8:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
 1af:	00 

00000000000001b0 <file_hooked_open>:
 1b0:	e8 00 00 00 00       	callq  1b5 <file_hooked_open+0x5>
			1b1: R_X86_64_PLT32	__fentry__-0x4
 1b5:	55                   	push   %rbp
 1b6:	48 89 e5             	mov    %rsp,%rbp
 1b9:	41 56                	push   %r14
 1bb:	41 89 d6             	mov    %edx,%r14d
 1be:	ba ff 00 00 00       	mov    $0xff,%edx
 1c3:	41 55                	push   %r13
 1c5:	41 89 fd             	mov    %edi,%r13d
 1c8:	48 8d bd d8 fe ff ff 	lea    -0x128(%rbp),%rdi
 1cf:	41 54                	push   %r12
 1d1:	49 89 f4             	mov    %rsi,%r12
 1d4:	53                   	push   %rbx
 1d5:	89 cb                	mov    %ecx,%ebx
 1d7:	48 81 ec 08 01 00 00 	sub    $0x108,%rsp
 1de:	65 48 8b 04 25 28 00 	mov    %gs:0x28,%rax
 1e5:	00 00 
 1e7:	48 89 45 d8          	mov    %rax,-0x28(%rbp)
 1eb:	31 c0                	xor    %eax,%eax
 1ed:	e8 00 00 00 00       	callq  1f2 <file_hooked_open+0x42>
			1ee: R_X86_64_PLT32	_copy_from_user-0x4
 1f2:	48 85 c0             	test   %rax,%rax
 1f5:	0f 85 00 00 00 00    	jne    1fb <file_hooked_open+0x4b>
			1f7: R_X86_64_PC32	.text.unlikely+0x9c
 1fb:	48 c7 c6 00 00 00 00 	mov    $0x0,%rsi
			1fe: R_X86_64_32S	.rodata.str1.1+0x17
 202:	48 8d bd d8 fe ff ff 	lea    -0x128(%rbp),%rdi
 209:	e8 00 00 00 00       	callq  20e <file_hooked_open+0x5e>
			20a: R_X86_64_PLT32	strstr-0x4
 20e:	48 85 c0             	test   %rax,%rax
 211:	0f 85 00 00 00 00    	jne    217 <file_hooked_open+0x67>
			213: R_X86_64_PC32	.text.unlikely+0x64
 217:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 21e <file_hooked_open+0x6e>
			21a: R_X86_64_PC32	.bss+0x1c
 21e:	0f b7 cb             	movzwl %bx,%ecx
 221:	44 89 f2             	mov    %r14d,%edx
 224:	4c 89 e6             	mov    %r12,%rsi
 227:	44 89 ef             	mov    %r13d,%edi
 22a:	e8 00 00 00 00       	callq  22f <file_hooked_open+0x7f>
			22b: R_X86_64_PLT32	__x86_indirect_thunk_rax-0x4
 22f:	48 8b 5d d8          	mov    -0x28(%rbp),%rbx
 233:	65 48 33 1c 25 28 00 	xor    %gs:0x28,%rbx
 23a:	00 00 
 23c:	75 14                	jne    252 <file_hooked_open+0xa2>
 23e:	48 81 c4 08 01 00 00 	add    $0x108,%rsp
 245:	5b                   	pop    %rbx
 246:	41 5c                	pop    %r12
 248:	41 5d                	pop    %r13
 24a:	41 5e                	pop    %r14
 24c:	5d                   	pop    %rbp
 24d:	e9 00 00 00 00       	jmpq   252 <file_hooked_open+0xa2>
			24e: R_X86_64_PLT32	__x86_return_thunk-0x4
 252:	e8 00 00 00 00       	callq  257 <file_hooked_open+0xa7>
			253: R_X86_64_PLT32	__stack_chk_fail-0x4

Disassembly of section .text.unlikely:

0000000000000000 <file_hooked_write.cold>:
   0:	65 48 8b 04 25 00 00 	mov    %gs:0x0,%rax
   7:	00 00 
			5: R_X86_64_32S	current_task
   9:	8b 88 c4 09 00 00    	mov    0x9c4(%rax),%ecx
   f:	8b 90 c0 09 00 00    	mov    0x9c0(%rax),%edx
  15:	48 8d b0 b8 0b 00 00 	lea    0xbb8(%rax),%rsi
  1c:	49 89 d8             	mov    %rbx,%r8
  1f:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			22: R_X86_64_32S	.rodata.str1.8
  26:	e8 00 00 00 00       	callq  2b <file_hooked_write.cold+0x2b>
			27: R_X86_64_PLT32	_printk-0x4
  2b:	48 83 c8 ff          	or     $0xffffffffffffffff,%rax
  2f:	e9 00 00 00 00       	jmpq   34 <file_hooked_read.cold>
			30: R_X86_64_PC32	.text+0xc7

0000000000000034 <file_hooked_read.cold>:
  34:	65 48 8b 04 25 00 00 	mov    %gs:0x0,%rax
  3b:	00 00 
			39: R_X86_64_32S	current_task
  3d:	8b 88 c4 09 00 00    	mov    0x9c4(%rax),%ecx
  43:	8b 90 c0 09 00 00    	mov    0x9c0(%rax),%edx
  49:	48 8d b0 b8 0b 00 00 	lea    0xbb8(%rax),%rsi
  50:	49 89 d8             	mov    %rbx,%r8
  53:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			56: R_X86_64_32S	.rodata.str1.8+0x48
  5a:	e8 00 00 00 00       	callq  5f <file_hooked_read.cold+0x2b>
			5b: R_X86_64_PLT32	_printk-0x4
  5f:	48 83 c8 ff          	or     $0xffffffffffffffff,%rax
  63:	e9 00 00 00 00       	jmpq   68 <file_hooked_open.cold>
			64: R_X86_64_PC32	.text+0x137

0000000000000068 <file_hooked_open.cold>:
  68:	65 48 8b 04 25 00 00 	mov    %gs:0x0,%rax
  6f:	00 00 
			6d: R_X86_64_32S	current_task
  71:	8b 88 c4 09 00 00    	mov    0x9c4(%rax),%ecx
  77:	8b 90 c0 09 00 00    	mov    0x9c0(%rax),%edx
  7d:	48 8d b0 b8 0b 00 00 	lea    0xbb8(%rax),%rsi
  84:	4c 8d 85 d8 fe ff ff 	lea    -0x128(%rbp),%r8
  8b:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			8e: R_X86_64_32S	.rodata.str1.8+0x90
  92:	e8 00 00 00 00       	callq  97 <file_hooked_open.cold+0x2f>
			93: R_X86_64_PLT32	_printk-0x4
  97:	48 83 c8 ff          	or     $0xffffffffffffffff,%rax
  9b:	e9 00 00 00 00       	jmpq   a0 <file_hooked_open.cold+0x38>
			9c: R_X86_64_PC32	.text+0x22b
  a0:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			a3: R_X86_64_32S	.rodata.str1.8+0xd8
  a7:	e8 00 00 00 00       	callq  ac <file_hooked_open.cold+0x44>
			a8: R_X86_64_PLT32	_printk-0x4
  ac:	e9 00 00 00 00       	jmpq   b1 <get_func_kallsyms_lookup_name>
			ad: R_X86_64_PC32	.text+0x213

00000000000000b1 <get_func_kallsyms_lookup_name>:
  b1:	e8 00 00 00 00       	callq  b6 <get_func_kallsyms_lookup_name+0x5>
			b2: R_X86_64_PLT32	__fentry__-0x4
  b6:	55                   	push   %rbp
  b7:	b9 20 00 00 00       	mov    $0x20,%ecx
  bc:	48 89 e5             	mov    %rsp,%rbp
  bf:	41 54                	push   %r12
  c1:	48 83 e4 f0          	and    $0xfffffffffffffff0,%rsp
  c5:	48 81 ec 90 00 00 00 	sub    $0x90,%rsp
  cc:	65 48 8b 04 25 28 00 	mov    %gs:0x28,%rax
  d3:	00 00 
  d5:	48 89 84 24 88 00 00 	mov    %rax,0x88(%rsp)
  dc:	00 
  dd:	31 c0                	xor    %eax,%eax
  df:	48 89 e7             	mov    %rsp,%rdi
  e2:	f3 ab                	rep stos %eax,%es:(%rdi)
  e4:	48 89 e7             	mov    %rsp,%rdi
  e7:	48 c7 44 24 30 00 00 	movq   $0x0,0x30(%rsp)
  ee:	00 00 
			ec: R_X86_64_32S	.rodata.str1.1+0x22
  f0:	e8 00 00 00 00       	callq  f5 <get_func_kallsyms_lookup_name+0x44>
			f1: R_X86_64_PLT32	register_kprobe-0x4
  f5:	85 c0                	test   %eax,%eax
  f7:	79 13                	jns    10c <get_func_kallsyms_lookup_name+0x5b>
  f9:	89 c6                	mov    %eax,%esi
  fb:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			fe: R_X86_64_32S	.rodata.str1.8+0x120
 102:	41 89 c4             	mov    %eax,%r12d
 105:	e8 00 00 00 00       	callq  10a <get_func_kallsyms_lookup_name+0x59>
			106: R_X86_64_PLT32	_printk-0x4
 10a:	eb 43                	jmp    14f <get_func_kallsyms_lookup_name+0x9e>
 10c:	48 8b 44 24 28       	mov    0x28(%rsp),%rax
 111:	48 89 e7             	mov    %rsp,%rdi
 114:	48 89 05 00 00 00 00 	mov    %rax,0x0(%rip)        # 11b <get_func_kallsyms_lookup_name+0x6a>
			117: R_X86_64_PC32	.bss+0x24
 11b:	e8 00 00 00 00       	callq  120 <get_func_kallsyms_lookup_name+0x6f>
			11c: R_X86_64_PLT32	unregister_kprobe-0x4
 120:	48 8b 35 00 00 00 00 	mov    0x0(%rip),%rsi        # 127 <get_func_kallsyms_lookup_name+0x76>
			123: R_X86_64_PC32	.bss+0x24
 127:	48 85 f6             	test   %rsi,%rsi
 12a:	75 14                	jne    140 <get_func_kallsyms_lookup_name+0x8f>
 12c:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			12f: R_X86_64_32S	.rodata.str1.8+0x170
 133:	41 bc fe ff ff ff    	mov    $0xfffffffe,%r12d
 139:	e8 00 00 00 00       	callq  13e <get_func_kallsyms_lookup_name+0x8d>
			13a: R_X86_64_PLT32	_printk-0x4
 13e:	eb 0f                	jmp    14f <get_func_kallsyms_lookup_name+0x9e>
 140:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			143: R_X86_64_32S	.rodata.str1.8+0x1b0
 147:	45 31 e4             	xor    %r12d,%r12d
 14a:	e8 00 00 00 00       	callq  14f <get_func_kallsyms_lookup_name+0x9e>
			14b: R_X86_64_PLT32	_printk-0x4
 14f:	48 8b 84 24 88 00 00 	mov    0x88(%rsp),%rax
 156:	00 
 157:	65 48 33 04 25 28 00 	xor    %gs:0x28,%rax
 15e:	00 00 
 160:	74 05                	je     167 <get_func_kallsyms_lookup_name+0xb6>
 162:	e8 00 00 00 00       	callq  167 <get_func_kallsyms_lookup_name+0xb6>
			163: R_X86_64_PLT32	__stack_chk_fail-0x4
 167:	44 89 e0             	mov    %r12d,%eax
 16a:	4c 8b 65 f8          	mov    -0x8(%rbp),%r12
 16e:	c9                   	leaveq 
 16f:	e9 00 00 00 00       	jmpq   174 <remove_hook>
			170: R_X86_64_PLT32	__x86_return_thunk-0x4

0000000000000174 <remove_hook>:
 174:	e8 00 00 00 00       	callq  179 <remove_hook+0x5>
			175: R_X86_64_PLT32	__fentry__-0x4
 179:	55                   	push   %rbp
 17a:	31 c9                	xor    %ecx,%ecx
 17c:	ba 01 00 00 00       	mov    $0x1,%edx
 181:	48 89 e5             	mov    %rsp,%rbp
 184:	41 54                	push   %r12
 186:	4c 8d 67 28          	lea    0x28(%rdi),%r12
 18a:	48 8b 77 20          	mov    0x20(%rdi),%rsi
 18e:	4c 89 e7             	mov    %r12,%rdi
 191:	e8 00 00 00 00       	callq  196 <remove_hook+0x22>
			192: R_X86_64_PLT32	ftrace_set_filter_ip-0x4
 196:	4c 89 e7             	mov    %r12,%rdi
 199:	e8 00 00 00 00       	callq  19e <remove_hook+0x2a>
			19a: R_X86_64_PLT32	unregister_ftrace_function-0x4
 19e:	41 5c                	pop    %r12
 1a0:	5d                   	pop    %rbp
 1a1:	e9 00 00 00 00       	jmpq   1a6 <remove_hook+0x32>
			1a2: R_X86_64_PLT32	__x86_return_thunk-0x4

Disassembly of section .init.text:

0000000000000000 <init_module>:
   0:	e8 00 00 00 00       	callq  5 <init_module+0x5>
			1: R_X86_64_PLT32	__fentry__-0x4
   5:	55                   	push   %rbp
   6:	48 89 e5             	mov    %rsp,%rbp
   9:	41 56                	push   %r14
   b:	41 55                	push   %r13
   d:	41 54                	push   %r12
   f:	53                   	push   %rbx
  10:	e8 00 00 00 00       	callq  15 <init_module+0x15>
			11: R_X86_64_PC32	.text.unlikely+0xad
  15:	85 c0                	test   %eax,%eax
  17:	74 16                	je     2f <init_module+0x2f>
  19:	89 c6                	mov    %eax,%esi
  1b:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			1e: R_X86_64_32S	.rodata.str1.8+0x1e8
  22:	41 89 c4             	mov    %eax,%r12d
  25:	e8 00 00 00 00       	callq  2a <init_module+0x2a>
			26: R_X86_64_PLT32	_printk-0x4
  2a:	e9 e1 01 00 00       	jmpq   210 <init_module+0x210>
  2f:	49 c7 c5 00 00 00 00 	mov    $0x0,%r13
			32: R_X86_64_32S	.data+0xe0
  36:	31 db                	xor    %ebx,%ebx
  38:	48 83 fb 05          	cmp    $0x5,%rbx
  3c:	76 0f                	jbe    4d <init_module+0x4d>
  3e:	48 89 de             	mov    %rbx,%rsi
  41:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			44: R_X86_64_32S	.data+0x80
  48:	e8 00 00 00 00       	callq  4d <init_module+0x4d>
			49: R_X86_64_PLT32	__ubsan_handle_out_of_bounds-0x4
  4d:	49 8b 7d 00          	mov    0x0(%r13),%rdi
  51:	48 8b 05 00 00 00 00 	mov    0x0(%rip),%rax        # 58 <init_module+0x58>
			54: R_X86_64_PC32	.bss+0x24
  58:	e8 00 00 00 00       	callq  5d <init_module+0x5d>
			59: R_X86_64_PLT32	__x86_indirect_thunk_rax-0x4
  5d:	49 89 45 20          	mov    %rax,0x20(%r13)
  61:	48 85 c0             	test   %rax,%rax
  64:	75 25                	jne    8b <init_module+0x8b>
  66:	48 69 c3 d8 00 00 00 	imul   $0xd8,%rbx,%rax
  6d:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			70: R_X86_64_32S	.rodata.str1.8+0x228
  74:	41 bc fe ff ff ff    	mov    $0xfffffffe,%r12d
  7a:	48 8b b0 00 00 00 00 	mov    0x0(%rax),%rsi
			7d: R_X86_64_32S	.data+0xe0
  81:	e8 00 00 00 00       	callq  86 <init_module+0x86>
			82: R_X86_64_PLT32	_printk-0x4
  86:	e9 23 01 00 00       	jmpq   1ae <init_module+0x1ae>
  8b:	49 8b 55 18          	mov    0x18(%r13),%rdx
  8f:	48 89 02             	mov    %rax,(%rdx)
  92:	49 83 7d 10 00       	cmpq   $0x0,0x10(%r13)
  97:	75 25                	jne    be <init_module+0xbe>
  99:	48 69 c3 d8 00 00 00 	imul   $0xd8,%rbx,%rax
  a0:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			a3: R_X86_64_32S	.rodata.str1.8+0x250
  a7:	41 bc ea ff ff ff    	mov    $0xffffffea,%r12d
  ad:	48 8b b0 00 00 00 00 	mov    0x0(%rax),%rsi
			b0: R_X86_64_32S	.data+0xe0
  b4:	e8 00 00 00 00       	callq  b9 <init_module+0xb9>
			b5: R_X86_64_PLT32	_printk-0x4
  b9:	e9 f0 00 00 00       	jmpq   1ae <init_module+0x1ae>
  be:	49 83 7d 18 00       	cmpq   $0x0,0x18(%r13)
  c3:	75 25                	jne    ea <init_module+0xea>
  c5:	48 69 c3 d8 00 00 00 	imul   $0xd8,%rbx,%rax
  cc:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			cf: R_X86_64_32S	.rodata.str1.8+0x288
  d3:	41 bc ea ff ff ff    	mov    $0xffffffea,%r12d
  d9:	48 8b b0 00 00 00 00 	mov    0x0(%rax),%rsi
			dc: R_X86_64_32S	.data+0xe0
  e0:	e8 00 00 00 00       	callq  e5 <init_module+0xe5>
			e1: R_X86_64_PLT32	_printk-0x4
  e5:	e9 c4 00 00 00       	jmpq   1ae <init_module+0x1ae>
  ea:	49 8b 45 08          	mov    0x8(%r13),%rax
  ee:	48 85 c0             	test   %rax,%rax
  f1:	75 25                	jne    118 <init_module+0x118>
  f3:	48 69 c3 d8 00 00 00 	imul   $0xd8,%rbx,%rax
  fa:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			fd: R_X86_64_32S	.rodata.str1.8+0x2c0
 101:	41 bc ea ff ff ff    	mov    $0xffffffea,%r12d
 107:	48 8b b0 00 00 00 00 	mov    0x0(%rax),%rsi
			10a: R_X86_64_32S	.data+0xe0
 10e:	e8 00 00 00 00       	callq  113 <init_module+0x113>
			10f: R_X86_64_PLT32	_printk-0x4
 113:	e9 96 00 00 00       	jmpq   1ae <init_module+0x1ae>
 118:	49 8b 75 20          	mov    0x20(%r13),%rsi
 11c:	4d 8d 75 28          	lea    0x28(%r13),%r14
 120:	31 c9                	xor    %ecx,%ecx
 122:	31 d2                	xor    %edx,%edx
 124:	4c 89 f7             	mov    %r14,%rdi
 127:	49 89 45 28          	mov    %rax,0x28(%r13)
 12b:	49 c7 45 38 04 10 00 	movq   $0x1004,0x38(%r13)
 132:	00 
 133:	e8 00 00 00 00       	callq  138 <init_module+0x138>
			134: R_X86_64_PLT32	ftrace_set_filter_ip-0x4
 138:	41 89 c4             	mov    %eax,%r12d
 13b:	85 c0                	test   %eax,%eax
 13d:	74 10                	je     14f <init_module+0x14f>
 13f:	89 c6                	mov    %eax,%esi
 141:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			144: R_X86_64_32S	.rodata.str1.8+0x300
 148:	e8 00 00 00 00       	callq  14d <init_module+0x14d>
			149: R_X86_64_PLT32	_printk-0x4
 14d:	eb 5f                	jmp    1ae <init_module+0x1ae>
 14f:	4c 89 f7             	mov    %r14,%rdi
 152:	e8 00 00 00 00       	callq  157 <init_module+0x157>
			153: R_X86_64_PLT32	register_ftrace_function-0x4
 157:	41 89 c4             	mov    %eax,%r12d
 15a:	85 c0                	test   %eax,%eax
 15c:	75 25                	jne    183 <init_module+0x183>
 15e:	48 ff c3             	inc    %rbx
 161:	49 81 c5 d8 00 00 00 	add    $0xd8,%r13
 168:	48 83 fb 05          	cmp    $0x5,%rbx
 16c:	0f 85 c6 fe ff ff    	jne    38 <init_module+0x38>
 172:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			175: R_X86_64_32S	.rodata.str1.8+0x398
 179:	e8 00 00 00 00       	callq  17e <init_module+0x17e>
			17a: R_X86_64_PLT32	_printk-0x4
 17e:	e9 8d 00 00 00       	jmpq   210 <init_module+0x210>
 183:	89 c6                	mov    %eax,%esi
 185:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			188: R_X86_64_32S	.rodata.str1.8+0x330
 18c:	e8 00 00 00 00       	callq  191 <init_module+0x191>
			18d: R_X86_64_PLT32	_printk-0x4
 191:	31 c9                	xor    %ecx,%ecx
 193:	ba 01 00 00 00       	mov    $0x1,%edx
 198:	4c 89 f7             	mov    %r14,%rdi
 19b:	48 69 c3 d8 00 00 00 	imul   $0xd8,%rbx,%rax
 1a2:	48 8b b0 00 00 00 00 	mov    0x0(%rax),%rsi
			1a5: R_X86_64_32S	.data+0x100
 1a9:	e8 00 00 00 00       	callq  1ae <init_module+0x1ae>
			1aa: R_X86_64_PLT32	ftrace_set_filter_ip-0x4
 1ae:	48 83 fb 04          	cmp    $0x4,%rbx
 1b2:	76 0f                	jbe    1c3 <init_module+0x1c3>
 1b4:	48 89 de             	mov    %rbx,%rsi
 1b7:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			1ba: R_X86_64_32S	.data+0x60
 1be:	e8 00 00 00 00       	callq  1c3 <init_module+0x1c3>
			1bf: R_X86_64_PLT32	__ubsan_handle_out_of_bounds-0x4
 1c3:	48 69 c3 d8 00 00 00 	imul   $0xd8,%rbx,%rax
 1ca:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			1cd: R_X86_64_32S	.rodata.str1.8+0x368
 1d1:	48 8b b0 00 00 00 00 	mov    0x0(%rax),%rsi
			1d4: R_X86_64_32S	.data+0xe0
 1d8:	e8 00 00 00 00       	callq  1dd <init_module+0x1dd>
			1d9: R_X86_64_PLT32	_printk-0x4
 1dd:	48 ff cb             	dec    %rbx
 1e0:	48 83 fb ff          	cmp    $0xffffffffffffffff,%rbx
 1e4:	74 2a                	je     210 <init_module+0x210>
 1e6:	48 83 fb 05          	cmp    $0x5,%rbx
 1ea:	76 0f                	jbe    1fb <init_module+0x1fb>
 1ec:	48 89 de             	mov    %rbx,%rsi
 1ef:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			1f2: R_X86_64_32S	.data+0x40
 1f6:	e8 00 00 00 00       	callq  1fb <init_module+0x1fb>
			1f7: R_X86_64_PLT32	__ubsan_handle_out_of_bounds-0x4
 1fb:	48 69 fb d8 00 00 00 	imul   $0xd8,%rbx,%rdi
 202:	48 81 c7 00 00 00 00 	add    $0x0,%rdi
			205: R_X86_64_32S	.data+0xe0
 209:	e8 00 00 00 00       	callq  20e <init_module+0x20e>
			20a: R_X86_64_PC32	.text.unlikely+0x170
 20e:	eb cd                	jmp    1dd <init_module+0x1dd>
 210:	5b                   	pop    %rbx
 211:	44 89 e0             	mov    %r12d,%eax
 214:	41 5c                	pop    %r12
 216:	41 5d                	pop    %r13
 218:	41 5e                	pop    %r14
 21a:	5d                   	pop    %rbp
 21b:	e9 00 00 00 00       	jmpq   220 <file_hooked_open+0x70>
			21c: R_X86_64_PLT32	__x86_return_thunk-0x4

Disassembly of section .exit.text:

0000000000000000 <cleanup_module>:
   0:	55                   	push   %rbp
   1:	c7 05 00 00 00 00 01 	movl   $0x1,0x0(%rip)        # b <cleanup_module+0xb>
   8:	00 00 00 
			3: R_X86_64_PC32	.bss+0x28
   b:	48 89 e5             	mov    %rsp,%rbp
   e:	41 54                	push   %r12
  10:	49 c7 c4 00 00 00 00 	mov    $0x0,%r12
			13: R_X86_64_32S	.data+0xe0
  17:	53                   	push   %rbx
  18:	31 db                	xor    %ebx,%ebx
  1a:	48 83 fb 05          	cmp    $0x5,%rbx
  1e:	76 0f                	jbe    2f <cleanup_module+0x2f>
  20:	48 89 de             	mov    %rbx,%rsi
  23:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			26: R_X86_64_32S	.data+0x20
  2a:	e8 00 00 00 00       	callq  2f <cleanup_module+0x2f>
			2b: R_X86_64_PLT32	__ubsan_handle_out_of_bounds-0x4
  2f:	4c 89 e7             	mov    %r12,%rdi
  32:	e8 00 00 00 00       	callq  37 <cleanup_module+0x37>
			33: R_X86_64_PC32	.text.unlikely+0x170
  37:	48 83 fb 04          	cmp    $0x4,%rbx
  3b:	76 0f                	jbe    4c <cleanup_module+0x4c>
  3d:	48 89 de             	mov    %rbx,%rsi
  40:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			43: R_X86_64_32S	.data
  47:	e8 00 00 00 00       	callq  4c <cleanup_module+0x4c>
			48: R_X86_64_PLT32	__ubsan_handle_out_of_bounds-0x4
  4c:	49 8b 34 24          	mov    (%r12),%rsi
  50:	48 c7 c7 00 00 00 00 	mov    $0x0,%rdi
			53: R_X86_64_32S	.rodata.str1.8+0x3d0
  57:	48 ff c3             	inc    %rbx
  5a:	49 81 c4 d8 00 00 00 	add    $0xd8,%r12
  61:	e8 00 00 00 00       	callq  66 <cleanup_module+0x66>
			62: R_X86_64_PLT32	_printk-0x4
  66:	48 83 fb 05          	cmp    $0x5,%rbx
  6a:	75 ae                	jne    1a <cleanup_module+0x1a>
  6c:	5b                   	pop    %rbx
  6d:	41 5c                	pop    %r12
  6f:	5d                   	pop    %rbp
  70:	e9 00 00 00 00       	jmpq   75 <__UNIQUE_ID_srcversion124+0x4>
			71: R_X86_64_PLT32	__x86_return_thunk-0x4
