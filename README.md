Name: SAHIL PATIL
ASU ID: 1217436155

Assignment 4: A Dynamic Stack Dumping in Linux Kernel

Custom System Calls

Steps to use the software:

0. Patch the kernel source code using new.patch file provided.
	
	run the following command inside the kernel directory to patch the kernel:

	patch -p1 <path_to_patch_file>/new.patch

1. Build the patched kernel

	a. Include the cross-compilation tools in your PATH using following command:

		export PATH=path_to_sdk/sysroots/x86_64-pokysdk-linux/usr/bin/i586-poky-linux:$PATH

	b. Cross-compile the kernel:

		ARCH=x86LOCALVERSION= CROSS_COMPILE=i586-poky-linux- make -j4

		Note: if you want to enable CONFIG_DYNAMIC_DUMP_STACK say 'y' when it prompts (while building)
		Note: you can also use .config file for configuration of CONFIG_DYNAMIC_DUMP_STACK

	c. Build and extract the kernel modules from the build to a target directory (e.g ../galileo-install)

		ARCH=x86LOCALVERSION=  INSTALL_MOD_PATH=../galileo-install
		CROSS_COMPILE=i586-poky-linux- make modules_install

	d. Extract the kernel image (bzImage) from the arch/x86/boot/ to a target directory (e.g ../galileo-install)

		cp arch/x86/boot/bzImage ../galileo-install/

	e. Install the new kernel and modules from the target directory (e.g ../galileo-install) to your micro SD card

		i. Replace the bzImage found in the first partition (ESP) of your micro SD card with the one from your target directory
		ii. Copy the kernel modules from the target directory to the /lib/modules/ directory found in the second partition of your micro SD card.

	f. Reboot into your new kernel

2. Testing

	a. Make any changes to the tester program according to your preferences and environment.

	b. Compile/make the dump_stack_tester.c using the provided Makefile

		make

	b. Run the dump_stack_tester program.
	
		./dump_stack_tester
	
	c. To clean source directory use:
	
		make clean

Note: 
1. In the tester program, I have written total 8 testcases which covers all the syscall features. 
2. Please take a look at the test program for more information on what each test case is about. 
3. I have used mutex for synchronization of probe list.
4. I have designed the termination feature using do_exit() in kernel/kernel/exit.c

SAMPLE OUTPUT:

A. STDOUT:

	root@quark:~/probe# ./dump_stack_tester 

	USER: Dumpstack test program 

	USER: Testcase 0: insdump with dumpmode 0 (testing for owner process)
	USER: insdump using symbol = sys_open
	USER: INSDUMP successful at sys_open with dumpid = 0 
	USER: calling sys_open  (as parent) to invoke dump_stack 
	USER: check dmesg for dump_stack
	USER: RMDUMP with dumpid = 0 success!

	USER: Testcase 1: insdump with dumpmode 0 (testing for other process)
	USER: insdump using symbol = sys_open
	USER: INSDUMP successful at sys_open with dumpid = 1 
	USER: requesting other process to invoke dump_stack at sys_open 
	OTHER PROCESS: calling sys_open to invoke dump_stack
	OTHER PROCESS: check dmesg. [dump_stack should not be printed this time. dumpmode = 0]
	USER: RMDUMP with dumpid = 1 success!

	USER: Testcase 2: insdump with dumpmode 1 (testing for sibling process)
	USER: insdump using symbol = sys_close
	USER: INSDUMP successful at sys_close with dumpid = 2 
	USER: requesting sibling process to invoke dump_stack at sys_close 
	SIBLING PROCESS: calling sys_close to invoke dump_stack
	SIBLING PROCESS: check dmesg. [dump_stack should be printed this time. dumpmode = 1]
	USER: RMDUMP with dumpid = 2 success!

	USER: Testcase 3: insdump with dumpmode 2 (testing for other process)
	USER: insdump using symbol = sys_close
	USER: INSDUMP successful at sys_close with dumpid = 3 
	USER: requesting other process to invoke dump_stack at sys_close 
	OTHER PROCESS: calling sys_close to invoke dump_stack
	OTHER PROCESS: check dmesg. [dump_stack should be printed this time. dumpmode > 1]
	USER: RMDUMP with dumpid = 3 success!

	USER: Testcase 4: insdump with INVALID symbol_name
	USER: insdump using symbol = invalid_symbol
	ERROR: while calling INSDUMP at invalid_symbol
	ERROR: ivalid symbol_name

	USER: Testcase 5: RMDUMP with invalid dumpid
	USER: insdump using symbol = sys_time
	USER: INSDUMP successful at sys_time with dumpid = 4 
	ERROR: while RMDUMP
	ERROR: invalid dump_id

	USER: Testcase 6: insdump with dumpmode 1. testing for other processes(non sibling)
	USER: insdump using symbol = sys_close
	USER: INSDUMP successful at sys_close with dumpid = 5 
	USER: requesting different process (not_sibling) to invoke dump_stack at sys_close 
	DIFFERENT PROCESS (CHILD): trying to call sys_close to invode dump_stack
	DIFFERENT PROCESS (CHILD): check dmesg. [dump_stack should not be printed this time. dumpmode = 1]
	DIFFERENT PROCESS (CHILD): Exiting
	USER: RMDUMP with dumpid = 5 success!

	USER: Testcase 7: testing process termination behaviour
	USER: Now, the dump_stack kprobe with dumpid 4 is not removed by test program
	USER: This remaining kprobe will be taken care of (removed) when this process exits.
	USER: Adding more insdumps for testing such termination behaviour
	USER: INSDUMP at sys_open and sys_close.

	USER: Exiting (without removing some dumpstacks/kprobes). Check if they are automatically removed.
	root@quark:~/probe# 


