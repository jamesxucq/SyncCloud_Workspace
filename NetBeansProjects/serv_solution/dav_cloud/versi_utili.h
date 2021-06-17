/* 
 * File:   veron_utili.h
 * Author: Administrator
 */

#ifndef VERSON_UTILI_H
#define	VERSON_UTILI_H

#ifdef	__cplusplus
extern "C" {
#endif

#define DELETE_BAKUP_DEFAULT   "~/.delete_bakup.acd"
#define MODIFY_VERSI_DEFAULT   "~/.modify_versi.acd"

//
extern uint32 _modi_interv_;
extern uint32 _dele_interv_;

//
    int delete_backup(char *location, char *file_name, time_t dele_time);
    int modify_verson(char *location, char *file_name, time_t modi_time);

#ifdef	__cplusplus
}
#endif

#endif	/* VERSON_UTILI_H */

