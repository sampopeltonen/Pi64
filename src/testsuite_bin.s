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
anda
andax
anday
andb
andix
andiy
andz
andzx
aneb
arrb
asla
aslax
asln
aslz
aslzx
asoa
asoax
asoay
asoix
asoiy
asoz
asozx
axsa
axsix
axsz
axszy
bccr
bcsr
beqr
bita
bitz
bmir
bner
bplr
branchwrap
brkn
bvcr
bvsr
clcn
cldn
clin
clvn
cmpa
cmpax
cmpay
cmpb
cmpix
cmpiy
cmpz
cmpzx
cpuport
cputiming
cpxa
cpxb
cpxz
cpya
cpyb
cpyz
dcma
dcmax
dcmay
dcmix
dcmiy
dcmz
dcmzx
deca
decax
decz
deczx
dexn
deyn
eora
eorax
eoray
eorb
eorix
eoriy
eorz
eorzx
inca
incax
incz
inczx
insa
insax
insay
insix
insiy
insz
inszx
inxn
inyn
jmpi
jmpw
jsrw
lasay
laxa
laxay
laxix
laxiy
laxz
laxzy
ldaa
ldaax
ldaay
ldab
ldaix
ldaiy
ldaz
ldazx
ldxa
ldxay
ldxb
ldxz
ldxzy
ldya
ldyax
ldyb
ldyz
ldyzx
lsea
lseax
lseay
lseix
lseiy
lsez
lsezx
lsra
lsrax
lsrn
lsrz
lsrzx
lxab
mmu
mmufetch
nopa
nopax
nopb
nopn
nopz
nopzx
oraa
oraax
oraay
orab
oraix
oraiy
oraz
orazx
phan
phpn
plan
plpn
rlaa
rlaax
rlaay
rlaix
rlaiy
rlaz
rlazx
rola
rolax
roln
rolz
rolzx
rora
rorax
rorn
rorz
rorzx
rraa
rraax
rraay
rraix
rraiy
rraz
rrazx
rtin
rtsn
sbca
sbcax
sbcay
sbcb
sbcb(eb)
sbcix
sbciy
sbcz
sbczx
sbxb
secn
sedn
sein
shaay
shaiy
shsay
shxay
shyax
staa
staax
staay
staix
staiy
staz
stazx
stxa
stxz
stxzy
stya
styz
styzx
taxn
tayn
trap1
trap10
trap11
trap12
trap13
trap14
trap15
trap16
trap17
trap2
trap3
trap4
trap5
trap6
trap7
trap8
trap9
tsxn
txan
txsn
tyan
rasterlinetest
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
.globl test_anda
.globl test_andax
.globl test_anday
.globl test_andb
.globl test_andix
.globl test_andiy
.globl test_andz
.globl test_andzx
.globl test_aneb
.globl test_arrb
.globl test_asla
.globl test_aslax
.globl test_asln
.globl test_aslz
.globl test_aslzx
.globl test_asoa
.globl test_asoax
.globl test_asoay
.globl test_asoix
.globl test_asoiy
.globl test_asoz
.globl test_asozx
.globl test_axsa
.globl test_axsix
.globl test_axsz
.globl test_axszy
.globl test_bccr
.globl test_bcsr
.globl test_beqr
.globl test_bita
.globl test_bitz
.globl test_bmir
.globl test_bner
.globl test_bplr
.globl test_branchwrap
.globl test_brkn
.globl test_bvcr
.globl test_bvsr
.globl test_clcn
.globl test_cldn
.globl test_clin
.globl test_clvn
.globl test_cmpa
.globl test_cmpax
.globl test_cmpay
.globl test_cmpb
.globl test_cmpix
.globl test_cmpiy
.globl test_cmpz
.globl test_cmpzx
.globl test_cpuport
.globl test_cputiming
.globl test_cpxa
.globl test_cpxb
.globl test_cpxz
.globl test_cpya
.globl test_cpyb
.globl test_cpyz
.globl test_dcma
.globl test_dcmax
.globl test_dcmay
.globl test_dcmix
.globl test_dcmiy
.globl test_dcmz
.globl test_dcmzx
.globl test_deca
.globl test_decax
.globl test_decz
.globl test_deczx
.globl test_dexn
.globl test_deyn
.globl test_eora
.globl test_eorax
.globl test_eoray
.globl test_eorb
.globl test_eorix
.globl test_eoriy
.globl test_eorz
.globl test_eorzx
.globl test_inca
.globl test_incax
.globl test_incz
.globl test_inczx
.globl test_insa
.globl test_insax
.globl test_insay
.globl test_insix
.globl test_insiy
.globl test_insz
.globl test_inszx
.globl test_inxn
.globl test_inyn
.globl test_jmpi
.globl test_jmpw
.globl test_jsrw
.globl test_lasay
.globl test_laxa
.globl test_laxay
.globl test_laxix
.globl test_laxiy
.globl test_laxz
.globl test_laxzy
.globl test_ldaa
.globl test_ldaax
.globl test_ldaay
.globl test_ldab
.globl test_ldaix
.globl test_ldaiy
.globl test_ldaz
.globl test_ldazx
.globl test_ldxa
.globl test_ldxay
.globl test_ldxb
.globl test_ldxz
.globl test_ldxzy
.globl test_ldya
.globl test_ldyax
.globl test_ldyb
.globl test_ldyz
.globl test_ldyzx
.globl test_lsea
.globl test_lseax
.globl test_lseay
.globl test_lseix
.globl test_lseiy
.globl test_lsez
.globl test_lsezx
.globl test_lsra
.globl test_lsrax
.globl test_lsrn
.globl test_lsrz
.globl test_lsrzx
.globl test_lxab
.globl test_mmu
.globl test_mmufetch
.globl test_nopa
.globl test_nopax
.globl test_nopb
.globl test_nopn
.globl test_nopz
.globl test_nopzx
.globl test_oraa
.globl test_oraax
.globl test_oraay
.globl test_orab
.globl test_oraix
.globl test_oraiy
.globl test_oraz
.globl test_orazx
.globl test_phan
.globl test_phpn
.globl test_plan
.globl test_plpn
.globl test_rlaa
.globl test_rlaax
.globl test_rlaay
.globl test_rlaix
.globl test_rlaiy
.globl test_rlaz
.globl test_rlazx
.globl test_rola
.globl test_rolax
.globl test_roln
.globl test_rolz
.globl test_rolzx
.globl test_rora
.globl test_rorax
.globl test_rorn
.globl test_rorz
.globl test_rorzx
.globl test_rraa
.globl test_rraax
.globl test_rraay
.globl test_rraix
.globl test_rraiy
.globl test_rraz
.globl test_rrazx
.globl test_rtin
.globl test_rtsn
.globl test_sbca
.globl test_sbcax
.globl test_sbcay
.globl test_sbcb
.globl test_sbcbeb
.globl test_sbcix
.globl test_sbciy
.globl test_sbcz
.globl test_sbczx
.globl test_sbxb
.globl test_secn
.globl test_sedn
.globl test_sein
.globl test_shaay
.globl test_shaiy
.globl test_shsay
.globl test_shxay
.globl test_shyax
.globl test_staa
.globl test_staax
.globl test_staay
.globl test_staix
.globl test_staiy
.globl test_staz
.globl test_stazx
.globl test_stxa
.globl test_stxz
.globl test_stxzy
.globl test_stya
.globl test_styz
.globl test_styzx
.globl test_taxn
.globl test_tayn
.globl test_trap1
.globl test_trap10
.globl test_trap11
.globl test_trap12
.globl test_trap13
.globl test_trap14
.globl test_trap15
.globl test_trap16
.globl test_trap17
.globl test_trap2
.globl test_trap3
.globl test_trap4
.globl test_trap5
.globl test_trap6
.globl test_trap7
.globl test_trap8
.globl test_trap9
.globl test_tsxn
.globl test_txan
.globl test_txsn
.globl test_tyan
.globl test_end
.globl rasterlinetest

