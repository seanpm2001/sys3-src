# C library -- time

# tvec = time(tvec);

	.set	time,13
.globl	_time

_time:
	.word	0x0000
	chmk	$time
	movl	4(ap),r1
	beql	nostore
	movl	r0,(r1)
nostore:
	ret
