; CC65C Lexer Test file
; 2016 David Jolly

+ & / % * | - ^

&& == > >= < <= != ||

] [

) (

adc and asl bbr0 bbr1 bbr2 bbr3 bbr4 bbr5 bbr6 bbr7 bbs0 bbs1 bbs2 bbs3 bbs4 bbs5 bbs6 bbs7 
bcc bcs beq bit bmi bne bpl bra brk bvc bvs clc cld cli clv cmp cpx cpy dec dex dey eor 
inc inx iny jmp jsr lda ldx ldy lsr nop ora pha php phx phy pla plp plx ply 
rmb0 rmb1 rmb2 rmb3 rmb4 rmb5 rmb6 rmb7 rol ror rti rts sbc sec sed sei 
smb0 smb1 smb2 smb3 smb4 smb5 smb6 smb7 sta stp stx sty stz tax tay 
trb tsb tsx txa txs tya wai

; Single line comment

;;******************
; Block Comment
;*****************;;

else elif elifdef endif if ifdef

db dw def org res seg undef

_ __ _abc abc a_bc _0 a0 _ab_0

#

incb inch incs

_: __: _abc: abc: a_bc: _0: a0: _ab_0:

'a' '0' ' ' '\a' '\b' '\f' '\n' '\r' '\t' '\v' '\\' '\'' '\"' '\?' '\x00' '\0' '\xe2' '\120'

"hello world!" "hello\nworld!" "a0 \a\b\f\n\r\t\v\\\'\"\?\x00\0\xe2\120"

byte high low word

$$ $

a x y

0b0 0b1 0b100101 -0b0 -0b1 -0b100101

0 123 34533 -0 -123 -34533

0x0 0x123 0xffee -0x0 -0x123 -0xffee

0c0 0c123 0c777 -0c0 -0c123 -0c777

,

~ !