test_adca:  .incbin "./Testsuite/adca"
test_adcax: .incbin "./Testsuite/adcax"
test_adcay: .incbin "./Testsuite/adcay"
test_adcb:  .incbin "./Testsuite/adcb"
test_adcix: .incbin "./Testsuite/adcix"
test_adciy: .incbin "./Testsuite/adciy"
test_adcz:  .incbin "./Testsuite/adcz"
test_adczx: .incbin "./Testsuite/adczx"
test_alrb:  .incbin "./Testsuite/alrb"
test_ancb:  .incbin "./Testsuite/ancb"
test_anda:  .incbin "./Testsuite/anda"
test_andax: .incbin "./Testsuite/andax"
test_anday: .incbin "./Testsuite/anday"
test_andb:  .incbin "./Testsuite/andb"
test_andix: .incbin "./Testsuite/andix"
test_andiy: .incbin "./Testsuite/andiy"
test_andz:  .incbin "./Testsuite/andz"
test_andzx: .incbin "./Testsuite/andzx"
test_aneb:  .incbin "./Testsuite/aneb"
test_arrb:  .incbin "./Testsuite/arrb"
test_asla:  .incbin "./Testsuite/asla"
test_aslax: .incbin "./Testsuite/aslax"
test_asln:  .incbin "./Testsuite/asln"
test_aslz:  .incbin "./Testsuite/aslz"
test_aslzx: .incbin "./Testsuite/aslzx"
test_asoa:  .incbin "./Testsuite/asoa"
test_asoax: .incbin "./Testsuite/asoax"
test_asoay: .incbin "./Testsuite/asoay"
test_asoix: .incbin "./Testsuite/asoix"
test_asoiy: .incbin "./Testsuite/asoiy"
test_asoz:  .incbin "./Testsuite/asoz"
test_asozx: .incbin "./Testsuite/asozx"
test_axsa:  .incbin "./Testsuite/axsa"
test_axsix: .incbin "./Testsuite/axsix"
test_axsz:  .incbin "./Testsuite/axsz"
test_axszy: .incbin "./Testsuite/axszy"
test_bccr:  .incbin "./Testsuite/bccr"
test_bcsr: .incbin "./Testsuite/bcsr"
test_beqr: .incbin "./Testsuite/beqr"
test_bita: .incbin "./Testsuite/bita"
test_bitz: .incbin "./Testsuite/bitz"
test_bmir: .incbin "./Testsuite/bmir"
test_bner: .incbin "./Testsuite/bner"
test_bplr: .incbin "./Testsuite/bplr"
test_branchwrap: .incbin "./Testsuite/branchwrap"
test_brkn: .incbin "./Testsuite/brkn"
test_bvcr: .incbin "./Testsuite/bvcr"
test_bvsr: .incbin "./Testsuite/bvsr"
test_clcn: .incbin "./Testsuite/clcn"
test_cldn: .incbin "./Testsuite/cldn"
test_clin: .incbin "./Testsuite/clin"
test_clvn: .incbin "./Testsuite/clvn"
test_cmpa: .incbin "./Testsuite/cmpa"
test_cmpax: .incbin "./Testsuite/cmpax"
test_cmpay: .incbin "./Testsuite/cmpay"
test_cmpb: .incbin "./Testsuite/cmpb"
test_cmpix: .incbin "./Testsuite/cmpix"
test_cmpiy: .incbin "./Testsuite/cmpiy"
test_cmpz: .incbin "./Testsuite/cmpz"
test_cmpzx: .incbin "./Testsuite/cmpzx"
test_cpuport: .incbin "./Testsuite/cpuport"
test_cputiming: .incbin "./Testsuite/cputiming"
test_cpxa: .incbin "./Testsuite/cpxa"
test_cpxb: .incbin "./Testsuite/cpxb"
test_cpxz: .incbin "./Testsuite/cpxz"
test_cpya: .incbin "./Testsuite/cpya"
test_cpyb: .incbin "./Testsuite/cpyb"
test_cpyz: .incbin "./Testsuite/cpyz"
test_dcma: .incbin "./Testsuite/dcma"
test_dcmax: .incbin "./Testsuite/dcmax"
test_dcmay: .incbin "./Testsuite/dcmay"
test_dcmix: .incbin "./Testsuite/dcmix"
test_dcmiy: .incbin "./Testsuite/dcmiy"
test_dcmz: .incbin "./Testsuite/dcmz"
test_dcmzx: .incbin "./Testsuite/dcmzx"
test_deca: .incbin "./Testsuite/deca"
test_decax: .incbin "./Testsuite/decax"
test_decz: .incbin "./Testsuite/decz"
test_deczx: .incbin "./Testsuite/deczx"
test_dexn: .incbin "./Testsuite/dexn"
test_deyn: .incbin "./Testsuite/deyn"
test_eora: .incbin "./Testsuite/eora"
test_eorax: .incbin "./Testsuite/eorax"
test_eoray: .incbin "./Testsuite/eoray"
test_eorb: .incbin "./Testsuite/eorb"
test_eorix: .incbin "./Testsuite/eorix"
test_eoriy: .incbin "./Testsuite/eoriy"
test_eorz: .incbin "./Testsuite/eorz"
test_eorzx: .incbin "./Testsuite/eorzx"
test_inca: .incbin "./Testsuite/inca"
test_incax: .incbin "./Testsuite/incax"
test_incz: .incbin "./Testsuite/incz"
test_inczx: .incbin "./Testsuite/inczx"
test_insa: .incbin "./Testsuite/insa"
test_insax: .incbin "./Testsuite/insax"
test_insay: .incbin "./Testsuite/insay"
test_insix: .incbin "./Testsuite/insix"
test_insiy: .incbin "./Testsuite/insiy"
test_insz: .incbin "./Testsuite/insz"
test_inszx: .incbin "./Testsuite/inszx"
test_inxn: .incbin "./Testsuite/inxn"
test_inyn: .incbin "./Testsuite/inyn"
test_jmpi: .incbin "./Testsuite/jmpi"
test_jmpw: .incbin "./Testsuite/jmpw"
test_jsrw: .incbin "./Testsuite/jsrw"
test_lasay: .incbin "./Testsuite/lasay"
test_laxa: .incbin "./Testsuite/laxa"
test_laxay: .incbin "./Testsuite/laxay"
test_laxix: .incbin "./Testsuite/laxix"
test_laxiy: .incbin "./Testsuite/laxiy"
test_laxz: .incbin "./Testsuite/laxz"
test_laxzy: .incbin "./Testsuite/laxzy"
test_ldaa: .incbin "./Testsuite/ldaa"
test_ldaax: .incbin "./Testsuite/ldaax"
test_ldaay: .incbin "./Testsuite/ldaay"
test_ldab: .incbin "./Testsuite/ldab"
test_ldaix: .incbin "./Testsuite/ldaix"
test_ldaiy: .incbin "./Testsuite/ldaiy"
test_ldaz: .incbin "./Testsuite/ldaz"
test_ldazx: .incbin "./Testsuite/ldazx"
test_ldxa: .incbin "./Testsuite/ldxa"
test_ldxay: .incbin "./Testsuite/ldxay"
test_ldxb: .incbin "./Testsuite/ldxb"
test_ldxz: .incbin "./Testsuite/ldxz"
test_ldxzy: .incbin "./Testsuite/ldxzy"
test_ldya: .incbin "./Testsuite/ldya"
test_ldyax: .incbin "./Testsuite/ldyax"
test_ldyb: .incbin "./Testsuite/ldyb"
test_ldyz: .incbin "./Testsuite/ldyz"
test_ldyzx: .incbin "./Testsuite/ldyzx"
test_lsea: .incbin "./Testsuite/lsea"
test_lseax: .incbin "./Testsuite/lseax"
test_lseay: .incbin "./Testsuite/lseay"
test_lseix: .incbin "./Testsuite/lseix"
test_lseiy: .incbin "./Testsuite/lseiy"
test_lsez: .incbin "./Testsuite/lsez"
test_lsezx: .incbin "./Testsuite/lsezx"
test_lsra: .incbin "./Testsuite/lsra"
test_lsrax: .incbin "./Testsuite/lsrax"
test_lsrn: .incbin "./Testsuite/lsrn"
test_lsrz: .incbin "./Testsuite/lsrz"
test_lsrzx: .incbin "./Testsuite/lsrzx"
test_lxab: .incbin "./Testsuite/lxab"
test_mmu: .incbin "./Testsuite/mmu"
test_mmufetch: .incbin "./Testsuite/mmufetch"
test_nopa: .incbin "./Testsuite/nopa"
test_nopax: .incbin "./Testsuite/nopax"
test_nopb: .incbin "./Testsuite/nopb"
test_nopn: .incbin "./Testsuite/nopn"
test_nopz: .incbin "./Testsuite/nopz"
test_nopzx: .incbin "./Testsuite/nopzx"
test_oraa: .incbin "./Testsuite/oraa"
test_oraax: .incbin "./Testsuite/oraax"
test_oraay: .incbin "./Testsuite/oraay"
test_orab: .incbin "./Testsuite/orab"
test_oraix: .incbin "./Testsuite/oraix"
test_oraiy: .incbin "./Testsuite/oraiy"
test_oraz: .incbin "./Testsuite/oraz"
test_orazx: .incbin "./Testsuite/orazx"
test_phan: .incbin "./Testsuite/phan"
test_phpn: .incbin "./Testsuite/phpn"
test_plan: .incbin "./Testsuite/plan"
test_plpn: .incbin "./Testsuite/plpn"
test_rlaa: .incbin "./Testsuite/rlaa"
test_rlaax: .incbin "./Testsuite/rlaax"
test_rlaay: .incbin "./Testsuite/rlaay"
test_rlaix: .incbin "./Testsuite/rlaix"
test_rlaiy: .incbin "./Testsuite/rlaiy"
test_rlaz: .incbin "./Testsuite/rlaz"
test_rlazx: .incbin "./Testsuite/rlazx"
test_rola: .incbin "./Testsuite/rola"
test_rolax: .incbin "./Testsuite/rolax"
test_roln: .incbin "./Testsuite/roln"
test_rolz: .incbin "./Testsuite/rolz"
test_rolzx: .incbin "./Testsuite/rolzx"
test_rora: .incbin "./Testsuite/rora"
test_rorax: .incbin "./Testsuite/rorax"
test_rorn: .incbin "./Testsuite/rorn"
test_rorz: .incbin "./Testsuite/rorz"
test_rorzx: .incbin "./Testsuite/rorzx"
test_rraa: .incbin "./Testsuite/rraa"
test_rraax: .incbin "./Testsuite/rraax"
test_rraay: .incbin "./Testsuite/rraay"
test_rraix: .incbin "./Testsuite/rraix"
test_rraiy: .incbin "./Testsuite/rraiy"
test_rraz: .incbin "./Testsuite/rraz"
test_rrazx: .incbin "./Testsuite/rrazx"
test_rtin: .incbin "./Testsuite/rtin"
test_rtsn: .incbin "./Testsuite/rtsn"
test_sbca: .incbin "./Testsuite/sbca"
test_sbcax: .incbin "./Testsuite/sbcax"
test_sbcay: .incbin "./Testsuite/sbcay"
test_sbcb: .incbin "./Testsuite/sbcb"
test_sbcbeb: .incbin "./Testsuite/sbcb(eb)"
test_sbcix: .incbin "./Testsuite/sbcix"
test_sbciy: .incbin "./Testsuite/sbciy"
test_sbcz: .incbin "./Testsuite/sbcz"
test_sbczx: .incbin "./Testsuite/sbczx"
test_sbxb: .incbin "./Testsuite/sbxb"
test_secn: .incbin "./Testsuite/secn"
test_sedn: .incbin "./Testsuite/sedn"
test_sein: .incbin "./Testsuite/sein"
test_shaay: .incbin "./Testsuite/shaay"
test_shaiy: .incbin "./Testsuite/shaiy"
test_shsay: .incbin "./Testsuite/shsay"
test_shxay: .incbin "./Testsuite/shxay"
test_shyax: .incbin "./Testsuite/shyax"
test_staa: .incbin "./Testsuite/staa"
test_staax: .incbin "./Testsuite/staax"
test_staay: .incbin "./Testsuite/staay"
test_staix: .incbin "./Testsuite/staix"
test_staiy: .incbin "./Testsuite/staiy"
test_staz: .incbin "./Testsuite/staz"
test_stazx: .incbin "./Testsuite/stazx"
test_stxa: .incbin "./Testsuite/stxa"
test_stxz: .incbin "./Testsuite/stxz"
test_stxzy: .incbin "./Testsuite/stxzy"
test_stya: .incbin "./Testsuite/stya"
test_styz: .incbin "./Testsuite/styz"
test_styzx: .incbin "./Testsuite/styzx"
test_taxn: .incbin "./Testsuite/taxn"
test_tayn: .incbin "./Testsuite/tayn"
test_trap1: .incbin "./Testsuite/trap1"
test_trap10: .incbin "./Testsuite/trap10"
test_trap11: .incbin "./Testsuite/trap11"
test_trap12: .incbin "./Testsuite/trap12"
test_trap13: .incbin "./Testsuite/trap13"
test_trap14: .incbin "./Testsuite/trap14"
test_trap15: .incbin "./Testsuite/trap15"
test_trap16: .incbin "./Testsuite/trap16"
test_trap17: .incbin "./Testsuite/trap17"
test_trap2: .incbin "./Testsuite/trap2"
test_trap3: .incbin "./Testsuite/trap3"
test_trap4: .incbin "./Testsuite/trap4"
test_trap5: .incbin "./Testsuite/trap5"
test_trap6: .incbin "./Testsuite/trap6"
test_trap7: .incbin "./Testsuite/trap7"
test_trap8: .incbin "./Testsuite/trap8"
test_trap9: .incbin "./Testsuite/trap9"
test_tsxn: .incbin "./Testsuite/tsxn"
test_txan: .incbin "./Testsuite/txan"
test_txsn: .incbin "./Testsuite/txsn"
test_tyan: .incbin "./Testsuite/tyan"

rasterlinetest: .incbin "./test_programs/rasterlinetest"

test_end: .byte 00
