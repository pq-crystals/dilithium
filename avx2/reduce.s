.global reduce32
reduce32:
mov		%rsp,%r11
and		$31,%r11
sub		%r11,%rsp

vmovdqa		_8x23ones(%rip),%ymm0
xor		%eax,%eax

.p2align 5
_looptop_rdc32:
#load
vmovdqa		(%rsi),%ymm1
vmovdqa		32(%rsi),%ymm3
vmovdqa		64(%rsi),%ymm5
vmovdqa		96(%rsi),%ymm7

#reduce
vpsrld		$23,%ymm1,%ymm2
vpsrld		$23,%ymm3,%ymm4
vpsrld		$23,%ymm5,%ymm6
vpsrld		$23,%ymm7,%ymm8
vpand		%ymm0,%ymm1,%ymm1
vpand		%ymm0,%ymm3,%ymm3
vpand		%ymm0,%ymm5,%ymm5
vpand		%ymm0,%ymm7,%ymm7
vpsubd		%ymm2,%ymm1,%ymm1
vpsubd		%ymm4,%ymm3,%ymm3
vpsubd		%ymm6,%ymm5,%ymm5
vpsubd		%ymm8,%ymm7,%ymm7
vpslld		$13,%ymm2,%ymm2
vpslld		$13,%ymm4,%ymm4
vpslld		$13,%ymm6,%ymm6
vpslld		$13,%ymm8,%ymm8
vpaddd		%ymm2,%ymm1,%ymm1
vpaddd		%ymm4,%ymm3,%ymm3
vpaddd		%ymm6,%ymm5,%ymm5
vpaddd		%ymm8,%ymm7,%ymm7

#store
vmovdqa		%ymm1,(%rdi)
vmovdqa		%ymm3,32(%rdi)
vmovdqa		%ymm5,64(%rdi)
vmovdqa		%ymm7,96(%rdi)

add		$1,%eax
add		$128,%rdi
add		$128,%rsi
cmp		$8,%eax
jb _looptop_rdc32

add		%r11,%rsp
ret

.global csubq
csubq:
mov		%rsp,%r11
and		$31,%r11
sub		%r11,%rsp

vmovdqa		_8xq(%rip),%ymm0
xor		%eax,%eax

.p2align 5
_looptop_csubq:
#load
vmovdqa		(%rsi),%ymm1
vmovdqa		32(%rsi),%ymm3
vmovdqa		64(%rsi),%ymm5
vmovdqa		96(%rsi),%ymm7

#csubq
vpsubd		%ymm0,%ymm1,%ymm1
vpsubd		%ymm0,%ymm3,%ymm3
vpsubd		%ymm0,%ymm5,%ymm5
vpsubd		%ymm0,%ymm7,%ymm7
vpsrad		$31,%ymm1,%ymm2
vpsrad		$31,%ymm3,%ymm4
vpsrad		$31,%ymm5,%ymm6
vpsrad		$31,%ymm7,%ymm8
vpand		%ymm0,%ymm2,%ymm2
vpand		%ymm0,%ymm4,%ymm4
vpand		%ymm0,%ymm6,%ymm6
vpand		%ymm0,%ymm8,%ymm8
vpaddd		%ymm2,%ymm1,%ymm1
vpaddd		%ymm4,%ymm3,%ymm3
vpaddd		%ymm6,%ymm5,%ymm5
vpaddd		%ymm8,%ymm7,%ymm7

#store
vmovdqa		%ymm1,(%rdi)
vmovdqa		%ymm3,32(%rdi)
vmovdqa		%ymm5,64(%rdi)
vmovdqa		%ymm7,96(%rdi)

add		$1,%eax
add		$128,%rdi
add		$128,%rsi
cmp		$8,%eax
jb _looptop_csubq

add		%r11,%rsp
ret
