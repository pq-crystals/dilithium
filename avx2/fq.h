.macro red16 r x=12
.endm

.macro csubq r x=12
.endm

.macro fqmul a,b x=12
vpmuludq	%ymm\a,%ymm\b,%ymm\b
vpmuludq	%ymm0,%ymm\b,%ymm\x
vpmuludq	%ymm1,%ymm\x,%ymm\x
vpaddq		%ymm\x,%ymm\b,%ymm\b
vpsrlq		$32,%ymm\b,%ymm\b
.endm
