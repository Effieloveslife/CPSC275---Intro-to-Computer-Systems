#File: numstr.s
#Author: Yuxuan Li
#Date: 10/10/2014

.hex:
		.string		"0x%x\n"
		.globl main

.L0:
		.string		".\n"
		.globl main

.L1:
		.string		"%d"
		.globl main

.L2:	
		.string		"Enter an integer: "
		.globl main
		
.L3:
		.string		"%c"
		.globl main

.L4:
		.string		"The numeric string equivalent to %d is "
		.globl main

.L5:
		.string		"The numeric string equivalent to -%d is "
		.globl main

main:
		pushl	%ebp				#save frame pointer
		movl	%esp, %ebp			#adjust stack pointer
		subl	$30, %esp			#reserve 30 bytes for current frame

		pushl   $.L2		    	#push the input prompt
		call    printf		    	#print the input prompt

		leal    -4(%ebp), %eax		#move the input number address to %eax
		movl    %eax, 4(%esp)		#push the address for scanf
		movl    $.L1, (%esp)    	#push format specifier for scanf
		call    scanf				#scan in the user input

		#prepare arguments for int2str
		leal	-8(%ebp), %eax		#move the buffer pointer to %eax
		movl	%eax, 4(%esp)		#move the buffer pointer to 4(%esp)
		movl	-4(%ebp), %eax		#move the input number to %eax
		movl	%eax, (%esp)		#move the input number to %esp

		#check if the input is negative
		cmpl	$0, %eax			#compare the input number with 0
		jl		flag				#if input < 0, jump to flag
		jmp		mainctd				#if input >=0, continue
		
	flag:
		movl	$1, 8(%esp)			#set the negative input flag to 1
		imull	$-1, %eax			#make the negative input its negative
		movl	%eax, (%esp)		#move its negative to the stack

	mainctd:
		call	int2str				#int2str stores the characters in buffer

		cmpl	$1, 8(%esp)			#check if the number is negative
		je		negPrompt			#if input is negative, use another prompt
	
		pushl	(%esp)				#else, push the number
		pushl	$.L4				#push the output prompt
		call	printf				#print the prompt
		jmp		bigFor				#skip prompt for negative numbers
	
	negPrompt:
		pushl	(%esp)				#push the number's negative(changed by flag)
		pushl	$.L5				#push the output prompt for negative number
		call	printf				#print the output prompt
	
	bigFor:
		leal	-8(%ebp), %eax		#move the beginning of buffer to %eax
		
		cmpl	%eax, %ebx			#compare %ebx to the beginning of buffer
		jg		endOfMain			#if %ebx has reached the begging of buffer,
									#end output process
		
		pushl	(%ebx)				#push the current character for printf
		pushl	$.L3				#push the "%c" for printf
		call	printf				#call printf to print the digit/comma/sign
		
		incl	%ebx				#increment %ebx for the next character
		jmp		bigFor				#go to the beginning of the for loop
		
endOfMain:
		pushl	$.L0				#push a period and a new line character
		call	printf				#print them
		
		leave						#restore the current activation
		ret							#return to caller

int2str:
		pushl	%ebp				#save frame pointer
		movl	%esp, %ebp			#adjust stack pointer

		movl	8(%ebp), %eax		#get input number
		movl	12(%ebp), %ebx		#get buffer pointer
		movl	$10, %esi			#%esi = 10
		movl	$1, %ecx			#%ecx = 1 for counting
		
	for:
		cmpl	%esi, %eax			#compare %eax to %esi
		jl		msb					#if %eax < 10 goto msb to deal with MSB
	
		cltd						#sign-extend %eax
		idivl	%esi				#divide %eax by 10
		addl	$48, %edx			#get the ascii value of the remainder
		movb	%dl, (%ebx)			#move the ascii value of the digit to buffer
		
		decl	%ebx				#decrement buffer pointer for the next digit
		
		incl	%ecx				#keep a count for every 3 digits
		cmpl	$3, %ecx			#compare count to 3
		jg		comma				#if count > 3 it's time to add a comma
		
		jmp		for					#else keep looping
		
	comma:
		movl	$1, %ecx			#reset count
		movb	$44, (%ebx)			#move the ascii value of comma to buffer
		decl	%ebx				#decrement %ebx for the next digit
		
		jmp 	for					#go back to the loop
		
	msb:
		addl	$48, %eax			#get ascii value of MSB
		movb	%al, (%ebx)			#move the ascii value of MSB into the buffer
		
		cmpl	$1, 16(%ebp)		#compare negative input flag to 1
		jne		finish				#if negative input flag is off, go to finish
		
		decl	%ebx				#else decrement %ebx for a sign character
		movb	$45, (%ebx)			#put in the ascii value of "-"
		
	finish:
		leave						#restore the current activation
		ret							#return to caller
