; CC65C Test file
; 2016 David Jolly

;;************************************************
; Definitions
;***********************************************;;
def org_code 0xc000
def org_data 0xd000
def org_vector 0xfffa
def tmp 0x0600

;;************************************************
; Code section
;***********************************************;;
seg code
org org_code

;;************************************************
; Restart interrupt routine
;***********************************************;;
rst:
	jsr main		; jump to main
	jsr halt		; jump to halt
	rti

;;************************************************
; Halt routine
;***********************************************;;
halt:
	jmp halt		; loop forever
	rts

;;************************************************
; Main routine
;***********************************************;;
main:
	jsr copy_string		; copy string from rom into ram
	jsr convert_string	; convert string to uppercase
	rts

;;
; Copy string routine
;;
copy_string:
	ldx #0x00		; clear x register
copy_loop:
	lda data_0, x		; load character from rom at index x
	sta tmp, x		; store character into ram at index x
	inx			; increment index x
	bne copy_loop		; exit loop if terminator
	rts

;;
; Convert string routine
;;
convert_string:
	ldx #0x00		; clear x register
convert_loop:
	lda tmp, x		; load character at index x
	beq convert_done	; exit loop if terminator
	sbc #0x20		; subtract ascii offset
	sta tmp, x		; store char at index x
	inx			; increment index x
	jmp convert_loop	; continue loop
convert_done:
	rts

;;************************************************
; Data section
;***********************************************;;
seg data
org org_data

data_0:
	db "hello", '\0'

;;************************************************
; Vector section
;***********************************************;;
seg vector
org org_vector

dw 0x0000, rst, 0x0000		; nmi, rst, irq
