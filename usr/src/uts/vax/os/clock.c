#include "sys/param.h"
#include "sys/systm.h"
#include "sys/sysinfo.h"
#include "sys/callo.h"
#include "sys/dir.h"
#include "sys/user.h"
#include "sys/proc.h"
#include "sys/text.h"
#include "sys/psl.h"
#include "sys/var.h"

/*
 * clock is called straight from
 * the real time clock interrupt.
 *
 * Functions:
 *	implement callouts
 *	maintain user/system times
 *	maintain date
 *	profile
 *	alarm clock signals
 *	jab the scheduler
 */

#define	PRF_ON	01
extern	int	prfstat;

clock(pc, ps)
caddr_t pc;
{
	register struct callo *p1, *p2;
	register struct proc *pp;
	register a;
	static short lticks;
	extern caddr_t waitloc;

	/*
	 * restart clock
	 */
	clkreld();

	/*
	 * callouts
	 * if none, just continue
	 * else update first non-zero time
	 */

	if(callout[0].c_func == NULL)
		goto out;
	p2 = &callout[0];
	while(p2->c_time<=0 && p2->c_func!=NULL)
		p2++;
	p2->c_time--;

	/*
	 * if ps is high, just return
	 */
	if (BASEPRI(ps))
		goto out;
	/*
	 * callout
	 */

	if(callout[0].c_time <= 0) {
		p1 = &callout[0];
		while(p1->c_func != 0 && p1->c_time <= 0) {
			(*p1->c_func)(p1->c_arg);
			p1++;
		}
		p2 = &callout[0];
		while(p2->c_func = p1->c_func) {
			p2->c_time = p1->c_time;
			p2->c_arg = p1->c_arg;
			p1++;
			p2++;
		}
	}

out:
	if(prfstat & PRF_ON)
		prfintr(pc, ps);
	if (USERMODE(ps)) {
		a = CPU_USER;
		u.u_utime++;
		if(u.u_prof.pr_scale)
			addupc(pc, &u.u_prof, 1);
	} else {
		if (pc == waitloc) {
			a = CPU_IDLE;
			if (syswait.iowait)
				sysinfo.wait[W_IO]++;
			if (syswait.swap)
				sysinfo.wait[W_SWAP]++;
			if (syswait.physio)
				sysinfo.wait[W_PIO]++;
		} else
			a = CPU_KERN;
		u.u_stime++;
	}
	sysinfo.cpu[a]++;
	pp = u.u_procp;
	if(pp->p_stat==SRUN) {
		u.u_mem += (unsigned)pp->p_size;
		if(pp->p_textp) {
			a = pp->p_textp->x_ccount;
			if(a==0)
				a++;
			u.u_mem += pp->p_textp->x_size/a;
		}
	}
	if(pp->p_cpu < 80)
		pp->p_cpu++;
	lbolt++;	/* time in ticks */
	if (--lticks <= 0) {
		if (BASEPRI(ps))
			return;
		lticks += HZ;
		++time;
		runrun++;
		for(pp = &proc[0]; pp < (struct proc *)v.ve_proc; pp++)
		if (pp->p_stat) {
			if(pp->p_time != 127)
				pp->p_time++;
			if(pp->p_clktim)
				if(--pp->p_clktim == 0)
					psignal(pp, SIGALRM);
			pp->p_cpu >>= 1;
			if(pp->p_pri >= PUSER) {
				pp->p_pri = (pp->p_cpu>>1) + PUSER +
					pp->p_nice - NZERO;
			}
		}
		if(runin!=0) {
			runin = 0;
			setrun(&proc[0]);
		}
	}
}

/*
 * timeout is called to arrange that fun(arg) is called in tim/HZ seconds.
 * An entry is sorted into the callout structure.
 * The time in each structure entry is the number of HZ's more
 * than the previous entry. In this way, decrementing the
 * first entry has the effect of updating all entries.
 *
 * The panic is there because there is nothing
 * intelligent to be done if an entry won't fit.
 */
timeout(fun, arg, tim)
int (*fun)();
caddr_t arg;
{
	register struct callo *p1, *p2;
	register int t;
	int s;

	t = tim;
	p1 = &callout[0];
	s = spl7();
	while(p1->c_func != 0 && p1->c_time <= t) {
		t -= p1->c_time;
		p1++;
	}
	if (p1 >= &callout[v.v_call-1])
		panic("Timeout table overflow");
	p1->c_time -= t;
	p2 = p1;
	while(p2->c_func != 0)
		p2++;
	while(p2 >= p1) {
		(p2+1)->c_time = p2->c_time;
		(p2+1)->c_func = p2->c_func;
		(p2+1)->c_arg = p2->c_arg;
		p2--;
	}
	p1->c_time = t;
	p1->c_func = fun;
	p1->c_arg = arg;
	splx(s);
}

#define	PDELAY	(PZERO-1)
delay(ticks)
{
	extern wakeup();

	if (ticks<=0)
		return;
	timeout(wakeup, (caddr_t)u.u_procp+1, ticks);
	sleep((caddr_t)u.u_procp+1, PDELAY);
}
