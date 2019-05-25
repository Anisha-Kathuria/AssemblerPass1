/* ASSEMBLER PASS 1(8086) - SYSTEM PROGRAMMING

FILE_ asmb.txt

Instructions Included:
-> MOV, LEA, LDS, LES, XCHG, ADD

Instruction Sizes:
1.REG <- REG :: 2 BYTES
2.REG-to/from-MEMORY (without displacement) :: 2 BYTES
3.REG-to/from-MEMORY (with displacement) :: 4 BYTES
4.REG <- IMMEDIATE Value :: 3 BYTES
5.MEMORY (with 16bit displacement) <- IMMEDIATE Value :: 5 BYTES

Assumptions: 
1. Label and instruction cannot be in same line.
2. var db "abcd" and DUP not included

*/

#include <bits/stdc++.h>
#include<algorithm>             //for uppercase inbuilt function
#include<conio.h>               //for getch

using namespace std;

fstream file;                  	//file pointer to open and read file containing .asm code
string word,line;				//temp variables to store line of file read
int len=0,c=0,k,lc=0;			//temp variables
string myArray[100];			//string array to store contents of file
string cur_seg;					//to store value of current segment initialized while reading the file
int off=0;						//to calculate offset address

class symb						//class object for each entry of symbol table
{
public:
	string name;
	string type;                //attributes = column names in symbol table
	string segment;
	int offset;
	int size;
}s[100];

/*
FUNCTION:: count_val
OBJECTIVE:: To count number of occurances of ',' as number of elements is one more than number of "," in variable initialization
PARAMETERS:: 
1. pos (integer) - position number of line in file
*/
int count_val(int pos)
{
    int cval;
	cval=count(myArray[pos].begin(), myArray[pos].end(), ',') + 1;  //in-built function
	return cval;                                                    
}
/*
FUNCTION:: decTohex
OBJECTIVE:: To convert decimal number to 5 digit hexadecimal number
PARAMETERS:: 
1. off (integer) - current offset value needed to convert before display
*/

string decTohex(int off)
{
	char arr[50]; 	 
    int i = 0; 
    while(off!=0)  				//convert decimal to hexa
    {   int temp  = 0; 	 
        temp = off % 16; 	
        if(temp < 10) 
         arr[i++] = temp + 48;  //0-48
        else 
         arr[i++] = temp + 55;  //A-65           
        off = off/16; 
    } 
    							//char converts ascii value to character
    string hex = "";
    for(int j=4; j>=0; j--)		//convert hexa to 5 digit hexa number with 0s in start
    {							//number is in reverse order use for loop in decreasing way
    	if(j>i-1)               //if j is greater than length of hexa number append 0
		hex+= "0";
		else
		hex+= arr[j]; 			//else append the hexa value digit
	}
    return hex;
}

/*
FUNCTION:: trim
OBJECTIVE:: trim spaces and tabs anywhere between, before, after a string 
PARAMETERS:: 
1.str (string) - string to trim with call by reference 
*/
void trim(string &str) 
{   int i,j=0;
   	for(i=0;str[i]!='\0';++i)
	{
        if(str[i]!=' '&&str[i]!='\t')
            str[j++]=str[i];                     //if not a space or tab write the character in string
    } 
    str[j]='\0';                                 //update end character to be null
    str.erase (j++,i);                           //erase the data left in the string after null
}

/*
FUNCTION:: display
OBJECTIVE:: To display the symbol table in output window
*/
void display()
{
	cout<<"NAME"<<setw(15)<<"SIZE"<<setw(15)<<"TYPE"<<setw(15)<<"SEGMENT"<<setw(15)<<"OFFSET"<<"\n\n";
	for(int i=0;i<c;i++)
	{ 
	trim(s[i].name); trim(s[i].type); trim(s[i].segment);
	cout<<s[i].name<<setw(15)<<s[i].size<<setw(15)<<s[i].type<<setw(15)<<s[i].segment<<setw(15)<<decTohex(s[i].offset)<<"\n";
    }
}

/*
FUNCTION:: is_found
OBJECTIVE:: To check if the word is already present as an entry in symbol table
*/
int is_found(string word){
	
	int f_index=-1;
	for(int i=0;i<c;i++){
		
		string temp=s[i].name;
		transform(temp.begin(),temp.end(),temp.begin(),::toupper);
		if(word.compare(temp)==0)
		{
			f_index=i;
			break;
		}	
	}
	
	return f_index;
}


string mnemonic[]={"MOV","LDS","LES","LEA","XCHG","ADD"};

string reg[]={"AX","BX","CX","DX","SI","DI","BP","AH","AL","BH","BL","CH","CL","DH","DL","DS","ES","SS"};

/*
FUNCTION:: in_register
OBJECTIVE:: To check if the value is present in string array register
*/
int in_register(string value)
{
	int i;
	transform(value.begin(),value.end(),value.begin(),::toupper);
    for (i = 0; i < 18; i++)
    {
        if (value == reg[i])
        {
            return i;
        }
    }
	i=-1;
    return i;
}

