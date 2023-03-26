/**
 *  \file probConst.h (interface file)
 *
 *  \brief Problem name: Text Processing.
 *
 *  Problem simulation parameters.
 *
 *  \author Alexandre Gago 
 *  \author Bernardo Kaluza
 */

#ifndef PROBCONST_H_
#define PROBCONST_H_

/* Generic parameters */

/** \brief maximum number of worker threads */
#define  MAXTHREADS           8

/**
 * @brief Size of the total bytes of data each chunk will have, default is 8192 bytes but can be changed by the user with the -s flag
 * 
 */
#define CHUNKSIZE 8192

#endif /* PROBCONST_H_ */
