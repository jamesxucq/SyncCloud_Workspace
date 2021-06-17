/* 
 * File:   query_xml.h
 * Author: David
 *
 * Created on 2011
 */

#ifndef QUERYXML_H
#define	QUERYXML_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>   

#include "dav_macro.h"    
#include "dav_stru.h"

#define NEW_QUERY_VALUE(xml_buff) xml_buff = malloc(XML_LENGTH);
#define DEL_QUERY_VALUE(xml_buff) if(xml_buff) free(xml_buff);    

    int query_from_xmlfile(unsigned int *cached_anchor, serv_addr *saddr, char *req_valid, char *xml_txt);

#ifdef	__cplusplus
}
#endif

#endif	/* QUERYXML_H */

