/* funct_fortran.F -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Table of constant values */

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__3 = 3;
static integer c__4 = 4;

/* Main program */ int MAIN__(void)
{
    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(void);

    /* Local variables */
    static integer i__;
    static real x;
    extern /* Subroutine */ int funct_(real *, integer *);

    /* Fortran I/O blocks */
    static cilist io___3 = { 0, 6, 0, 0, 0 };
    static cilist io___4 = { 0, 6, 0, 0, 0 };


    i__ = 42;
    x = 2.71828f;
    s_wsle(&io___3);
    do_lio(&c__9, &c__1, "begining of main(): i= ", (ftnlen)23);
    do_lio(&c__3, &c__1, (char *)&i__, (ftnlen)sizeof(integer));
    do_lio(&c__9, &c__1, " x= ", (ftnlen)4);
    do_lio(&c__4, &c__1, (char *)&x, (ftnlen)sizeof(real));
    e_wsle();
    funct_(&x, &i__);
    s_wsle(&io___4);
    do_lio(&c__9, &c__1, "end of main(): i= ", (ftnlen)18);
    do_lio(&c__3, &c__1, (char *)&i__, (ftnlen)sizeof(integer));
    do_lio(&c__9, &c__1, " x= ", (ftnlen)4);
    do_lio(&c__4, &c__1, (char *)&x, (ftnlen)sizeof(real));
    e_wsle();
    return 0;
} /* MAIN__ */

/* Main program alias */ int main_ () { MAIN__ (); return 0; }
