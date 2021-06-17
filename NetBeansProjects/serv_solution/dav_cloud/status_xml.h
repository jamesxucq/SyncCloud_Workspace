/* 
 * File:   status_xml.h
 * Author: David
 *
 * Created on 2011
 */

#ifndef STATUSXML_H
#define	STATUSXML_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>   

#include "dav_macro.h"    

#define NEW_STATUS_VALUE(xml_buff) xml_buff = malloc(XML_LENGTH);
#define DEL_STATUS_VALUE(xml_buff) if(xml_buff) free(xml_buff);    

    int status_from_xmlfile(char *location, char *xml_txt);

#ifdef	__cplusplus
}
#endif

#endif	/* STATUSXML_H */

