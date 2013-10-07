.section .rodata
/*
adca
adcax
adcay
adcb
adcix
adciy
adcz
adczx
alrb
ancb
*/

.globl test_adca
.globl test_adcax
.globl test_adcay
.globl test_adcb
.globl test_adcix
.globl test_adciy
.globl test_adcz
.globl test_adczx
.globl test_alrb
.globl test_ancb


test_adca: .incbin "./Testsuite/adca"
test_adcax: .incbin "./Testsuite/adcax"
test_adcay: .incbin "./Testsuite/adcay"
test_adcb: .incbin "./Testsuite/adcb"
test_adcix: .incbin "./Testsuite/adcix"
test_adciy: .incbin "./Testsuite/adciy"
test_adcz: .incbin "./Testsuite/adcz"
test_adczx: .incbin "./Testsuite/adczx"
test_alrb: .incbin "./Testsuite/alrb"
test_ancb: .incbin "./Testsuite/ancb"