/*
FUNCTION:: find_op
OBJECTIVE:: To check if the value is present in string array mnemonic
*/
int find_op(string line){
	
	int i;
	transform(line.begin(),line.end(),line.begin(),::toupper);
	for(i=0;i<6;i++){
		if(line.find(mnemonic[i])!=-1)
			return i;
	}
	return -1;
}

/*
FUNCTION:: off_add
OBJECTIVE:: To calculate and update offset-off after each instruction is read
PARAMETER::
1. line(string):: line of code
*/ 
void off_add(string line)
{
	
	trim(line);
	int op_index=find_op(line);   //To find index at which the current opcode matches in the array of opcodes

	if(op_index!=-1)              //If mnemonic found calculate its length
		op_index=mnemonic[op_index].length();
	
	int co_index = line.find(",");
	string dest = line.substr(op_index,co_index-op_index);
	string source = line.substr(co_index+1);

	//register to register- 2bytes
	//if dest and source belong in the in_register array of registers
	if((in_register(dest)!=-1)&&(in_register(source)!=-1))        
		off+=2;
		
	//memory to register without displacement- 2 bytes
	//if source contains square braces at start and end and no +(disp)
	//if destination is a regitser
	if((in_register(dest)!=-1)&&((is_found(source)!=-1)||((source[0]=='[' && source[source.length()-1]==']'))))                    
	{
		if((source.find("+")==-1)&&	(count(source.begin(), source.end(), '[')<=1))
			off+=2;
	}
	
	//register to memory without displacement - 2bytes
	//if source is a register
	// if destination contains square braces at start and end and no +(disp)
	if((in_register(source)!=-1)&&((is_found(dest)!=-1)||((dest[0]=='[' && dest[dest.length()-1]==']'))))                   
	{
		if((dest.find("+")==-1)&&(count(dest.begin(), dest.end(), '[')<=1))
			off+=2;
	}
	
	//immediate to register
	//if destination is a register
	//if source has first character as digit it is immediate operand
	if(((in_register(dest)!=-1))&&(isdigit(source[0])==1))
			off += 3;
	
	//memory to register with displacement
	//2 braces or +
	if((in_register(dest)!=-1)&&((count(source.begin(), source.end(), '[')>1)||(source.find("+")!=-1)))                        
			off+=4;
	
	//memory to register with displacement
	//case: var[reg] and avoid var1[ax+6](covered above)
	if((in_register(dest)!=-1)&&((count(source.begin(), source.end(), '[')==1)&&((source.find("+")==-1))&&((source[0]=='[' && source[source.length()-1]!=']')||(source[0]!='[' && source[source.length()-1]==']'))))
		    off+=4;
	
	//register to memory with displacement
	//2 braces or +
	if((in_register(source)!=-1)&&((count(dest.begin(), dest.end(), '[')>1)||(dest.find("+")!=-1)))                        
			off+=4;
	
	//register to memory with displacement
	//case: var[reg] and avoid var1[ax+6](covered above)
	if((in_register(source)!=-1)&&((count(dest.begin(), dest.end(), '[')==1)&&((dest.find("+")==-1))&&((dest[0]=='[' && dest[dest.length()-1]!=']')||(dest[0]!='[' && dest[dest.length()-1]==']'))))                        
		    off+=4;
	
	//immediate value to memory with 16 bit displacement and without both 5 bytes
	if((in_register(dest)==-1)&&(isdigit(source[0])==1))
	off+=5;
			
			
	if((isdigit(dest[0])))     //I<-anything
	{
     	cout<<"ERRROR: immediate cannot be destination";
					getch(); exit(0);
	}
}

