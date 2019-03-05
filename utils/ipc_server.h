/**
 * @file    ipc_server.h
 * @author  ln
 * @brief   ipc by AF_UNIX socket
 **/

#ifndef __IPC_SERVER_H__
#define __IPC_SERVER_H__

#include <pthread.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    pthread_mutex_t     mux;
    pthread_mutexattr_t attr;
} mux_t;

extern int      ipcs_open   (char *filename, int flag, int listen_num); // server : client
// thread accept, 
//
extern int      ipcs_close  (int ipcs);

extern int      ipcs_receive(int ipcs);
// hdr=id+len

extern mux_t*   ipcs_connect(mux_t **mux);
// connect + send:id(fd cannot id)
//
extern void     mux_destroy (mux_t *mux);

extern int      mux_lock    (mux_t *mux);
extern int      mux_unlock  (mux_t *mux);

#ifdef __cplusplus
}
#endif

#endif /* __THR_MUX__ */

