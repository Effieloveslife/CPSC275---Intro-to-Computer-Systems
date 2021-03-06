/*
 * Program: vsm.c
 * Purpose: Simulates a very simple machine that is capable of arithmetic 
 *			operations and string input/output.
 * Author: Yuxuan Li
 * Date: 9/25/2014
 */

#include<stdio.h>
#include<stdlib.h>
#define MAX 1024

//initializing components of VSM
unsigned short memory[MAX];
short acc = 0x0000;
int iC=0x0000;
int iR=0x0000;
int opCode=0x0;
int operand=0x0000;

void decode(unsigned short n);

//error-checking functions
void illAddD(int operand);  //illegal address for data
void illAddI(int operand);  //illegal address for instructions
void illAddS(int isNum);  //illegal address specifiers -- 5th bit
void zeroDiv();  //detects division by zero

//basic real number operations
void asmd(int opCode, int isNum, int operand);

void dump(void);

int main(void) {
	printf("*** Initializing VSM ***\n");
	
	//scan in instructions until reach EOC
	int count;
	while (scanf("%hx",&memory[count])!=EOF) {
		if (memory[count]==0x0000) break;
		count++;
	}
	
	//initializing iC and iR
	iC=0;
	iR=memory[iC];
	
	//implementing the instructions
	while (iR!=0x0000) {
		//dump();  //for trace and debugging only
		decode(iR);
		iR=memory[iC]; //update iR
	}
	
	return 0;
}

void decode(unsigned short num) {
	//extract opCode, 5th digit isNum, and operand by masking
	opCode = num & 0xF000;
	int isNum = num & 0x0800; isNum >>= 11;
	operand = num & 0x07FF;
	
	//for tracing purpose only
	//printf("opCode=%.4hx, isNum=%d, operand=%d\n",opCode,isNum,operand); 
	
	//switch cases that performs the correspoding operation
	switch (opCode) {
		case 0x0000:  //end of code
			break;
		case 0x1000:  // load
			if (isNum) acc = operand;
			else {
				illAddD(operand);
				acc = memory[operand/2];
			}
			iC++;
			break;
		case 0x2000:  //store
			illAddS(isNum);
			illAddD(operand);
			memory[operand/2] = acc;
			iC++;
			break;
		case 0x3000:  //read
			illAddS(isNum);
			illAddD(operand);
			scanf("%hx",&memory[operand/2]);
			iC++;
			break;
		case 0x4000:  //write
			illAddS(isNum);
			illAddD(operand);
			printf("%d\n",memory[operand/2]); //for write out the data
			iC++;
			break;
		case 0x5000:  //add
			asmd(opCode,isNum,operand);
			iC++;
			break;
		case 0x6000:  //subtract
			asmd(opCode,isNum,operand);
			iC++;
			break;
		case 0x7000:  //multiply
			asmd(opCode,isNum,operand);
			iC++;
			break;
		case 0x8000:  //divide
			asmd(opCode,isNum,operand);
			iC++;
			break;
		case 0x9000:  //jump
			illAddS(isNum);
			illAddI(operand);
			iC=operand/2;
			break;
		case 0xA000:  //jump if acc is negative
			if (acc<0) {
				illAddS(isNum);
				illAddI(operand);
				iC=operand/2;
			} else iC++;
			break;
		case 0xB000:  //jump if acc is 0
			if (acc==0) {
				illAddS(isNum);
				illAddI(operand);
				iC=operand/2;
			} else iC++;
			break;
		case 0xC000:  //halt
			iR=0x0000;
			dump();
			printf("\n*** Terminating VSM ***\n");
			exit(1);
			break;
		case 0xD000:  //mod
			asmd(opCode,isNum,operand);
			iC++;
			break;
			break;
		case 0xE000:  //take string input
			illAddS(isNum);
			illAddD(operand);
			int i = operand/2;
			while (1) {
				scanf("%hx",&memory[i++])!='\n';
				if (memory[i-1]==0) break;
			}
			iC++;
			break;
		case 0xF000:  //output string
			illAddS(isNum);
			illAddD(operand);
			i = operand/2;
			while (memory[i]!=0)
				printf("%c",memory[i++]);
			printf("\n");
			iC++;
			break;
		default:  //illegal opCode, terminating the program,technically never 
				  //reaches here
			printf("Illegal opCode.\n");
			dump();
			printf("\n*** Terminating VSM ***\n");
			exit(1);
			break;
	}
}