/*
FUNCTION:: identify
OBJECTIVE::To identify if it is a segment initialization, label, variable declaration or an other instruction  
PARAMETERS:: 
1.line (string) - single trimmed line of a file 
2.i (integer)- actual position of that line in the file 
*/
void identify(string line,int i)
{
	string vname="";
	
	if(line.find("SEGMENT")!=-1)							//if line is a SEGMENT INITIALIZATION
	{
	 k=line.find("SEGMENT");								//k :: position of word segment in line
	 vname=myArray[i].substr(0,k); 							//extract segment name :: data before te index of SEGMENT keyword :: vname
	 trim(vname);											//trim to remove spaces
	if(is_found(vname)!=-1)									//SEGMENT name is already present in symbol table then display an error and exit
	{
		cout<<"\nERROR:segment name already declared";
		getch();
		exit(0);
	}
	 s[c].name=vname;					//update a new entry :: segment name 
	 s[c].type="SEGMENT";				//type
	 s[c].segment="(ITSELF)";			//segment
	 s[c].size=-5;						//size of segment by default -5
	 cur_seg=myArray[i].substr(0,k);	//store the segment name to identofy further in statments which current segment the program is reading
	 s[c].offset=0;						//offset at each segment = 0
	 off=0;								//set global offset paramter to 0
	 c++;								//c:: to keep in track number of objects in symbol table class/entries
	}
	else if(line.find(":") != -1)							//if line is a label
	{
	 k=line.find(":");										//k :: position of character ':' in line
	 vname=myArray[i].substr(0,k); 							//extract label name :: data before te index of ':' character :: vname
	 trim(vname);											//trim to remove spaces
	if((is_found(vname)!=-1))								//If label name is already present in symbol table then display an error and exit
	{
		cout<<"\nERROR: label name already declared";
		getch();
		exit(0);
	}
	 s[c].name=vname;					//update a new entry :: label name
	 s[c].type="LABEL";					//type
	 s[c].segment=cur_seg;				//store the name of current running segment 
	 s[c].size=-1;						//size of label by default -1
	 s[c].offset=off;
	 c++;
	}
	// data variable db
	else if(line.find(" DB ")!=-1 )							//if line is a db varibale initialization
	{
	 k=line.find(" DB ");
	 vname=myArray[i].substr(0,k); 
	 trim(vname);
	if(is_found(vname)!=-1)
	{
		cout<<"\nERROR: variable name already declared";
		getch();
		exit(0);
	}
	 s[c].name=vname;
	 s[c].segment=cur_seg;
	 s[c].type="VAR";
	 s[c].size= 1;
	 s[c].offset=off;
	 off += (1 * count_val(i));							//1*(number of commas+1)
	 c++;
	}
	// data variable dw
	else if(line.find(" DW ")!=-1)							//if line is a dw varibale initialization
	{
	  k=line.find(" DW ");
	  vname=myArray[i].substr(0,k); 
	  trim(vname);
	if(is_found(vname)!=-1)
	{
		cout<<"ERROR: variable name already declared";
		getch();
		exit(0);
	}
	 s[c].name=vname;
	 s[c].type="VAR";
	 s[c].segment=cur_seg;
	 s[c].size= 2;
	 s[c].offset=off;
	 off += (2 * count_val(i));								//2*(number of commas+1)
	 c++;
	}
	// data variable dd
	else if(line.find(" DD ")!=-1)                         //if line is a dd varibale initialization
	{
	  k=line.find(" DD ");
	  vname=myArray[i].substr(0,k); 
	  trim(vname); 
	if(is_found(vname)!=-1)
	{
		cout<<"\nERROR: variable name already declared";
		getch();
		exit(0);
	}
	 s[c].name=vname;
	 s[c].type="VAR";
	 s[c].segment=cur_seg;
	 s[c].size= 4;
	 s[c].offset=off;
	 off += (4 * count_val(i));                             //4* (number of commas+1)
	 c++;
	}
	else 
		off_add(line);
}

/*
FUNCTION:: read
OBJECTIVE:: read file with assembly code in file ASSEMBLY.txt
*/
void read()
{
	file.open("ASSEMBLY.txt");                                                      //open file
    if(file.is_open())																//if file opens successfully
    {
    	while ( getline (file,myArray[len]) )										//write the contents of file to myArray
     	len++;																		//len = length of file
		
		for(int j = 0; j < len; ++j)													
    	{
    		word=myArray[j];														//store one line 
    		transform(word.begin(),word.end(),word.begin(),::toupper);				//convert to uppercase
    		k=word.find(";");														//check for ; for a comment
	    	if(k!=0)																//if ; not at 0th position extract the code before
				word=word.substr(0,k-1); 
			else
				word="";															//if ; at 0th position nullify the word
			
			string tem_line=word;
			trim(tem_line);															//trim the line
			if((word.find("ASSUME")!=-1) || (word.find("END")!=-1) || tem_line=="") //if line contains assume, end or null leave the iteration 
				continue;
		
			identify(word,j);                                                     //call to check for a - segment label or variable(db,dw,dd)
		}
	}
	else
		cout<<"Cannot open file";
	file.close();
}

/*
FUNCTION:: write
OBJECTIVE:: write symbol table to file symbol_table.txt 
*/
void write()
{
	
	file.open("asmb.txt",ios::out);
	
	file<<"\n"<<setw(40)<<"SYMBOL TABLE \n";
	file<<"==========================================================================\n\n";
	file<<"NAME"<<setw(15)<<"OFFSET"<<setw(15)<<"SIZE"<<setw(15)<<"TYPE"<<setw(15)<<"SEGMENT"<<"\n\n";
	file<<"==========================================================================\n";
	for(int i=0;i<c;i++)	
		file<<s[i].name<<setw(15)<<decTohex(s[i].offset)<<setw(15)<<s[i].size<<setw(15)<<s[i].type<<setw(15)<<s[i].segment<<"\n";
	
	cout<<"\nSymbol table created!";
	
	file.close();
	
}
    
int main()
{
read();			//call read file function
//display();	
write();		//call write file function
return 0;
}





