; function main
L0:
main:
	declare x tuple 
	declare y tuple 
	declare z tuple 
	declare a Integer 
	declare b Integer 
	declare Temp&1 Integer 
	ADD  x y Temp&1
	STORE  Temp&1 z 
	EQ  a b Temp&1
	TEST Temp&1
	BRF L1
	declare zippy subroutine 
	BSR zippy
L1:
	RETURN   
; function zippy
L3:
zippy:
	declare rte Integer 
	GETROUTE @rte  
	CMP rte 4
	BRT L6
	CMP rte 0
	BRF L5
L6:
	TAKER 1  
	BRA L4
L5:
	CMP rte 1
	BRF L7
	RANDG 200  
	FLAG   Temp&1 
	TEST Temp&1
	BRF L8
	TAKER 0  
	BRA L9
L8:
	TAKER 4  
L9:
	BRA L4
L7:
L4:
	RETURN   
	HALT   