/*
 * Function: illAddD
 * Purpose: To detect illegal data address
 * 			Legal instruction address range: (1024,2047).
 */
void illAddD(int operand){
	if (operand>2047 || operand<1024) {
		printf("Illegal address for data.\n");
		dump();
		printf("\n*** Terminating VSM ***\n");
		exit(1);
	}
}
/*
 * Function: illAddI
 * Purpose: To detect illegal instruction address
 * 			Legal instruction address range: (0,1023).
 */
void illAddI(int operand){
	if (operand<0 || operand>1023) {
		printf("Illegal address for instructions.\n");
		dump();
		printf("\n*** Terminating VSM ***\n");
		exit(1);
	}
}
/*
 * Function: illAddS
 * Purpose: To detect illegal address specifier.
 *			When the operant is supposed to refer to an address, the 5th digit
 *			should be 0.
 */
void illAddS(int isNum) {
	if (isNum) {
		printf("Illegal address specifier\n");
		dump();
		printf("\n*** Terminating VSM ***\n");
		exit(1);
	}
}
/*
 * Function: zeroDiv
 * Purpose: To detect division by zero. Once occurred, terminates the program.
 */
void zeroDiv(void){
	printf("Division by zero.\n");
	dump();
	printf("\n*** Terminating VSM ***\n");
	exit(1);
}
/*
 * Function: asmd (stands for Add Subtract Multiple Divide)
 * Purpose: To perform real number arithmetic operations,
 * 			including +,-,*,/,%.
 *			It calls zeroDiv() to detect division by zero.
 * Parameters: opCode, isNum, operand
 * Return: Update acc.
 */
void asmd(int opCode, int isNum, int operand){
	int temp;
	if (isNum) temp = operand;
	else {
		illAddD(operand);
		temp = memory[operand/2];
	}
	if (opCode == 0x5000) acc+=temp;
	else if (opCode == 0x6000) acc-=temp; //deal with overflow
	else if (opCode == 0x7000) acc*=temp;
	else if (temp == 0) zeroDiv();
	else if (opCode == 0x8000) acc/=temp;
	else if (opCode == 0xD000) acc%=temp;
	else printf("Unknow real number operation.");
}

/*
 * Function: dump
 * Purpose: To show the actual values of instructions and data values at the  
 *			moment execution terminated in a formated fashion
 */
void dump(void){
	printf("\nREGISTERS:\n");
	printf("accumulator            0x%.4hx\n",acc);
	printf("instructionCounter     0x%.4x\n",2*iC);
	printf("instructionRegister    0x%.4x\n\n",iR);
	printf("opCode                 0x%.1x\n",opCode);
	printf("operand                0x%.4x\n\n",operand);
	printf("CODE:\n     0  1  2  3  4  5  6  7  8  9\n");
	int i;
	for(i=0;i<=45;i+=5) {
		printf("%.4d ", 2*i);
		int k;
		for(k=i;k<i+5;k++) {
			int elmt = memory[k];
			int part1 = elmt >> 8;
			int part2 = elmt & 0xFF;
			printf("%.2hx %.2hx ",part1,part2);
		}
		printf("\n");
	}
	printf("...\n\n");
	
	printf("DATA:\n     0  1  2  3  4  5  6  7  8  9\n");
	for(i=512;i<=557;i+=5) {
		printf("%.4d ",2*i);
		int k;
		for(k=i;k<i+5;k++) {
			int elmt = memory[k];
			int part1 = elmt >> 8;
			int part2 = elmt & 0xFF;
			printf("%.2hx %.2hx ",part1,part2);
		}
		printf("\n");
	}
	printf("...\n");
}