B. DMESG:

	root@quark:~/probe# dmesg 
	[   42.684245] random: nonblocking pool is initialized
	[  121.779460] SYSCALL: insdump triggered by process with pid = 317
	[  121.799019] INSDUMP: kprobe/dumpstack registered at symbol = sys_open. owner = 317. dumpmode = 0. dump_id = 0
	[  121.814729] DUMPSTACK BEGIN: symbol = sys_open, dumpmode = 0 - invoked by owner with pid = 317 
	[  121.820042] CPU: 0 PID: 317 Comm: dump_stack_test Not tainted 3.19.8-yocto-standard #1
	[  121.820042] Hardware name: Intel Corp. QUARK/GalileoGen2, BIOS 0x01000200 01/01/2014
	[  121.820042]  cd350244 cd350244 cd2fbf0c c1453a61 cd2fbf2c c125401d c15b03b0 cd350244
	[  121.820042]  0000013d cd35027c cd783780 cd2fbf74 cd2fbf44 c10a27d2 cd350284 cd2fbf74
	[  121.820042]  cd783780 c111af21 cd2fbf5c c1027fa4 c111af20 cd2fbf74 00000000 08048620
	[  121.820042] Call Trace:
	[  121.820042]  [<c1453a61>] dump_stack+0x16/0x18
	[  121.820042]  [<c125401d>] handler_pre+0x9d/0xc0
	[  121.820042]  [<c10a27d2>] aggr_pre_handler+0x32/0x70
	[  121.820042]  [<c111af21>] ? SyS_open+0x1/0x20
	[  121.820042]  [<c1027fa4>] kprobe_int3_handler+0xb4/0x130
	[  121.820042]  [<c111af20>] ? do_sys_open+0x210/0x210
	[  121.820042]  [<c1002964>] do_int3+0x44/0xa0
	[  121.820042]  [<c1457fd3>] int3+0x33/0x40
	[  121.820042]  [<c111af21>] ? SyS_open+0x1/0x20
	[  121.820042]  [<c1450060>] ? netlbl_cipsov4_add+0x350/0x780
	[  121.820042]  [<c1457484>] ? syscall_call+0x7/0x7
	[  121.820042] DUMPSTACK END: symbol = sys_open, dumpmode = 0 - invoked by owner with pid = 317 
	[  121.932609] SYSCALL: rmdump triggered by process with pid = 317
	[  121.938798] RMDUMP: removing kprobe/dumpstack from sys_open with dumpid = 0.
	[  121.952501] SYSCALL: insdump triggered by process with pid = 317
	[  121.970443] INSDUMP: kprobe/dumpstack registered at symbol = sys_open. owner = 317. dumpmode = 0. dump_id = 1
	[  121.989140] SYSCALL: rmdump triggered by process with pid = 317
	[  121.995706] RMDUMP: removing kprobe/dumpstack from sys_open with dumpid = 1.
	[  122.008785] SYSCALL: insdump triggered by process with pid = 317
	[  122.027705] INSDUMP: kprobe/dumpstack registered at symbol = sys_close. owner = 317. dumpmode = 1. dump_id = 2
	[  122.043728] DUMPSTACK BEGIN: symbol = sys_close, dumpmode = 1 - invoked by owner/sibling with pid = 319 
	[  122.050039] CPU: 0 PID: 319 Comm: dump_stack_test Not tainted 3.19.8-yocto-standard #1
	[  122.050039] Hardware name: Intel Corp. QUARK/GalileoGen2, BIOS 0x01000200 01/01/2014
	[  122.050039]  cd3393c4 cd3393c4 cd23bf0c c1453a61 cd23bf2c c1253fd3 c15b0450 cd3393c4
	[  122.050039]  0000013f cd3393fc cd7835a0 cd23bf74 cd23bf44 c10a27d2 cd339404 cd23bf74
	[  122.050039]  cd7835a0 c1119771 cd23bf5c c1027fa4 c1119770 cd23bf74 00000000 b771bb40
	[  122.050039] Call Trace:
	[  122.050039]  [<c1453a61>] dump_stack+0x16/0x18
	[  122.050039]  [<c1253fd3>] handler_pre+0x53/0xc0
	[  122.050039]  [<c10a27d2>] aggr_pre_handler+0x32/0x70
	[  122.050039]  [<c1119771>] ? SyS_close+0x1/0x40
	[  122.050039]  [<c1027fa4>] kprobe_int3_handler+0xb4/0x130
	[  122.050039]  [<c1119770>] ? filp_close+0x60/0x60
	[  122.050039]  [<c1002964>] do_int3+0x44/0xa0
	[  122.050039]  [<c1457fd3>] int3+0x33/0x40
	[  122.050039]  [<c111007b>] ? unuse_mm+0x1cb/0x420
	[  122.050039]  [<c1119771>] ? SyS_close+0x1/0x40
	[  122.050039]  [<c1450060>] ? netlbl_cipsov4_add+0x350/0x780
	[  122.050039]  [<c1457484>] ? syscall_call+0x7/0x7
	[  122.050039] DUMPSTACK END: symbol = sys_close, dumpmode = 1 - invoked by owner/sibling with pid = 319 
	[  122.206435] SYSCALL: rmdump triggered by process with pid = 317
	[  122.212943] RMDUMP: removing kprobe/dumpstack from sys_close with dumpid = 2.
	[  122.228612] SYSCALL: insdump triggered by process with pid = 317
	[  122.247339] INSDUMP: kprobe/dumpstack registered at symbol = sys_close. owner = 317. dumpmode = 2. dump_id = 3
	[  122.264775] DUMPSTACK BEGIN: symbol = sys_close, dumpmode > 1 - invoked by process with pid = 320 
	[  122.270069] CPU: 0 PID: 320 Comm: dump_stack_test Not tainted 3.19.8-yocto-standard #1
	[  122.270069] Hardware name: Intel Corp. QUARK/GalileoGen2, BIOS 0x01000200 01/01/2014
	[  122.270069]  cd378844 cd378844 cd23bf28 c1453a61 cd23bf48 c1253ff8 c15b0500 cd378844
	[  122.270069]  00000140 cd37887c cd7835a0 cd23bf74 cd23bf60 c10a2772 cd378884 00000246
	[  122.270069]  00000000 b771bb40 cd23bf6c c102853a 00000003 cd23a000 d27bb05c 00000003
	[  122.270069] Call Trace:
	[  122.270069]  [<c1453a61>] dump_stack+0x16/0x18
	[  122.270069]  [<c1253ff8>] handler_pre+0x78/0xc0
	[  122.270069]  [<c10a2772>] opt_pre_handler+0x32/0x60
	[  122.270069]  [<c102853a>] optimized_callback+0x5a/0x70
	[  122.270069]  [<c1119771>] ? SyS_close+0x1/0x40
	[  122.270069]  [<c1457484>] ? syscall_call+0x7/0x7
	[  122.270069] DUMPSTACK END: symbol = sys_close, dumpmode > 1 - invoked by process with pid = 320 
	[  122.362307] SYSCALL: rmdump triggered by process with pid = 317
	[  122.368486] RMDUMP: removing kprobe/dumpstack from sys_close with dumpid = 3.
	[  122.382452] SYSCALL: insdump triggered by process with pid = 317
	[  122.483771] INSDUMP: Symbol invalid_symbol  not found
	[  122.489245] SYSCALL: insdump triggered by process with pid = 317
	[  122.501251] INSDUMP: kprobe/dumpstack registered at symbol = sys_time. owner = 317. dumpmode = 0. dump_id = 4
	[  122.511440] SYSCALL: rmdump triggered by process with pid = 317
	[  122.517409] RMDUMP: kprobe/dumpstack with dump_id = 999 not found
	[  122.523868] SYSCALL: insdump triggered by process with pid = 317
	[  122.557883] INSDUMP: kprobe/dumpstack registered at symbol = sys_close. owner = 317. dumpmode = 1. dump_id = 5
	[  122.573336] SYSCALL: rmdump triggered by process with pid = 317
	[  122.579311] RMDUMP: removing kprobe/dumpstack from sys_close with dumpid = 5.
	[  122.596547] SYSCALL: insdump triggered by process with pid = 317
	[  122.616313] INSDUMP: kprobe/dumpstack registered at symbol = sys_open. owner = 317. dumpmode = 1. dump_id = 6
	[  122.626386] SYSCALL: insdump triggered by process with pid = 317
	[  122.646372] INSDUMP: kprobe/dumpstack registered at symbol = sys_close. owner = 317. dumpmode = 1. dump_id = 7
	[  122.662672] EXIT DYNAMIC_DUMP_STACK: removing residual kprobe with dumpid 7 created by pid = 317 (terminated)
	[  122.672960] EXIT DYNAMIC_DUMP_STACK: removing residual kprobe with dumpid 6 created by pid = 317 (terminated)
	[  122.685267] EXIT DYNAMIC_DUMP_STACK: removing residual kprobe with dumpid 4 created by pid = 317 (terminated)
	root@quark:~/probe# 
